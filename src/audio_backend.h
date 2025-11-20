#ifndef AUDIO_BACKEND_H
#define AUDIO_BACKEND_H

#ifdef __cplusplus
extern "C" {
#endif

// Core API
__declspec(dllimport) int audio_coreInitialize();
__declspec(dllimport) void audio_coreFree();

// Version API
__declspec(dllimport) int audio_versionGetMajor();
__declspec(dllimport) int audio_versionGetMinor();
__declspec(dllimport) int audio_versionGetPatch();

// Error API
__declspec(dllimport) void audio_errorGetLast(char* buffer, int size);

#ifdef __cplusplus
}
#endif

#endif // AUDIO_BACKEND_H
