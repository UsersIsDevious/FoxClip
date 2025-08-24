#pragma once
#include <string>
#include <optional>
#include "infra_shared/common/domain/Result.h"

namespace foxclip::infra_shared::fs::domain {

// ファイルシステム操作の抽象ポート
class IFileSystem {
public:
	virtual ~IFileSystem() = default;

	virtual foxclip::infra_shared::common::domain::Result<std::string> readText(const std::string &path) = 0;
	virtual foxclip::infra_shared::common::domain::Result<void> writeText(const std::string &path,
									      const std::string &content) = 0;
	virtual foxclip::infra_shared::common::domain::Result<void> createDirectories(const std::string &path) = 0;
	virtual foxclip::infra_shared::common::domain::Result<bool> exists(const std::string &path) = 0;
};

// パス解決の抽象ポート
class IPathResolver {
public:
	virtual ~IPathResolver() = default;

	virtual std::optional<std::string> toFullPath(const std::string &relativePath) = 0;
	virtual bool isValidRelativePath(const std::string &path) = 0;
};

} // namespace foxclip::infra_shared::fs::domain
