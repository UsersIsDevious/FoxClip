#include "NlohmannManifestReader.h"
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

using nlohmann::json;
namespace fs = std::filesystem;

namespace {
bool isNonEmptyString(const json &j, const char *key)
{
	return j.contains(key) && j[key].is_string() && !j[key].get_ref<const std::string &>().empty();
}
} // namespace

namespace foxclip::plugin_loader::infrastructure {

using foxclip::plugin_loader::domain::Manifest;
using foxclip::plugin_loader::domain::ManifestReadResult;

static fs::path u8(const std::string &s)
{
	return fs::u8path(s);
}

ManifestReadResult NlohmannManifestReader::readFromDir(const std::string &pluginDirUtf8)
{
	const fs::path manifestPath = u8(pluginDirUtf8) / u8("plugin.json");

	std::error_code ec;
	if (!fs::is_regular_file(manifestPath, ec)) {
		return ManifestReadResult::failure("plugin.json not found: " + manifestPath.u8string());
	}
	std::ifstream ifs(manifestPath, std::ios::binary);
	if (!ifs) {
		return ManifestReadResult::failure("failed to open plugin.json: " + manifestPath.u8string());
	}

	json j;
	try {
		ifs >> j;
	} catch (const std::exception &e) {
		return ManifestReadResult::failure(std::string("invalid JSON: ") + e.what());
	}

	if (!isNonEmptyString(j, "id"))
		return ManifestReadResult::failure("missing/empty 'id'");
	if (!isNonEmptyString(j, "name"))
		return ManifestReadResult::failure("missing/empty 'name'");
	if (!isNonEmptyString(j, "version"))
		return ManifestReadResult::failure("missing/empty 'version'");
	if (!isNonEmptyString(j, "sdk_version"))
		return ManifestReadResult::failure("missing/empty 'sdk_version'");
	if (!j.contains("entry") || !j["entry"].is_object())
		return ManifestReadResult::failure("missing/invalid 'entry' object");

	Manifest m;
	m.id = j["id"].get<std::string>();
	m.name = j["name"].get<std::string>();
	m.version = j["version"].get<std::string>();
	m.sdkVersion = j["sdk_version"].get<std::string>();

	if (j.contains("min_host_version") && j["min_host_version"].is_string())
		m.minHostVersion = j["min_host_version"].get<std::string>();

	if (j.contains("capabilities") && j["capabilities"].is_array()) {
		for (const auto &e : j["capabilities"])
			if (e.is_string())
				m.capabilities.emplace_back(e.get<std::string>());
	}
	if (j.contains("locales") && j["locales"].is_array()) {
		for (const auto &e : j["locales"])
			if (e.is_string())
				m.locales.emplace_back(e.get<std::string>());
	}

	for (auto it = j["entry"].begin(); it != j["entry"].end(); ++it) {
		if (it.value().is_string()) {
			const auto rel = it.value().get<std::string>();
			if (!rel.empty())
				m.entry[it.key()] = rel;
		}
	}
	if (m.entry.empty())
		return ManifestReadResult::failure("entry has no valid paths");

	return ManifestReadResult::success(std::move(m));
}

} // namespace foxclip::plugin_loader::infrastructure
