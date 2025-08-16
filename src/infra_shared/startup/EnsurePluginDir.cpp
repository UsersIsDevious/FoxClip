#include "EnsurePluginDir.h"
#include "infra_shared/fs/roots/ObsConfigRootProvider.h"
#include "infra_shared/fs/PathResolver.h"
#include "infra_shared/fs/FsUtils.h"

namespace foxclip::infra_shared::startup {

EnsureResult ensure_obs_writable_dir(const std::string &subdir)
{
	using foxclip::infra_shared::fs::roots::ObsConfigRootProvider;
	using foxclip::infra_shared::fs::PathResolver;
	using foxclip::infra_shared::fs::create_dirs;

	// 1) OBS ルート取得（…/plugin_config/<plugin>）
	ObsConfigRootProvider root;
	const std::string base = root.root();
	if (base.empty())
		return {false, {}, "failed to resolve OBS config root"};

	// 2) 相対 subdir をルート配下のフルパスへ正規化
	PathResolver resolver(root);
	bool ok = false;
	const std::string full = resolver.to_full(subdir, &ok);
	if (!ok || full.empty())
		return {false, {}, "invalid subdir (absolute or contains ..)"};

	std::error_code ec;
	if (!create_dirs(full, ec)) {
		std::string msg = "failed to create '" + subdir + "'";
		if (ec)
			msg += ": " + ec.message();
		return {false, {}, msg};
	}

	return {true, full, {}};
}

} // namespace foxclip::infra_shared::startup
