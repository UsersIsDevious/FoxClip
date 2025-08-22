// src/infra_shared/ui/window/app/WindowFacade.cpp
#include "infra_shared/ui/window/app/WindowFacade.h"

namespace foxclip::infra_shared::ui::window {

WindowFacade::WindowFacade(std::unique_ptr<IWindow> window) : window_(std::move(window)) {}

WindowFacade::~WindowFacade() = default;

void WindowFacade::showHaloWindow()
{
	if (window_) {
		window_->show();
	}
}

} // namespace foxclip::infra_shared::ui::window