#pragma once
#include <string>
#include <system_error>

namespace foxclip::infra_shared::fs {

bool existsDir(const std::string &path, std::error_code &ec) noexcept;
bool createDirs(const std::string &path, std::error_code &ec) noexcept;
std::string join(const std::string &base, const std::string &name);
bool isAbs(const std::string &path) noexcept;

} // namespace foxclip::infra_shared::fs
