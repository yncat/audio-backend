#ifndef VROBJ_H
#define VROBJ_H

#include "vrstructs.h"
#include "sound_attributes.h"

#ifdef __cplusplus
extern "C" {
#endif

// Play a oneshot sound at a relative position to the listener
// follow: if true, the sound follows the listener's head rotation (FMOD_3D_HEADRELATIVE)
int vrOneshotRelative(const char* sample_key, const Position3D* position3d, SoundAttributes* sound_attributes, bool follow);

// Play a oneshot sound at an absolute world position
int vrOneshotAbsolute(const char* sample_key, const Position3D* position3d, SoundAttributes* sound_attributes);

#ifdef __cplusplus
}
#endif

#endif // VROBJ_H
