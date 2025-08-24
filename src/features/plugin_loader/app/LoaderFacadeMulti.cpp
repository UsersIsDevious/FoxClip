#include "features/plugin_loader/app/LoaderFacadeMulti.h"

#include <filesystem>
#include <system_error>

#include "infra_shared/log/ObsLogger.h"
#include "features/plugin_loader/infrastructure/NlohmannManifestReader.h"
#include "features/plugin_loader/app/ValidationInputFactory.h"
#include "features/plugin_loader/domain/ManifestValidator.h"

#include "infra_shared/plugin/MultiPluginHost.h"

namespace fs = std::filesystem;

namespace foxclip::plugin_loader::app {

using foxclip::plugin_loader::domain::LoadOneResult;
using foxclip::plugin_loader::domain::LoadAllResult;
using foxclip::plugin_loader::domain::LoadSuccess;
using foxclip::plugin_loader::domain::LoadError;
using foxclip::plugin_loader::infrastructure::NlohmannManifestReader;
namespace host = foxclip::infra_shared::plugin;

static fs::path u8(const std::string &s)
{
	return fs::u8path(s);
}

LoadOneResult LoaderFacadeMulti::loadOne(const std::string &pluginDirUtf8)
{
	std::error_code ec;
	if (!fs::is_directory(u8(pluginDirUtf8), ec))
		return LoadOneResult::failureOf({pluginDirUtf8, "not a directory"});

	NlohmannManifestReader reader;
	auto rr = reader.readFromDir(pluginDirUtf8);
	if (!rr.ok)
		return LoadOneResult::failureOf({pluginDirUtf8, rr.errorMessage});

	auto in = makeValidationInput();
	auto vr = foxclip::plugin_loader::domain::validate(*rr.manifest, in);
	if (!vr.ok) {
		std::string msg = "manifest invalid";
		for (auto &e : vr.errors)
			msg += "; " + e;
		return LoadOneResult::failureOf({pluginDirUtf8, std::move(msg)});
	}

	auto rel = rr.manifest->entryFor(in.platformKey);
	if (!rel)
		return LoadOneResult::failureOf({pluginDirUtf8, "entry path missing for platform"});

	fs::path modulePath = u8(pluginDirUtf8) / u8(*rel);

	host::PluginIdentity ident{rr.manifest->id, rr.manifest->name, rr.manifest->version};
	bool ok = host::MultiPluginHost::instance().loadAndStart(ident, modulePath.u8string());
	if (!ok) {
		return LoadOneResult::failureOf({pluginDirUtf8, "load/start failed"});
	}

	return LoadOneResult::successOf(LoadSuccess{pluginDirUtf8, modulePath.u8string(), rr.manifest->id,
						    rr.manifest->name, rr.manifest->version});
}

LoadAllResult LoaderFacadeMulti::loadAll(const std::string &pluginsRootUtf8)
{
	LoadAllResult out;
	std::error_code ec;
	if (!fs::is_directory(u8(pluginsRootUtf8), ec)) {
		out.errors.push_back({pluginsRootUtf8, "plugins root not found"});
		return out;
	}
	const auto opts = fs::directory_options::skip_permission_denied;
	for (fs::directory_iterator it(u8(pluginsRootUtf8), opts, ec), end; it != end && !ec; it.increment(ec)) {
		if (!it->is_directory(ec) || ec)
			continue;
		const std::string dirUtf8 = it->path().u8string();
		auto r = loadOne(dirUtf8);
		if (r.ok)
			out.successes.emplace_back(*r.success);
		else
			out.errors.emplace_back(*r.error);
	}
	return out;
}

} // namespace foxclip::plugin_loader::app
