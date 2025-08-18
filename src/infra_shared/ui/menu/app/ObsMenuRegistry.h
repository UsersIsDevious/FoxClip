// src/infra_shared/ui/menu/app/ObsMenuRegistry.h
#pragma once
#include <QString>
#include "infra_shared/ui/menu/domain/MenuTypes.h"

namespace foxclip::ui::menu {

class ObsMenuRegistry {
public:
	static void ensureTopLevelMenu(const MenuId &topMenuId, const QString &visibleTitle = QString());

	static void addMenuAction(const MenuId &topMenuId, const ActionId &actionId, const QString &title,
				  UiVoidFn onTriggered, bool checkable = false);

	static void teardownAll();

private:
	ObsMenuRegistry() = delete;
};

} // namespace foxclip::ui::menu
