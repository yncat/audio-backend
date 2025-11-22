#include <Windows.h>
#include <cstring>
#include "version.h"
#include "context.h"
#include "bgm.h"
#include "core.h"
#include "sample.h"
#include "vr.h"

// External declaration of global context
extern AudioBackendContext* g_context;

// DLL Export functions
extern "C" {
    // Core lifecycle API functions
    __declspec(dllexport) int audio_coreInitialize() {
        return coreInitialize();
    }

    __declspec(dllexport) void audio_coreFree() {
        coreFree();
    }

    // Version API functions
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
        if (g_context == nullptr || size <= 0) {
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

    // BGM API functions
    __declspec(dllexport) int audio_globalSetBgmVolume(float volume) {
        return globalSetBgmVolume(volume);
    }

    __declspec(dllexport) int audio_bgmLoad(const void* address, int size) {
        return bgmLoad(address, size);
    }

    __declspec(dllexport) int audio_bgmPause(int slot) {
        return bgmPause(slot);
    }

    __declspec(dllexport) int audio_bgmResume(int slot) {
        return bgmResume(slot);
    }

    __declspec(dllexport) int audio_bgmStop(int slot) {
        return bgmStop(slot);
    }

    __declspec(dllexport) int audio_bgmFadeout(int slot, int ms) {
        return bgmFadeout(slot, ms);
    }

    __declspec(dllexport) int audio_bgmFadein(int slot, int ms) {
        return bgmFadein(slot, ms);
    }

    __declspec(dllexport) int audio_bgmCrossfade(int slot1, int slot2, int ms) {
        return bgmCrossfade(slot1, slot2, ms);
    }

    __declspec(dllexport) int audio_bgmSetLoopPoint(int slot, int ms) {
        return bgmSetLoopPoint(slot, ms);
    }

    __declspec(dllexport) int audio_bgmPlay(int slot) {
        return bgmPlay(slot);
    }

    __declspec(dllexport) int audio_bgmFree(int slot) {
        return bgmFree(slot);
    }

    // Sample API functions
    __declspec(dllexport) int audio_sampleLoad(const void* address, int size, const char* key) {
        return sampleLoad(address, size, key);
    }

    __declspec(dllexport) int audio_sampleOneshot(const char* key, SoundAttributes* attributes) {
        return sampleOneshot(key, attributes);
    }

    // VR Audio API functions
    __declspec(dllexport) int audio_vrInitialize(const char* plugin_path) {
        return vrInitialize(plugin_path);
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
