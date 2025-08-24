#pragma once
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <optional>
#include "infra_shared/plugin/ThreadedPlugin.h"

namespace foxclip::infra_shared::plugin {

// スレッドセーフなマルチプラグインホスト（シングルトン）
class MultiPluginHost {
public:
	static MultiPluginHost &instance();

	// 1つロード＆起動
	// ident: manifest 由来のID/名前/バージョン（ログ/管理用）
	// returns: true=OK
	bool loadAndStart(const PluginIdentity &ident, const std::string &modulePathUtf8);

	// id で停止＆アンロード（id は manifest.id）
	bool stopAndUnloadById(const std::string &id);

	// 全停止＆アンロード
	void stopAndUnloadAll();

	// 一覧
	struct Item {
		PluginIdentity ident;
		std::string modulePath;
	};
	std::vector<Item> list() const;

private:
	MultiPluginHost() = default;

	mutable std::mutex mu;
	std::vector<std::unique_ptr<ThreadedPlugin>> items;
};

} // namespace foxclip::infra_shared::plugin
