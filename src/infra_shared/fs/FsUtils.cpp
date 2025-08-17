#include "FsUtils.h"
#include <filesystem>

namespace stdfs = std::filesystem;

namespace foxclip::infra_shared::fs {

bool existsDir(const std::string &pathStr, std::error_code &ec) noexcept
{
	ec.clear();
	// exists と is_directory を std::filesystem 側に明示
	return stdfs::exists(pathStr, ec) && stdfs::is_directory(pathStr, ec);
}

bool createDirs(const std::string &pathStr, std::error_code &ec) noexcept
{
	ec.clear();
	if (existsDir(pathStr, ec))
		return !ec;
	const bool ok = stdfs::create_directories(pathStr, ec);
	if (ec)
		return false;
	// 既存でも成功扱い
	return ok || existsDir(pathStr, ec);
}

std::string join(const std::string &base, const std::string &name)
{
	return (stdfs::path(base) / stdfs::path(name)).string();
}

bool isAbs(const std::string &pathStr) noexcept
{
	return stdfs::path(pathStr).is_absolute();
}

} // namespace foxclip::infra_shared::fs
