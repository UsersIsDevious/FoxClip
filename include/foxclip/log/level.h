#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// ログレベルを表す列挙型
typedef enum FoxclipLogLevel {
	kLogTrace = 0,
	kLogDebug = 1,
	kLogInfo = 2,
	kLogWarn = 3,
	kLogError = 4
} FoxclipLogLevel;

#ifdef __cplusplus
}
#endif
