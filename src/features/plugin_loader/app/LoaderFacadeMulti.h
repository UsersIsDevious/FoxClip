#pragma once
#include <string>
#include "features/plugin_loader/domain/LoadTypes.h"

namespace foxclip::plugin_loader::app {

class LoaderFacadeMulti {
public:
	// 単一ディレクトリをロード（別スレッドで起動）
	domain::LoadOneResult loadOne(const std::string &pluginDirUtf8);

	// ルート直下を一括ロード（各プラグインが別スレッドで起動）
	domain::LoadAllResult loadAll(const std::string &pluginsRootUtf8);
};

} // namespace foxclip::plugin_loader::app
