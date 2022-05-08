#pragma once
#include "common.h"

SDK_IMPORT_EXPORT void* 
find_class_by_name(
	_In_ const char* name
);

SDK_IMPORT_EXPORT size_t
find_field_offset_by_name(
    _In_ const char* klass,
    _In_ const char* field
);

SDK_IMPORT_EXPORT void*
find_method_rva_by_name(
    _In_ const char* klass,
    _In_ const char* method,
    _In_ int arg_count
);