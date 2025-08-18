#include <QAction>
#include <QObject>
#include <obs-module.h>
#include <obs-frontend-api.h>

#include "features/startup_check/app/StartupCheckFacade.h"
#include "infra_shared/fs/DirectoryLister.h"
#include "infra_shared/fs/roots/ObsConfigRootProvider.h"
#include "infra_shared/fs/PathResolver.h"
#include "infra_shared/log/ObsLogger.h"
#include "infra_shared/config/build/plugin-config.h"
#include "infra_shared/plugin/FoxclipPluginHost.h"
#include "infra_shared/plugin/PluginFolderLogger.h"
#include "features/plugin_loader/app/LoaderFacadeMulti.h"
#include "features/plugin_loader/domain/LoadTypes.h"
#include "infra_shared/plugin/MultiPluginHost.h"

static std::string resolvePluginsRootUtf8(const std::string &pluginDirName)
{
	using foxclip::infra_shared::fs::roots::ObsConfigRootProvider;
	using foxclip::infra_shared::fs::PathResolver;

	ObsConfigRootProvider root;
	PathResolver resolver(root);
	auto full = resolver.toFull(pluginDirName);
	return full.value_or(std::string{});
}

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

bool obs_module_load(void)
{
	// loding message
	OBS_LOG_INFO("plugin loaded successfully (version %s)", PLUGIN_VERSION);

	//checking plugins directory
	using foxclip::startup_check::app::StartupCheckFacade;
	using foxclip::startup_check::infrastructure::StdFsDirectoryChecker;

	const std::string pluginDirName = "foxclip-plugins";

	auto checker = std::make_unique<StdFsDirectoryChecker>();
	StartupCheckFacade facade(std::move(checker), pluginDirName);
	auto r = facade.run();
	if (!r.ok) {
		// Log error Directory does not exist
		OBS_LOG_ERROR("Startup check failed: %s", r.message.c_str());
		// but for now, we just log the error and continue
		// TODO: In the future, add functionality to automatically create the directory if it does not exist
		//return false;
	}

	foxclip::infra_shared::plugin::logPluginSubfolders(pluginDirName);

	{
		using foxclip::plugin_loader::app::LoaderFacadeMulti;

		const std::string pluginsRoot = resolvePluginsRootUtf8(pluginDirName);
		if (!pluginsRoot.empty()) {
			LoaderFacadeMulti m;
			auto res = m.loadAll(pluginsRoot);
			OBS_LOG_INFO("[foxclip] multi-load: %zu success, %zu errors", res.successes.size(),
				     res.errors.size());
		}
	}

	// Tools メニューにカスタム QAction を追加
	const char *label = obs_module_text("Tools.Menu.FoxClip");
	void *raw = obs_frontend_add_tools_menu_qaction(label);

	// まず QObject* に静的キャストし、その後 qobject_cast で安全に QAction* へ
	QObject *obj = static_cast<QObject *>(raw);
	QAction *act = qobject_cast<QAction *>(obj);

	if (!act) {
		OBS_LOG_ERROR("Failed to create Tools menu QAction");
		return true;
	}

	QObject::connect(act, &QAction::triggered,
			 []() { OBS_LOG_INFO("QAction triggered (version %s)", PLUGIN_VERSION); });

	//foxclip::Host::Load("../FoxClip-sample-plugin/build/Release/foxclip-plugin.dll");

	return true;
}

void obs_module_unload(void)
{
	// Unload all plugins
	foxclip::infra_shared::plugin::MultiPluginHost::instance().stopAndUnloadAll();
	OBS_LOG_INFO("plugin unloaded");
}
