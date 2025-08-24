#include "infra_shared/fs/DirectoryLister.h"

#include <filesystem>
#include <system_error>

namespace foxclip::infra_shared::fs {

std::vector<std::filesystem::path> DirectoryLister::listSubDirectories(const std::filesystem::path &root)
{
	std::vector<std::filesystem::path> result;

	// ルート検証（非throw）
	std::error_code ec;
	if (!std::filesystem::is_directory(root, ec)) {
		return result; // ディレクトリでなければ空返し
	}

	// パーミッション拒否はスキップ。以降も常に non-throw で回す。
	const auto opts = std::filesystem::directory_options::skip_permission_denied;

	std::filesystem::directory_iterator it(root, opts, ec), end;
	if (ec) {
		// begin 構築時に失敗したら終了
		return result;
	}

	for (; it != end; it.increment(ec)) {
		// increment(ec) でエラーが出ると it==end になりループ終了するため、
		// 明示的な ec チェックやクリア処理は不要。

		// エントリがディレクトリかどうか（非throw）
		std::error_code typeEc;
		if (it->is_directory(typeEc) && !typeEc) {
			result.push_back(it->path());
		}
		// typeEc が立っても次へ
	}

	return result;
}

} // namespace foxclip::infra_shared::fs
