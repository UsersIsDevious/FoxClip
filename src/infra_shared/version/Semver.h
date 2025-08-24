#pragma once
#include <string>
#include <optional>

namespace foxclip::infra_shared::version {

struct Semver {
	int major{0};
	int minor{0};
	int patch{0};

	// C++17 互換: 明示的に演算子を定義
	bool operator==(const Semver &rhs) const
	{
		return major == rhs.major && minor == rhs.minor && patch == rhs.patch;
	}
	bool operator!=(const Semver &rhs) const { return !(*this == rhs); }
	bool operator<(const Semver &rhs) const
	{
		if (major != rhs.major)
			return major < rhs.major;
		if (minor != rhs.minor)
			return minor < rhs.minor;
		return patch < rhs.patch;
	}
	bool operator>(const Semver &rhs) const { return rhs < *this; }
	bool operator<=(const Semver &rhs) const { return !(rhs < *this); }
	bool operator>=(const Semver &rhs) const { return !(*this < rhs); }

	static std::optional<Semver> parse(const std::string &s);
	std::string toString() const;
};

struct CompatPolicy {
	bool requireSameMajor{true};
	bool requireHostMinorGreaterEqual{true};
	bool ignorePatch{true};
};

bool supportsSdk(const Semver &host, const Semver &sdk, const CompatPolicy &policy = {});
bool satisfiesMinHost(const Semver &host, const Semver &minHost);

} // namespace foxclip::infra_shared::version
