/*
My Plugin - logging wrapper for OBS blog()/blogva()
GPL-2.0-or-later
*/
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>

#include <obs-module.h>

#include "infra_shared/config/build/plugin-config.h"

// printf互換（先頭に "[PLUGIN_NAME] " を付与して OBS へ出力）
void obs_log(int log_level, const char *format, ...);

// 便利マクロ
#define OBS_LOG_INFO(...)  obs_log(LOG_INFO, __VA_ARGS__)
#define OBS_LOG_WARN(...)  obs_log(LOG_WARNING, __VA_ARGS__)
#define OBS_LOG_ERROR(...) obs_log(LOG_ERROR, __VA_ARGS__)

#ifdef __cplusplus
}
#endif
