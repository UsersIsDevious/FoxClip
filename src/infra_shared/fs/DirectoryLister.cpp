#include "infra_shared/fs/DirectoryLister.h"

#include <system_error>
#include <filesystem>

namespace foxclip::infra_shared::fs {

std::vector<std::filesystem::path> DirectoryLister::listSubdirectories(const std::filesystem::path &root)
{
	std::vector<std::filesystem::path> result;

	std::error_code ec;
	if (!std::filesystem::exists(root, ec) || !std::filesystem::is_directory(root, ec)) {
		return result; // ルートが無い/ディレクトリでない -> 空返し
	}

	for (const auto &entry : std::filesystem::directory_iterator(root, ec)) {
		if (ec) { // 途中でエラーが出たらスキップして継続
			ec.clear();
			continue;
		}
		std::error_code typeEc;
		if (entry.is_directory(typeEc) && !typeEc) {
			result.push_back(entry.path());
		}
	}
	return result;
}

} // namespace foxclip::infra_shared::fs
