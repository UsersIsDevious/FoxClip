#pragma once
#include "features/plugin_loader/domain/ManifestReader.h"

namespace foxclip::plugin_loader::infrastructure {

class NlohmannManifestReader final : public foxclip::plugin_loader::domain::ManifestReader {
public:
	foxclip::plugin_loader::domain::ManifestReadResult readFromDir(const std::string &pluginDirUtf8) override;
};

} // namespace foxclip::plugin_loader::infrastructure
