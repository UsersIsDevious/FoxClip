#pragma once
#include <string>
#include <memory>
#include <vector>
#include <optional>
#include "../domain/IPluginHost.h"

namespace foxclip::plugin_host::app {

// Plugin host facade - application layer orchestration
class PluginHostFacade {
public:
	explicit PluginHostFacade(std::unique_ptr<domain::IPluginHost> host,
				  std::unique_ptr<domain::IMultiPluginHost> multiHost);

	// Single plugin operations
	domain::PluginLoadResult loadPlugin(const std::string &modulePathUtf8);
	void unloadPlugin(const std::string &pluginId);
	bool isPluginLoaded(const std::string &pluginId) const;

	// Multi-plugin operations
	std::vector<domain::PluginLoadResult> loadPluginsFromDirectory(const std::string &directoryPath);
	void stopAndUnloadAllPlugins();

	// Query operations
	std::vector<domain::PluginMetadata> getAllLoadedPlugins() const;
	std::optional<domain::PluginMetadata> getPluginMetadata(const std::string &pluginId) const;

private:
	std::unique_ptr<domain::IPluginHost> pluginHost;
	std::unique_ptr<domain::IMultiPluginHost> multiPluginHost;
};

} // namespace foxclip::plugin_host::app
