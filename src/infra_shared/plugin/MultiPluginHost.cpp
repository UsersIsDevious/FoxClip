#include "infra_shared/plugin/MultiPluginHost.h"
#include "infra_shared/plugin/ModuleLoader.h"
#include "infra_shared/log/ObsLogger.h"
#include <algorithm>

namespace foxclip::infra_shared::plugin {

MultiPluginHost &MultiPluginHost::instance()
{
	static MultiPluginHost g;
	return g;
}

bool MultiPluginHost::loadAndStart(const PluginIdentity &ident, const std::string &modulePathUtf8)
{
	auto mod = ModuleLoader::load(modulePathUtf8);
	if (!mod) {
		OBS_LOG_WARN("[foxclip] module load failed: %s", modulePathUtf8.c_str());
		return false;
	}

	auto tp = std::make_unique<ThreadedPlugin>(ident, *mod);
	// mod の所有権は ThreadedPlugin(ctor) にムーブ済み（*mod → tp 内保持）

	if (!tp->start()) {
		OBS_LOG_WARN("[foxclip] thread start failed: %s", modulePathUtf8.c_str());
		return false;
	}

	std::lock_guard<std::mutex> lk(mu);
	items.emplace_back(std::move(tp));
	return true;
}

bool MultiPluginHost::stopAndUnloadById(const std::string &id)
{
	std::unique_ptr<ThreadedPlugin> victim;

	{
		std::lock_guard<std::mutex> lk(mu);
		auto it = std::find_if(items.begin(), items.end(), [&](const std::unique_ptr<ThreadedPlugin> &p) {
			return p->identity().id == id;
		});
		if (it == items.end())
			return false;
		victim = std::move(*it);
		items.erase(it);
	}
	// ロック外で stop（join を含む）
	victim->stop();
	// dtor でモジュール開放
	return true;
}

void MultiPluginHost::stopAndUnloadAll()
{
	std::vector<std::unique_ptr<ThreadedPlugin>> tmp;
	{
		std::lock_guard<std::mutex> lk(mu);
		tmp.swap(items);
	}
	for (auto &p : tmp)
		p->stop();
	// dtor で順次アンロード
}

std::vector<MultiPluginHost::Item> MultiPluginHost::list() const
{
	std::lock_guard<std::mutex> lk(mu);
	std::vector<Item> out;
	out.reserve(items.size());
	for (const auto &p : items) {
		out.push_back(Item{p->identity(), p->modulePath()});
	}
	return out;
}

} // namespace foxclip::infra_shared::plugin
