#pragma once
#include <string>
#include "features/plugin_loader/domain/LoadTypes.h"

namespace foxclip::plugin_loader::app {

// 単一プラグインのロード、および一括ロード（直下ディレクトリ）を提供するファサード。
// 「読み取り→検証→エントリ解決→ロード」まで実行する。
class LoaderFacade {
public:
	// pluginDirUtf8: 1プラグインのルート（…/comExampleMyplugin）
	domain::LoadOneResult loadOne(const std::string &pluginDirUtf8, const domain::LoadOptions &opt = {});

	// pluginsRootUtf8: ルート（…/foxclip-plugins）、直下のディレクトリだけ対象
	domain::LoadAllResult loadAll(const std::string &pluginsRootUtf8, const domain::LoadOptions &opt = {});

private:
	// 内部ヘルパ（単一ロードのコア）
	domain::LoadOneResult loadOneImpl(const std::string &pluginDirUtf8, const domain::LoadOptions &opt);
};

} // namespace foxclip::plugin_loader::app
