#pragma once
#include <stdarg.h>
#include <stdbool.h>
#include "foxclip/core/export.h"
#include "foxclip/log/level.h"

/* ABI */
#define FC_API_ABI 1

typedef void(FC_CALL *fc_event_callback_t)(const char *event, const char *payload, void *user);

typedef struct fc_api_v1 {
	int abi; /* == FC_API_ABI */
	void(FC_CALL *log_set_min_level)(fc_log_level_t);
	void(FC_CALL *log)(fc_log_level_t, const char *tag, const char *fmt, ...);
	void(FC_CALL *log_v)(fc_log_level_t, const char *tag, const char *fmt, va_list);
	int(FC_CALL *register_event_handler)(const char *event, fc_event_callback_t cb, void *user);
	int(FC_CALL *unregister_event_handler)(const char *event, fc_event_callback_t cb, void *user);
} fc_api_v1;

typedef fc_api_v1 fc_api_t;

/* プラグインがエクスポートすべき初期化/終了関数 */
typedef bool(FC_CALL *foxclip_plugin_init_fn)(const fc_api_t *api);
typedef void(FC_CALL *foxclip_plugin_deinit_fn)(void);

/* vtable を使うプラグイン側の共有ポインタ（定義はプラグイン側） */
#if defined(FC_USE_VTABLE)
FC_EXTERN const fc_api_t *g_fc;
#endif
