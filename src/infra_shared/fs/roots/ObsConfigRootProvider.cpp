// ObsConfigRootProvider.cpp
#include "ObsConfigRootProvider.h"
#include "infra_shared/config/path/ObsConfigPathProvider.h"

namespace foxclip::infra_shared::fs::roots {

using foxclip::infra_shared::config::path::makeObsConfigPathProvider;
using foxclip::infra_shared::config::path::IConfigPathProvider;

ObsConfigRootProvider::ObsConfigRootProvider(std::shared_ptr<IConfigPathProvider> provider)
	: provider_(std::move(provider))
{
	if (!provider_) {
		provider_ = makeObsConfigPathProvider(); // コンストラクタで1回だけ生成
	}
}

std::string ObsConfigRootProvider::root()
{
	return provider_->config_path("");
}

} // namespace foxclip::infra_shared::fs::roots
