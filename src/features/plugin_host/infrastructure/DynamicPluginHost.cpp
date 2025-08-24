#include "DynamicPluginHost.h"
#include "foxclip/plugin_api.h"
#include "foxclip/log/log.h"
#include <filesystem>
#include <cstring>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
using LibHandle = HMODULE;

static void *resolveSym(LibHandle h, const char *name)
{
	return reinterpret_cast<void *>(::GetProcAddress(h, name));
}

static void closeLib(LibHandle h)
{
	if (h)
		::FreeLibrary(h);
}

static std::wstring utf8ToWide(const std::string &s)
{
	if (s.empty())
		return std::wstring();
	int wlen = ::MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), nullptr, 0);
	if (wlen <= 0)
		return std::wstring();
	std::wstring w(wlen, L'\0');
	::MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), w.data(), wlen);
	return w;
}

static LibHandle loadLib(const std::string &path)
{
	std::wstring w = utf8ToWide(path);
	if (w.empty())
		return nullptr;
	return ::LoadLibraryW(w.c_str());
}
#else
#include <dlfcn.h>
using LibHandle = void *;

static void *resolveSym(LibHandle h, const char *name)
{
	return ::dlsym(h, name);
}

static void closeLib(LibHandle h)
{
	if (h)
		::dlclose(h);
}

static LibHandle loadLib(const std::string &path)
{
	return ::dlopen(path.c_str(), RTLD_NOW);
}
#endif

namespace foxclip::plugin_host::infrastructure {

namespace {
constexpr const char *kInitSym = "foxclip_plugin_init";
constexpr const char *kDeinitSym = "foxclip_plugin_deinit";

FoxclipApi makeApi()
{
	FoxclipApiV1 api{};
	api.abi = FOXCLIP_API_ABI;
	api.logSetMinLevel = &foxclipLogSetMinLevel;
	api.log = &foxclipLog;
	api.logVa = &foxclipLogVa;
	api.registerEventHandler = nullptr;
	api.unregisterEventHandler = nullptr;
	return api;
}
} // namespace

DynamicPluginHost::~DynamicPluginHost()
{
	unloadAll();
}

domain::PluginLoadResult DynamicPluginHost::load(const std::string &modulePathUtf8)
{
	std::string pluginId = generatePluginId(modulePathUtf8);

	// Check if already loaded
	if (isLoaded(pluginId)) {
		return domain::PluginLoadResult::failure("Plugin already loaded: " + pluginId);
	}

	// Load library
	LibHandle handle = loadLib(modulePathUtf8);
	if (!handle) {
		return domain::PluginLoadResult::failure("Failed to load library: " + modulePathUtf8);
	}

	// Resolve init function
	auto initFn = reinterpret_cast<FoxclipPluginInitFn>(resolveSym(handle, kInitSym));
	if (!initFn) {
		closeLib(handle);
		return domain::PluginLoadResult::failure("Init function not found: " + std::string(kInitSym));
	}

	// Initialize plugin
	FoxclipApi api = makeApi();
	if (!initFn(&api)) {
		closeLib(handle);
		return domain::PluginLoadResult::failure("Plugin initialization failed");
	}

	// Resolve deinit function (optional)
	auto deinitFn = reinterpret_cast<FoxclipPluginDeinitFn>(resolveSym(handle, kDeinitSym));

	// Create metadata
	domain::PluginMetadata metadata{};
	metadata.id = pluginId;
	metadata.name = std::filesystem::path(modulePathUtf8).stem().string();
	metadata.version = "1.0.0"; // TODO: Extract from plugin
	metadata.path = modulePathUtf8;
	metadata.isLoaded = true;

	// Store loaded plugin
	LoadedPlugin plugin{};
	plugin.metadata = metadata;
	plugin.handle = handle;
	plugin.deinitFn = deinitFn;

	loadedPlugins[pluginId] = std::move(plugin);

	return domain::PluginLoadResult::ok();
}

void DynamicPluginHost::unload(const std::string &pluginId)
{
	auto it = loadedPlugins.find(pluginId);
	if (it == loadedPlugins.end()) {
		return;
	}

	const auto &plugin = it->second;

	// Call deinit if available
	if (plugin.deinitFn) {
		plugin.deinitFn();
	}

	// Close library
	closeLib(static_cast<LibHandle>(plugin.handle));

	// Remove from map
	loadedPlugins.erase(it);
}

void DynamicPluginHost::unloadAll()
{
	// Create a copy of plugin IDs to avoid iterator invalidation
	std::vector<std::string> pluginIds;
	pluginIds.reserve(loadedPlugins.size());

	for (const auto &pair : loadedPlugins) {
		pluginIds.push_back(pair.first);
	}

	// Unload each plugin
	for (const std::string &id : pluginIds) {
		unload(id);
	}
}

bool DynamicPluginHost::isLoaded(const std::string &pluginId) const
{
	return loadedPlugins.find(pluginId) != loadedPlugins.end();
}

std::optional<domain::PluginMetadata> DynamicPluginHost::getMetadata(const std::string &pluginId) const
{
	auto it = loadedPlugins.find(pluginId);
	if (it == loadedPlugins.end()) {
		return std::nullopt;
	}
	return it->second.metadata;
}

std::string DynamicPluginHost::generatePluginId(const std::string &path) const
{
	// Use filename without extension as plugin ID
	return std::filesystem::path(path).stem().string();
}

void DynamicPluginHost::closeLibrary(void *handle)
{
	closeLib(static_cast<LibHandle>(handle));
}

void *DynamicPluginHost::loadLibrary(const std::string &path)
{
	return loadLib(path);
}

void *DynamicPluginHost::resolveSymbol(void *handle, const char *name)
{
	return resolveSym(static_cast<LibHandle>(handle), name);
}

} // namespace foxclip::plugin_host::infrastructure
