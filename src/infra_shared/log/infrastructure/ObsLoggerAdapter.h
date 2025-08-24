#pragma once
#include "../domain/ILogger.h"

namespace foxclip::infra_shared::log::infrastructure {

// OBS ログシステムへのアダプタ
class ObsLoggerAdapter : public domain::ILogger {
public:
	void setMinLevel(domain::LogLevel level) override;
	void log(domain::LogLevel level, const char *format, ...) override;
	void logVa(domain::LogLevel level, const char *format, va_list args) override;

private:
	domain::LogLevel minLevel = domain::LogLevel::Info;
	int toObsLogLevel(domain::LogLevel level) const;
};

} // namespace foxclip::infra_shared::log::infrastructure
