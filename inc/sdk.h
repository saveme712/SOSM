#pragma once
#include "common.h"
#include <vector>

/// <summary>
/// A callback for when the game is forwarding a time.
/// </summary>
typedef void(*PluginRender)();

struct Plugin
{
	const char* name;
	/// <summary>
	/// Called to render this plugin's menu contents.
	/// </summary>
	PluginRender on_render = nullptr;
};

struct PluginManager
{
	/// <summary>
	/// All plugins loaded into SOSM SDK.
	/// </summary>
	std::vector<Plugin> plugins;
};

SDK_IMPORT_EXPORT void
register_plugin(
	_In_ Plugin plugin
);

SDK_IMPORT_EXPORT bool
sdk_checkbox(
	_In_ const char* text,
	_In_ bool* enabled
); 

SDK_IMPORT_EXPORT bool
sdk_button(
	_In_ const char* text
);

SDK_IMPORT_EXPORT bool
sdk_input_int(
	_In_ const char* text,
	_Inout_ int* v,
	_In_ int step = 1,
	_In_ int step_fast = 100
);

SDK_IMPORT_EXPORT bool
sdk_input_float(
	_In_ const char* text,
	_Inout_ float* v,
	_In_ float step = 0.f,
	_In_ float step_fast = 0.f
);

extern PluginManager pm;