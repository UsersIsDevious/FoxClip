#pragma once
#include "../domain/IUiFramework.h"
#include <memory>
#include <unordered_map>

namespace foxclip::infra_shared::ui::core::app {

// UI operations coordinator - single point of UI framework access
class UiCoordinator {
public:
	explicit UiCoordinator(std::unique_ptr<domain::IUiFramework> framework);
	~UiCoordinator() = default;

	// Menu management
	void createMenu(const std::string &menuId, const std::string &title);
	void addMenuItem(const std::string &menuId, const domain::MenuItem &item);
	void removeMenu(const std::string &menuId);

	// Window management
	void showWindow(const std::string &windowId, const domain::WindowProperties &props);
	void hideWindow(const std::string &windowId);
	bool isWindowVisible(const std::string &windowId) const;

	// Tools integration
	void addToolsMenuItem(const std::string &title, domain::ActionCallback callback);

	// Lifecycle
	void teardownAll();

	// Singleton access for application-wide coordination
	static UiCoordinator &instance();
	static void initialize(std::unique_ptr<domain::IUiFramework> framework);

private:
	std::unique_ptr<domain::IUiFramework> uiFramework;
	std::unordered_map<std::string, bool> createdMenus;
	std::unordered_map<std::string, bool> visibleWindows;

	static std::unique_ptr<UiCoordinator> instance_;
};

} // namespace foxclip::infra_shared::ui::core::app
