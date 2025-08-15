#pragma once
#include "Result.h"
#include "DirectoryPolicy.h"

namespace foxclip::domain {

struct IDirectoryChecker {
	virtual ~IDirectoryChecker() = default;
	virtual bool existsDir(const std::string &path) const = 0;
};

class StartupCheckService {
public:
	StartupCheckService(const IDirectoryChecker &checker, DirectoryPolicy policy, std::string basePath);
	Result run() const;

private:
	const IDirectoryChecker &checker_;
	DirectoryPolicy policy_;
	std::string basePath_;
};

} // namespace foxclip::domain
