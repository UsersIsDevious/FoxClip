#pragma once
#include <stdarg.h>
#include "foxclip/core/export.h"
#include "level.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(FC_USE_VTABLE) /* ← プラグイン側だけで有効になる */
#include "foxclip/plugin_api.h"

/* g_fc は plugin_api.h で extern 宣言。プラグイン側で定義してください。 */
#define fc_log_set_min_level(...) do { if (g_fc && g_fc->log_set_min_level) g_fc->log_set_min_level(__VA_ARGS__); } while(0)
#define fc_log(...)               do { if (g_fc && g_fc->log)               g_fc->log(__VA_ARGS__); } while(0)
#define fc_log_v(...)             do { if (g_fc && g_fc->log_v)             g_fc->log_v(__VA_ARGS__); } while(0)

#else
/* ← 従来どおりライブラリ直リンク時の宣言 */
/* 最小ログレベル設定 */
FC_EXTERN FC_API void FC_CALL fc_log_set_min_level(fc_log_level_t level);

/* printf 互換 */
FC_EXTERN FC_API void FC_CALL fc_log(fc_log_level_t level, const char *tag, const char *fmt, ...);

/* va_list 版 */
FC_EXTERN FC_API void FC_CALL fc_log_v(fc_log_level_t level, const char *tag, const char *fmt, va_list args);
#endif

#ifdef __cplusplus
}
#endif
