#include <common.h>
#include <sdk.h>
#include <mono.h>

#include <MinHook.h>

#pragma comment(lib, "libMinHook-x64-v141-mt.lib")

/// <summary>
/// Injects a hook into the game.
/// </summary>
/// <param name="src"> The original function to hook.</param>
/// <param name="dst">The hook to call before the original.</param>
/// <param name="original">A pointer to the original instructions.</param>
/// <returns>If the hook was injected successfully or not.</returns>
extern "C" 
__declspec(dllexport)
bool
inject_hook(
	_In_ void* src,
	_In_ void* dst,
	_Out_ void** original
)
{
	wait_for_init();
	*original = nullptr;

	bool r = false;

	if (MH_CreateHook(src, dst, original) != MH_OK)
	{
		goto _ret;
	}

	if (MH_EnableHook(src) != MH_OK)
	{
		goto _ret;
	}

	r = true;

_ret:
	return r;
}

/// <summary>
/// Injects a mono hook into the game.
/// </summary>
/// <param name="klass">The name of the class to hook.</param>
/// <param name="method">The name of the method to hook.</param>
/// <param name="method_arg_count">The number of arguments in the method to hook.</param>
/// <param name="dst">The hook to call before the original.</param>
/// <param name="original">A pointer to the original instructions.</param>
/// <returns>If the hook was injected successfully or not.</returns>
extern "C" 
__declspec(dllexport)
bool
inject_hook_mono(
	_In_ const char* klass,
	_In_ const char* method,
	_In_ int method_arg_count,
	_In_ void* dst,
	_Out_ void** original
)
{
	wait_for_init();
	*original = nullptr;

	bool r = false;
	void* m = find_method_rva_by_name(klass, method, method_arg_count);
	if (!m)
	{
		goto _ret;
	}
	

	r = inject_hook(m, dst, original);

_ret:
	return r;
}

/// <summary>
/// Injects our hooks into the game, which provides us all of our functionality.
/// </summary>
bool
inject(
)
{
	bool r = false;
	if (MH_Initialize() != MH_OK)
	{
		goto _ret;
	}

	r = true;

_ret:
	return r;
}