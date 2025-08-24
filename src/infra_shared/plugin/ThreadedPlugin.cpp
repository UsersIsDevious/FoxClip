#include "infra_shared/plugin/ThreadedPlugin.h"
#include "infra_shared/plugin/HostApiV1.h"
#include "infra_shared/log/ObsLogger.h"
#include <chrono>

namespace foxclip::infra_shared::plugin {

ThreadedPlugin::ThreadedPlugin(PluginIdentity ident, ModuleHandles mod) : ident(std::move(ident)), mod(std::move(mod))
{
}

ThreadedPlugin::~ThreadedPlugin()
{
	stop();
	ModuleLoader::unload(mod);
}

bool ThreadedPlugin::start()
{
	if (running.load())
		return true;
	running.store(true);
	worker = std::thread(&ThreadedPlugin::threadMain, this);
	return true;
}

void ThreadedPlugin::stop()
{
	if (!running.exchange(false))
		return;
	if (worker.joinable())
		worker.join();
}

void ThreadedPlugin::threadMain()
{
	const FoxclipApi *api = getHostApiV1();
	// プラグイン初期化
	bool ok = mod.init(api);
	if (!ok) {
		OBS_LOG_ERROR("[foxclip] plugin init failed: %s (%s)", ident.name.c_str(), mod.modulePath.c_str());
		running.store(false);
		return;
	}
	OBS_LOG_INFO("[foxclip] plugin started: %s (%s)", ident.name.c_str(), ident.version.c_str());

	// ここでは「専用スレッドを保持」するだけの idle ループ。
	// 将来、イベント待ちやメッセージキュー等をここに実装。
	while (running.load()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	// 終了処理
	mod.deinit();
	OBS_LOG_INFO("[foxclip] plugin stopped: %s", ident.name.c_str());
}

} // namespace foxclip::infra_shared::plugin
