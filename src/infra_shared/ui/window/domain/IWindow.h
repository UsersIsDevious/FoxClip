// src/infra_shared/ui/window/domain/IWindow.h
#pragma once

#include <memory>

namespace foxclip::infra_shared::ui::window {

class IWindow {
public:
	virtual ~IWindow() = default;
	// ウィンドウを表示（非モーダル）
	virtual void show() = 0;
};

} // namespace foxclip::infra_shared::ui::window