#include "PathResolver.h"
#include "FsUtils.h"
#include "infra_shared/log/ObsLogger.h"
#include <filesystem>
#include <optional>

namespace stdfs = std::filesystem;

namespace foxclip::infra_shared::fs {

// Constants following kUpperCamel naming convention
static constexpr const char* kParentDirRef = "..";
static constexpr char kWindowsSeparator = '\\';
static constexpr char kUnixSeparator = '/';

static bool containsParentRef(const std::string &relativePath)
{
	const auto path = stdfs::path(relativePath);
	for (const auto &component : path) {
		if (component == kParentDirRef) {
			return true;
		}
	}
	return false;
}

static bool isPathTraversalSafe(const stdfs::path &basePath, const stdfs::path &fullPath)
{
	// Ensure the resolved path is still within the base directory
	const auto normalizedBase = basePath.lexically_normal();
	const auto normalizedFull = fullPath.lexically_normal();
	
	OBS_LOG_INFO("[PathResolver] Security check - base: '%s', full: '%s'", 
		normalizedBase.string().c_str(), normalizedFull.string().c_str());
	
	// Use string comparison for simpler and more reliable check
	std::string baseStr = normalizedBase.string();
	const std::string fullStr = normalizedFull.string();
	
	// Remove trailing slash from base path if present
	if (!baseStr.empty() && (baseStr.back() == kWindowsSeparator || baseStr.back() == kUnixSeparator)) {
		baseStr.pop_back();
		OBS_LOG_INFO("[PathResolver] Removed trailing slash from base: '%s'", baseStr.c_str());
	}
	
	// Check if fullPath starts with basePath
	if (fullStr.length() < baseStr.length()) {
		OBS_LOG_ERROR("[PathResolver] Full path shorter than base path");
		return false;
	}
	
	// Compare the base portion
	if (fullStr.substr(0, baseStr.length()) != baseStr) {
		OBS_LOG_ERROR("[PathResolver] Full path doesn't start with base path");
		return false;
	}
	
	// If lengths are equal, paths are identical (allowed)
	if (fullStr.length() == baseStr.length()) {
		OBS_LOG_INFO("[PathResolver] Security check result: PASS (identical paths)");
		return true;
	}
	
	// Check that the next character is a path separator (prevents partial matches)
	char nextChar = fullStr[baseStr.length()];
	bool isSeparator = (nextChar == kWindowsSeparator || nextChar == kUnixSeparator);
	
	OBS_LOG_INFO("[PathResolver] Security check result: %s (next char: '%c')", 
		isSeparator ? "PASS" : "FAIL", nextChar);
	return isSeparator;
}

std::optional<std::string> PathResolver::toFull(const std::string &relative) const
{
	OBS_LOG_INFO("[PathResolver] Resolving relative path: '%s'", relative.c_str());
	
	// Input validation: reject empty, absolute paths, or parent directory references
	if (relative.empty()) {
		OBS_LOG_ERROR("[PathResolver] Empty relative path provided");
		return std::nullopt;
	}
	
	if (isAbsolute(relative)) {
		OBS_LOG_ERROR("[PathResolver] Absolute path rejected: '%s'", relative.c_str());
		return std::nullopt;
	}
	
	if (containsParentRef(relative)) {
		OBS_LOG_ERROR("[PathResolver] Path contains parent reference (..) rejected: '%s'", relative.c_str());
		return std::nullopt;
	}

	// Get root directory
	const std::string baseDir = root.root();
	OBS_LOG_INFO("[PathResolver] Base directory from root provider: '%s'", baseDir.c_str());
	if (baseDir.empty()) {
		OBS_LOG_ERROR("[PathResolver] Root provider returned empty base directory");
		return std::nullopt;
	}

	// Combine and normalize paths
	const auto basePath = stdfs::path(baseDir);
	const auto fullPath = (basePath / stdfs::path(relative)).lexically_normal();
	OBS_LOG_INFO("[PathResolver] Combined path before security check: '%s'", fullPath.string().c_str());

	// Security check: ensure the resolved path stays within base directory
	if (!isPathTraversalSafe(basePath, fullPath)) {
		OBS_LOG_ERROR("[PathResolver] Path traversal security check failed for: '%s'", fullPath.string().c_str());
		return std::nullopt;
	}

	OBS_LOG_INFO("[PathResolver] Path resolution successful: '%s' -> '%s'", relative.c_str(), fullPath.string().c_str());
	return fullPath.string();
}

} // namespace foxclip::infra_shared::fs