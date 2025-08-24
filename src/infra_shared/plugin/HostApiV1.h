
#pragma once
#include "foxclip/plugin_api.h"

namespace foxclip::infra_shared::plugin {

// 共有の API v1 テーブルを返す（スレッドセーフ・不変）
const FoxclipApi *getHostApiV1();

} // namespace foxclip::infra_shared::plugin
