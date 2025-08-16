#include <filesystem>
#include "StartupCheckService.h"

namespace foxclip::startup_check::domain {

StartupCheckService::StartupCheckService(const IDirectoryChecker &checker, DirectoryPolicy policy, std::string basePath,
					 DirectoryCreator &creator)
	: checker_(checker),
	  policy_(std::move(policy)),
	  basePath_(std::move(basePath)),
	  creator_(creator)
{
}

Result StartupCheckService::run()
{
	// basePath_ が空なら requiredName を「そのままフルパス」として扱う
	std::filesystem::path targetPath = basePath_.empty()
						   ? std::filesystem::path(policy_.requiredName)
						   : (std::filesystem::path(basePath_) / policy_.requiredName);
	std::string target = targetPath.string();
	if (!checker_.existsDir(target)) {
		// 無ければ作成を試みる
		std::error_code ec;
		const bool ok = creator_.create_if_missing(target, ec);
		if (!ok) {
			if (ec) {
				return Result::failure("必須ディレクトリ '" + policy_.requiredName +
						       "' の作成に失敗: " + ec.message());
			}
			return Result::failure("必須ディレクトリ '" + policy_.requiredName + "' の作成に失敗");
		}
		// 念のため再チェック
		if (!checker_.existsDir(target)) {
			return Result::failure("必須ディレクトリ '" + policy_.requiredName +
					       "' を作成後に確認できませんでした");
		}
		return Result::success("必須ディレクトリ '" + policy_.requiredName + "' を新規作成しました");
	}
	return Result::success("必須ディレクトリ '" + policy_.requiredName + "' が存在します");
}

} // namespace foxclip::startup_check::domain
