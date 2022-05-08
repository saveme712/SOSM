#include <sdk.h>
#include "imgui.h"

SDK_EXPORT bool
sdk_checkbox(
	_In_ const char* text,
	_In_ bool* enabled
)
{
	return ImGui::Checkbox(text, enabled);
}

SDK_EXPORT bool
sdk_button(
	_In_ const char* text
)
{
	return ImGui::Button(text);
}

SDK_EXPORT bool
sdk_input_int(
	_In_ const char* text,
	_Inout_ int* v,
	_In_ int step,
	_In_ int step_fast
)
{
	return ImGui::InputInt(text, v, step, step_fast);
}

SDK_EXPORT bool
sdk_input_float(
	_In_ const char* text,
	_Inout_ float* v,
	_In_ float step,
	_In_ float step_fast
)
{
	return ImGui::InputFloat(text, v, step, step_fast);
}