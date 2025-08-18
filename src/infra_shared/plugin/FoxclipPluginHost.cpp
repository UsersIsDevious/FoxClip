#include "FoxclipPluginHost.h"

#include "foxclip/plugin_api.h" // FoxclipApi / init/deinit の型
#include "foxclip/log/log.h"    // ログ関数

#include <cstring>
#include <utility>

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

// UTF-8 -> UTF-16
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
#endif

namespace {
constexpr const char *kInitSym = "foxclip_plugin_init";
constexpr const char *kDeinitSym = "foxclip_plugin_deinit";

/* ロード状態 */
struct State {
	LibHandle handle = nullptr;
	FoxclipPluginDeinitFn deinit = nullptr;
} state;

/* ホスト側がプラグインへ渡す vtable を構築 */
static FoxclipApi makeApi()
{
	FoxclipApiV1 api{};
	api.abi = FOXCLIP_API_ABI;
	api.logSetMinLevel = &foxclipLogSetMinLevel;
	api.log = &foxclipLog;
	api.logVa = &foxclipLogVa;

	// 将来イベントAPIを実装したらここでセット
	api.registerEventHandler = nullptr;
	api.unregisterEventHandler = nullptr;
	return api;
}

/* 実際のロード + init 呼び出し（ハンドルが既にある前提） */
static bool initWithHandle(LibHandle h)
{
	if (!h)
		return false;

	auto init = reinterpret_cast<FoxclipPluginInitFn>(resolveSym(h, kInitSym));
	if (!init)
		return false;

	FoxclipApi api = makeApi();
	if (!init(&api))
		return false;

	// deinit は任意（存在しない実装も許容）
	auto deinit = reinterpret_cast<FoxclipPluginDeinitFn>(resolveSym(h, kDeinitSym));

	state.handle = h;
	state.deinit = deinit;
	return true;
}

} // namespace

namespace foxclip::Host {

bool isLoaded()
{
	return state.handle != nullptr;
}

bool load(const std::string &modulePathUtf8)
{
	// すでにロード済みなら一旦降ろす（多重ロード回避）
	if (isLoaded())
		unload();

#if defined(_WIN32)
	std::wstring w = utf8ToWide(modulePathUtf8);
	if (w.empty())
		return false;
	HMODULE h = ::LoadLibraryW(w.c_str());
	if (!h)
		return false;
	if (!initWithHandle(h)) {
		::FreeLibrary(h);
		return false;
	}
	return true;
#else
	void *h = ::dlopen(modulePathUtf8.c_str(), RTLD_NOW);
	if (!h)
		return false;
	if (!initWithHandle(h)) {
		::dlclose(h);
		return false;
	}
	return true;
#endif
}

#ifdef _WIN32
bool loadW(const wchar_t *modulePathW)
{
	if (isLoaded())
		unload();

	if (!modulePathW || !modulePathW[0])
		return false;
	HMODULE h = ::LoadLibraryW(modulePathW);
	if (!h)
		return false;
	if (!initWithHandle(h)) {
		::FreeLibrary(h);
		return false;
	}
	return true;
}
#endif

void unload()
{
	if (!state.handle)
		return;

	if (state.deinit) {
		// プラグイン側のクリーンアップ（登録ハンドラ解除など）
		state.deinit();
	}

	closeLib(state.handle);
	state.handle = nullptr;
	state.deinit = nullptr;
}

} // namespace foxclip::Host
