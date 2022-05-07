#pragma once
#include "common.h"

extern "C"
#ifdef API
__declspec(dllimport)
#else
__declspec(dllexport)
#endif
void* 
find_class_by_name(
	_In_ const char* name
);

extern "C"
#ifdef API
__declspec(dllimport)
#else
__declspec(dllexport)
#endif
size_t
find_field_offset_by_name(
    _In_ const char* klass,
    _In_ const char* field
);

extern "C"
#ifdef API
__declspec(dllimport)
#else
__declspec(dllexport)
#endif
void*
find_method_rva_by_name(
    _In_ const char* klass,
    _In_ const char* method,
    _In_ int arg_count
);