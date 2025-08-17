#pragma once
#include <string>
#include <cstdint>

/**
 * foxclip-plugin を動的ロード/アンロードし、関数テーブルを注入するホスト用ユーティリティ。
 *
 * 使い方（例）:
 *   bool ok = foxclip::Host::Load("path/to/foxclip-plugin.dll"); // or .so/.dylib
 *   ...
 *   foxclip::Host::Unload();
 */

namespace foxclip::Host {

/** プラグインがロード済みか？ */
bool IsLoaded();

/** プラグインをロードして初期化（UTF-8パス）。成功で true。 */
bool Load(const std::string &modulePathUtf8);

/** Windows: UTF-16パス版（必要な場合だけ使用）。*/
#ifdef _WIN32
bool LoadW(const wchar_t *modulePathW);
#endif

/** プラグインを終了/アンロード。多重呼び出し可。*/
void Unload();

} // namespace foxclip::Host
