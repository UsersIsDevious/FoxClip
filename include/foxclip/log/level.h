#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum fc_log_level_t {
	FC_LOG_TRACE = 0,
	FC_LOG_DEBUG = 1,
	FC_LOG_INFO = 2,
	FC_LOG_WARN = 3,
	FC_LOG_ERROR = 4
} fc_log_level_t;

#ifdef __cplusplus
}
#endif
