// src/infra_shared/ui/window/app/WindowFacade.h
#pragma once
#include <functional>
#include "infra_shared/ui/window/domain/IWindow.h"

namespace foxclip::infra_shared::ui::window {

class WindowFacade {
public:
	explicit WindowFacade(std::function<IWindow *()> provider);
	~WindowFacade();

	// 最小ユースケース: HaloWorld ウィンドウを開く
	void showHaloWindow();

private:
	std::function<IWindow *()> windowProvider;
};

} // namespace foxclip::infra_shared::ui::window
