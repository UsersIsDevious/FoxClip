#include "StartupCheckService.h"

namespace foxclip::domain {

StartupCheckService::StartupCheckService(const IDirectoryChecker &checker, DirectoryPolicy policy, std::string basePath)
	: checker_(checker),
	  policy_(std::move(policy)),
	  basePath_(std::move(basePath))
{
}

Result StartupCheckService::run() const
{
	std::string target = basePath_ + "/" + policy_.requiredName;
	if (!checker_.existsDir(target)) {
		return Result::failure("必須ディレクトリ '" + policy_.requiredName + "' が存在しません");
	}
	return Result::success("必須ディレクトリ '" + policy_.requiredName + "' が存在します");
}

} // namespace foxclip::domain
