#pragma once
#include <string>

namespace foxclip::infra_shared::startup {

// 成功時: fullPath を返す / 失敗時: 空文字と errorMessage を返す
struct EnsureResult {
	bool ok{};
	std::string fullPath;
	std::string errorMessage;
};

EnsureResult ensure_obs_writable_dir(const std::string &subdir);

} // namespace foxclip::infra_shared::startup
