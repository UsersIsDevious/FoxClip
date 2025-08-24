#include "StartupCheckFacade.h"
#include "StartupCheckService.h"
#include "infra_shared/log/ObsLogger.h"
#include "infra_shared/startup/EnsurePluginDir.h"
#include <memory>

namespace foxclip::startup_check::app {

StartupCheckFacade::StartupCheckFacade(std::unique_ptr<domain::IDirectoryChecker> checker, std::string requiredName)
	: dirChecker(std::move(checker)),
	  creator()
{
	// 起動用ユーティリティ（OBSの書き込み可能パスを解決 → 必要なら作成）
	using foxclip::infra_shared::startup::ensureObsWritableDir;
	const auto er = ensureObsWritableDir(requiredName);

	if (er.ok && !er.fullPath.empty()) {
		// StartupCheckService へは「requiredName=フルパス」「basePath=""」で渡す
		service = std::make_unique<StartupCheckService>(
			*dirChecker.get(), domain::DirectoryPolicy{er.fullPath}, "", creator);
		OBS_LOG_INFO("[foxclip] using OBS config dir: %s", er.fullPath.c_str());
	} else {
		// フォールバック（相対パスで '.' / 実際の作成は service.run() に委ねる）
		service = std::make_unique<StartupCheckService>(
			*dirChecker.get(), domain::DirectoryPolicy{requiredName}, ".", creator);

		const std::string errorMsg = er.errorMessage.empty() ? "failed to ensure obs dir" : er.errorMessage;

		OBS_LOG_WARN("[foxclip] %s; fallback to ./'%s'", errorMsg.c_str(), requiredName.c_str());
	}
}

foxclip::startup_check::domain::Result StartupCheckFacade::run()
{
	auto res = service->run();
	if (res.ok)
		OBS_LOG_INFO("%s", res.message.c_str());
	else
		OBS_LOG_ERROR("%s", res.message.c_str());
	return res;
}

} // namespace foxclip::startup_check::app