#pragma once
#include "features/startup_check/domain/DirectoryCreator.h"

namespace foxclip::startup_check::infrastructure {

using foxclip::startup_check::domain::DirectoryCreator;

class StdFsDirectoryCreator final : public DirectoryCreator {
public:
	bool create_if_missing(const std::string &path, std::error_code &ec) override;
};

} // namespace foxclip::startup_check::infrastructure
