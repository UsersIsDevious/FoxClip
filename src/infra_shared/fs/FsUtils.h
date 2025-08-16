#pragma once
#include <string>
#include <system_error>

namespace foxclip::infra_shared::fs {

bool exists_dir(const std::string &path, std::error_code &ec) noexcept;
bool create_dirs(const std::string &path, std::error_code &ec) noexcept;
std::string join(const std::string &base, const std::string &name);
bool is_abs(const std::string &path) noexcept;

} // namespace foxclip::infra_shared::fs
