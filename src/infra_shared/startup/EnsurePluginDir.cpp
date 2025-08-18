#include "EnsurePluginDir.h"
#include "infra_shared/fs/roots/ObsConfigRootProvider.h"
#include "infra_shared/fs/PathResolver.h"
#include "infra_shared/fs/FsUtils.h"

namespace foxclip::infra_shared::startup {

EnsureResult ensureObsWritableDir(const std::string &subdir)
{
	using foxclip::infra_shared::fs::roots::ObsConfigRootProvider;
	using foxclip::infra_shared::fs::PathResolver;
	using foxclip::infra_shared::fs::createDirs;

	// 1) OBS ルート取得（…/plugin_config/<plugin>）
	ObsConfigRootProvider root;
	const std::string base = root.root();
	if (base.empty())
		return {false, {}, "failed to resolve OBS config root"};

	// 2) 相対 subdir をルート配下のフルパスへ正規化
	PathResolver resolver(root);
	const auto fullOpt = resolver.toFull(subdir);
	if (!fullOpt || fullOpt->empty())
		return {false, {}, "invalid subdir (absolute or contains ..)"};

	const std::string &full = *fullOpt;

	std::error_code ec;
	if (!createDirs(full, ec)) {
		std::string msg = "failed to create '" + subdir + "'";
		if (ec)
			msg += ": " + ec.message();
		return {false, {}, msg};
	}

	return {true, full, {}};
}

} // namespace foxclip::infra_shared::startup
