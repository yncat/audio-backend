#ifndef AUDIO_BACKEND_H
#define AUDIO_BACKEND_H

#ifdef __cplusplus
extern "C" {
#endif

// Version API
__declspec(dllimport) int audio_versionGetMajor();
__declspec(dllimport) int audio_versionGetMinor();
__declspec(dllimport) int audio_versionGetPatch();

#ifdef __cplusplus
}
#endif

#endif // AUDIO_BACKEND_H
