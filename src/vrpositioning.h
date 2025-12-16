#ifndef VRPOSITIONING_H
#define VRPOSITIONING_H

#include "vrobj.h"
#include "vrstructs.h"

#ifdef __cplusplus
extern "C" {
#endif

// Update the sound position of a VR object based on player position
// For wide objects, calculates the closest point on the object to the player
// For non-wide objects, simply copies center to sound_position
void updateObjectSoundPosition(VRObject* obj, Position3D* playerPos);

#ifdef __cplusplus
}
#endif

#endif // VRPOSITIONING_H
