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

// BGM API
__declspec(dllimport) int audio_globalSetBgmVolume(float volume);
__declspec(dllimport) int audio_bgmLoad(const void* address, int size);
__declspec(dllimport) int audio_bgmPause(int slot);
__declspec(dllimport) int audio_bgmResume(int slot);
__declspec(dllimport) int audio_bgmStop(int slot);
__declspec(dllimport) int audio_bgmFadeout(int slot, int ms);
__declspec(dllimport) int audio_bgmFadein(int slot, int ms);
__declspec(dllimport) int audio_bgmCrossfade(int slot1, int slot2, int ms);
__declspec(dllimport) int audio_bgmSetLoopPoint(int slot, int ms);
__declspec(dllimport) int audio_bgmPlay(int slot);
__declspec(dllimport) int audio_bgmFree(int slot);

#ifdef __cplusplus
}
#endif

#endif // AUDIO_BACKEND_H
