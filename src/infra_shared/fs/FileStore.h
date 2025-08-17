#pragma once
#include <string>
#include <system_error>
#include "PathResolver.h"

namespace foxclip::infra_shared::fs {

class FileStore {
public:
	explicit FileStore(PathResolver &resolver) : resolver_(resolver) {}

	// 相対パスで OK。内部で OBS ルートに解決し、必要なら親ディレクトリ作成。
	bool write_text(const std::string &rel, const std::string &utf8, std::error_code &ec);
	bool read_text(const std::string &rel, std::string &out, std::error_code &ec) const;
	bool ensure_dir(const std::string &relDir, std::error_code &ec);
	bool exists(const std::string &rel, std::error_code &ec) const;

private:
	bool resolve_path(const std::string& rel, std::string& full, std::error_code& ec) const;
	PathResolver &resolver_;
};

} // namespace foxclip::infra_shared::fs
