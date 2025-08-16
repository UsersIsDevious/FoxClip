#include "StartupCheckFacade.h"
#include "infra_shared/log/ObsLogger.h"
#include "infra_shared/config/path/ObsConfigPathProvider.h"
#include <memory>

namespace foxclip::startup_check::app {

StartupCheckFacade::StartupCheckFacade(std::unique_ptr<foxclip::startup_check::domain::IDirectoryChecker> checker,
				       std::string basePath, std::string requiredName)
	: checker_(std::move(checker)),
	  creator_()
{
	// OBS の書き込み可能ディレクトリに解決
	using foxclip::infra_shared::config::path::MakeObsConfigPathProvider;
	auto provider = MakeObsConfigPathProvider();
	const std::string full = provider->config_path(requiredName);
	if (!full.empty()) {
		service_ = std::make_unique<foxclip::startup_check::domain::StartupCheckService>(
			*checker_, foxclip::startup_check::domain::DirectoryPolicy{full}, "", creator_);
		OBS_LOG_INFO("[foxclip] using OBS config dir: %s", full.c_str());
	} else {
		service_ = std::make_unique<foxclip::startup_check::domain::StartupCheckService>(
			*checker_, foxclip::startup_check::domain::DirectoryPolicy{requiredName}, basePath, creator_);
		OBS_LOG_WARN("[foxclip] failed to get OBS config dir; fallback to basePath='%s', name='%s'",
			     basePath.c_str(), requiredName.c_str());
	}
}

StartupCheckFacade::StartupCheckFacade(std::unique_ptr<domain::IDirectoryChecker> checker, std::string requiredName)
	: StartupCheckFacade(std::move(checker), "", std::move(requiredName))
{
}

foxclip::startup_check::domain::Result StartupCheckFacade::run()
{
	auto res = service_->run();
	if (res.ok)
		OBS_LOG_INFO("%s", res.message.c_str());
	else
		OBS_LOG_ERROR("%s", res.message.c_str());
	return res;
}

} // namespace foxclip::startup_check::app