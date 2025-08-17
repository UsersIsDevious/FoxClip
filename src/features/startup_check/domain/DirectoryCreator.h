#pragma once
#include <string>
#include <system_error>

namespace foxclip::startup_check::domain {

/// ディレクトリを（必要なら親も含めて）作成する最小インターフェース
class DirectoryCreator {
public:
	virtual ~DirectoryCreator() = default;

	/// 成功: true / 既に存在でも true
	/// 失敗: false（error_code に詳細）
	virtual bool createIfMissing(const std::string &path, std::error_code &ec) = 0;
};

} // namespace foxclip::startup_check::domain
