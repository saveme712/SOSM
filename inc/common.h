#pragma once
#include <Windows.h>
#include <cstdint>

#ifdef API
#define SDK_IMPORT_EXPORT \
extern "C" \
__declspec(dllimport)
#else
#define SDK_IMPORT_EXPORT \
extern "C" \
__declspec(dllexport)
#endif

#define SDK_EXPORT extern "C" \
__declspec(dllexport)

typedef void* (*Il2cppDomainGet)();
typedef void** (*Il2cppDomainGetAssemblies)(void* domain, uint64_t* count);
typedef void* (*Il2cppAssemblyGetImage)(void* image);
typedef size_t (*Il2cppImageGetClassCount)(void* image);
typedef void* (*Il2cppImageGetClass)(void* image, size_t index);
typedef const char* (*Il2cppClassGetName)(void* klass);
typedef void* (*Il2cppClassGetFieldFromName)(void* klass, const char* name);
typedef void* (*Il2cppClassGetMethodFromName)(void* klass, const char* name, int argCount);
typedef size_t(*Il2cppFieldGetOffset)(void* field);

/// <summary>
/// Contains a cache of information related to game metadata.
/// </summary>
struct GameMetaCache
{
	bool initialized = false;

	HMODULE base_unityplayer;
	HMODULE base_gameassembly;

	Il2cppDomainGet il2cpp_domain_get;
	Il2cppDomainGetAssemblies il2cpp_domain_get_assemblies;
	Il2cppAssemblyGetImage il2cpp_assembly_get_image;
	Il2cppImageGetClassCount il2cpp_image_get_class_count;
	Il2cppImageGetClass il2cpp_image_get_class;
	Il2cppClassGetName il2cpp_class_get_name;
	Il2cppClassGetFieldFromName il2cpp_class_get_field_from_name;
	Il2cppClassGetMethodFromName il2cpp_class_get_method_from_name;
	Il2cppFieldGetOffset il2cpp_field_get_offset;
};

/// <summary>
/// Accesses a generic piece of data from some memory.
/// </summary>
template <typename T>
__forceinline T*
access_generic(
	_In_ void* ptr,
	_In_ int64_t offset
)
{
	return (T*)((char*)ptr + offset);
}

/// <summary>
/// Reads a generic piece of data from some memory.
/// </summary>
template <typename T>
__forceinline T 
read_generic(
	_In_ void* ptr,
	_In_ int64_t offset
)
{
	return *(T*)((char*)ptr + offset);
}

extern GameMetaCache mc;

/// <summary>
/// Waits for the cache to initialize.
/// </summary>
extern "C"
#ifdef API
__declspec(dllimport)
#else
__declspec(dllexport)
#endif
void
wait_for_init(
);

/// <summary>
/// Runs some code later.
/// </summary>
/// <param name="func">The function to run.</param>
__forceinline void
later(
	void* func
)
{
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)func, NULL, 0, NULL);
}