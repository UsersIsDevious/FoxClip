#pragma once

#include <filesystem>
#include <vector>

namespace foxclip::infra_shared::fs {

// ディレクトリ直下のサブディレクトリ（フォルダー）のみを列挙するユーティリティ。
// 例外は投げず、アクセス不能などは空ベクタを返す。
class DirectoryLister {
public:
	static std::vector<std::filesystem::path> listSubdirectories(const std::filesystem::path &root);
};

} // namespace foxclip::infra_shared::fs
