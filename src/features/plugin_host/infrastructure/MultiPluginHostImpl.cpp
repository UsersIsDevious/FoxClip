#include "MultiPluginHostImpl.h"
#include <filesystem>
#include <algorithm>

namespace foxclip::plugin_host::infrastructure {

MultiPluginHostImpl::MultiPluginHostImpl(std::unique_ptr<domain::IPluginHost> singleHost)
	: pluginHost(std::move(singleHost))
{
}

std::vector<domain::PluginLoadResult> MultiPluginHostImpl::loadFromDirectory(const std::string &directoryPath)
{
	std::vector<domain::PluginLoadResult> results;

	if (!pluginHost) {
		results.push_back(domain::PluginLoadResult::failure("Plugin host not initialized"));
		return results;
	}

	std::vector<std::string> pluginFiles = findPluginFiles(directoryPath);

	for (const std::string &filePath : pluginFiles) {
		domain::PluginLoadResult result = pluginHost->load(filePath);
		results.push_back(result);

		if (result.success) {
			// Extract plugin ID from path for tracking
			std::string pluginId = std::filesystem::path(filePath).stem().string();
			loadedPluginIds.push_back(pluginId);
		}
	}

	return results;
}

void MultiPluginHostImpl::stopAndUnloadAll()
{
	if (!pluginHost) {
		return;
	}

	// Unload all tracked plugins
	for (const std::string &pluginId : loadedPluginIds) {
		pluginHost->unload(pluginId);
	}

	loadedPluginIds.clear();
}

std::vector<domain::PluginMetadata> MultiPluginHostImpl::getAllPlugins() const
{
	std::vector<domain::PluginMetadata> plugins;

	if (!pluginHost) {
		return plugins;
	}

	for (const std::string &pluginId : loadedPluginIds) {
		auto metadata = pluginHost->getMetadata(pluginId);
		if (metadata) {
			plugins.push_back(*metadata);
		}
	}

	return plugins;
}

std::vector<std::string> MultiPluginHostImpl::findPluginFiles(const std::string &directoryPath) const
{
	std::vector<std::string> pluginFiles;

	try {
		if (!std::filesystem::exists(directoryPath) || !std::filesystem::is_directory(directoryPath)) {
			return pluginFiles;
		}

		for (const auto &entry : std::filesystem::directory_iterator(directoryPath)) {
			if (entry.is_regular_file()) {
				std::string filename = entry.path().filename().string();
				if (isPluginFile(filename)) {
					pluginFiles.push_back(entry.path().string());
				}
			}
		}
	} catch (const std::filesystem::filesystem_error &) {
		// Return empty vector on filesystem errors
	}

	return pluginFiles;
}

bool MultiPluginHostImpl::isPluginFile(const std::string &filename) const
{
	// Check for common plugin file extensions
#if defined(_WIN32)
	return filename.size() >= 4 && filename.substr(filename.size() - 4) == ".dll";
#elif defined(__APPLE__)
	return filename.size() >= 6 && filename.substr(filename.size() - 6) == ".dylib";
#else
	return filename.size() >= 3 && filename.substr(filename.size() - 3) == ".so";
#endif
}

} // namespace foxclip::plugin_host::infrastructure
