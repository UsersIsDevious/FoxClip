#include "ObsLoggerImpl.h"
#include "../ObsLogger.h"
#include <obs-module.h>
#include <cstdarg>

namespace foxclip::infra_shared::log::infrastructure {

ObsLoggerImpl::ObsLoggerImpl() : minLevel(domain::LogLevel::Debug)
{
}

void ObsLoggerImpl::setMinLevel(domain::LogLevel level)
{
    minLevel = level;
}

void ObsLoggerImpl::log(domain::LogLevel level, const char *format, ...)
{
    if (level < minLevel) return;
    
    va_list args;
    va_start(args, format);
    logVa(level, format, args);
    va_end(args);
}

void ObsLoggerImpl::logVa(domain::LogLevel level, const char *format, va_list args)
{
    if (level < minLevel) return;
    
    int obsLevel = toObsLogLevel(level);
    
    // Use OBS's blogva function directly
    blogva(obsLevel, format, args);
}

int ObsLoggerImpl::toObsLogLevel(domain::LogLevel level) const
{
    switch (level) {
        case domain::LogLevel::Debug:
            return LOG_DEBUG;
        case domain::LogLevel::Info:
            return LOG_INFO;
        case domain::LogLevel::Warning:
            return LOG_WARNING;
        case domain::LogLevel::Error:
            return LOG_ERROR;
        default:
            return LOG_INFO;
    }
}

} // namespace foxclip::infra_shared::log::infrastructure
