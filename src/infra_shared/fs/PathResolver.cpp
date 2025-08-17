#include "PathResolver.h"
#include "FsUtils.h"
#include <filesystem>

namespace stdfs = std::filesystem;
namespace foxclip::infra_shared::fs {

static bool contains_parent_ref(const std::string &rel)
{
	auto p = stdfs::path(rel);
	for (auto &part : p) {
		if (part == "..")
			return true;
	}
	return false;
}

std::string PathResolver::to_full(const std::string &relative, bool *ok) const
{
	if (relative.empty() || is_abs(relative) || contains_parent_ref(relative)) {
		if (ok) *ok = false;
		return {};
	}

	std::string base = root_.root();
	if (base.empty()) {
		if (ok) *ok = false;
		return {};
	}

	if (ok) *ok = true;
	return (stdfs::path(base) / stdfs::path(relative)).lexically_normal().string();
}

} // namespace foxclip::infra_shared::fs
