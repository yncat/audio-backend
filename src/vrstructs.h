#ifndef VRSTRUCTS_H
#define VRSTRUCTS_H

#include "fmod/fmod.hpp"
#include <string>

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

// 3D size structure for room dimensions
// Maps to FMOD coordinate system: width->X, depth->Z, height->Y
typedef struct {
    float width;   // X in FMOD
    float depth;   // Z in FMOD
    float height;  // Y in FMOD
} Size3D;

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

// Wall materials structure for room effect
typedef struct {
    const char* front;
    const char* back;
    const char* left;
    const char* right;
    const char* floor;
    const char* ceiling;
} WallMaterials;

#ifdef __cplusplus
}

struct StoredRoom {
    Position3D centerPosition;
    Size3D roomSize;

    // Wall materials as std::string for automatic memory management
    std::string materialFront;
    std::string materialBack;
    std::string materialLeft;
    std::string materialRight;
    std::string materialFloor;
    std::string materialCeiling;
};

#endif

#endif // VRSTRUCTS_H
