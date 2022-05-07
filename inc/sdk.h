#pragma once
#include "common.h"
#include <vector>

/// <summary>
/// A callback for when the game is forwarding a time.
/// </summary>
typedef void(*PluginFwdTime)(float* seconds);

struct Plugin
{
	/// <summary>
	/// Called whenever the game is attempting to forward the time.
	/// </summary>
	PluginFwdTime on_fwd_time = nullptr;
};

struct PluginManager
{
	/// <summary>
	/// All plugins loaded into SOSM SDK.
	/// </summary>
	std::vector<Plugin> plugins;
};

extern PluginManager pm;