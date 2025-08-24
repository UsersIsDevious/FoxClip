#pragma once
#include "infra_shared/common/domain/Result.h"
#include <system_error>
#include <string>

namespace foxclip::startup_check::domain {

struct IDirectoryChecker {
	virtual ~IDirectoryChecker() = default;
	virtual bool existsDir(const std::string &path) const = 0;
};

// Pure domain types - moved service implementation to app layer
struct DirectoryPolicy {
	std::string requiredName;
};

// Forward declaration - implementation moved to separate header
class IDirectoryCreator;

// Use shared result type
using Result = foxclip::infra_shared::common::domain::Result<void>;

} // namespace foxclip::startup_check::domain
