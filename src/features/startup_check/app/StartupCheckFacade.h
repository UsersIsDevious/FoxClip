#pragma once
#include "features/startup_check/domain/StartupCheckService.h"
#include "features/startup_check/infrastructure/StdFsDirectoryChecker.h"
#include "features/startup_check/infrastructure/StdFsDirectoryCreator.h"

namespace foxclip::startup_check::app {

class StartupCheckFacade {
public:
	// 起動時に作る/確認するサブディレクトリ名だけ受け取る（例: "foxclip-plugins"）
	StartupCheckFacade(std::unique_ptr<domain::IDirectoryChecker> checker, std::string requiredName);
	foxclip::startup_check::domain::Result run(); // ログ出力やエラー整形もここで
private:
	std::unique_ptr<foxclip::startup_check::domain::IDirectoryChecker> dirChecker;
	foxclip::startup_check::infrastructure::StdFsDirectoryCreator creator;
	std::unique_ptr<foxclip::startup_check::domain::StartupCheckService> service;
};

} // namespace foxclip::startup_check::app