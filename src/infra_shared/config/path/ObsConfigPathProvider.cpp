#include "ObsConfigPathProvider.h"
#include <obs-module.h>    // obs_module_config_path
#include <util/platform.h> // bfree
#include <memory>

namespace foxclip::infra_shared::config::path {

class ObsConfigPathProvider final : public IConfigPathProvider {
public:
	std::string configPath(const std::string &subdir) override
	{
		const char *arg = subdir.empty() ? nullptr : subdir.c_str();
		char *raw = obs_module_config_path(arg);
		if (!raw)
			return {};
		std::string path(raw);
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
