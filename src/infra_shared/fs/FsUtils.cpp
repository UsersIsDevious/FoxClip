#include "FsUtils.h"
#include <filesystem>

namespace stdfs = std::filesystem;

namespace foxclip::infra_shared::fs {

// Constants following kUpperCamel naming convention
static constexpr std::size_t kMaxPathLength = 260; // Windows MAX_PATH

bool existsDir(const std::string &pathStr, std::error_code &ec) noexcept
{
	ec.clear();
	// Single filesystem call optimization - avoid double error_code check
	const auto status = stdfs::status(pathStr, ec);
	if (ec) {
		return false;
	}
	return stdfs::is_directory(status);
}

bool createDirs(const std::string &pathStr, std::error_code &ec) noexcept
{
	ec.clear();

	if (pathStr.empty()) {
		ec = std::make_error_code(std::errc::invalid_argument);
		return false;
	}

	// Check if directory already exists
	if (existsDir(pathStr, ec)) {
		return true; // Already exists, success
	}

	// Clear any error from existsDir check (e.g., "file not found" is expected)
	ec.clear();

	// Attempt to create the directory structure
	const bool created = stdfs::create_directories(pathStr, ec);
	if (ec) {
		return false; // Creation failed
	}

	// create_directories returns false if directory already existed
	// but we consider that a success case
	return true;
}

std::string join(const std::string &base, const std::string &name)
{
	// Input validation for better error handling
	if (base.empty()) {
		return name;
	}
	if (name.empty()) {
		return base;
	}

	// Path length validation
	const auto result = (stdfs::path(base) / stdfs::path(name)).string();
	if (result.length() > kMaxPathLength) {
		// Return empty string to indicate path too long
		return {};
	}

	return result;
}

bool isAbsolute(const std::string &pathStr) noexcept
{
	if (pathStr.empty()) {
		return false;
	}
	return stdfs::path(pathStr).is_absolute();
}

} // namespace foxclip::infra_shared::fs
