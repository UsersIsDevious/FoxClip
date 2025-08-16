#pragma once
#include "IRootProvider.h"

namespace foxclip::infra_shared::fs::roots {

class ObsConfigRootProvider final : public IRootProvider {
public:
	std::string root() override; // 実装は .cpp
};

} // namespace foxclip::infra_shared::fs::roots
