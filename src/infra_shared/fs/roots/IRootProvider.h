#pragma once
#include <string>

namespace foxclip::infra_shared::fs::roots {

class IRootProvider {
public:
	virtual ~IRootProvider() = default;
	// 末尾セパレータ無しの絶対パス（UTF-8）を返す。失敗時は空文字を返す。
	virtual std::string root() = 0;
};

} // namespace foxclip::infra_shared::fs::roots
