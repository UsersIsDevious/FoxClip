#pragma once
#include <string>
#include "Semver.h"

namespace foxclip::infra_shared::version::domain {

// バージョン情報提供の抽象ポート
class IVersionProvider {
public:
	virtual ~IVersionProvider() = default;

	virtual Semver getHostVersion() = 0;
	virtual Semver getSdkVersion() = 0;
	virtual std::string getVersionString() = 0;
};

} // namespace foxclip::infra_shared::version::domain
