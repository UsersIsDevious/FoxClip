#include "FileStore.h"
#include "FsUtils.h"
#include <filesystem>
#include <fstream>

namespace stdfs = std::filesystem;
namespace foxclip::infra_shared::fs {

bool FileStore::resolvePath(const std::string &rel, std::string &full, std::error_code &ec) const
{
	ec.clear();
	// 相対パスを OBS ルートと結合してフルパスを取得
	if (auto fullOpt = resolver.toFull(rel)) {
		full = *fullOpt;
		return true;
	}
	ec = std::make_error_code(std::errc::invalid_argument);
	return false;
}

bool FileStore::ensureDirectory(const std::string &relativeDirectory, std::error_code &ec)
{
	ec.clear();

	// パス解決
	std::string full;
	if (!resolvePath(relativeDirectory, full, ec))
		return false;

	// 既に存在するか確認
	return createDirs(
		full,
		ec); 
}

bool FileStore::writeText(const std::string &relativePath, const std::string &utf8Content, std::error_code &ec)
{
	ec.clear();

	// パス解決
	std::string full;
	if (!resolvePath(relativePath, full, ec))
		return false;

	// 既に存在する場合は何もしない
	stdfs::path p(full);
	if (!createDirs(p.parent_path().string(), ec))
		return false;

	// ファイルを開く（存在しない場合は新規作成）
	std::ofstream ofs(full, std::ios::binary | std::ios::trunc);
	if (!ofs) {
		// オープンに失敗（権限以外の可能性もあるので io_error の方が無難）
		ec = std::make_error_code(std::errc::io_error);
		return false;
	}

	// 書き込み
	ofs.write(utf8Content.data(), static_cast<std::streamsize>(utf8Content.size()));
	if (!ofs.good()) {
		// 書き込み途中のエラー（ディスクフルなど）
		ec = std::make_error_code(std::errc::io_error);
		return false;
	}
	// 明示的にフラッシュ
	// デストラクタが呼ばれる前に書き込みをディスクに反映させるため、明示的にフラッシュ
	ofs.flush();
	if (!ofs.good()) {
		ec = std::make_error_code(std::errc::io_error);
		return false;
	}
	return true;
}

bool FileStore::readText(const std::string &relativePath, std::string &output, std::error_code &ec) const
{
	ec.clear();

	// パス解決
	std::string full;
	if (!resolvePath(relativePath, full, ec))
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
		output.reserve(static_cast<size_t>(sz));

	// 内容を文字列として読み込み
	output.assign(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());

	// 読み込みに失敗した場合はエラーコードを設定
	// eof() は正常な終了なので、good() でチェック
	if (!ifs.good() && !ifs.eof()) {
		ec = std::make_error_code(std::errc::io_error);
		return false;
	}

	return true;
}

bool FileStore::exists(const std::string &relativePath, std::error_code &ec) const
{
	ec.clear();

	std::string full;
	if (!resolvePath(relativePath, full, ec))
		return false;

	return stdfs::exists(full, ec);
}

} // namespace foxclip::infra_shared::fs
