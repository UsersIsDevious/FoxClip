#include "infra_shared/version/Semver.h"
#include <cctype>
#include <sstream>

namespace foxclip::infra_shared::version {

static bool isDigit(char c)
{
	return c >= '0' && c <= '9';
}

std::optional<Semver> Semver::parse(const std::string &s)
{
	int nums[3] = {0, 0, 0};
	int n = 0;
	size_t i = 0;
	while (i < s.size() && std::isspace((unsigned char)s[i]))
		++i;
	while (n < 3 && i < s.size() && isDigit(s[i])) {
		int v = 0;
		while (i < s.size() && isDigit(s[i])) {
			v = v * 10 + (s[i] - '0');
			++i;
		}
		nums[n++] = v;
		if (i < s.size() && s[i] == '.') {
			++i;
			continue;
		} else
			break;
	}
	if (n == 0)
		return std::nullopt;
	Semver sv;
	sv.major = nums[0];
	sv.minor = (n >= 2 ? nums[1] : 0);
	sv.patch = (n >= 3 ? nums[2] : 0);
	return sv;
}

std::string Semver::toString() const
{
	std::ostringstream oss;
	oss << major << '.' << minor << '.' << patch;
	return oss.str();
}

bool supportsSdk(const Semver &host, const Semver &sdk, const CompatPolicy &p)
{
	if (p.requireSameMajor && host.major != sdk.major)
		return false;
	if (p.requireHostMinorGreaterEqual && host.minor < sdk.minor)
		return false;
	return true; // patch は無視（p.ignorePatch は設計上デフォルトで無視）
}

bool satisfiesMinHost(const Semver &host, const Semver &minHost)
{
	return !(host < minHost); // host >= minHost
}

} // namespace foxclip::infra_shared::version
