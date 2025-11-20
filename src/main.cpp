#include <Windows.h>
#include "version.h"

// Version API functions
extern "C" {
    __declspec(dllexport) int audio_versionGetMajor() {
        return getMajorVersion();
    }

    __declspec(dllexport) int audio_versionGetMinor() {
        return getMinorVersion();
    }

    __declspec(dllexport) int audio_versionGetPatch() {
        return getPatchVersion();
    }
}

// DLL Entry Point
BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        // DLL is being loaded into a process
        break;
    case DLL_THREAD_ATTACH:
        // A new thread is being created
        break;
    case DLL_THREAD_DETACH:
        // A thread is exiting cleanly
        break;
    case DLL_PROCESS_DETACH:
        // DLL is being unloaded from a process
        break;
    }
    return TRUE;
}
