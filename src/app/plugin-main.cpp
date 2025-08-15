#include <QAction>
#include <QObject>
#include <obs-module.h>
#include <obs-frontend-api.h>

#include "infra_shared/log/ObsLogger.h"
#include "infra_shared/support/plugin-support.h"
#include "features/startup_check/app/StartupCheckFacade.h"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

bool obs_module_load(void)
{
	// loding message
	OBS_LOG_INFO("plugin loaded successfully (version %s)", PLUGIN_VERSION);

	//checking plugins directory
	using foxclip::features::startup_check::app::StartupCheckFacade;
	StartupCheckFacade facade(".", "foxclip-plugins");
	auto r = facade.run();

	// Set Custom QAction in OBS Tools menu
	QAction *act = (QAction *)obs_frontend_add_tools_menu_qaction("FoxClip Mmenu");
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
	obs_log(LOG_INFO, "plugin unloaded");
}
