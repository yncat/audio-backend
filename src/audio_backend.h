#ifndef AUDIO_BACKEND_H
#define AUDIO_BACKEND_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float pan;
    float volume;
    float pitch;
} SoundAttributes;

// 3D position structure
// Maps to FMOD coordinate system: width->X, depth->Z, height->Y
typedef struct {
    float width;   // X in FMOD
    float depth;   // Z in FMOD
    float height;  // Y in FMOD
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

// Core API
__declspec(dllimport) int audio_coreInitialize();
__declspec(dllimport) void audio_coreFree();

// Version API
__declspec(dllimport) int audio_versionGetMajor();
__declspec(dllimport) int audio_versionGetMinor();
__declspec(dllimport) int audio_versionGetPatch();

// Error API
__declspec(dllimport) void audio_errorGetLast(char* buffer, int size);

// Sample API
__declspec(dllimport) int audio_sampleLoad(const void* address, int size, const char* key);
__declspec(dllimport) int audio_sampleOneshot(const char* key, SoundAttributes* attributes);

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

// Wall materials structure for room effect
typedef struct {
    const char* front;
    const char* back;
    const char* left;
    const char* right;
    const char* floor;
    const char* ceiling;
} WallMaterials;

// VRObjectInfo structure for passing VR object information
typedef struct {
    Position3D position;
    Size3D size;
    const char* looped_sample_key;  // Can be NULL if no looped sound
} VRObjectInfo;

// VR Audio API
__declspec(dllimport) int audio_vrInitialize(const char* plugin_path);
__declspec(dllimport) int audio_vrOneshotRelative(const char* sample_key, const Position3D* position3d, SoundAttributes* sound_attributes, bool follow);
__declspec(dllimport) int audio_vrOneshotAbsolute(const char* sample_key, const Position3D* position3d, SoundAttributes* sound_attributes);
__declspec(dllimport) int audio_vrOneshotPlayer(const char* sample_key, SoundAttributes* sound_attributes);
__declspec(dllimport) int audio_vrPlayerSetPosition(float width, float depth, float height);
__declspec(dllimport) int audio_vrPlayerSetRotation(const UnitVector3D* front, const UnitVector3D* up);
__declspec(dllimport) int audio_vrRoomAdd(Position3D centerPosition, Size3D roomSize, WallMaterials* materials);
__declspec(dllimport) int audio_vrRoomChange(int index);
__declspec(dllimport) int audio_vrRoomClear();

// VR Object API
__declspec(dllimport) int audio_vrObjectAdd(const char* key, VRObjectInfo* info);
__declspec(dllimport) int audio_vrObjectRemove(const char* key);
__declspec(dllimport) int audio_vrObjectStartLooping(const char* key);
__declspec(dllimport) int audio_vrObjectPauseLooping(const char* key);
__declspec(dllimport) int audio_vrObjectResumeLooping(const char* key);
__declspec(dllimport) int audio_vrObjectPlayOneshot(const char* object_key, const char* sample_key, SoundAttributes* attributes);

// Plugin Inspector API
__declspec(dllimport) int audio_corePluginInspect(const char* plugin_path, const char* output_path);

#ifdef __cplusplus
}
#endif

#endif // AUDIO_BACKEND_H
