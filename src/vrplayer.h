#ifndef VRPLAYER_H
#define VRPLAYER_H

#include "vrstructs.h"

#ifdef __cplusplus
extern "C" {
#endif

// Set player position in 3D space
int setPlayerPosition(float width, float depth, float height);

// Set player rotation (orientation) in 3D space
int setPlayerRotation(const UnitVector3D* front, const UnitVector3D* up);

#ifdef __cplusplus
}
#endif

#endif // VRPLAYER_H
