// src/infra_shared/ui/window/app/WindowFacade.h
#pragma once

#include "infra_shared/ui/window/domain/IWindow.h"

namespace foxclip::infra_shared::ui::window {

class WindowFacade {
public:
	explicit WindowFacade(IWindow *window);
	~WindowFacade();

	// 最小ユースケース: HaloWorld ウィンドウを開く
	void showHaloWindow();

private:
	IWindow *window{nullptr};
};

} // namespace foxclip::infra_shared::ui::window
