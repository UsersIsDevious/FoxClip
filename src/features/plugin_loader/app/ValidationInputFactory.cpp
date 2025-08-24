#include "features/plugin_loader/app/ValidationInputFactory.h"

#include "infra_shared/version/Semver.h"
#include "infra_shared/config/build/plugin-config.h" // PLUGIN_VERSION
#include "foxclip/core/version.h"                    // FOXCLIP_SDK_VERSION_*

namespace v = foxclip::infra_shared::version;

namespace foxclip::plugin_loader::app {

static v::Semver sdkFromMacros()
{
	v::Semver s;
	s.major = FOXCLIP_SDK_VERSION_MAJOR;
	s.minor = FOXCLIP_SDK_VERSION_MINOR;
	s.patch = FOXCLIP_SDK_VERSION_PATCH;
	return s;
}

static std::string currentPlatformKey()
{
#if defined(_WIN32)
	return "windows";
#elif defined(__APPLE__)
	return "macos";
#else
	return "linux";
#endif
}

domain::ValidationInput makeValidationInput()
{
	domain::ValidationInput in;
	in.hostSdk = sdkFromMacros();

	// PLUGIN_VERSION は "2.0.0" のような文字列想定（plugin-config.c.in 由来）
	auto hv = v::Semver::parse(PLUGIN_VERSION);
	in.hostVersion = hv.value_or(v::Semver{0, 0, 0});

	in.platformKey = currentPlatformKey();
	// 互換ポリシはデフォルト（major 同一 & host.minor >= sdk.minor）
	return in;
}

} // namespace foxclip::plugin_loader::app
