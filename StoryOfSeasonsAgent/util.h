#pragma once
#include <common.h>
#include <sstream>

#define LOG(X) { \
    std::stringstream ss; \
    ss << "[" << __FUNCTION__ << "] "; \
    ss << X; \
    ss << std::endl; \
    OutputDebugStringA(ss.str().c_str()); \
}

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
);