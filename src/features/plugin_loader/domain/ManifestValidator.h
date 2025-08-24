#pragma once
#include <string>
#include <vector>
#include "Manifest.h"
#include "infra_shared/version/Semver.h"

namespace foxclip::plugin_loader::domain {

struct ValidationInput {
	foxclip::infra_shared::version::Semver hostSdk;     // FOXCLIP_SDK_VERSION_*
	foxclip::infra_shared::version::Semver hostVersion; // PLUGIN_VERSION
	std::string platformKey{"windows"};                 // "windows"|"linux"|"macos"
	foxclip::infra_shared::version::CompatPolicy sdkPolicy{};
};

struct ValidationResult {
	bool ok;
	std::vector<std::string> errors;
	std::vector<std::string> warnings;
	static ValidationResult success() { return {true, {}, {}}; }
	static ValidationResult failure(std::vector<std::string> e, std::vector<std::string> w = {})
	{
		return {false, std::move(e), std::move(w)};
	}
};

ValidationResult validate(const Manifest &m, const ValidationInput &in);

} // namespace foxclip::plugin_loader::domain
