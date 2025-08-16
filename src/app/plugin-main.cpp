#include <QAction>
#include <QObject>
#include <obs-module.h>
#include <obs-frontend-api.h>

#include "infra_shared/log/ObsLogger.h"
#include "infra_shared/config/build/plugin-config.h"
#include "features/startup_check/app/StartupCheckFacade.h"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

bool obs_module_load(void)
{
	// loding message
	OBS_LOG_INFO("plugin loaded successfully (version %s)", PLUGIN_VERSION);

	//checking plugins directory
	using foxclip::features::startup_check::app::StartupCheckFacade;
	using foxclip::features::startup_check::infrastructure::StdFsDirectoryChecker;

	auto checker = std::make_unique<StdFsDirectoryChecker>();
	StartupCheckFacade facade(std::move(checker), ".", "foxclip-plugins");
	auto r = facade.run();
	if (!r.ok) {
		// Log error Directory does not exist
		OBS_LOG_ERROR("Startup check failed: %s", r.message.c_str());
		// but for now, we just log the error and continue
		// TODO: In the future, add functionality to automatically create the directory if it does not exist
		//return false;
	}

	// Set Custom QAction in OBS Tools menu
	QAction *act = static_cast<QAction *>(obs_frontend_add_tools_menu_qaction("FoxClip Mmenu"));
	if (!act) {
		OBS_LOG_ERROR("Failed to create Tools menu QAction");
		return true;
	}
	QObject::connect(act, &QAction::triggered,
			 []() { OBS_LOG_INFO("QAction triggered (version %s)", PLUGIN_VERSION); });

	return true;
}

void obs_module_unload(void)
{
	OBS_LOG_INFO("plugin unloaded");
}
