#pragma once
#include "features/startup_check/domain/DirectoryCreator.h"

namespace foxclip::startup_check::infrastructure {

using foxclip::startup_check::domain::IDirectoryCreator;

class StdFsDirectoryCreator final : public IDirectoryCreator {
public:
	bool createIfMissing(const std::string &path, std::error_code &ec) override;
};

} // namespace foxclip::startup_check::infrastructure
