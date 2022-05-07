#include <common.h>

extern "C"
__declspec(dllexport)
void
wait_for_init(
)
{
	while (!mc.initialized)
	{
		Sleep(100);
	}
}