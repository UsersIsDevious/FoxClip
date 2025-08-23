// src/infra_shared/ui/window/app/WindowFacade.cpp
#include <functional>
#include "infra_shared/ui/window/app/WindowFacade.h"

namespace foxclip::infra_shared::ui::window {

WindowFacade::WindowFacade(std::function<IWindow *()> provider) : windowProvider(std::move(provider)) {}

WindowFacade::~WindowFacade() = default;

void WindowFacade::showHaloWindow()
{
	if (windowProvider) {
		if (IWindow *window = windowProvider()) {
			window->show();
		}
	}
}

} // namespace foxclip::infra_shared::ui::window
