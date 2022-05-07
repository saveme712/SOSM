#include "common.h"
#include <stdio.h>
#include <iostream>

#define EXECUTABLE "STORY OF SEASONS Pioneers of Olive Town.exe"
#define AGENT "StoryOfSeasonsAgent.dll"

/// <summary>
/// Injects a DLL into the game process by starting a new thread at LoadLibrary.
/// </summary>
/// <param name="process">The process to inject into.</param>
/// <param name="dll">The DLL to inject.</param>
static void 
inject_dll(
    _In_ HANDLE process,
    _In_ const char* dll
)
{
    LPVOID memory;
    HANDLE thr;

    memory = VirtualAllocEx(process, nullptr, strlen(dll) + 1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    WriteProcessMemory(process, memory, dll, strlen(dll) + 1, nullptr);
    thr = CreateRemoteThread(process, nullptr, NULL, LPTHREAD_START_ROUTINE(LoadLibraryA), memory, NULL, nullptr);

}

/// <summary>
/// Injects all plugins into the game.
/// </summary>
/// <param name="process">The process to inject into.</param>
/// <param name="dir">The directory containing the plugins.</param>
/// <param name="spec">The search wildcard to use.</param>
/// <returns>If injection was successful or not.</returns>
static bool
inject_plugins(
    _In_ HANDLE process, 
    _In_ const char* dir,
    _In_ const char* spec
)
{
    char full[MAX_PATH] = { 0 };
    strcat_s(full, dir);
    strcat_s(full, "\\");
    strcat_s(full, spec);

    char tmp[MAX_PATH];

    HANDLE find = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAA ffd;

    find = FindFirstFileA(full, &ffd);
    if (find == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    do 
    {
        if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
        {
            strcpy_s(tmp, dir);
            strcat_s(tmp, "\\");
            strcat_s(tmp, ffd.cFileName);

            std::cout << tmp << std::endl;

            inject_dll(process, tmp);
        }
    } while (FindNextFileA(find, &ffd) != 0);

    if (GetLastError() != ERROR_NO_MORE_FILES) 
    {
        FindClose(find);
        return false;
    }

    FindClose(find);
    find = INVALID_HANDLE_VALUE;

    return true;
}

int 
main(
)
{
	STARTUPINFOA info = { sizeof(info) };
	PROCESS_INFORMATION processInfo;

	char path[128];
	strcpy_s(path, EXECUTABLE);

	char args[128] = { 0 };
	strcat_s(args, "\"");
	strcat_s(args, EXECUTABLE);
	strcat_s(args, "\"");

	if (!CreateProcessA(path, NULL, NULL, NULL, FALSE, 0x0, NULL, NULL, &info, &processInfo))
	{
		goto _ret;
	}

    inject_dll(processInfo.hProcess, AGENT);
    inject_plugins(processInfo.hProcess, "plugins", "*.dll");

_ret:
	return 0;
}
