#pragma once
#include <string>
#include <thread>
#include <atomic>
#include <memory>
#include "infra_shared/plugin/ModuleLoader.h"

namespace foxclip::features::plugin_host::infrastructure {

struct PluginIdentity {
	std::string id;
	std::string name;
	std::string version;
};

class ThreadedPlugin {
public:
	ThreadedPlugin(PluginIdentity ident, foxclip::infra_shared::plugin::ModuleHandles mod);
	~ThreadedPlugin();

	// スレッド起動（init を呼んでから idle ループへ）
	bool start();
	// 終了（deinit を呼んでスレッド join）
	void stop();

	const PluginIdentity &identity() const { return ident; }
	const std::string &modulePath() const { return mod.modulePath; }

private:
	void threadMain();

	PluginIdentity ident;
	foxclip::infra_shared::plugin::ModuleHandles mod;
	std::thread worker;
	std::atomic<bool> running{false};
};

} // namespace foxclip::features::plugin_host::infrastructure
