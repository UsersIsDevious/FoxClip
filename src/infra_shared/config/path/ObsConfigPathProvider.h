#pragma once
#include <string>

namespace foxclip::infra_shared::config::path {

// 「プラグインの書き込み可能な設定ディレクトリ」を返す
class IConfigPathProvider {
public:
  virtual ~IConfigPathProvider() = default;

  // arg が空ならモジュールのルート、非空ならそのサブディレクトリのフルパス
  // 取得に失敗したら空文字を返す（例外は投げない）
  virtual std::string config_path(const std::string& subdir) = 0;
};

} // namespace foxclip::infra_shared::config
