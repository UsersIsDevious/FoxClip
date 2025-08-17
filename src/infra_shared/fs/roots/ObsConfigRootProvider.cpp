#include "ObsConfigRootProvider.h"
#include "infra_shared/config/path/ObsConfigPathProvider.h"

namespace foxclip::infra_shared::fs::roots {

using foxclip::infra_shared::config::path::makeObsConfigPathProvider;
using foxclip::infra_shared::config::path::IConfigPathProvider;


ObsConfigRootProvider::ObsConfigRootProvider(std::shared_ptr<IConfigPathProvider> provider)
    : pathProvider(provider ? std::move(provider) : makeObsConfigPathProvider())
{
    // 本体
}

std::string ObsConfigRootProvider::root()
{
	// 遅延初期化（保険）。工場が null を返す可能性にも対応
	if (!pathProvider) {
		auto created = makeObsConfigPathProvider();
		if (!created) {
			// ここでは決して参照しない。空文字を返して上位で失敗扱いにする。
			return {};
		}
		pathProvider = std::move(created);
	}

	// ここまで来たら必ず非 null
	return pathProvider->configPath("");
}

} // namespace foxclip::infra_shared::fs::roots
