#pragma once
#include "../domain/StartupCheckService.h"
#include "../domain/DirectoryCreator.h"
#include <string>

namespace foxclip::startup_check::app {

// Application service - moved from domain layer
class StartupCheckService {
public:
	StartupCheckService(const domain::IDirectoryChecker &checker, 
			    domain::DirectoryPolicy policy, 
			    std::string basePath,
			    domain::IDirectoryCreator &creator);
	
	domain::Result run();

private:
	const domain::IDirectoryChecker &checker;
	domain::DirectoryPolicy policy;
	std::string basePath;
	domain::IDirectoryCreator &creator;
};

} // namespace foxclip::startup_check::app
