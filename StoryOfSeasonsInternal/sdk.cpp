#include <sdk.h>

PluginManager pm;
GameMetaCache mc;

/// <summary>
/// Registers a new plugin to the SDK. This is exported so it can be easily searched up by external
/// DLLs for any reason.
/// </summary>
/// <param name="plugin">The plugin to register.</param>
SDK_EXPORT void 
register_plugin(
	_In_ Plugin plugin
)
{
	pm.plugins.push_back(plugin);
}
