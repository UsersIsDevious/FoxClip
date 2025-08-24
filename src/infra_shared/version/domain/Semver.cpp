#include "Semver.h"
#include <sstream>
#include <regex>

namespace foxclip::infra_shared::version::domain {

std::optional<Semver> Semver::parse(const std::string &s)
{
	std::regex semverRegex(R"(^(\d+)\.(\d+)\.(\d+)$)");
	std::smatch match;

	if (!std::regex_match(s, match, semverRegex)) {
		return std::nullopt;
	}

	try {
		int major = std::stoi(match[1].str());
		int minor = std::stoi(match[2].str());
		int patch = std::stoi(match[3].str());
		return Semver{major, minor, patch};
	} catch (...) {
		return std::nullopt;
	}
}

std::string Semver::toString() const
{
	std::ostringstream oss;
	oss << major << "." << minor << "." << patch;
	return oss.str();
}

bool supportsSdk(const Semver &host, const Semver &sdk, const CompatPolicy &policy)
{
	if (policy.requireSameMajor && host.major != sdk.major) {
		return false;
	}

	if (policy.requireHostMinorGreaterEqual && host.minor < sdk.minor) {
		return false;
	}

	return true;
}

bool satisfiesMinHost(const Semver &host, const Semver &minHost)
{
	return host >= minHost;
}

} // namespace foxclip::infra_shared::version::domain
