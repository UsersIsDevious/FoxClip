#include "ObsLogger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 可搬で安全な実装：必要なバッファ長を先に計算
void obs_log(int log_level, const char *format, ...)
{
	// PLUGIN_NAME が未設定/空のときのフォールバック
	const char *name = (PLUGIN_NAME && *PLUGIN_NAME) ? PLUGIN_NAME : "plugin";

	// "[<name>] " の長さを計算（終端NULはsnprintfが付けるので +1 は後でまとめる）
	int prefix_len = snprintf(NULL, 0, "[%s] ", name);
	if (prefix_len < 0)
		return;

	// format の長さ
	size_t format_len = strlen(format);

	// 合計 = prefix + format + 終端NUL
	size_t total_len = (size_t)prefix_len + format_len + 1;

	char *fmtbuf = (char *)malloc(total_len);
	if (!fmtbuf)
		return;

	// ここで "[name] <format>" の「フォーマット文字列」を組み立てる
	// 例: "[MyPlugin] %s: %d"
	// 注意：第2引数はバッファサイズ（NUL含む）
	snprintf(fmtbuf, total_len, "[%s] %s", name, format);

	va_list args;
	va_start(args, format);
	blogva(log_level, fmtbuf, args); // OBSの可変引数版ロガー
	va_end(args);

	free(fmtbuf);
}
