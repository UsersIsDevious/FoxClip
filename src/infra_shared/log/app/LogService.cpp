#include "LogService.h"
#include <cstdarg>
#include <stdexcept>

namespace foxclip::infra_shared::log::app {

std::unique_ptr<LogService> LogService::instance_;

LogService::LogService(std::unique_ptr<domain::ILogger> logger)
    : logger(std::move(logger))
{
}

void LogService::setMinLevel(domain::LogLevel level)
{
    if (logger) {
        logger->setMinLevel(level);
    }
}

void LogService::debug(const char *format, ...)
{
    if (!logger) return;
    
    va_list args;
    va_start(args, format);
    logger->logVa(domain::LogLevel::Debug, format, args);
    va_end(args);
}

void LogService::info(const char *format, ...)
{
    if (!logger) return;
    
    va_list args;
    va_start(args, format);
    logger->logVa(domain::LogLevel::Info, format, args);
    va_end(args);
}

void LogService::warning(const char *format, ...)
{
    if (!logger) return;
    
    va_list args;
    va_start(args, format);
    logger->logVa(domain::LogLevel::Warning, format, args);
    va_end(args);
}

void LogService::error(const char *format, ...)
{
    if (!logger) return;
    
    va_list args;
    va_start(args, format);
    logger->logVa(domain::LogLevel::Error, format, args);
    va_end(args);
}

LogService &LogService::instance()
{
    if (!instance_) {
        throw std::runtime_error("LogService not initialized. Call LogService::initialize() first.");
    }
    return *instance_;
}

void LogService::initialize(std::unique_ptr<domain::ILogger> logger)
{
    instance_ = std::make_unique<LogService>(std::move(logger));
}

} // namespace foxclip::infra_shared::log::app
