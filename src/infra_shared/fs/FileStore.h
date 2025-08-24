#pragma once
#include <string>
#include <system_error>
#include "PathResolver.h"

namespace foxclip::infra_shared::fs {

class FileStore {
public:
	explicit FileStore(PathResolver &resolver) : resolver(resolver) {}

	// 相対パスで OK。内部で OBS ルートに解決し、必要なら親ディレクトリ作成。
	bool writeText(const std::string &relativePath, const std::string &utf8Content, std::error_code &ec);
	bool readText(const std::string &relativePath, std::string &output, std::error_code &ec) const;
	bool ensureDirectory(const std::string &relativeDirectory, std::error_code &ec);
	bool exists(const std::string &relativePath, std::error_code &ec) const;

private:
	bool resolvePath(const std::string &rel, std::string &full, std::error_code &ec) const;
	PathResolver &resolver;
};

} // namespace foxclip::infra_shared::fs
