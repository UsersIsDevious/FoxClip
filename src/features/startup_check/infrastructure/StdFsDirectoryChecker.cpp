#include "StdFsDirectoryChecker.h"
#include "infra_shared/log/ObsLogger.h"  // OBS_LOG_ERROR など

namespace foxclip::features::startup_check::infrastructure {

bool StdFsDirectoryChecker::existsDir(const std::string& path) const
{
    namespace fs = std::filesystem;
    std::error_code ec;

    // ※ シンボリックリンクを辿って最終ターゲットを評価
    fs::file_status st = fs::status(path, ec);
    if (ec) {
        OBS_LOG_ERROR("Directory check failed for '%s': %s", path.c_str(), ec.message().c_str());
        return false; // エラー時は安全側に倒す
    }

    return fs::is_directory(st);
}

} // namespace foxclip::features::startup_check::infrastructure
