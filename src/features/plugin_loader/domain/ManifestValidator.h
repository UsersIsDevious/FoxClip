#pragma once
#include <string>
#include <vector>
#include "Manifest.h"
#include "infra_shared/version/Semver.h"
#include "infra_shared/common/domain/Result.h"

namespace foxclip::plugin_loader::domain {

struct ValidationInput {
	foxclip::infra_shared::version::Semver hostSdk;     // FOXCLIP_SDK_VERSION_*
	foxclip::infra_shared::version::Semver hostVersion; // PLUGIN_VERSION
	std::string platformKey{"windows"};                 // "windows"|"linux"|"macos"
	foxclip::infra_shared::version::CompatPolicy sdkPolicy{};
};

// Use shared validation result type
using ValidationResult = foxclip::infra_shared::common::domain::ValidationResult;

ValidationResult validate(const Manifest &m, const ValidationInput &in);

} // namespace foxclip::plugin_loader::domain
