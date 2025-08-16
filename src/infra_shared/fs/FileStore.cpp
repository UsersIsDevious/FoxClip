#include "FileStore.h"
#include "FsUtils.h"
#include <filesystem>
#include <fstream>

namespace stdfs = std::filesystem;
namespace foxclip::infra_shared::fs {

bool FileStore::ensure_dir(const std::string &relDir, std::error_code &ec)
{
	bool ok = false;
	std::string full = resolver_.to_full(relDir, &ok);
	if (!ok) {
		ec = std::make_error_code(std::errc::invalid_argument);
		return false;
	}
	return create_dirs(full, ec);
}

bool FileStore::write_text(const std::string &rel, const std::string &utf8, std::error_code &ec)
{
	bool ok = false;
	std::string full = resolver_.to_full(rel, &ok);
	if (!ok) {
		ec = std::make_error_code(std::errc::invalid_argument);
		return false;
	}

	// 親を作成
	ec.clear();
	stdfs::path p(full);
	if (!create_dirs(p.parent_path().string(), ec))
		return false;

	std::ofstream ofs(full, std::ios::binary | std::ios::trunc);
	if (!ofs) {
		ec = std::make_error_code(std::errc::permission_denied);
		return false;
	}
	ofs.write(utf8.data(), static_cast<std::streamsize>(utf8.size()));
	return ofs.good();
}

bool FileStore::read_text(const std::string &rel, std::string &out, std::error_code &ec) const
{
	bool ok = false;
	std::string full = resolver_.to_full(rel, &ok);
	if (!ok) {
		ec = std::make_error_code(std::errc::invalid_argument);
		return false;
	}

	ec.clear();
	std::ifstream ifs(full, std::ios::binary);
	if (!ifs) {
		ec = std::make_error_code(std::errc::no_such_file_or_directory);
		return false;
	}
	out.assign((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	return true;
}

bool FileStore::exists(const std::string &rel, std::error_code &ec) const
{
	bool ok = false;
	std::string full = resolver_.to_full(rel, &ok);
	if (!ok) {
		ec = std::make_error_code(std::errc::invalid_argument);
		return false;
	}
	return exists_dir(full, ec) || std::filesystem::exists(full, ec);
}

} // namespace foxclip::infra_shared::fs
