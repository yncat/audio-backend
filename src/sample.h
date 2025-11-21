#ifndef SAMPLE_H
#define SAMPLE_H

#include "sound_attributes.h"

#ifdef BUILDING_DLL
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

DLL_EXPORT int audio_sampleLoad(const void* address, int size, const char* key);
DLL_EXPORT int audio_sampleOneshot(const char* key, SoundAttributes* attributes);

#ifdef __cplusplus
}
#endif

#endif // SAMPLE_H
