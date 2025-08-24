#pragma once
#include <string>
#include <vector>
#include <optional>

namespace foxclip::plugin_loader::domain {

struct LoadOptions {
	// すでに何かがロード済みなら、ロード前にアンロードするか
	bool unloadBeforeLoad{true};
	// 一括ロード時、最初の成功で終了する（true）／最後まで試す（false）
	bool stopOnFirstSuccess{true};
};

struct LoadSuccess {
	std::string pluginDir;  // ルート（…/comExampleMyplugin）
	std::string modulePath; // ロードした共有ライブラリのフルパス
	std::string id;         // manifest.id
	std::string name;       // manifest.name
	std::string version;    // manifest.version
};

struct LoadError {
	std::string pluginDir;
	std::string message;
};

struct LoadOneResult {
	bool ok;
	std::optional<LoadSuccess> success;
	std::optional<LoadError> error;

	static LoadOneResult successOf(LoadSuccess s) { return {true, std::make_optional(std::move(s)), std::nullopt}; }
	static LoadOneResult failureOf(LoadError e) { return {false, std::nullopt, std::make_optional(std::move(e))}; }
};

struct LoadAllResult {
	std::vector<LoadSuccess> successes;
	std::vector<LoadError> errors;
};

} // namespace foxclip::plugin_loader::domain
