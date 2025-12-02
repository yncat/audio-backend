#ifndef VROBJ_H
#define VROBJ_H

#include "vrstructs.h"
#include "sound_attributes.h"

#ifdef __cplusplus
extern "C" {
#endif

// VRObjectInfo structure for passing VR object information
typedef struct {
    Position3D position;
    Size3D size;
    const char* looped_sample_key;  // Can be NULL if no looped sound
} VRObjectInfo;

// Play a oneshot sound at a relative position to the listener
// follow: if true, the sound follows the listener's head rotation (FMOD_3D_HEADRELATIVE)
int vrOneshotRelative(const char* sample_key, const Position3D* position3d, SoundAttributes* sound_attributes, bool follow);

// Play a oneshot sound at an absolute world position
int vrOneshotAbsolute(const char* sample_key, const Position3D* position3d, SoundAttributes* sound_attributes);

// Play a oneshot sound at the player's position (for player-emitted sounds)
int vrOneshotPlayer(const char* sample_key, SoundAttributes* sound_attributes);

// VR Object management functions
int vrObjectAdd(const char* key, VRObjectInfo* info);
int vrObjectRemove(const char* key);
int vrObjectStartLooping(const char* key);
int vrObjectPauseLooping(const char* key);
int vrObjectResumeLooping(const char* key);
int vrObjectPlayOneshot(const char* object_key, const char* sample_key, SoundAttributes* attributes);

#ifdef __cplusplus
}

// C++ only structures
#include "fmod/fmod.hpp"

// VRObject structure (internal C++ structure)
struct VRObject {
    Position3D center;
    Size3D size;
    FMOD::Sound* looped_sound;
    FMOD::Channel* looped_channel;
    FMOD::ChannelGroup* channel_group;

    VRObject() : looped_sound(nullptr), looped_channel(nullptr), channel_group(nullptr) {
        center = {0.0f, 0.0f, 0.0f};
        size = {0.0f, 0.0f, 0.0f};
    }
};

#endif

#endif // VROBJ_H
