#pragma once
#include <string>
#include "Manifest.h"

namespace foxclip::plugin_loader::domain {

class ManifestReader {
public:
	virtual ~ManifestReader() = default;
	virtual ManifestReadResult readFromDir(const std::string &pluginDirUtf8) = 0; // pluginDir/plugin.json
};

} // namespace foxclip::plugin_loader::domain
