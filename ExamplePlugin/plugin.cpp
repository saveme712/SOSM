#define API
#include <common.h>
#include <hook.h>
#include <mono.h>

typedef void(*DateManager_Update)(void* date_manager);
typedef int64_t(*UndocumentedFwdTime)(int64_t* time, int64_t useless_1, int64_t useless_2, int64_t useless_3, float seconds);

static DateManager_Update original_DateManager_Update;

/// <summary>
/// This code will be called before the real date manager update function.
/// </summary>
/// <param name="date_manager">The date manager object.</param>
static void
hook_DateManager_Update(
    _In_ void* date_manager
)
{
    // make the game run REALLY fast.
    *access_generic<float>(date_manager, find_field_offset_by_name("DateManager", "TimeScale")) = 10000.f;

    return original_DateManager_Update(date_manager);
}

/// <summary>
/// Injects our hooks into the game.
/// </summary>
void load(
)
{
    inject_hook_mono("DateManager", "Update", 0, &hook_DateManager_Update, (void**)&original_DateManager_Update);
}

BOOL APIENTRY DllMain(HMODULE Module, DWORD ReasonForCall, LPVOID Reserved)
{
    switch (ReasonForCall)
    {
    case DLL_PROCESS_ATTACH:
        later(load);
        break;
    }
    return TRUE;
}

