#pragma once
#include <stdarg.h>
#include "foxclip/core/export.h"
#include "foxclip/log/level.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(FOXCLIP_USE_VTABLE)
/* vtable 経由でコール（プラグイン側のみ） */
#include "foxclip/plugin_api.h"

/* foxclipApi は plugin_api.h で extern 宣言。プラグイン側で定義してください。 */
#define foxclipLogSetMinLevel(...) \
  do { if (foxclipApi && foxclipApi->logSetMinLevel) foxclipApi->logSetMinLevel(__VA_ARGS__); } while(0)
#define foxclipLog(...) \
  do { if (foxclipApi && foxclipApi->log) foxclipApi->log(__VA_ARGS__); } while(0)
#define foxclipLogVa(...) \
  do { if (foxclipApi && foxclipApi->logVa) foxclipApi->logVa(__VA_ARGS__); } while(0)

#else
/* ライブラリ直リンク時の宣言 */
FOXCLIP_EXTERN FOXCLIP_API void FOXCLIP_CALL foxclipLogSetMinLevel(FoxclipLogLevel level);
FOXCLIP_EXTERN FOXCLIP_API void FOXCLIP_CALL foxclipLog(FoxclipLogLevel level, const char *tag, const char *fmt, ...);
FOXCLIP_EXTERN FOXCLIP_API void FOXCLIP_CALL foxclipLogVa(FoxclipLogLevel level, const char *tag, const char *fmt,
							  va_list args);
#endif

#ifdef __cplusplus
}
#endif
