#include <Windows.h>
#include <cstring>
#include "version.h"
#include "context.h"

// External declaration of global context
extern AudioBackendContext* g_context;

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

    __declspec(dllexport) void audio_errorGetLast(char* buffer, int size) {
        // If global context is NULL, do nothing and return
        if (g_context == nullptr) {
            return;
        }

        // Get the last error string from the context
        std::string error = g_context->getLastError();

        // Copy up to size - 1 bytes to the buffer
        if (size > 0) {
            size_t max_copy = static_cast<size_t>(size - 1);
            size_t copy_size = (error.length() < max_copy) ? error.length() : max_copy;
            memcpy(buffer, error.c_str(), copy_size);
            buffer[copy_size] = '\0';  // Null terminate
        }
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
