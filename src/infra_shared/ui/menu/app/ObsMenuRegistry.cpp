// src/infra_shared/ui/menu/app/ObsMenuRegistry.cpp
#include "ObsMenuRegistry.h"

#include <obs-frontend-api.h>
#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QPointer>
#include <QMetaObject>
#include <QThread>

namespace foxclip::ui::menu {

namespace {
struct ActionRecord {
	QPointer<QAction> action;
	UiVoidFn callback;
};

struct MenuRecord {
	QPointer<QMenu> menu;
	std::unordered_map<ActionId, ActionRecord> actions;
};

std::unordered_map<MenuId, MenuRecord> &menuMap()
{
	static std::unordered_map<MenuId, MenuRecord> s;
	return s;
}

std::mutex &mtx()
{
	static std::mutex s;
	return s;
}

// UI スレッドで func を実行（Qt 標準ディスパッチ版）
inline void callUi(std::function<void()> func)
{
	auto *app = QCoreApplication::instance();
	if (!app) { // 念のため：アプリがまだ無いなら同期呼び出し（起動初期など）
		func();
		return;
	}
	// すでに UI スレッドならそのまま実行
	if (QThread::currentThread() == app->thread()) {
		func();
		return;
	}
	// UI スレッドに投げる
	QMetaObject::invokeMethod(app, [f = std::move(func)]() { f(); }, Qt::QueuedConnection);
}

inline QMainWindow *mainWindow()
{
	return reinterpret_cast<QMainWindow *>(obs_frontend_get_main_window());
}

} // namespace

void ObsMenuRegistry::ensureTopLevelMenu(const MenuId &topMenuId, const QString &visibleTitle)
{
	{
		std::lock_guard<std::mutex> lock(mtx());
		if (menuMap().count(topMenuId)) {
			if (auto m = menuMap().at(topMenuId).menu) {
				if (!visibleTitle.isEmpty())
					m->setTitle(visibleTitle);
			}
			return;
		}
	}

	callUi([=]() {
		auto *mw = mainWindow();
		if (!mw)
			return;
		auto *bar = mw->menuBar();
		if (!bar)
			return;

		// 既存検索（objectName）
		for (auto *act : bar->actions()) {
			if (auto *menu = act->menu()) {
				if (menu->objectName() == QString::fromStdString(topMenuId)) {
					std::lock_guard<std::mutex> lock(mtx());
					menuMap()[topMenuId] = MenuRecord{menu, {}};
					if (!visibleTitle.isEmpty())
						menu->setTitle(visibleTitle);
					return;
				}
			}
		}

		// 新規作成
		auto *menu = new QMenu(bar);
		menu->setObjectName(QString::fromStdString(topMenuId));
		menu->setTitle(visibleTitle.isEmpty() ? QString::fromStdString(topMenuId) : visibleTitle);
		bar->addMenu(menu);

		std::lock_guard<std::mutex> lock(mtx());
		menuMap()[topMenuId] = MenuRecord{menu, {}};
	});
}

void ObsMenuRegistry::addMenuAction(const MenuId &topMenuId, const ActionId &actionId, const QString &title,
				    UiVoidFn onTriggered, bool checkable)
{
	ensureTopLevelMenu(topMenuId);

	callUi([=]() mutable {
		std::lock_guard<std::mutex> lock(mtx());
		auto it = menuMap().find(topMenuId);
		if (it == menuMap().end() || !it->second.menu)
			return;

		auto &rec = it->second;

		auto aIt = rec.actions.find(actionId);
		if (aIt != rec.actions.end()) {
			if (aIt->second.action) {
				aIt->second.action->setText(title);
				aIt->second.callback = std::move(onTriggered);
				return;
			} else {
				rec.actions.erase(aIt);
			}
		}

		QAction *act = new QAction(title, rec.menu);
		act->setCheckable(checkable);

		QObject::connect(act, &QAction::triggered, [actionId](bool) {
			std::lock_guard<std::mutex> lock2(mtx());
			for (auto &[mid, mrec] : menuMap()) {
				auto f = mrec.actions.find(actionId);
				if (f != mrec.actions.end() && f->second.callback) {
					f->second.callback(); // UIスレッド内
					break;
				}
			}
		});

		rec.menu->addAction(act);
		rec.actions.emplace(actionId, ActionRecord{act, std::move(onTriggered)});
	});
}

void ObsMenuRegistry::teardownAll()
{
	callUi([]() {
		std::lock_guard<std::mutex> lock(mtx());
		for (auto &[mid, mrec] : menuMap()) {
			if (mrec.menu) {
				for (auto &[aid, arec] : mrec.actions) {
					if (arec.action)
						mrec.menu->removeAction(arec.action);
				}
				if (auto *bar = mrec.menu->parentWidget()) {
					if (auto *menuBar = qobject_cast<QMenuBar *>(bar)) {
						menuBar->removeAction(mrec.menu->menuAction());
					}
				}
				mrec.menu->deleteLater();
			}
		}
		menuMap().clear();
	});
}

} // namespace foxclip::ui::menu
