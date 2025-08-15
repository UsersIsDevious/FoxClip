#include "StartupCheckFacade.h"
#include "infra_shared/log/ObsLogger.h"

namespace foxclip::features::startup_check::app {

StartupCheckFacade::StartupCheckFacade(std::string basePath, std::string requiredName)
	: checker_(),
	  service_(checker_, {std::move(requiredName)}, std::move(basePath))
{
}

foxclip::domain::Result StartupCheckFacade::run()
{
	auto res = service_.run();
	if (res.ok)
		OBS_LOG_INFO("%s", res.message.c_str());
	else
		OBS_LOG_ERROR("%s", res.message.c_str());
	return res;
}

} // namespace foxclip::features::startup_check::app