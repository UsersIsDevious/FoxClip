#include "ObsConfigPathProvider.h"
#include "infra_shared/log/ObsLogger.h"
#include <obs-module.h>    // obs_module_config_path
#include <util/platform.h> // bfree
#include <memory>

namespace foxclip::infra_shared::config::path {

class ObsConfigPathProvider final : public IConfigPathProvider {
public:
	std::string configPath(const std::string &subdir) override
	{
		OBS_LOG_INFO("[ObsConfigPathProvider] Requesting config path for subdir: '%s'", subdir.c_str());
		const char *arg = subdir.empty() ? nullptr : subdir.c_str();
		char *raw = obs_module_config_path(arg);
		if (!raw) {
			OBS_LOG_ERROR("[ObsConfigPathProvider] obs_module_config_path returned null for subdir: '%s'",
				      subdir.c_str());
			return {};
		}
		std::string path(raw);
		OBS_LOG_INFO("[ObsConfigPathProvider] Config path resolved to: '%s'", path.c_str());
		bfree(raw);
		return path;
	}
};

// 生成ヘルパ（必要なら）
std::unique_ptr<IConfigPathProvider> makeObsConfigPathProvider()
{
	return std::make_unique<ObsConfigPathProvider>();
}

} // namespace foxclip::infra_shared::config::path
