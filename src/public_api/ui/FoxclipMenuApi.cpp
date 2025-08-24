#include "foxclip/ui/menu.h"
#include "infra_shared/ui/core/app/UiCoordinator.h"
#include <string.h>

using namespace foxclip::infra_shared::ui::core;

namespace {
// Convert C callback to C++ callback
domain::ActionCallback wrapCallback(FoxclipMenuCallback callback, const char *menuId, const char *itemId,
				    void *userData)
{
	if (!callback)
		return nullptr;

	// Capture by value to ensure lifetime safety
	std::string capturedMenuId = menuId ? menuId : "";
	std::string capturedItemId = itemId ? itemId : "";

	return [callback, capturedMenuId, capturedItemId, userData]() {
		callback(capturedMenuId.c_str(), capturedItemId.c_str(), userData);
	};
}

bool validateMenuItem(const FoxclipMenuItem *item)
{
	return item && item->size >= sizeof(FoxclipMenuItem) && item->itemId && item->title && item->callback;
}
} // namespace

extern "C" {

int FOXCLIP_CALL foxclipCreateMenu(const char *menuId, const char *title)
{
	if (!menuId || !title) {
		return FOXCLIP_MENU_E_INVALID_ARG;
	}

	try {
		auto &ui = app::UiCoordinator::instance();
		ui.createMenu(menuId, title);
		return FOXCLIP_MENU_SUCCESS;
	} catch (...) {
		return FOXCLIP_MENU_E_UI_ERROR;
	}
}

int FOXCLIP_CALL foxclipAddMenuItem(const char *menuId, const FoxclipMenuItem *item)
{
	if (!menuId || !validateMenuItem(item)) {
		return FOXCLIP_MENU_E_INVALID_ARG;
	}

	try {
		auto &ui = app::UiCoordinator::instance();

		domain::MenuItem menuItem{};
		menuItem.id = item->itemId;
		menuItem.title = item->title;
		menuItem.onTriggered = wrapCallback(item->callback, menuId, item->itemId, item->userData);
		menuItem.checkable = item->checkable != 0;

		ui.addMenuItem(menuId, menuItem);
		return FOXCLIP_MENU_SUCCESS;
	} catch (...) {
		return FOXCLIP_MENU_E_UI_ERROR;
	}
}

int FOXCLIP_CALL foxclipRemoveMenuItem(const char *menuId, const char *itemId)
{
	if (!menuId || !itemId) {
		return FOXCLIP_MENU_E_INVALID_ARG;
	}

	// TODO: Implement item removal in UiCoordinator
	return FOXCLIP_MENU_E_UI_ERROR;
}

int FOXCLIP_CALL foxclipRemoveMenu(const char *menuId)
{
	if (!menuId) {
		return FOXCLIP_MENU_E_INVALID_ARG;
	}

	try {
		auto &ui = app::UiCoordinator::instance();
		ui.removeMenu(menuId);
		return FOXCLIP_MENU_SUCCESS;
	} catch (...) {
		return FOXCLIP_MENU_E_UI_ERROR;
	}
}

int FOXCLIP_CALL foxclipAddToolsMenuItem(const char *title, FoxclipMenuCallback callback, void *userData)
{
	if (!title || !callback) {
		return FOXCLIP_MENU_E_INVALID_ARG;
	}

	try {
		auto &ui = app::UiCoordinator::instance();
		auto wrappedCallback = wrapCallback(callback, "tools", title, userData);
		ui.addToolsMenuItem(title, wrappedCallback);
		return FOXCLIP_MENU_SUCCESS;
	} catch (...) {
		return FOXCLIP_MENU_E_UI_ERROR;
	}
}

} // extern "C"
