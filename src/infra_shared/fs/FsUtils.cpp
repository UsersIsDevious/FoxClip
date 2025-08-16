#include "FsUtils.h"
#include <filesystem>

namespace stdfs = std::filesystem;  

namespace foxclip::infra_shared::fs {

bool exists_dir(const std::string& pathStr, std::error_code& ec) noexcept {
  ec.clear();
  // exists と is_directory を std::filesystem 側に明示
  return stdfs::exists(pathStr, ec) && stdfs::is_directory(pathStr, ec);
}

bool create_dirs(const std::string& pathStr, std::error_code& ec) noexcept {
  ec.clear();
  if (exists_dir(pathStr, ec)) return !ec;
  const bool ok = stdfs::create_directories(pathStr, ec);
  if (ec) return false;
  // 既存でも成功扱い
  return ok || exists_dir(pathStr, ec);
}

std::string join(const std::string& base, const std::string& name) {
  return (stdfs::path(base) / stdfs::path(name)).string();
}

bool is_abs(const std::string& pathStr) noexcept {
  std::error_code ec;
  (void)ec;
  return stdfs::path(pathStr).is_absolute();
}

} // namespace foxclip::infra_shared::fs
