
#include "infra_shared/plugin/HostApiV1.h"
#include "foxclip/log/log.h" // foxclipLogSetMinLevel, foxclipLog, foxclipLogVa

namespace foxclip::infra_shared::plugin {

static void logSetMinLevelBridge(FoxclipLogLevel lvl)
{
	foxclipLogSetMinLevel(lvl);
}
static void logBridge(FoxclipLogLevel lvl, const char *tag, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	foxclipLogVa(lvl, tag, fmt, ap);
	va_end(ap);
}
static void logVaBridge(FoxclipLogLevel lvl, const char *tag, const char *fmt, va_list ap)
{
	foxclipLogVa(lvl, tag, fmt, ap);
}

// TODO: registerEventHandler / unregisterEventHandler は将来のイベント基盤に接続
static int registerEventHandler(const char *, FoxclipEventCallback, void *)
{
	return 0;
}
static int unregisterEventHandler(const char *, FoxclipEventCallback, void *)
{
	return 0;
}

const FoxclipApi *getHostApiV1()
{
	static FoxclipApi api = {/*abi*/ FOXCLIP_API_ABI,
				 /*logSetMinLevel*/ &logSetMinLevelBridge,
				 /*log*/ &logBridge,
				 /*logVa*/ &logVaBridge,
				 /*registerEventHandler*/ &registerEventHandler,
				 /*unregisterEventHandler*/ &unregisterEventHandler};
	return &api;
}

} // namespace foxclip::infra_shared::plugin
