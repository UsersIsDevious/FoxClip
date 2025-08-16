#pragma once
#include <stdarg.h>
#include "foxclip/core/export.h"
#include "level.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 最小ログレベル設定 */
FC_EXTERN FC_API void FC_CALL fc_log_set_min_level(fc_log_level_t level);

/* printf 互換 */
FC_EXTERN FC_API void FC_CALL fc_log(fc_log_level_t level, const char *tag, const char *fmt, ...);

/* va_list 版 */
FC_EXTERN FC_API void FC_CALL fc_log_v(fc_log_level_t level, const char *tag, const char *fmt, va_list args);

#ifdef __cplusplus
}
#endif
