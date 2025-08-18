#pragma once

#include <string>

namespace foxclip::infra_shared::plugin {

// プラグイン用ディレクトリ直下のフォルダー一覧を取得し、OBS のログに出力する。
// 失敗時は WARN を出して何も列挙しない。例外は投げない。
void logPluginSubfolders(const std::string &pluginDirName);

} // namespace foxclip::infra_shared::plugin
