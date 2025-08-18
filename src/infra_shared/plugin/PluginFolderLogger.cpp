#include "infra_shared/plugin/PluginFolderLogger.h"

#include <filesystem>

#include "infra_shared/log/ObsLogger.h"
#include "infra_shared/fs/DirectoryLister.h"
#include "infra_shared/fs/roots/ObsConfigRootProvider.h"
#include "infra_shared/fs/PathResolver.h"

namespace foxclip::infra_shared::plugin {

void logPluginSubfolders(const std::string &pluginDirName)
{
	using foxclip::infra_shared::fs::roots::ObsConfigRootProvider;
	using foxclip::infra_shared::fs::PathResolver;
	using foxclip::infra_shared::fs::DirectoryLister;

	ObsConfigRootProvider root;
	PathResolver resolver(root);
	const auto fullOpt = resolver.toFull(pluginDirName);

	if (!fullOpt || fullOpt->empty()) {
		OBS_LOG_WARN("[foxclip] failed to resolve plugin root for '%s'", pluginDirName.c_str());
		return;
	}

	const std::string pluginRoot = *fullOpt;
	const std::filesystem::path rootPath{pluginRoot};
	const auto subs = DirectoryLister::listSubdirectories(rootPath);

	OBS_LOG_INFO("[foxclip] Plugin folder listing: %s", pluginRoot.c_str());
	if (subs.empty()) {
		OBS_LOG_INFO("[foxclip] (no subfolders found)");
		return;
	}
	for (const auto &p : subs) {
		OBS_LOG_INFO(" - %s", p.filename().string().c_str());
	}
}

} // namespace foxclip::infra_shared::plugin
