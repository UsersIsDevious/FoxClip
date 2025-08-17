#include "FoxclipPluginHost.h"

#include "foxclip/plugin_api.h" // fc_api_t / init/deinit の型
#include "foxclip/log/log.h"    // 実装側なので通常宣言（FC_USE_VTABLEは無関係）

#include <cstring>
#include <utility>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
using LibHandle = HMODULE;
static void *ResolveSym(LibHandle h, const char *name)
{
	return reinterpret_cast<void *>(::GetProcAddress(h, name));
}
static void CloseLib(LibHandle h)
{
	if (h)
		::FreeLibrary(h);
}

// UTF-8 -> UTF-16
static std::wstring Utf8ToWide(const std::string &s)
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
static void *ResolveSym(LibHandle h, const char *name)
{
	return ::dlsym(h, name);
}
static void CloseLib(LibHandle h)
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
	foxclip_plugin_deinit_fn deinit = nullptr;
} g_state;

/* ホスト側がプラグインへ渡す vtable を構築 */
static fc_api_t MakeApi()
{
	fc_api_t api{};
	api.abi = FC_API_ABI;
	api.log_set_min_level = &fc_log_set_min_level;
	api.log = &fc_log;
	api.log_v = &fc_log_v;

	// 将来イベントAPIを実装したらここでセット
	api.register_event_handler = nullptr;
	api.unregister_event_handler = nullptr;
	return api;
}

/* 実際のロード + init 呼び出し（ハンドルが既にある前提） */
static bool InitWithHandle(LibHandle h)
{
	if (!h)
		return false;

	auto init = reinterpret_cast<foxclip_plugin_init_fn>(ResolveSym(h, kInitSym));
	if (!init)
		return false;

	fc_api_t api = MakeApi();
	if (!init(&api))
		return false;

	// deinit は任意（存在しない実装も許容）
	auto deinit = reinterpret_cast<foxclip_plugin_deinit_fn>(ResolveSym(h, kDeinitSym));

	g_state.handle = h;
	g_state.deinit = deinit;
	return true;
}

} // namespace

namespace foxclip::Host {

bool IsLoaded()
{
	return g_state.handle != nullptr;
}

bool Load(const std::string &modulePathUtf8)
{
	// すでにロード済みなら一旦降ろす（多重ロード回避）
	if (IsLoaded())
		Unload();

#if defined(_WIN32)
	std::wstring w = Utf8ToWide(modulePathUtf8);
	if (w.empty())
		return false;
	HMODULE h = ::LoadLibraryW(w.c_str());
	if (!h)
		return false;
	if (!InitWithHandle(h)) {
		::FreeLibrary(h);
		return false;
	}
	return true;
#else
	void *h = ::dlopen(modulePathUtf8.c_str(), RTLD_NOW);
	if (!h)
		return false;
	if (!InitWithHandle(h)) {
		::dlclose(h);
		return false;
	}
	return true;
#endif
}

#ifdef _WIN32
bool LoadW(const wchar_t *modulePathW)
{
	if (IsLoaded())
		Unload();

	if (!modulePathW || !modulePathW[0])
		return false;
	HMODULE h = ::LoadLibraryW(modulePathW);
	if (!h)
		return false;
	if (!InitWithHandle(h)) {
		::FreeLibrary(h);
		return false;
	}
	return true;
}
#endif

void Unload()
{
	if (!g_state.handle)
		return;

	if (g_state.deinit) {
		// プラグイン側のクリーンアップ（登録ハンドラ解除 など）
		g_state.deinit();
	}

	CloseLib(g_state.handle);
	g_state.handle = nullptr;
	g_state.deinit = nullptr;
}

} // namespace foxclip::Host
