#include "ObsConfigRootProvider.h"
#include "infra_shared/config/path/ObsConfigPathProvider.h"

namespace foxclip::infra_shared::fs::roots {

std::string ObsConfigRootProvider::root()
{
	using foxclip::infra_shared::config::path::makeObsConfigPathProvider;
	auto prov = makeObsConfigPathProvider();
	// ""（または nullptr 相当）で「このプラグインのルート」ディレクトリ
	return prov->config_path(""); // 例: .../obs-studio/plugin_config/<plugin>
}

} // namespace foxclip::infra_shared::fs::roots
