#pragma once
#include <stdarg.h>
#include <stdbool.h>
#include "foxclip/core/export.h"
#include "foxclip/log/level.h"

/* ABI 定義 */
#define FOXCLIP_API_ABI 1

// ログ出力
typedef void(FOXCLIP_CALL *FoxclipEventCallback)(const char *event, const char *payload, void *user);
// 録画開始
typedef int(FOXCLIP_CALL *FoxclipRecordStartFn)(const struct FoxclipRecorderStartOptions *opts);

typedef struct FoxclipApiV1 {
	int abi; /* == FOXCLIP_API_ABI */
	void(FOXCLIP_CALL *logSetMinLevel)(FoxclipLogLevel);
	void(FOXCLIP_CALL *log)(FoxclipLogLevel, const char *tag, const char *fmt, ...);
	void(FOXCLIP_CALL *logVa)(FoxclipLogLevel, const char *tag, const char *fmt, va_list);
	int(FOXCLIP_CALL *registerEventHandler)(const char *event, FoxclipEventCallback cb, void *user);
	int(FOXCLIP_CALL *unregisterEventHandler)(const char *event, FoxclipEventCallback cb, void *user);

	FoxclipRecordStartFn recordStart; /* 録画開始関数 */
} FoxclipApiV1;

typedef FoxclipApiV1 FoxclipApi;

/* プラグインがエクスポートすべき初期化/終了関数 */
typedef bool(FOXCLIP_CALL *FoxclipPluginInitFn)(const FoxclipApi *api);
typedef void(FOXCLIP_CALL *FoxclipPluginDeinitFn)(void);

/* vtable を使うプラグイン側の共有ポインタ（定義はプラグイン側） */
#if defined(FOXCLIP_USE_VTABLE)
FOXCLIP_EXTERN const FoxclipApi *foxclipApi;
#endif
