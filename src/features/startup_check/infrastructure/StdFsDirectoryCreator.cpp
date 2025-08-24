#include "StdFsDirectoryCreator.h"
#include "infra_shared/log/ObsLogger.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace foxclip::startup_check::infrastructure {

bool StdFsDirectoryCreator::createIfMissing(const std::string &path, std::error_code &ec)
{
	ec.clear();
	OBS_LOG_INFO("[StdFsDirectoryCreator] Attempting to create directory: '%s'", path.c_str());

	if (path.empty()) {
		OBS_LOG_ERROR("[StdFsDirectoryCreator] Empty path provided");
		ec = std::make_error_code(std::errc::invalid_argument);
		return false;
	}

	// Check if already exists and is a directory
	OBS_LOG_INFO("[StdFsDirectoryCreator] Checking if directory exists: '%s'", path.c_str());
	if (fs::exists(path, ec)) {
		if (ec) {
			OBS_LOG_ERROR("[StdFsDirectoryCreator] Error checking existence: %s", ec.message().c_str());
			return false;
		}
		OBS_LOG_INFO("[StdFsDirectoryCreator] Path exists, checking if it's a directory");
		if (fs::is_directory(path, ec)) {
			OBS_LOG_INFO("[StdFsDirectoryCreator] Directory already exists: '%s'", path.c_str());
			return true; // Already exists as directory
		}

		// Exists but is not a directory
		OBS_LOG_ERROR("[StdFsDirectoryCreator] Path exists but is not a directory: '%s'", path.c_str());
		if (!ec) {
			ec = std::make_error_code(std::errc::file_exists);
		}
		return false;
	}

	// Clear "file not found" error - this is expected when creating new directories
	if (ec) {
		OBS_LOG_INFO("[StdFsDirectoryCreator] Directory doesn't exist (expected), error was: %s",
			     ec.message().c_str());
		ec.clear();
	}

	// Create directory structure
	OBS_LOG_INFO("[StdFsDirectoryCreator] Creating directory structure: '%s'", path.c_str());
	const bool created = fs::create_directories(path, ec);
	if (ec) {
		OBS_LOG_ERROR("[StdFsDirectoryCreator] Failed to create directory: %s", ec.message().c_str());
		return false; // Creation failed
	}

	OBS_LOG_INFO("[StdFsDirectoryCreator] Directory creation result: %s, path: '%s'",
		     created ? "created new" : "already existed", path.c_str());

	// Verify creation was successful
	std::error_code verifyEc;
	if (fs::exists(path, verifyEc) && fs::is_directory(path, verifyEc)) {
		OBS_LOG_INFO("[StdFsDirectoryCreator] Directory creation verified successfully: '%s'", path.c_str());
		return true;
	} else {
		OBS_LOG_ERROR("[StdFsDirectoryCreator] Directory creation verification failed: '%s', verify error: %s",
			      path.c_str(), verifyEc.message().c_str());
		if (!ec) {
			ec = verifyEc;
		}
		return false;
	}
}

} // namespace foxclip::startup_check::infrastructure
