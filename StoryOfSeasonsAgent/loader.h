#pragma once
#include <common.h>
#include <D3D11.h>

#define NAME "SOSM"
#define HWND_TEXT "STORY OF SEASONS: Pioneers of Olive Town"

#define GET_SC_SIG "\xE8\x00\x00\x00\x00\x4C\x8B\xF0\x48\x85\xC0\x0F\x84\x00\x00\x00\x00\x48\x8B\x08"
#define GET_SC_MASK "x????xxxxxxxx????xxx"

#define VT_IDX_PRESENT 8
#define VT_IDX_RESIZE_BUFFERS 13

typedef IDXGISwapChain* (*GetSwapChain)();
typedef void (*Present)(IDXGISwapChain*, size_t, size_t);
typedef HRESULT(*ResizeBuffers)(IDXGISwapChain*, uint32_t, uint32_t, uint32_t, DXGI_FORMAT, uint32_t);
