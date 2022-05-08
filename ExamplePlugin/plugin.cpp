#define API
#include <common.h>
#include <hook.h>
#include <mono.h>
#include <sdk.h>

typedef void(*FnUpdate)(void* o);
typedef void(*LayerObjectQuadtreeInit)(void* qt, int width, int height, void* fc);

static bool enabled = false;
static float time_scale = 1.f;
static float camera_distance = 100.f;
static float camera_pitch = 55.f;
static float camera_yaw = 90.f;

static FnUpdate original_DateManager_Update;
static FnUpdate original_FollowingCamera_Update;
static LayerObjectQuadtreeInit original_LayerObjectQuadtree_Init;

#pragma pack(push, 1)
template <typename T>
struct NetArray
{
    // 0x0
    char padding000[0x18];
    // 0x18
    int size;
    // 0x1c
    char padding001[0x4];
    // 0x20
    T arr[1];
};

#pragma pack(pop)

static void
hook_DateManager_Update(
    _In_ void* date_manager
)
{
    float s = enabled ? time_scale : 1.f;
    *access_generic<float>(date_manager, find_field_offset_by_name("DateManager", "TimeScale")) = s;

    return original_DateManager_Update(date_manager);
}

static void
hook_FollowingCamera_LateUpdate(
    _In_ void* following_camera
)
{
    *access_generic<float>(following_camera, find_field_offset_by_name("FollowingCamera", "polarAngle")) = camera_pitch;
    *access_generic<float>(following_camera, find_field_offset_by_name("FollowingCamera", "azimuthalAngle")) = camera_yaw;

    auto dist_arr = *access_generic<NetArray<float>*>(following_camera, find_field_offset_by_name("FollowingCamera", "distanceArray"));
    for (int i = 0; i < dist_arr->size; i++)
    {
        dist_arr->arr[i] = camera_distance;
    }

    // force distance update
    auto distance_change = (FnUpdate)find_method_rva_by_name("FollowingCamera", "DistanceChange", 0);
    distance_change(following_camera);

    return original_FollowingCamera_Update(following_camera);
}

static void
hook_LayerObjectQuadtree_Init(
    _In_ void* qt,
    _In_ int width,
    _In_ int height,
    _In_ void* fc
)
{
    original_LayerObjectQuadtree_Init(qt, width, height, fc);

    // disable culling on objects
    // might need to handle this manually for NPCs in another hook..
    *access_generic<bool>(qt, find_field_offset_by_name("LayerObjectQuadtree", "isNotUnregister")) = true;
}

/// <summary>
/// Handles the rendering of our plugin specific menu settings.
/// </summary>
static void
plugin_on_render(
)
{
    sdk_checkbox("Enabled", &enabled);
    if (enabled)
    {
        sdk_input_float("Time Scale", &time_scale, 0.1f, 1.f);
        sdk_input_float("Camera Distance", &camera_distance, 1.f, 5.f);
        sdk_input_float("Camera Pitch", &camera_pitch, 1.f, 5.f);
        sdk_input_float("Camera Yaw", &camera_yaw, 1.f, 5.f);
    }
}

/// <summary>
/// Injects our hooks into the game.
/// </summary>
void load(
)
{
    inject_hook_mono("DateManager", "Update", 0, &hook_DateManager_Update, (void**)&original_DateManager_Update);
    inject_hook_mono("FollowingCamera", "LateUpdate", 0, &hook_FollowingCamera_LateUpdate, (void**)&original_FollowingCamera_Update);
    inject_hook_mono("LayerObjectQuadtree", "Initialize", 3, &hook_LayerObjectQuadtree_Init, (void**)&original_LayerObjectQuadtree_Init);

    Plugin plugin;
    plugin.name = "Example";
    plugin.on_render = plugin_on_render;
    register_plugin(plugin);
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

