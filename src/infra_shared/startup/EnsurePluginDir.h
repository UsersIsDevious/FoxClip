#pragma once
#include <string>
#include <system_error>

namespace foxclip::infra_shared::startup {

// 成功時: full_path を返す / 失敗時: 空文字と error_message を返す
struct EnsureResult {
  bool ok{};
  std::string full_path;
  std::string error_message;
};

EnsureResult ensure_obs_writable_dir(const std::string& subdir);

} // namespace foxclip::infra_shared::startup
