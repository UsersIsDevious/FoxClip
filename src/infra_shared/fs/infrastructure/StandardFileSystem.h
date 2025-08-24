#pragma once
#include "../domain/IFileSystem.h"
#include "../roots/IRootProvider.h"

namespace foxclip::infra_shared::fs::infrastructure {

// 標準ファイルシステム実装
class StandardFileSystem : public domain::IFileSystem {
public:
	foxclip::infra_shared::common::domain::Result<std::string> readText(const std::string &path) override;
	foxclip::infra_shared::common::domain::Result<void> writeText(const std::string &path,
								      const std::string &content) override;
	foxclip::infra_shared::common::domain::Result<void> createDirectories(const std::string &path) override;
	foxclip::infra_shared::common::domain::Result<bool> exists(const std::string &path) override;
};

// 既存のPathResolverをIPathResolverに適応
class PathResolverAdapter : public domain::IPathResolver {
public:
	explicit PathResolverAdapter(roots::IRootProvider &rootProvider);

	std::optional<std::string> toFullPath(const std::string &relativePath) override;
	bool isValidRelativePath(const std::string &path) override;

private:
	roots::IRootProvider &rootProvider;
};

} // namespace foxclip::infra_shared::fs::infrastructure
