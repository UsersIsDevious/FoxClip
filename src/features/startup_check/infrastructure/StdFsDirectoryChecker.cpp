#include "StdFsDirectoryChecker.h"

namespace foxclip::features::startup_check::infrastructure {

bool StdFsDirectoryChecker::existsDir(const std::string &path) const
{
	namespace fs = std::filesystem;
	std::error_code ec;
	return fs::exists(path, ec) && fs::is_directory(path, ec);
}

} // namespace foxclip::features::startup_check::infrastructure