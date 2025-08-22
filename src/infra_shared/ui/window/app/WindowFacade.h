// src/infra_shared/ui/window/app/WindowFacade.h
#pragma once

#include <memory>
#include "infra_shared/ui/window/domain/IWindow.h"

namespace foxclip::infra_shared::ui::window {

class WindowFacade {
public:
	explicit WindowFacade(std::unique_ptr<IWindow> window);
	~WindowFacade();

	// 最小ユースケース: HaloWorld ウィンドウを開く
	void showHaloWindow();

private:
	std::unique_ptr<IWindow> window;
};

} // namespace foxclip::infra_shared::ui::window
