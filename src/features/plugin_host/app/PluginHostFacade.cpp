#include "PluginHostFacade.h"

namespace foxclip::plugin_host::app {

PluginHostFacade::PluginHostFacade(std::unique_ptr<domain::IPluginHost> host,
				   std::unique_ptr<domain::IMultiPluginHost> multiHost)
	: pluginHost(std::move(host)),
	  multiPluginHost(std::move(multiHost))
{
}

domain::PluginLoadResult PluginHostFacade::loadPlugin(const std::string &modulePathUtf8)
{
	if (!pluginHost) {
		return domain::PluginLoadResult::failure("Plugin host not initialized");
	}
	return pluginHost->load(modulePathUtf8);
}

void PluginHostFacade::unloadPlugin(const std::string &pluginId)
{
	if (pluginHost) {
		pluginHost->unload(pluginId);
	}
}

bool PluginHostFacade::isPluginLoaded(const std::string &pluginId) const
{
	if (!pluginHost) {
		return false;
	}
	return pluginHost->isLoaded(pluginId);
}

std::vector<domain::PluginLoadResult> PluginHostFacade::loadPluginsFromDirectory(const std::string &directoryPath)
{
	if (!multiPluginHost) {
		return {domain::PluginLoadResult::failure("Multi-plugin host not initialized")};
	}
	return multiPluginHost->loadFromDirectory(directoryPath);
}

void PluginHostFacade::stopAndUnloadAllPlugins()
{
	if (multiPluginHost) {
		multiPluginHost->stopAndUnloadAll();
	}
}

std::vector<domain::PluginMetadata> PluginHostFacade::getAllLoadedPlugins() const
{
	if (!multiPluginHost) {
		return {};
	}
	return multiPluginHost->getAllPlugins();
}

std::optional<domain::PluginMetadata> PluginHostFacade::getPluginMetadata(const std::string &pluginId) const
{
	if (!pluginHost) {
		return std::nullopt;
	}
	return pluginHost->getMetadata(pluginId);
}

} // namespace foxclip::plugin_host::app
