#pragma once
#include <string>
#include <system_error>

namespace foxclip::startup_check::domain {

class IDirectoryCreator {
public:
	virtual ~IDirectoryCreator() = default;
	virtual bool createIfMissing(const std::string &path, std::error_code &ec) = 0;
};

} // namespace foxclip::startup_check::domain
