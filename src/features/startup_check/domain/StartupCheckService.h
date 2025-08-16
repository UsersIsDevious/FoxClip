#pragma once
#include "Result.h"
#include "DirectoryPolicy.h"
#include "DirectoryCreator.h"

namespace foxclip::startup_check::domain {

struct IDirectoryChecker {
	virtual ~IDirectoryChecker() = default;
	virtual bool existsDir(const std::string &path) const = 0;
};

class StartupCheckService {
public:
	//StartupCheckService(const IDirectoryChecker &checker, DirectoryPolicy policy, std::string basePath);
	//Result run() const;
	StartupCheckService(const IDirectoryChecker &checker, DirectoryPolicy policy, std::string basePath,
			    DirectoryCreator &creator);
	Result run();

private:
	const IDirectoryChecker &checker_;
	DirectoryPolicy policy_;
	std::string basePath_;
	DirectoryCreator &creator_;
};

} // namespace foxclip::startup_check::domain
