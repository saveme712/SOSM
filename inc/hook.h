#pragma once
#include "common.h"

extern "C"
#ifdef API
__declspec(dllimport)
#else
__declspec(dllexport)
#endif
bool
inject_hook(
	_In_ void* src,
	_In_ void* dst,
	_Out_ void** original
);

extern "C"
#ifdef API
__declspec(dllimport)
#else
__declspec(dllexport)
#endif
bool
inject_hook_mono(
	_In_ const char* klass,
	_In_ const char* method,
	_In_ int method_arg_count,
	_In_ void* dst,
	_Out_ void** original
);

bool inject(
);