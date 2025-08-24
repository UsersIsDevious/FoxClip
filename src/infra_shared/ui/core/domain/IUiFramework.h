#pragma once
#include <string>
#include <functional>
#include <memory>

namespace foxclip::infra_shared::ui::core::domain {

// UI action callback type
using ActionCallback = std::function<void()>;

// Menu item definition
struct MenuItem {
	std::string id;
	std::string title;
	ActionCallback onTriggered;
	bool checkable = false;
};

// Window properties
struct WindowProperties {
	std::string title;
	int width = 400;
	int height = 300;
	bool modal = false;
};

// UI framework abstraction port
class IUiFramework {
public:
	virtual ~IUiFramework() = default;

	// Menu operations
	virtual void createTopLevelMenu(const std::string &menuId, const std::string &title) = 0;
	virtual void addMenuItem(const std::string &menuId, const MenuItem &item) = 0;
	virtual void removeMenu(const std::string &menuId) = 0;

	// Window operations
	virtual void showWindow(const std::string &windowId, const WindowProperties &props) = 0;
	virtual void hideWindow(const std::string &windowId) = 0;
	virtual bool isWindowVisible(const std::string &windowId) const = 0;

	// Tools menu integration
	virtual void addToolsMenuItem(const std::string &title, ActionCallback callback) = 0;

	// Cleanup
	virtual void teardownAll() = 0;
};

} // namespace foxclip::infra_shared::ui::core::domain
