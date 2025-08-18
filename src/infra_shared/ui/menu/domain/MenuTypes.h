// src/infra_shared/ui/menu/domain/MenuTypes.h
#pragma once
#include <string>
#include "infra_shared/ui/common/domain/UiTypes.h"

namespace foxclip::ui::menu {

using MenuId = std::string;   // トップレベルメニュー識別子
using ActionId = std::string; // QAction識別子
using UiVoidFn = foxclip::ui::common::UiVoidFn;

// QAction の属性をまとめた構造体
struct ActionProperties {
	ActionId actionId;
	QString title;
	UiVoidFn onTriggered;
	bool checkable = false;
};

} // namespace foxclip::ui::menu
