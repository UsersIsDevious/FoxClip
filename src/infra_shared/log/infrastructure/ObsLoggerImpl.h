#pragma once
#include "../domain/ILogger.h"

namespace foxclip::infra_shared::log::infrastructure {

// OBS logging system を使った ILogger の実装
class ObsLoggerImpl : public domain::ILogger {
public:
    ObsLoggerImpl();
    virtual ~ObsLoggerImpl() = default;

    void setMinLevel(domain::LogLevel level) override;
    void log(domain::LogLevel level, const char *format, ...) override;
    void logVa(domain::LogLevel level, const char *format, va_list args) override;

private:
    domain::LogLevel minLevel;
    int toObsLogLevel(domain::LogLevel level) const;
};

} // namespace foxclip::infra_shared::log::infrastructure
