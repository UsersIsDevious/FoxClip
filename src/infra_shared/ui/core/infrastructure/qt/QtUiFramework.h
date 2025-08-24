#pragma once
#include "../../domain/IUiFramework.h"
#include <unordered_map>
#include <memory>

// Forward declarations to avoid Qt header pollution
class QMenu;
class QAction;
class QWidget;

namespace foxclip::infra_shared::ui::core::infrastructure::qt {

// Qt-specific UI framework implementation
class QtUiFramework : public domain::IUiFramework {
public:
	QtUiFramework();
	~QtUiFramework() override;

	// IUiFramework implementation
	void createTopLevelMenu(const std::string &menuId, const std::string &title) override;
	void addMenuItem(const std::string &menuId, const domain::MenuItem &item) override;
	void removeMenu(const std::string &menuId) override;

	void showWindow(const std::string &windowId, const domain::WindowProperties &props) override;
	void hideWindow(const std::string &windowId) override;
	bool isWindowVisible(const std::string &windowId) const override;

	void addToolsMenuItem(const std::string &title, domain::ActionCallback callback) override;
	void teardownAll() override;

private:
	struct MenuData {
		QMenu *menu;
		std::unordered_map<std::string, QAction *> actions;
	};

	struct WindowData {
		QWidget *widget;
		bool visible;
	};

	std::unordered_map<std::string, MenuData> menus;
	std::unordered_map<std::string, WindowData> windows;
	std::vector<QAction *> toolsActions;

	// Helper methods
	void executeOnUiThread(std::function<void()> func);
	QWidget *getMainWindow();
};

} // namespace foxclip::infra_shared::ui::core::infrastructure::qt
