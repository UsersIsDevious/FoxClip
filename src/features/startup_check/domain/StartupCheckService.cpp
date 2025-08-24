#include <filesystem>
#include "StartupCheckService.h"
#include "infra_shared/log/ObsLogger.h"

namespace foxclip::startup_check::domain {

StartupCheckService::StartupCheckService(const IDirectoryChecker &checker, DirectoryPolicy policy, std::string basePath,
					 DirectoryCreator &creator)
	: checker(checker),
	  policy(std::move(policy)),
	  basePath(std::move(basePath)),
	  creator(creator)
{
}

Result StartupCheckService::run()
{
	// basePath_ が空なら requiredName を「そのままフルパス」として扱う
	std::filesystem::path targetPath = basePath.empty() ? std::filesystem::path(policy.requiredName)
							    : (std::filesystem::path(basePath) / policy.requiredName);
	std::string target = targetPath.string();

	OBS_LOG_INFO("[StartupCheckService] Checking directory - basePath: '%s', requiredName: '%s', target: '%s'",
		     basePath.c_str(), policy.requiredName.c_str(), target.c_str());

	if (!&checker) {
		OBS_LOG_ERROR("[StartupCheckService] IDirectoryChecker is null");
		return Result::failure("IDirectoryChecker is null");
	}

	OBS_LOG_INFO("[StartupCheckService] Checking if directory exists: '%s'", target.c_str());
	if (!checker.existsDir(target)) {
		OBS_LOG_INFO("[StartupCheckService] Directory doesn't exist, attempting to create: '%s'",
			     target.c_str());
		// 無ければ作成を試みる
		std::error_code ec;
		const bool ok = creator.createIfMissing(target, ec);
		if (!ok) {
			OBS_LOG_ERROR("[StartupCheckService] Directory creation failed for '%s'", target.c_str());
			if (ec) {
				return Result::failure("必須ディレクトリ '" + policy.requiredName +
						       "' の作成に失敗: " + ec.message());
			}
			return Result::failure("必須ディレクトリ '" + policy.requiredName + "' の作成に失敗");
		}

		OBS_LOG_INFO("[StartupCheckService] Directory creation succeeded, verifying: '%s'", target.c_str());
		// 念のため再チェック
		if (!checker.existsDir(target)) {
			OBS_LOG_ERROR("[StartupCheckService] Directory verification failed after creation: '%s'",
				      target.c_str());
			return Result::failure("必須ディレクトリ '" + policy.requiredName +
					       "' を作成後に確認できませんでした");
		}
		OBS_LOG_INFO("[StartupCheckService] Directory successfully created and verified: '%s'", target.c_str());
		return Result::success("必須ディレクトリ '" + policy.requiredName + "' を新規作成しました");
	}
	OBS_LOG_INFO("[StartupCheckService] Directory already exists: '%s'", target.c_str());
	return Result::success("必須ディレクトリ '" + policy.requiredName + "' が存在します");
}

} // namespace foxclip::startup_check::domain
