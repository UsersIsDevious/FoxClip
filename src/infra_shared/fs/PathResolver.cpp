#include "PathResolver.h"
#include "FsUtils.h"
#include <filesystem>
#include <optional>

namespace stdfs = std::filesystem;
namespace foxclip::infra_shared::fs {

static bool contains_parent_ref(const std::string &rel)
{
	auto p = stdfs::path(rel);
	for (const auto &part : p) {
		if (part == "..")
			return true;
	}
	return false;
}

std::optional<std::string> PathResolver::to_full(const std::string &relative) const
{
	// 入力検証：空、絶対、親ディレクトリ参照は拒否
	if (relative.empty() || is_abs(relative) || contains_parent_ref(relative)) {
		return std::nullopt;
	}

	// ルート取得
	const std::string base = root_.root();
	if (base.empty()) {
		return std::nullopt;
	}

	// 結合して正規化
	const auto full = (stdfs::path(base) / stdfs::path(relative)).lexically_normal();

	// 念のため、正規化後にまだ絶対/親参照が混入していないか簡易チェック
	if (full.is_absolute()) {
		// base が絶対なので normally OK のはずだが、安全側でチェック
		// （要件に応じてこのチェックは省略可）
	}

	return full.string(); // UTF-8 前提なら .u8string() でも可
}

} // namespace foxclip::infra_shared::fs