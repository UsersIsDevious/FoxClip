#include "FileStore.h"
#include "FsUtils.h"
#include <filesystem>
#include <fstream>

namespace stdfs = std::filesystem;
namespace foxclip::infra_shared::fs {

bool FileStore::resolve_path(const std::string &rel, std::string &full, std::error_code &ec) const
{
	bool ok = false;
	full = resolver_.to_full(rel, &ok);
	if (!ok) {
		ec = std::make_error_code(std::errc::invalid_argument);
		return false;
	}
	return true;
}

bool FileStore::ensure_dir(const std::string &relDir, std::error_code &ec)
{
	ec.clear();

	// パス解決
	std::string full;
	if (!resolve_path(relDir, full, ec))
		return false;

	// 既に存在するか確認
	return create_dirs(full, ec);
}

bool FileStore::write_text(const std::string &rel, const std::string &utf8, std::error_code &ec)
{
	ec.clear();

	// パス解決
	std::string full;
	if (!resolve_path(rel, full, ec))
		return false;

	// 既に存在する場合は何もしない
	stdfs::path p(full);
	if (!create_dirs(p.parent_path().string(), ec))
		return false;

	// ファイルを開く（存在しない場合は新規作成）
	std::ofstream ofs(full, std::ios::binary | std::ios::trunc);
	if (!ofs) {
		// オープンに失敗（権限以外の可能性もあるので io_error の方が無難）
		ec = std::make_error_code(std::errc::io_error);
		return false;
	}

	// 書き込み
	ofs.write(utf8.data(), static_cast<std::streamsize>(utf8.size()));
	if (!ofs.good()) {
		// 書き込み途中のエラー（ディスクフルなど）
		ec = std::make_error_code(std::errc::io_error);
		return false;
	}
	// 明示的にフラッシュ
	// (std::ofstream は自動でフラッシュしないので、書き込み後に明示的に呼ぶ)
	ofs.flush();
	if (!ofs.good()) {
		ec = std::make_error_code(std::errc::io_error);
		return false;
	}
	return true;
}

bool FileStore::read_text(const std::string &rel, std::string &out, std::error_code &ec) const
{
	ec.clear();

	// パス解決
	std::string full;
	if (!resolve_path(rel, full, ec))
		return false;

	// ファイルを開く
	std::ifstream ifs(full, std::ios::binary);
	if (!ifs) {
		// ファイルが存在しない、または開けない
		ec = std::make_error_code(std::errc::no_such_file_or_directory);
		return false;
	}

	// ファイルサイズを取得（読み込みのための予約）
	// ここではファイルが空でないことを確認するために、サイズを取得
	std::error_code fec;
	auto sz = stdfs::file_size(full, fec);
	if (!fec && sz > 0)
		out.reserve(static_cast<size_t>(sz));

	// ファイルの内容を読み込む
	out.assign((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	// 読み込みに失敗した場合はエラーコードを設定
	// eof() は正常な終了なので、good() でチェック
	if (!ifs.good() && !ifs.eof()) {
		ec = std::make_error_code(std::errc::io_error);
		return false;
	}

	return true;
}

bool FileStore::exists(const std::string &rel, std::error_code &ec) const
{
	ec.clear();

	std::string full;
	if (!resolve_path(rel, full, ec))
		return false;
	return stdfs::exists(full, ec);
}

} // namespace foxclip::infra_shared::fs
