// src/infra_shared/ui/window/app/WindowFacade.cpp
#include "infra_shared/ui/window/app/WindowFacade.h"

namespace foxclip::infra_shared::ui::window {

WindowFacade::WindowFacade(IWindow *window) : window(window) {}

WindowFacade::~WindowFacade() = default;

void WindowFacade::showHaloWindow()
{
	if (window != nullptr) {
		window->show();
	}
}

} // namespace foxclip::infra_shared::ui::window
