#include "UiCoordinator.h"
#include <stdexcept>

namespace foxclip::infra_shared::ui::core::app {

std::unique_ptr<UiCoordinator> UiCoordinator::instance_;

UiCoordinator::UiCoordinator(std::unique_ptr<domain::IUiFramework> framework) : uiFramework(std::move(framework)) {}

void UiCoordinator::createMenu(const std::string &menuId, const std::string &title)
{
	if (!uiFramework)
		return;

	uiFramework->createTopLevelMenu(menuId, title);
	createdMenus[menuId] = true;
}

void UiCoordinator::addMenuItem(const std::string &menuId, const domain::MenuItem &item)
{
	if (!uiFramework)
		return;

	uiFramework->addMenuItem(menuId, item);
}

void UiCoordinator::removeMenu(const std::string &menuId)
{
	if (!uiFramework)
		return;

	uiFramework->removeMenu(menuId);
	createdMenus.erase(menuId);
}

void UiCoordinator::showWindow(const std::string &windowId, const domain::WindowProperties &props)
{
	if (!uiFramework)
		return;

	uiFramework->showWindow(windowId, props);
	visibleWindows[windowId] = true;
}

void UiCoordinator::hideWindow(const std::string &windowId)
{
	if (!uiFramework)
		return;

	uiFramework->hideWindow(windowId);
	visibleWindows[windowId] = false;
}

bool UiCoordinator::isWindowVisible(const std::string &windowId) const
{
	if (!uiFramework)
		return false;

	return uiFramework->isWindowVisible(windowId);
}

void UiCoordinator::addToolsMenuItem(const std::string &title, domain::ActionCallback callback)
{
	if (!uiFramework)
		return;

	uiFramework->addToolsMenuItem(title, std::move(callback));
}

void UiCoordinator::teardownAll()
{
	if (!uiFramework)
		return;

	uiFramework->teardownAll();
	createdMenus.clear();
	visibleWindows.clear();
}

UiCoordinator &UiCoordinator::instance()
{
	if (!instance_) {
		throw std::runtime_error("UiCoordinator not initialized. Call initialize() first.");
	}
	return *instance_;
}

void UiCoordinator::initialize(std::unique_ptr<domain::IUiFramework> framework)
{
	instance_ = std::make_unique<UiCoordinator>(std::move(framework));
}

} // namespace foxclip::infra_shared::ui::core::app
