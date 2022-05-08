#include "loader.h"
#include <mono.h>
#include <hook.h>
#include <sdk.h>

#include <stdio.h>

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include "util.h"

static GetSwapChain get_sc;
static Present original_present;
static ResizeBuffers original_resize_buffers;
static WNDPROC original_wndproc;

static IDXGISwapChain* swap_chain;
static ID3D11Device* device;
static ID3D11DeviceContext* context;
static ID3D11RenderTargetView* render_target_view = NULL;

static bool resize_requested = false;

/// <summary>
/// Creates the render target for rendering our custom visuals.
/// </summary>
static void
create_render_target(
)
{
	ID3D11Texture2D* back_buf;
	swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buf));
	device->CreateRenderTargetView(back_buf, NULL, &render_target_view);
	back_buf->Release();
}

/// <summary>
/// Releases and zeros out the current render target. This should be done on window resize.
/// </summary>
static void
cleanup_render_target(
)
{
	if (render_target_view)
	{
		render_target_view->Release();
		render_target_view = NULL;
	}
}

/// <summary>
/// Our wndproc hook. This allows us to process input.
/// </summary>
static LRESULT
hook_wndproc(
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam
)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}

	return original_wndproc(hWnd, msg, wParam, lParam);
}

/// <summary>
/// Our present hook. The mod menu rendering will be done here.
/// </summary>
static void
hook_present(
	IDXGISwapChain* swap_chain,
	size_t who_cares_1,
	size_t who_cares_2
)
{
	ID3D11RenderTargetView* orig_render_target;
	ID3D11DepthStencilView* orig_depth_stencil;

	if (!render_target_view)
	{
		create_render_target();
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (ImGui::Begin(NAME))
	{
		if (ImGui::BeginTabBar("Plugins"))
		{
			for (auto& plugin : pm.plugins)
			{
				if (ImGui::BeginTabItem(plugin.name))
				{
					plugin.on_render();
					ImGui::EndTabItem();
				}
			}
			ImGui::EndTabBar();
		}
	}

	ImGui::End();
	ImGui::Render();

	context->OMGetRenderTargets(1, &orig_render_target, &orig_depth_stencil);
	context->OMSetRenderTargets(1, &render_target_view, NULL);

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	context->OMSetRenderTargets(1, &orig_render_target, orig_depth_stencil);
	return original_present(swap_chain, who_cares_1, who_cares_2);
}

/// <summary>
/// Our resize buffers hook.
/// </summary>
static HRESULT
hook_resize_buffers(
	IDXGISwapChain* swap_chain,
	uint32_t buffer_count,
	uint32_t width,
	uint32_t height,
	DXGI_FORMAT new_format,
	uint32_t swap_chain_flags
)
{
	cleanup_render_target();
	auto& io = ImGui::GetIO();
	io.DisplaySize.x = (float)width;
	io.DisplaySize.y = (float)height;
	return original_resize_buffers(swap_chain, buffer_count, width, height, new_format, swap_chain_flags);
}


/// <summary>
/// Hooks a function in a virtual table.
/// </summary>
static void
hook_vt(
	_Inout_ void** vt,
	_In_ size_t idx,
	_In_ void* hook,
	_In_ void** orig
)
{
	DWORD old_protect;

	*orig = vt[idx];
	VirtualProtect(&vt[idx], sizeof(void*), PAGE_READWRITE, &old_protect);
	vt[idx] = (void*)hook;
	VirtualProtect(&vt[idx], sizeof(void*), old_protect, &old_protect);
}

/// <summary>
/// Initializes direct-x rendering, and places hooks related to rendering.
/// </summary>
static bool
init_dx(
)
{
	size_t get_sc_ins;
	int32_t offset;
	void** vt;
	DWORD old_protect;
	HWND hwnd;
	bool r = false;

	if (!find_pattern(GET_SC_SIG, '\x00', sizeof(GET_SC_SIG) - 1, (void*)mc.base_unityplayer, 0x100000000, (void**)&get_sc_ins))
	{
		LOG("Failed to find get_sc signature");
		goto _ret;
	}

	offset = *((int32_t*)(get_sc_ins + 1));
	get_sc = (decltype(get_sc))(get_sc_ins + 5 + offset);
	while (!(swap_chain = get_sc()))
	{
		Sleep(100);
	}

	vt = *((void***)swap_chain);

	swap_chain->GetDevice(__uuidof(ID3D11Device), (void**)&device);
	device->GetImmediateContext(&context);
	create_render_target();

	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	hwnd = FindWindowA(NULL, HWND_TEXT);

	original_wndproc = (WNDPROC)GetWindowLongPtrW(hwnd, GWLP_WNDPROC);
	if (!original_wndproc)
	{
		LOG("Failed to find wndproc");
		goto _ret;
	}

	SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)hook_wndproc);

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(device, context);

	hook_vt(vt, VT_IDX_PRESENT, (void*)hook_present, (void**)&original_present);
	hook_vt(vt, VT_IDX_RESIZE_BUFFERS, (void*)hook_resize_buffers, (void**)&original_resize_buffers);
	r = true;

_ret:
	return r;
}

/// <summary>
/// Creates SOSM initialization data.
/// </summary>
/// <returns>If the initialization data was created successfully.</returns>
static bool
init(
)
{
	bool r = false;

	mc.base_gameassembly = GetModuleHandleW(L"GameAssembly.dll");
	if (!mc.base_gameassembly)
	{
		goto _ret;
	}

	mc.base_unityplayer = GetModuleHandleW(L"UnityPlayer.dll");
	if (!mc.base_unityplayer)
	{
		goto _ret;
	}

	mc.il2cpp_domain_get = (decltype(mc.il2cpp_domain_get))
		GetProcAddress(mc.base_gameassembly, "il2cpp_domain_get");

	mc.il2cpp_domain_get_assemblies = (decltype(mc.il2cpp_domain_get_assemblies))
		GetProcAddress(mc.base_gameassembly, "il2cpp_domain_get_assemblies");

	mc.il2cpp_assembly_get_image = (decltype(mc.il2cpp_assembly_get_image))
		GetProcAddress(mc.base_gameassembly, "il2cpp_assembly_get_image");

	mc.il2cpp_image_get_class_count = (decltype(mc.il2cpp_image_get_class_count))
		GetProcAddress(mc.base_gameassembly, "il2cpp_image_get_class_count");

	mc.il2cpp_image_get_class = (decltype(mc.il2cpp_image_get_class))
		GetProcAddress(mc.base_gameassembly, "il2cpp_image_get_class");

	mc.il2cpp_class_get_name = (decltype(mc.il2cpp_class_get_name))
		GetProcAddress(mc.base_gameassembly, "il2cpp_class_get_name");

	mc.il2cpp_class_get_field_from_name = (decltype(mc.il2cpp_class_get_field_from_name))
		GetProcAddress(mc.base_gameassembly, "il2cpp_class_get_field_from_name");

	mc.il2cpp_class_get_method_from_name = (decltype(mc.il2cpp_class_get_method_from_name))
		GetProcAddress(mc.base_gameassembly, "il2cpp_class_get_method_from_name");

	mc.il2cpp_field_get_offset = (decltype(mc.il2cpp_field_get_offset))
		GetProcAddress(mc.base_gameassembly, "il2cpp_field_get_offset");

	r = init_dx();

_ret:
	return r;
}

/// <summary>
/// Boots SOSM.
/// </summary>
/// <returns>If we booted successfully.</returns>
static bool
boot(
)
{
	bool r = false;
	while (!init())
	{
		Sleep(1000);
	}

	r = inject();
	if (!r)
	{
		goto _ret;
	}

	mc.initialized = true;
_ret:
	return r;
}

BOOL APIENTRY
DllMain(
	_In_ HMODULE mod,
	_In_ DWORD reason,
	_In_ LPVOID res
)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		later(boot);
		break;
	}
	return TRUE;
}

