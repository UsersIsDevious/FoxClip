#include "StdFsDirectoryCreator.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace foxclip::startup_check::infrastructure {

bool StdFsDirectoryCreator::create_if_missing(const std::string &path, std::error_code &ec)
{
	ec.clear();
	// 既にある場合は true を返す（何もしない）
	if (fs::exists(path, ec)) {
		if (ec)
			return false;
		return true;
	}
	// 足りない親までまとめて作成
	const bool ok = fs::create_directories(path, ec);
	if (ec)
		return false;
	// create_directories は「作ったら true / 既存なら false」を返すが、
	// 我々の API としては「存在すれば成功」で良いので true を返す
	return ok || fs::exists(path, ec);
}

} // namespace foxclip::startup_check::infrastructure
