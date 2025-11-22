#ifndef SAMPLE_H
#define SAMPLE_H

#include "sound_attributes.h"

int sampleLoad(const void* address, int size, const char* key);
int sampleOneshot(const char* key, SoundAttributes* attributes);

#endif // SAMPLE_H
