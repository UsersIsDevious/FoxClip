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
#define OBS_LOG_INFO(format, ...)  obs_log(LOG_INFO, format, ##__VA_ARGS__)
#define OBS_LOG_WARN(format, ...)  obs_log(LOG_WARNING, format, ##__VA_ARGS__)
#define OBS_LOG_ERROR(format, ...) obs_log(LOG_ERROR, format, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif
