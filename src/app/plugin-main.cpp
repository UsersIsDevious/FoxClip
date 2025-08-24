#include <QAction>
#include <QObject>
#include <obs-module.h>
#include <obs-frontend-api.h>

#include "features/startup_check/app/StartupCheckFacade.h"
#include "infra_shared/fs/DirectoryLister.h"
#include "infra_shared/fs/roots/ObsConfigRootProvider.h"
#include "infra_shared/fs/PathResolver.h"
#include "infra_shared/log/ObsLogger.h"
#include "infra_shared/log/app/LogService.h"
#include "infra_shared/log/infrastructure/ObsLoggerImpl.h"
#include "infra_shared/config/build/plugin-config.h"
#include "infra_shared/plugin/FoxclipPluginHost.h"
#include "infra_shared/plugin/PluginFolderLogger.h"
#include "features/plugin_loader/app/LoaderFacadeMulti.h"
#include "features/plugin_loader/domain/LoadTypes.h"
#include "infra_shared/plugin/MultiPluginHost.h"

static std::string resolvePluginsRootUtf8(const std::string &pluginDirName)
{
	using foxclip::infra_shared::fs::roots::ObsConfigRootProvider;
	using foxclip::infra_shared::fs::PathResolver;

	ObsConfigRootProvider root;
	PathResolver resolver(root);
	auto full = resolver.toFull(pluginDirName);
	return full.value_or(std::string{});
}

#include "infra_shared/ui/menu/app/ObsMenuRegistry.h"

#include "infra_shared/ui/window/app/WindowFacade.h"
#include "infra_shared/ui/window/infrastructure/qt/QtHelloWindow.h"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

using foxclip::infra_shared::ui::window::WindowFacade;
using foxclip::infra_shared::ui::window::QtHelloWindow;
namespace {
std::unique_ptr<WindowFacade> windowFacade;
QtHelloWindow *helloWindow = nullptr;
} // namespace

static void onOpenHaloWindow(void *)
{
	if (windowFacade) {
		windowFacade->showHaloWindow();
	}
}
// 予約識別子回避＆規約どおり kUpperCamel で定数化
static const char *const kTestOriginalMenuId = "TestOriginal";
static const char *const kTestOriginalActionLogId = "TestOriginal.Log";

static void onLogAction()
{
	OBS_LOG_INFO("Top menu action clicked (version %s)", PLUGIN_VERSION);
}

bool obs_module_load(void)
{
	// Initialize logging system
	using foxclip::infra_shared::log::app::LogService;
	using foxclip::infra_shared::log::infrastructure::ObsLoggerImpl;
	
	auto logger = std::make_unique<ObsLoggerImpl>();
	LogService::initialize(std::move(logger));
	
	// loding message
	OBS_LOG_INFO("plugin loaded successfully (version %s)", PLUGIN_VERSION);

	//checking plugins directory
	using foxclip::startup_check::app::StartupCheckFacade;
	using foxclip::startup_check::infrastructure::StdFsDirectoryChecker;

	const std::string pluginDirName = "foxclip-plugins";

	auto checker = std::make_unique<StdFsDirectoryChecker>();
	StartupCheckFacade facade(std::move(checker), pluginDirName);
	auto r = facade.run();
	if (!r.ok) {
		// Log error Directory does not exist
		OBS_LOG_ERROR("Startup check failed: %s", r.message.c_str());
		// but for now, we just log the error and continue
		// TODO: In the future, add functionality to automatically create the directory if it does not exist
		//return false;
	}

	foxclip::infra_shared::plugin::logPluginSubfolders(pluginDirName);

	{
		using foxclip::plugin_loader::app::LoaderFacadeMulti;

		const std::string pluginsRoot = resolvePluginsRootUtf8(pluginDirName);
		if (!pluginsRoot.empty()) {
			LoaderFacadeMulti m;
			auto res = m.loadAll(pluginsRoot);
			OBS_LOG_INFO("[foxclip] multi-load: %zu success, %zu errors", res.successes.size(),
				     res.errors.size());
		}
	}
	// ▼▼ ここからトップレベルメニューを追加 ▼▼
	// ID は英数字（objectName用）、表示名は日本語
	foxclip::ui::menu::ObsMenuRegistry::ensureTopLevelMenu(
		/*topMenuId=*/kTestOriginalMenuId,
		/*visibleTitle=*/QObject::tr(u8"テストオリジナルメニュー"));

	// メニュー配下にクリック可能な項目を1つ追加（押されたらログ）
	foxclip::ui::menu::ObsMenuRegistry::addMenuAction(
		/*topMenuId=*/kTestOriginalMenuId,
		/*props=*/foxclip::ui::menu::ActionProperties{kTestOriginalActionLogId, QObject::tr(u8"ログ出力"),
							      onLogAction, false});

	// Tools メニューにカスタム QAction を追加
	const char *label = obs_module_text("Tools.Menu.FoxClip");
	void *raw = obs_frontend_add_tools_menu_qaction(label);

	// まず QObject* に静的キャストし、その後 qobject_cast で安全に QAction* へ
	QObject *obj = static_cast<QObject *>(raw);
	QAction *act = qobject_cast<QAction *>(obj);

	if (!act) {
		OBS_LOG_ERROR("Failed to create Tools menu QAction");
		return true;
	}

	QObject::connect(act, &QAction::triggered,
			 []() { OBS_LOG_INFO("QAction triggered (version %s)", PLUGIN_VERSION); });

	//foxclip::Host::Load("../FoxClip-sample-plugin/build/Release/foxclip-plugin.dll");

	// OBS のメインウィンドウ（Qt）を親にする
	QWidget *main = static_cast<QWidget *>(obs_frontend_get_main_window());

	helloWindow = new QtHelloWindow(main, QStringLiteral("HaloWorld"));
	QObject::connect(helloWindow, &QObject::destroyed, [](QObject *) { helloWindow = nullptr; });
	windowFacade = std::make_unique<WindowFacade>([]() { return helloWindow; });

	// Tools メニューに項目追加
	obs_frontend_add_tools_menu_item("Open Halo Window", onOpenHaloWindow, nullptr);

	OBS_LOG_INFO("foxclip: module loaded");

	return true;
}

void obs_module_unload(void)
{
	// Unload all plugins
	foxclip::infra_shared::plugin::MultiPluginHost::instance().stopAndUnloadAll();
	if (helloWindow != nullptr) {
		helloWindow->deleteLater();
		helloWindow = nullptr;
	}
	windowFacade.reset();
	// 追加: 生成した QMenu/QAction を確実に破棄
	foxclip::ui::menu::ObsMenuRegistry::teardownAll();
	OBS_LOG_INFO("plugin unloaded");
}
