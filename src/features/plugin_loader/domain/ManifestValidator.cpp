#include "features/plugin_loader/domain/ManifestValidator.h"

namespace v = foxclip::infra_shared::version;

namespace foxclip::plugin_loader::domain {

ValidationResult validate(const Manifest &m, const ValidationInput &in)
{
	std::vector<std::string> errs, warns;

	if (m.id.empty())
		errs.emplace_back("missing/empty 'id'");
	if (m.name.empty())
		errs.emplace_back("missing/empty 'name'");
	if (m.version.empty())
		errs.emplace_back("missing/empty 'version'");
	if (m.sdkVersion.empty())
		errs.emplace_back("missing/empty 'sdk_version'");
	if (m.entry.empty())
		errs.emplace_back("missing/invalid 'entry'");
	if (!errs.empty())
		return ValidationResult::failure(std::move(errs));

	auto sdkSem = v::Semver::parse(m.sdkVersion);
	if (!sdkSem)
		return ValidationResult::failure({"invalid 'sdk_version' format: " + m.sdkVersion});

	auto pluginSem = v::Semver::parse(m.version);
	if (!pluginSem)
		warns.emplace_back("plugin 'version' is not semver-like: " + m.version);

	if (m.minHostVersion) {
		auto minH = v::Semver::parse(*m.minHostVersion);
		if (!minH)
			return ValidationResult::failure({"invalid 'min_host_version' format: " + *m.minHostVersion});
		if (!v::satisfiesMinHost(in.hostVersion, *minH)) {
			return ValidationResult::failure({"host version " + in.hostVersion.toString() +
							  " < min_host_version " + minH->toString()});
		}
	}

	if (!v::supportsSdk(in.hostSdk, *sdkSem, in.sdkPolicy)) {
		return ValidationResult::failure(
			{"incompatible sdk_version: plugin " + sdkSem->toString() + ", host " + in.hostSdk.toString()});
	}

	if (!m.entryFor(in.platformKey)) {
		return ValidationResult::failure({"entry for platform '" + in.platformKey + "' not found"});
	}

	return ValidationResult::success();
}

} // namespace foxclip::plugin_loader::domain
