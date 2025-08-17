#include <filesystem>
#include "StartupCheckService.h"

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
	if (!&checker) {
		return Result::failure("IDirectoryChecker is null");
	}
	if (!checker.existsDir(target)) {
		// 無ければ作成を試みる
		std::error_code ec;
		const bool ok = creator.createIfMissing(target, ec);
		if (!ok) {
			if (ec) {
				return Result::failure("必須ディレクトリ '" + policy.requiredName +
						       "' の作成に失敗: " + ec.message());
			}
			return Result::failure("必須ディレクトリ '" + policy.requiredName + "' の作成に失敗");
		}
		// 念のため再チェック
		if (!checker.existsDir(target)) {
			return Result::failure("必須ディレクトリ '" + policy.requiredName +
					       "' を作成後に確認できませんでした");
		}
		return Result::success("必須ディレクトリ '" + policy.requiredName + "' を新規作成しました");
	}
	return Result::success("必須ディレクトリ '" + policy.requiredName + "' が存在します");
}

} // namespace foxclip::startup_check::domain
