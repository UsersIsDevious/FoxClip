// src/infra_shared/ui/common/domain/UiTypes.h
#pragma once
#include <functional>

namespace foxclip::ui::common {
// UIスレッドで実行される引数なしコールバック
using UiVoidFn = std::function<void()>;
} // namespace foxclip::ui::common
