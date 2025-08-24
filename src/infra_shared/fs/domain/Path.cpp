#include "Path.h"
#include <algorithm>

namespace foxclip::infra_shared::fs::domain {

std::optional<Path> Path::fromRelative(const std::string &relativePath)
{
	if (relativePath.empty() || relativePath[0] == '/' || relativePath[0] == '\\') {
		return std::nullopt;
	}
	if (relativePath.find("..") != std::string::npos) {
		return std::nullopt;
	}
	return Path(relativePath, false);
}

std::optional<Path> Path::fromAbsolute(const std::string &absolutePath)
{
	if (absolutePath.empty()) {
		return std::nullopt;
	}
	bool isAbs = (absolutePath[0] == '/' || absolutePath[0] == '\\' ||
		      (absolutePath.length() > 1 && absolutePath[1] == ':'));
	if (!isAbs) {
		return std::nullopt;
	}
	return Path(absolutePath, true);
}

std::optional<Path> Path::join(const std::string &component) const
{
	if (component.empty() || component.find("..") != std::string::npos) {
		return std::nullopt;
	}

	std::string joined = path;
	if (!joined.empty() && joined.back() != '/' && joined.back() != '\\') {
		joined += "/";
	}
	joined += component;

	return Path(joined, absolute);
}

} // namespace foxclip::infra_shared::fs::domain
