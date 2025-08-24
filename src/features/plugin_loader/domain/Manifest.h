#pragma once
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>

namespace foxclip::plugin_loader::domain {

struct Manifest {
	std::string id;
	std::string name;
	std::string version;
	std::string sdkVersion;
	std::optional<std::string> minHostVersion;
	std::vector<std::string> capabilities;
	std::vector<std::string> locales;
	std::unordered_map<std::string, std::string> entry; // windows/linux/macos -> relpath

	std::optional<std::string> entryFor(const std::string &platform) const
	{
		auto it = entry.find(platform);
		if (it == entry.end() || it->second.empty())
			return std::nullopt;
		return it->second;
	}
};

struct ManifestReadResult {
	bool ok;
	std::optional<Manifest> manifest;
	std::string errorMessage;
	static ManifestReadResult success(Manifest m) { return {true, std::make_optional(std::move(m)), {}}; }
	static ManifestReadResult failure(std::string msg) { return {false, std::nullopt, std::move(msg)}; }
};

} // namespace foxclip::plugin_loader::domain
