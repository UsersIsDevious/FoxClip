#include "features/plugin_loader/app/LoaderFacade.h"

#include <filesystem>
#include <system_error>

#include "infra_shared/log/ObsLogger.h"
#include "features/plugin_loader/infrastructure/NlohmannManifestReader.h"
#include "features/plugin_loader/app/ValidationInputFactory.h"
#include "features/plugin_loader/domain/ManifestValidator.h"
#include "infra_shared/plugin/FoxclipPluginHost.h"

namespace fs = std::filesystem;
using foxclip::plugin_loader::infrastructure::NlohmannManifestReader;
using foxclip::plugin_loader::domain::Manifest;
using foxclip::plugin_loader::domain::LoadOptions;
using foxclip::plugin_loader::domain::LoadOneResult;
using foxclip::plugin_loader::domain::LoadAllResult;
using foxclip::plugin_loader::domain::LoadSuccess;
using foxclip::plugin_loader::domain::LoadError;

namespace foxclip::plugin_loader::app {

static fs::path u8(const std::string &s)
{
	return fs::u8path(s);
}
static std::string toU8(const fs::path &p)
{
	return p.u8string();
}

// 単一
LoadOneResult LoaderFacade::loadOne(const std::string &pluginDirUtf8, const LoadOptions &opt)
{
	return loadOneImpl(pluginDirUtf8, opt);
}

LoadOneResult LoaderFacade::loadOneImpl(const std::string &pluginDirUtf8, const LoadOptions &opt)
{
	std::error_code ec;
	if (!fs::is_directory(u8(pluginDirUtf8), ec)) {
		return LoadOneResult::failureOf(LoadError{pluginDirUtf8, "not a directory"});
	}

	// 1) manifest 読み取り
	NlohmannManifestReader reader;
	auto rr = reader.readFromDir(pluginDirUtf8);
	if (!rr.ok) {
		return LoadOneResult::failureOf(LoadError{pluginDirUtf8, rr.errorMessage});
	}

	// 2) 仕様検証（SDK互換・min_host_version・エントリ存在）
	auto in = makeValidationInput();
#if defined(_WIN32)
	const char *platform = "windows";
#elif defined(__APPLE__)
	const char *platform = "macos";
#else
	const char *platform = "linux";
#endif
	in.platformKey = platform;

	auto vr = domain::validate(*rr.manifest, in);
	if (!vr.ok) {
		std::string msg = "manifest invalid";
		for (auto &e : vr.errors)
			msg += "; " + e;
		for (auto &w : vr.warnings)
			OBS_LOG_WARN("[foxclip] %s: %s", pluginDirUtf8.c_str(), w.c_str());
		return LoadOneResult::failureOf(LoadError{pluginDirUtf8, std::move(msg)});
	}

	// 3) エントリのフルパスを組み立て
	auto rel = rr.manifest->entryFor(in.platformKey);
	if (!rel || rel->empty()) {
		return LoadOneResult::failureOf(LoadError{pluginDirUtf8, "entry path missing for platform"});
	}
	fs::path modulePath = u8(pluginDirUtf8) / u8(*rel);

	// 4) 既にロード済みならオプションに従ってアンロード
	if (opt.unloadBeforeLoad && foxclip::Host::isLoaded()) {
		OBS_LOG_INFO("[foxclip] unloading currently loaded plugin...");
		foxclip::Host::unload();
	}

	// 5) ロード実行
	const bool ok = foxclip::Host::load(toU8(modulePath));
	if (!ok) {
		return LoadOneResult::failureOf(LoadError{pluginDirUtf8, "load failed: " + toU8(modulePath)});
	}

	// 6) 成功
	OBS_LOG_INFO("[foxclip] loaded plugin: %s (%s) from %s", rr.manifest->name.c_str(),
		     rr.manifest->version.c_str(), toU8(modulePath).c_str());

	return LoadOneResult::successOf(LoadSuccess{/*pluginDir*/ pluginDirUtf8,
						    /*modulePath*/ toU8(modulePath),
						    /*id*/ rr.manifest->id,
						    /*name*/ rr.manifest->name,
						    /*version*/ rr.manifest->version});
}

// 一括
LoadAllResult LoaderFacade::loadAll(const std::string &pluginsRootUtf8, const LoadOptions &opt)
{
	LoadAllResult out;

	std::error_code ec;
	if (!fs::is_directory(u8(pluginsRootUtf8), ec)) {
		out.errors.push_back(LoadError{pluginsRootUtf8, "plugins root not found"});
		return out;
	}

	const auto opts = fs::directory_options::skip_permission_denied;
	for (fs::directory_iterator it(u8(pluginsRootUtf8), opts, ec), end; it != end && !ec; it.increment(ec)) {
		if (!it->is_directory(ec) || ec)
			continue;

		const std::string dirUtf8 = it->path().u8string();
		auto r = loadOneImpl(dirUtf8, opt);
		if (r.ok) {
			out.successes.push_back(*r.success);
			if (opt.stopOnFirstSuccess)
				break;
		} else {
			out.errors.push_back(*r.error);
			// 続行
		}
	}
	return out;
}

} // namespace foxclip::plugin_loader::app
