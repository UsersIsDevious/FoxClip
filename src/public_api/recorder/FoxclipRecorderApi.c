#include "foxclip/recording/start.h"
#include "foxclip/plugin_api.h"
#include <string.h>

int FOXCLIP_CALL foxclipRecordStart(const FoxclipRecorderStartOptions *opts)
{
#if defined(FOXCLIP_USE_VTABLE)
	if (!foxclipApi)
		return FOXCLIP_RECORDER_E_NOT_SUPPORTED;
	if (!foxclipApi->recordStart)
		return FOXCLIP_RECORDER_E_NOT_SUPPORTED;

	FoxclipRecorderStartOptions local = FOXCLIP_RECORDER_START_OPTIONS_INIT;
	if (opts) {
		/* size ゲーティング: 受け取れた範囲だけコピー */
		size_t n = opts->size;
		if (n > sizeof(FoxclipRecorderStartOptions))
			n = sizeof(FoxclipRecorderStartOptions);
		/* 安全な逐次コピー: size に基づいて必要な分だけコピー */
		memcpy(&local, opts, n);
	}
	return foxclipApi->recordStart(&local);
#else
	/* vtable を使わないビルド構成では未サポート */
	(void)opts;
	return FOXCLIP_RECORDER_E_NOT_SUPPORTED;
#endif
}
