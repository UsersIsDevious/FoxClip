// src/infra_shared/ui/menu/app/ObsMenuRegistry.h
#pragma once
#include <QString>
#include "infra_shared/ui/menu/domain/MenuTypes.h"

namespace foxclip::ui::menu {

class ObsMenuRegistry {
public:
	static void ensureTopLevelMenu(const MenuId &topMenuId, const QString &visibleTitle = QString());

	// 引数を ActionProperties に集約
	static void addMenuAction(const MenuId &topMenuId, ActionProperties props);
	static void teardownAll();

private:
	ObsMenuRegistry() = delete;
};

} // namespace foxclip::ui::menu
