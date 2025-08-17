#pragma once
#include <string>

namespace foxclip::startup_check::domain {

struct Result {
	bool ok;
	std::string message;
	static Result success(std::string msg = "") { return {true, std::move(msg)}; }
	static Result failure(std::string msg) { return {false, std::move(msg)}; }
};

} // namespace foxclip::startup_check::domain