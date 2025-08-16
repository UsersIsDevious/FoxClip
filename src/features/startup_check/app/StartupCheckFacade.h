#pragma once
#include "features/startup_check/domain/StartupCheckService.h"
#include "features/startup_check/infrastructure/StdFsDirectoryChecker.h"

namespace foxclip::features::startup_check::app {

class StartupCheckFacade {
public:
	StartupCheckFacade(std::unique_ptr<foxclip::domain::IDirectoryChecker> checker, std::string basePath, std::string requiredName);
	foxclip::domain::Result run(); // ログ出力やエラー整形もここで
private:
	std::unique_ptr<foxclip::domain::IDirectoryChecker> checker_;
	foxclip::domain::StartupCheckService service_;
};

} // namespace foxclip::features::startup_check::app