#include "foxclip/log/log.h" // ← "include/..." ではなくこちら
#include "infra_shared/log/ObsLogger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 既定は INFO 以上 */
static fc_log_level_t g_min_level = FC_LOG_INFO;

void FC_CALL fc_log_set_min_level(fc_log_level_t level)
{
	g_min_level = level;
}

/* fc_log_level_t -> ObsLogger の level へ変換 */
static inline int level_to_obs(fc_log_level_t lv)
{
#if defined(LOG_DEBUG)
	switch (lv) {
	case FC_LOG_TRACE:
	case FC_LOG_DEBUG:
		return LOG_DEBUG;
	case FC_LOG_INFO:
		return LOG_INFO;
	case FC_LOG_WARN:
		return LOG_WARNING;
	case FC_LOG_ERROR:
		return LOG_ERROR;
	default:
		return LOG_INFO;
	}
#else
	/* LOG_DEBUG が無い実装向けのフォールバック */
	switch (lv) {
	case FC_LOG_WARN:
		return LOG_WARNING;
	case FC_LOG_ERROR:
		return LOG_ERROR;
	default:
		return LOG_INFO;
	}
#endif
}

/* 可変長引数版（va_list） */
void FC_CALL fc_log_v(fc_log_level_t level, const char *tag, const char *fmt, va_list args)
{
	if (level < g_min_level)
		return;

	/* 1) 本文を vsnprintf で整形（サイズ見積 → 確保 → 成形） */
	va_list args_copy;
	va_copy(args_copy, args);

#if defined(_MSC_VER)
	/* MSVC: _vscprintf は終端NULを含まない必要サイズを返す */
	int body_len = _vscprintf(fmt, args_copy);
	if (body_len < 0) {
		va_end(args_copy);
		return;
	}
	size_t body_size = (size_t)body_len + 1;
#else
	int body_len = vsnprintf(NULL, 0, fmt, args_copy);
	if (body_len < 0) {
		va_end(args_copy);
		return;
	}
	size_t body_size = (size_t)body_len + 1;
#endif

	va_end(args_copy);

	char *body = (char *)malloc(body_size);
	if (!body)
		return;

	vsnprintf(body, body_size, fmt, args);

	/* 2) 先頭に "[tag] " を付与（tag が NULL/空ならそのまま） */
	const char *tag_prefix = (tag && tag[0]) ? tag : NULL;
	const char *sep = (tag_prefix ? "] " : "");
	size_t prefix_len = tag_prefix ? (2 /* '[' + ']' */ + strlen(tag_prefix) + strlen(sep)) : 0;

	/* 例: "[my-plugin] " + 本文 + NUL */
	size_t total_size = prefix_len + strlen(body) + 1;
	char *line = (char *)malloc(total_size);
	if (!line) {
		free(body);
		return;
	}

	if (tag_prefix) {
		/* "[tag] " を作る */
		int n = snprintf(line, total_size, "[%s]%s%s", tag_prefix, sep, body);
		(void)n;
	} else {
		/* タグ無し */
		memcpy(line, body, total_size);
	}

	/* 3) 出力（obs_log は可変長…なので整形済み文字列を "%s" で渡す） */
	obs_log(level_to_obs(level), "%s", line);

	free(line);
	free(body);
}

/* printf 互換版 */
void FC_CALL fc_log(fc_log_level_t level, const char *tag, const char *fmt, ...)
{
	if (level < g_min_level)
		return;

	va_list args;
	va_start(args, fmt);
	fc_log_v(level, tag, fmt, args);
	va_end(args);
}
