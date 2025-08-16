#pragma once
#include "features/startup_check/domain/StartupCheckService.h"
#include <filesystem>

namespace foxclip::features::startup_check::infrastructure {

class StdFsDirectoryChecker : public foxclip::domain::IDirectoryChecker {
public:
	bool existsDir(const std::string &path) const override;
};

} // namespace foxclip::features::startup_check::infrastructure
