#pragma once
#include "features/startup_check/domain/StartupCheckService.h"
#include <filesystem>

namespace foxclip::startup_check::infrastructure {

class StdFsDirectoryChecker : public foxclip::startup_check::domain::IDirectoryChecker {
public:
	bool existsDir(const std::string &path) const override;
};

} // namespace foxclip::startup_check::infrastructure
