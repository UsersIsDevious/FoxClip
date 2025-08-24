#pragma once
#include <string>

namespace foxclip::features::startup_check::infrastructure {

// 成功時: fullPath を返す / 失敗時: 空文字と errorMessage を返す
struct EnsureResult {
	bool ok{};
	std::string fullPath;
	std::string errorMessage;
};

EnsureResult ensureObsWritableDir(const std::string &subdir);

} // namespace foxclip::features::startup_check::infrastructure
