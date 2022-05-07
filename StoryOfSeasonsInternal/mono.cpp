#include <mono.h>

/// <summary>
/// Iterates all classes in the current domain.
/// </summary>
template <typename T> void
iterate_all_classes(
    _In_ T func
)
{
    void* domain;
    uint64_t count;
    void** assemblies;
    void* img;
    size_t class_count;
    void* klass;

    domain = mc.il2cpp_domain_get();
    count = 0ull;
    assemblies = mc.il2cpp_domain_get_assemblies(domain, &count);
    for (auto i = 0ull; i < count; i++)
    {
        img = mc.il2cpp_assembly_get_image(assemblies[i]);
        class_count = mc.il2cpp_image_get_class_count(img);
        for (auto j = 0ull; j < class_count; j++)
        {
            klass = mc.il2cpp_image_get_class(img, j);
            func(klass);
        }
    }
}

/// <summary>
/// Finds a .NET class by its name.
/// </summary>
/// <param name="name">The name of the class to find.</param>
/// <returns>The found class.</returns>
extern "C"
__declspec(dllexport)
void* 
find_class_by_name(
    _In_ const char* name
)
{
    void* found = nullptr;
    iterate_all_classes([&found, name](auto klass) 
    {
        if (!strcmp(mc.il2cpp_class_get_name(klass), name))
        {
            found = klass;
        }
    });

    return found;
}

/// <summary>
/// Finds a .NET field offset by its name.
/// </summary>
/// <param name="name">The name of the class to find.</param>
/// <returns>The found class.</returns>
extern "C"
__declspec(dllexport)
size_t
find_field_offset_by_name(
    _In_ const char* klass,
    _In_ const char* field
)
{
    size_t found = 0;
    void* f = nullptr;
    void* k = find_class_by_name(klass);
    if (k)
    {
        f = mc.il2cpp_class_get_field_from_name(k, field);
        if (f)
        {
            found = mc.il2cpp_field_get_offset(f);
        }
    }

    return found;
}

/// <summary>
/// Finds a .NET method RVA by its name.
/// </summary>
/// <param name="name">The name of the class to find.</param>
/// <returns>The found class.</returns>
extern "C"
__declspec(dllexport)
void*
find_method_rva_by_name(
    _In_ const char* klass,
    _In_ const char* method,
    _In_ int arg_count
)
{
    void* found = nullptr;
    void* m = nullptr;
    void* k = find_class_by_name(klass);
    if (k)
    {
        m = mc.il2cpp_class_get_method_from_name(k, method, arg_count);
        if (m)
        {
            found = *((void**)m);
        }
    }

    return found;
}