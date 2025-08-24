#pragma once
#ifdef __cplusplus
extern "C" {
#endif

typedef enum FoxclipRecorderError {
	FOXCLIP_RECORDER_OK = 0,
	FOXCLIP_RECORDER_E_NOT_SUPPORTED = -1, /* ホストが本機能を未実装 */
	FOXCLIP_RECORDER_E_INVALID_ARG = -2,
	FOXCLIP_RECORDER_E_BUSY = -3,          /* 競合(既に開始中/ロック) */
	FOXCLIP_RECORDER_E_BACKEND_ERROR = -4, /* OBS側エラー */
	FOXCLIP_RECORDER_E_TIMEOUT = -5,       /* waitUntilStartedMs でタイムアウト */
	FOXCLIP_RECORDER_E_THREADING = -6,     /* スレッド/ディスパッチの問題 */
} FoxclipRecorderError;

#ifdef __cplusplus
} /* extern "C" */
#endif
