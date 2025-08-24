#pragma once
#include <string>
#include <optional>

namespace foxclip::infra_shared::fs::domain {

// パス値オブジェクト
class Path {
public:
	static std::optional<Path> fromRelative(const std::string &relativePath);
	static std::optional<Path> fromAbsolute(const std::string &absolutePath);

	const std::string &value() const { return path; }
	bool isAbsolute() const { return absolute; }
	bool isRelative() const { return !absolute; }

	// パス結合
	std::optional<Path> join(const std::string &component) const;

private:
	explicit Path(std::string path, bool absolute) : path(std::move(path)), absolute(absolute) {}

	std::string path;
	bool absolute;
};

} // namespace foxclip::infra_shared::fs::domain
