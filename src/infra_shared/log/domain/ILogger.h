#pragma once
#include <string>
#include <cstdarg>

namespace foxclip::infra_shared::log::domain {

enum class LogLevel { Debug = 0, Info = 1, Warning = 2, Error = 3 };

// ログ出力の抽象ポート
class ILogger {
public:
	virtual ~ILogger() = default;

	virtual void setMinLevel(LogLevel level) = 0;
	virtual void log(LogLevel level, const char *format, ...) = 0;
	virtual void logVa(LogLevel level, const char *format, va_list args) = 0;
};

} // namespace foxclip::infra_shared::log::domain
