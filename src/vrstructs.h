#ifndef VRSTRUCTS_H
#define VRSTRUCTS_H

#include "fmod/fmod.hpp"

#ifdef __cplusplus
extern "C" {
#endif

// 3D position structure
// Maps to FMOD coordinate system: width->X, depth->Z, height->Y
typedef struct {
    int width;   // X in FMOD
    int depth;   // Z in FMOD
    int height;  // Y in FMOD
} Position3D;

// Unit vector structure for rotations
// Maps to FMOD coordinate system: width->X, depth->Z, height->Y
typedef struct {
    float width;   // X in FMOD
    float depth;   // Z in FMOD
    float height;  // Y in FMOD
} UnitVector3D;

// Listener attributes structure
typedef struct {
    FMOD_VECTOR pos;
    FMOD_VECTOR vel;
    FMOD_VECTOR forward;
    FMOD_VECTOR up;
} ListenerAttributes;

#ifdef __cplusplus
}
#endif

#endif // VRSTRUCTS_H
