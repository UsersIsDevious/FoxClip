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

	/* 1) 本文長を見積もる（args は消費しないよう必ずコピーを使う） */
	va_list args_copy;
	va_copy(args_copy, args);

#if defined(_MSC_VER)
	/* _vscprintf は終端NULを含まない必要サイズを返す（負ならエラー） */
	int body_len = _vscprintf(fmt, args_copy);
#else
	int body_len = vsnprintf(NULL, 0, fmt, args_copy);
#endif
	va_end(args_copy);

	if (body_len < 0) {
		return; /* フォーマットエラー */
	}

	/* 2) プレフィックス("[tag] ")長を見積もる（tag が NULL/空なら0） */
	const char *tag_prefix = (tag && tag[0]) ? tag : NULL;
	int prefix_len = 0;
	if (tag_prefix) {
		/* ここで "] " を含む完全な形を一発で作るため、余計な ']' は入れない */
		int n = snprintf(NULL, 0, "[%s] ", tag_prefix);
		if (n > 0)
			prefix_len = n;
		else
			prefix_len = 0; /* 念のため */
	}

	/* 3) 合計サイズを計算して1回の malloc で確保（+1はNUL） */
	size_t total_len = (size_t)prefix_len + (size_t)body_len;
	char *line = (char *)malloc(total_len + 1);
	if (!line) {
		return;
	}

	/* 4) 実際に書き込む：先にプレフィックス、続いて本文 */
	int written = 0;
	if (tag_prefix) {
		/* prefix_len は NUL を含まない長さなので、size は +1 でOK */
		written = snprintf(line, (size_t)prefix_len + 1, "[%s] ", tag_prefix);
		if (written < 0) {
			/* 万一失敗したらプレフィックス無しで本文だけ出す */
			written = 0;
		}
	}

	/* 本文は vsnprintf で後ろに連結。上では args_copy を消費しただけなので args は未消費 */
	/* body_len は NUL を含まない長さなので、書き込みバッファは +1 */
	(void)vsnprintf(line + written, (size_t)body_len + 1, fmt, args);

	/* 5) 出力（整形済みを "%s" で渡す） */
	obs_log(level_to_obs(level), "%s", line);

	free(line);
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
