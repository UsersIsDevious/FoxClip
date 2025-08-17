#pragma once
#include <string>
#include <optional>
#include "infra_shared/fs/roots/IRootProvider.h"

namespace foxclip::infra_shared::fs {

class PathResolver {
public:
	explicit PathResolver(roots::IRootProvider &root) : root_(root) {}
	// 相対パスのみ受け付け、root() と結合したフルパスを返す。絶対や ".." は拒否/正規化。
	std::optional<std::string> to_full(const std::string &relative) const;

private:
	roots::IRootProvider &root_;
};

} // namespace foxclip::infra_shared::fs
