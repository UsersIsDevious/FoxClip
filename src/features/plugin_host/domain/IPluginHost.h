#pragma once
#include <string>
#include <memory>
#include <optional>
#include <vector>

namespace foxclip::plugin_host::domain {

// Plugin loading result
struct PluginLoadResult {
	bool success;
	std::string errorMessage;

	static PluginLoadResult ok() { return {true, {}}; }
	static PluginLoadResult failure(std::string msg) { return {false, std::move(msg)}; }
};

// Plugin metadata
struct PluginMetadata {
	std::string id;
	std::string name;
	std::string version;
	std::string path;
	bool isLoaded;
};

// Plugin host port (domain interface)
class IPluginHost {
public:
	virtual ~IPluginHost() = default;

	// Load a single plugin from path
	virtual PluginLoadResult load(const std::string &modulePathUtf8) = 0;

	// Unload a specific plugin
	virtual void unload(const std::string &pluginId) = 0;

	// Unload all plugins
	virtual void unloadAll() = 0;

	// Check if plugin is loaded
	virtual bool isLoaded(const std::string &pluginId) const = 0;

	// Get plugin metadata
	virtual std::optional<PluginMetadata> getMetadata(const std::string &pluginId) const = 0;
};

// Multi-plugin host port
class IMultiPluginHost {
public:
	virtual ~IMultiPluginHost() = default;

	// Load multiple plugins from directory
	virtual std::vector<PluginLoadResult> loadFromDirectory(const std::string &directoryPath) = 0;

	// Stop and unload all plugins
	virtual void stopAndUnloadAll() = 0;

	// Get all loaded plugins
	virtual std::vector<PluginMetadata> getAllPlugins() const = 0;
};

} // namespace foxclip::plugin_host::domain
