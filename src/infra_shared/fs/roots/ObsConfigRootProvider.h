#pragma once
#include <memory>
#include "IRootProvider.h"

namespace foxclip::infra_shared::config::path {
class IConfigPathProvider;
}

namespace foxclip::infra_shared::fs::roots {

class ObsConfigRootProvider final : public IRootProvider {
public:
	using Provider = foxclip::infra_shared::config::path::IConfigPathProvider;

	// 既定は nullptr。nullptr の場合は .cpp 側でファクトリから生成
	explicit ObsConfigRootProvider(std::shared_ptr<Provider> provider = nullptr);

	std::string root() override;

private:
	std::shared_ptr<Provider> pathProvider;
};

} // namespace foxclip::infra_shared::fs::roots
