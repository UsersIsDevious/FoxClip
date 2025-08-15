/*
My Plugin - logging wrapper for OBS blog()/blogva()
GPL-2.0-or-later
*/
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>

// OBSのログレベルやblog()/blogva()宣言を含む
// （plugin-support.hでも可だが、通常はobs-module.hを直接使う）
#include <obs-module.h>

// CMakeから埋め込む場合はconfigure_fileで生成したヘッダをincludeしてもOK。
// ここでは外部で定義されたシンボルを参照する前提にします。
extern const char *PLUGIN_NAME;    // 例: "@CMAKE_PROJECT_NAME@"
extern const char *PLUGIN_VERSION; // 例: "@CMAKE_PROJECT_VERSION@"

// printf互換（先頭に "[PLUGIN_NAME] " を付与して OBS へ出力）
void obs_log(int log_level, const char *format, ...);

// 便利マクロ
#define OBS_LOG_INFO(...)  obs_log(LOG_INFO, __VA_ARGS__)
#define OBS_LOG_WARN(...)  obs_log(LOG_WARNING, __VA_ARGS__)
#define OBS_LOG_ERROR(...) obs_log(LOG_ERROR, __VA_ARGS__)

#ifdef __cplusplus
}
#endif
