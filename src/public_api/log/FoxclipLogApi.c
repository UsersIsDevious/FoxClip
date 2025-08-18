#include "foxclip/log/log.h"
#include "infra_shared/log/ObsLogger.h" /* 既存の依存を流用 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/* 既定は INFO 以上 */
static FoxclipLogLevel gMinLevel = kLogInfo;

/* レベル変換（ObsLogger 側の LOG_* に合わせる） */
static inline int levelToObs(FoxclipLogLevel lv)
{
#if defined(LOG_DEBUG)
	switch (lv) {
	case kLogTrace:
	case kLogDebug:
		return LOG_DEBUG;
	case kLogInfo:
		return LOG_INFO;
	case kLogWarn:
		return LOG_WARNING;
	case kLogError:
		return LOG_ERROR;
	default:
		return LOG_INFO;
	}
#else
	switch (lv) {
	case kLogWarn:
		return LOG_WARNING;
	case kLogError:
		return LOG_ERROR;
	default:
		return LOG_INFO;
	}
#endif
}

void FOXCLIP_CALL foxclipLogSetMinLevel(FoxclipLogLevel level)
{
	gMinLevel = level;
}

/* va_list 版 */
void FOXCLIP_CALL foxclipLogVa(FoxclipLogLevel level, const char *tag, const char *fmt, va_list args)
{
	if (level < gMinLevel)
		return;

	/* 1) 本文長を見積もる（args を消費しないようコピー） */
	va_list argsCopy;
	va_copy(argsCopy, args);

#if defined(_MSC_VER)
	int bodyLen = _vscprintf(fmt, argsCopy); /* NUL 含まず */
#else
	int bodyLen = vsnprintf(NULL, 0, fmt, argsCopy);
#endif
	va_end(argsCopy);

	if (bodyLen < 0)
		return; /* フォーマットエラー */

	/* 2) "[tag] " の長さ（tag が NULL/空なら 0） */
	const char *tagPrefix = (tag && tag[0]) ? tag : NULL;
	int prefixLen = 0;
	if (tagPrefix) {
		int n = snprintf(NULL, 0, "[%s] ", tagPrefix);
		prefixLen = (n > 0) ? n : 0;
	}

	/* 3) 一括確保（+1 は終端 NUL） */
	size_t totalLen = (size_t)prefixLen + (size_t)bodyLen;
	char *line = (char *)malloc(totalLen + 1);
	if (!line)
		return;

	/* 4) 出力文字列を組み立てる */
	int written = 0;
	if (tagPrefix) {
		written = snprintf(line, (size_t)prefixLen + 1, "[%s] ", tagPrefix);
		if (written < 0)
			written = 0;
	}
	(void)vsnprintf(line + written, (size_t)bodyLen + 1, fmt, args);

	/* 5) 出力 */
	obs_log(levelToObs(level), "%s", line);

	free(line);
}

/* printf 互換 */
void FOXCLIP_CALL foxclipLog(FoxclipLogLevel level, const char *tag, const char *fmt, ...)
{
	if (level < gMinLevel)
		return;

	va_list args;
	va_start(args, fmt);
	foxclipLogVa(level, tag, fmt, args);
	va_end(args);
}
