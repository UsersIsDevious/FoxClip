// src/infra_shared/ui/window/app/WindowFacade.cpp
#include "infra_shared/ui/window/app/WindowFacade.h"

namespace foxclip::infra_shared::ui::window {

WindowFacade::WindowFacade(std::unique_ptr<IWindow> window) : window(std::move(window)) {}

WindowFacade::~WindowFacade() = default;

void WindowFacade::showHaloWindow()
{
	if (window) {
		window->show();
	}
}

} // namespace foxclip::infra_shared::ui::window
