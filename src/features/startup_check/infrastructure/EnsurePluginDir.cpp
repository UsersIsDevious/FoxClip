#include "EnsurePluginDir.h"
#include "infra_shared/fs/roots/ObsConfigRootProvider.h"
#include "infra_shared/fs/app/FileService.h"
#include "infra_shared/fs/infrastructure/StandardFileSystem.h"
#include "infra_shared/log/app/LogService.h"
#include "infra_shared/log/ObsLogger.h"
#include "infra_shared/fs/PathResolver.h"
#include "infra_shared/fs/FsUtils.h"
#include <system_error>

namespace foxclip::features::startup_check::infrastructure {

EnsureResult ensureObsWritableDir(const std::string &subdir)
{
	using foxclip::infra_shared::fs::roots::ObsConfigRootProvider;
	using foxclip::infra_shared::fs::app::FileService;
	using foxclip::infra_shared::log::app::LogService;

	LogService::instance().info("[EnsurePluginDir] Starting directory resolution for subdir: '%s'", subdir.c_str());

	// 1) OBS ルート取得（…/plugin_config/<plugin>）
	ObsConfigRootProvider root;
	const std::string base = root.root();
	OBS_LOG_INFO("[EnsurePluginDir] OBS config root resolved to: '%s'", base.c_str());
	if (base.empty()) {
		OBS_LOG_ERROR("[EnsurePluginDir] Failed to resolve OBS config root");
		return {false, {}, "failed to resolve OBS config root"};
	}

	// 2) 相対 subdir をルート配下のフルパスへ正規化
	foxclip::infra_shared::fs::PathResolver resolver(root);
	const auto fullOpt = resolver.toFull(subdir);
	if (!fullOpt || fullOpt->empty()) {
		OBS_LOG_ERROR("[EnsurePluginDir] Path resolution failed for subdir: '%s'", subdir.c_str());
		return {false, {}, "invalid subdir (absolute or contains ..)"};
	}

	const std::string &full = *fullOpt;
	OBS_LOG_INFO("[EnsurePluginDir] Full path resolved to: '%s'", full.c_str());

	std::error_code ec;
	OBS_LOG_INFO("[EnsurePluginDir] Attempting to create directory: '%s'", full.c_str());
	if (!foxclip::infra_shared::fs::createDirs(full, ec)) {
		std::string msg = "failed to create '" + subdir + "'";
		if (ec) {
			msg += ": " + ec.message();
			OBS_LOG_ERROR("[EnsurePluginDir] Directory creation failed: %s", ec.message().c_str());
		}
		OBS_LOG_ERROR("[EnsurePluginDir] %s", msg.c_str());
		return {false, {}, msg};
	}

	OBS_LOG_INFO("[EnsurePluginDir] Directory creation successful: '%s'", full.c_str());
	return {true, full, {}};
}

} // namespace foxclip::features::startup_check::infrastructure
