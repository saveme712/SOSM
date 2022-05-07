#include <common.h>
#include <mono.h>
#include <hook.h>

#include <stdio.h>

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

    r = true;

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

BOOL APIENTRY DllMain(HMODULE Module, DWORD ReasonForCall, LPVOID Reserved)
{
    switch (ReasonForCall)
    {
    case DLL_PROCESS_ATTACH:
        later(boot);
        break;
    }
    return TRUE;
}

