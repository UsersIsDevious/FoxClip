#pragma once
#include <memory>
#include "../domain/ILogger.h"

namespace foxclip::infra_shared::log::app {

// ログ操作の調停サービス
class LogService {
public:
	explicit LogService(std::unique_ptr<domain::ILogger> logger);

	void setMinLevel(domain::LogLevel level);
	void debug(const char *format, ...);
	void info(const char *format, ...);
	void warning(const char *format, ...);
	void error(const char *format, ...);

	// シングルトンアクセス
	static LogService &instance();
	static void initialize(std::unique_ptr<domain::ILogger> logger);

private:
	std::unique_ptr<domain::ILogger> logger;
	static std::unique_ptr<LogService> instance_;
};

} // namespace foxclip::infra_shared::log::app
