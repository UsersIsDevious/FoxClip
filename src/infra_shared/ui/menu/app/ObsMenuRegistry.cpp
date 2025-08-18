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

// 前提: UIスレッド上で呼ばれること / mtx() がロック済みであること
// 役割: menuMap と実UI(QMenuBar)の両方を見て、対象トップメニューを見つける or 作成して登録する
// 戻り値: menuMap 内の MenuRecord 参照（actions を保持したまま menu を設定/更新）
static MenuRecord &findOrCreateMenuLocked(const MenuId &topMenuId, QMenuBar *bar,
					  const QString *visibleTitleOpt /*nullable*/)
{
	// 1) まず menuMap に既知の QMenu があるならそれを返す
	auto it = menuMap().find(topMenuId);
	if (it != menuMap().end() && it->second.menu) {
		// タイトル更新が要ればこの時点で反映
		if (visibleTitleOpt && !visibleTitleOpt->isEmpty())
			it->second.menu->setTitle(*visibleTitleOpt);
		return it->second;
	}

	// 2) QMenuBar 上に既存があるか objectName で探索
	QMenu *found = nullptr;
	const auto qTopMenuId = QString::fromStdString(topMenuId);
	for (auto *a : bar->actions()) {
		if (auto *m = a->menu()) {
			if (m->objectName() == qTopMenuId) {
				found = m;
				break;
			}
		}
	}

	// 3) 無ければ作成
	if (!found) {
		found = new QMenu(bar);
		// ループ前に作った qTopMenuId を再利用（余分な変換を回避）
		found->setObjectName(qTopMenuId);
		bar->addMenu(found);
	}

	// 4) タイトル更新（作成/既存の分岐から分離して一元化）
	if (visibleTitleOpt && !visibleTitleOpt->isEmpty()) {
		found->setTitle(*visibleTitleOpt);
	} else if (found->title().isEmpty()) {
		// 指定なし かつ 既存タイトル空なら ID を表示名にフォールバック
		found->setTitle(qTopMenuId);
	}

	// 5) menuMap を上書きせずに登録 or 更新（actions は保持）
	auto entry = menuMap().try_emplace(topMenuId).first;
	entry->second.menu = found; // actions は保持
	return entry->second;
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

	// UI操作は常にUIスレッドへ（参照キャプチャは避けて値キャプチャ）
	callUi([topMenuId, visibleTitle]() {
		auto *mw = mainWindow();
		if (!mw)
			return;
		auto *bar = mw->menuBar();
		if (!bar)
			return;

		std::lock_guard<std::mutex> lock(mtx());

		// 検索・作成・タイトル更新を一元処理
		(void)findOrCreateMenuLocked(topMenuId, bar, &visibleTitle);
	});
}

void ObsMenuRegistry::addMenuAction(const MenuId &topMenuId, const ActionId &actionId, const QString &title,
				    UiVoidFn onTriggered, bool checkable)
{
	// ★ UIスレッド内で「メニューの存在保証→アクション追加」までを一気に行う
	callUi([topMenuId, actionId, title, onTriggered = std::move(onTriggered), checkable]() mutable {
		auto *mw = mainWindow();
		if (!mw)
			return;
		auto *bar = mw->menuBar();
		if (!bar)
			return;

		std::lock_guard<std::mutex> lock(mtx());

		// 一元化: メニューを取得（必要なら作成）
		auto &rec = findOrCreateMenuLocked(topMenuId, bar, /*visibleTitleOpt=*/nullptr);

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

		// ★ topMenuId もキャプチャして O(1) で該当アクションを引く
		QObject::connect(act, &QAction::triggered, [topMenuId, actionId](bool) {
			std::lock_guard<std::mutex> lock(mtx());
			const auto menuIt = menuMap().find(topMenuId);
			if (menuIt == menuMap().end())
				return;
			const auto actionIt = menuIt->second.actions.find(actionId);
			if (actionIt != menuIt->second.actions.end() && actionIt->second.callback) {
				actionIt->second.callback(); // UIスレッド内
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
