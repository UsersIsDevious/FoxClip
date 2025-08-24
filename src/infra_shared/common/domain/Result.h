#pragma once
#include <string>
#include <vector>
#include <optional>

namespace foxclip::infra_shared::common::domain {

// Generic result type for operations that can succeed or fail
template<typename T = void> struct Result {
	bool ok;
	std::string message;
	std::optional<T> value;

	static Result success(std::string msg = "") { return {true, std::move(msg), std::nullopt}; }

	static Result success(T val, std::string msg = "")
	{
		return {true, std::move(msg), std::make_optional(std::move(val))};
	}

	static Result failure(std::string msg) { return {false, std::move(msg), std::nullopt}; }
};

// Specialization for void (no value)
template<> struct Result<void> {
	bool ok;
	std::string message;

	static Result success(std::string msg = "") { return {true, std::move(msg)}; }

	static Result failure(std::string msg) { return {false, std::move(msg)}; }
};

// Validation result with errors and warnings
struct ValidationResult {
	bool ok;
	std::vector<std::string> errors;
	std::vector<std::string> warnings;

	static ValidationResult success() { return {true, {}, {}}; }

	static ValidationResult failure(std::vector<std::string> e, std::vector<std::string> w = {})
	{
		return {false, std::move(e), std::move(w)};
	}
};

} // namespace foxclip::infra_shared::common::domain
