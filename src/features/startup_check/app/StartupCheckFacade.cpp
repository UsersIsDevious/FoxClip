#include "StartupCheckFacade.h"
#include "infra_shared/log/ObsLogger.h"
#include "infra_shared/startup/EnsurePluginDir.h"
#include <memory>

namespace foxclip::startup_check::app {

StartupCheckFacade::StartupCheckFacade(std::unique_ptr<domain::IDirectoryChecker> checker, std::string requiredName)
	: checker_(std::move(checker)),
	  creator_()
{
	// 起動用ユーティリティ（OBSの書き込み可能パスを解決 → 必要なら作成）
	using foxclip::infra_shared::startup::ensure_obs_writable_dir;
	const auto er = ensure_obs_writable_dir(requiredName);

	if (er.ok && !er.fullPath.empty()) {
		// StartupCheckService へは「requiredName=フルパス」「basePath=""」で渡す
		service_ = std::make_unique<domain::StartupCheckService>(
			*checker_, domain::DirectoryPolicy{er.fullPath}, "", creator_);
		OBS_LOG_INFO("[foxclip] using OBS config dir: %s", er.fullPath.c_str());
	} else {
		// フォールバック（相対パスで '.' / 実際の作成は service_.run() に委ねる）
		service_ = std::make_unique<domain::StartupCheckService>(
			*checker_, domain::DirectoryPolicy{requiredName}, ".", creator_);
		OBS_LOG_WARN("[foxclip] %s; fallback to ./'%s'",
			     er.errorMessage.empty() ? "failed to ensure obs dir" : er.errorMessage.c_str(),
			     requiredName.c_str());
	}
}

foxclip::startup_check::domain::Result StartupCheckFacade::run()
{
	auto res = service_->run();
	if (res.ok)
		OBS_LOG_INFO("%s", res.message.c_str());
	else
		OBS_LOG_ERROR("%s", res.message.c_str());
	return res;
}

} // namespace foxclip::startup_check::app