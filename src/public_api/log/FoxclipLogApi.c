#include "foxclip/log/log.h"
#include "infra_shared/log/ObsLogger.h"
#include <stdio.h>

static fc_log_level_t g_min_level = FC_LOG_INFO;

void fc_log_set_min_level(fc_log_level_t level) {
  g_min_level = level;
}

static inline int level_to_obs(fc_log_level_t lv) {
  // ObsLogger 側のレベルに合わせてマッピング（仮）
  // 例: 0:TRACE 1:DEBUG 2:INFO 3:WARN 4:ERROR
  return (int)lv;
}

void fc_log_v(fc_log_level_t level, const char* tag, const char* fmt, va_list args) {
  if (level < g_min_level) return;
  // ObsLogger にブリッジ（実体は既存の出力関数を想定）
  // 例: ObsLogger_vlog(int level, const char* tag, const char* fmt, va_list args)
  ObsLogger_vlog(level_to_obs(level), tag, fmt, args);
}

void fc_log(fc_log_level_t level, const char* tag, const char* fmt, ...) {
  if (level < g_min_level) return;
  va_list args;
  va_start(args, fmt);
  fc_log_v(level, tag, fmt, args);
  va_end(args);
}
