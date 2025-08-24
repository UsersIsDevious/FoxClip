#include "infra_shared/plugin/ModuleLoader.h"
#include <string>
#include <system_error>

#if defined(_WIN32)
#define NOMINMAX
#include <windows.h>
static std::wstring toW(const std::string &u8)
{
	int wlen = MultiByteToWideChar(CP_UTF8, 0, u8.c_str(), -1, nullptr, 0);
	std::wstring ws(wlen, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, u8.c_str(), -1, ws.data(), wlen);
	if (!ws.empty() && ws.back() == L'\0')
		ws.pop_back();
	return ws;
}
#else
#include <dlfcn.h>
#endif

namespace foxclip::infra_shared::plugin {

void *ModuleLoader::openLibrary(const std::string &utf8Path)
{
#if defined(_WIN32)
	std::wstring w = toW(utf8Path);
	HMODULE h = ::LoadLibraryW(w.c_str());
	return (void *)h;
#else
	return ::dlopen(utf8Path.c_str(), RTLD_NOW);
#endif
}

void *ModuleLoader::resolve(void *handle, const char *symbol)
{
#if defined(_WIN32)
	return (void *)::GetProcAddress((HMODULE)handle, symbol);
#else
	return ::dlsym(handle, symbol);
#endif
}

void ModuleLoader::closeLibrary(void *handle) noexcept
{
	if (!handle)
		return;
#if defined(_WIN32)
	::FreeLibrary((HMODULE)handle);
#else
	::dlclose(handle);
#endif
}

std::optional<ModuleHandles> ModuleLoader::load(const std::string &modulePathUtf8)
{
	void *h = openLibrary(modulePathUtf8);
	if (!h)
		return std::nullopt;

	// 既定エクスポート名（プラグイン側で定義）
	auto init = (FoxclipPluginInitFn)resolve(h, "foxclip_plugin_init");
	auto deinit = (FoxclipPluginDeinitFn)resolve(h, "foxclip_plugin_deinit");
	if (!init || !deinit) {
		closeLibrary(h);
		return std::nullopt;
	}
	ModuleHandles out;
	out.handle = h;
	out.init = init;
	out.deinit = deinit;
	out.modulePath = modulePathUtf8;
	return out;
}

void ModuleLoader::unload(ModuleHandles &mod) noexcept
{
	if (mod.handle) {
		closeLibrary(mod.handle);
		mod.handle = nullptr;
	}
	mod.init = nullptr;
	mod.deinit = nullptr;
}

} // namespace foxclip::infra_shared::plugin
