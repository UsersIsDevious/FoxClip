#pragma once
#include <string>
#include <optional>
#include "foxclip/plugin_api.h"

namespace foxclip::infra_shared::plugin {

struct ModuleHandles {
	void *handle{nullptr}; // HMODULE / void*
	FoxclipPluginInitFn init{nullptr};
	FoxclipPluginDeinitFn deinit{nullptr};
	std::string modulePath; // UTF-8
};

class ModuleLoader {
public:
	// 成功時 ModuleHandles を返す。失敗時 std::nullopt。
	static std::optional<ModuleHandles> load(const std::string &modulePathUtf8);
	static void unload(ModuleHandles &mod) noexcept;

private:
	static void *openLibrary(const std::string &utf8Path);
	static void *resolve(void *handle, const char *symbol);
	static void closeLibrary(void *handle) noexcept;
};

} // namespace foxclip::infra_shared::plugin
