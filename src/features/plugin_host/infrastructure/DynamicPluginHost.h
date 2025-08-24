#pragma once
#include "../domain/IPluginHost.h"
#include <unordered_map>
#include <optional>
#include <string>

namespace foxclip::plugin_host::infrastructure {

// Dynamic library plugin host implementation
class DynamicPluginHost : public domain::IPluginHost {
public:
	DynamicPluginHost() = default;
	~DynamicPluginHost() override;

	// IPluginHost implementation
	domain::PluginLoadResult load(const std::string &modulePathUtf8) override;
	void unload(const std::string &pluginId) override;
	void unloadAll() override;
	bool isLoaded(const std::string &pluginId) const override;
	std::optional<domain::PluginMetadata> getMetadata(const std::string &pluginId) const override;

private:
	struct LoadedPlugin {
		domain::PluginMetadata metadata;
		void *handle;
		void (*deinitFn)();
	};

	std::unordered_map<std::string, LoadedPlugin> loadedPlugins;

	// Helper methods
	std::string generatePluginId(const std::string &path) const;
	void closeLibrary(void *handle);
	void *loadLibrary(const std::string &path);
	void *resolveSymbol(void *handle, const char *name);
};

} // namespace foxclip::plugin_host::infrastructure
