#pragma once
#include <memory>
#include "../domain/IFileSystem.h"
#include "../domain/Path.h"
#include "infra_shared/common/domain/Result.h"

namespace foxclip::infra_shared::fs::app {

// ファイル操作の調停サービス
class FileService {
public:
	explicit FileService(std::unique_ptr<domain::IFileSystem> fileSystem,
			     std::unique_ptr<domain::IPathResolver> pathResolver);

	// 相対パスでの操作（自動的にフルパスに解決）
	foxclip::infra_shared::common::domain::Result<std::string> readTextRelative(const std::string &relativePath);
	foxclip::infra_shared::common::domain::Result<void> writeTextRelative(const std::string &relativePath,
									      const std::string &content);
	foxclip::infra_shared::common::domain::Result<void> ensureDirectoryRelative(const std::string &relativePath);

	// 絶対パスでの直接操作
	foxclip::infra_shared::common::domain::Result<std::string> readText(const std::string &absolutePath);
	foxclip::infra_shared::common::domain::Result<void> writeText(const std::string &absolutePath,
								      const std::string &content);

private:
	std::unique_ptr<domain::IFileSystem> fileSystem;
	std::unique_ptr<domain::IPathResolver> pathResolver;
};

} // namespace foxclip::infra_shared::fs::app
