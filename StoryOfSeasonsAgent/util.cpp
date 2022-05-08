#include "util.h"

/// <summary>
/// Searches for a pattern.
/// </summary>
bool
find_pattern(
    _In_ const char* pattern,
    _In_ unsigned char wc,
    _In_ size_t len,
    _In_ void* base,
    _In_ size_t size,
    _Out_ void** found
)
{
    for (size_t i = 0; i < size - len; i++)
    {
        bool was_found = true;
        for (size_t j = 0; j < len; j++)
        {
            if (pattern[j] != wc && pattern[j] != ((const char*)base)[i + j])
            {
                was_found = false;
                break;
            }
        }

        if (was_found)
        {
            *found = (void*)((const char*)base + i);
            return true;
        }
    }

    return false;
}