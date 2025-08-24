#pragma once
#include <memory>
#include "../domain/IVersionProvider.h"
#include "../domain/Semver.h"

namespace foxclip::infra_shared::version::app {

// バージョン管理の調停サービス
class VersionService {
public:
	explicit VersionService(std::unique_ptr<domain::IVersionProvider> provider);

	domain::Semver getHostVersion();
	domain::Semver getSdkVersion();
	std::string getVersionString();

	bool isCompatible(const domain::Semver &pluginVersion, const domain::CompatPolicy &policy = {});
	bool satisfiesMinimumHost(const domain::Semver &minHost);

private:
	std::unique_ptr<domain::IVersionProvider> provider;
};

} // namespace foxclip::infra_shared::version::app
