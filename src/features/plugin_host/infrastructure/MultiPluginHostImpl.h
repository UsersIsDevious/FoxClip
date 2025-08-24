#pragma once
#include "../domain/IPluginHost.h"
#include <memory>
#include <vector>
#include <string>

namespace foxclip::plugin_host::infrastructure {

class MultiPluginHostImpl : public domain::IMultiPluginHost {
public:
	explicit MultiPluginHostImpl(std::unique_ptr<domain::IPluginHost> singleHost);
	~MultiPluginHostImpl() override = default;

	// IMultiPluginHost implementation
	std::vector<domain::PluginLoadResult> loadFromDirectory(const std::string &directoryPath) override;
	void stopAndUnloadAll() override;
	std::vector<domain::PluginMetadata> getAllPlugins() const override;

private:
	std::unique_ptr<domain::IPluginHost> pluginHost;
	std::vector<std::string> loadedPluginIds;

	// Helper methods
	std::vector<std::string> findPluginFiles(const std::string &directoryPath) const;
	bool isPluginFile(const std::string &filename) const;
};

} // namespace foxclip::plugin_host::infrastructure
