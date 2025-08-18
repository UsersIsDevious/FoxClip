#pragma once
#include <string>
#include "features/plugin_loader/domain/ManifestValidator.h"

namespace foxclip::plugin_loader::app {

// ホストの SDK/アプリ版とプラットフォームキーを組み立てる
domain::ValidationInput makeValidationInput();

} // namespace foxclip::plugin_loader::app
