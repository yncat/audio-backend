#ifndef CONTEXT_H
#define CONTEXT_H

#include <string>
#include <vector>
#include <unordered_map>
#include "fmod/fmod.hpp"
#include "vrstructs.h"

// Structure to hold BGM slot data
struct BgmSlot {
    FMOD::Sound* sound;
    FMOD::Channel* channel;
    void* buffer;  // Copied memory buffer
    int loop_point_ms;
    bool is_used;

    BgmSlot() : sound(nullptr), channel(nullptr), buffer(nullptr), loop_point_ms(-1), is_used(false) {}
};

class AudioBackendContext {
private:
    std::string last_error;
    bool backend_initialized;
    FMOD::System* fmod_system;
    FMOD::ChannelGroup* bgm_channel_group;
    std::vector<BgmSlot> bgm_slots;
    std::unordered_map<std::string, FMOD::Sound*> samples_map;

    // VR audio related
    unsigned int vr_plugin_handle;
    unsigned int vr_source_plugin_handle;
    FMOD::DSP* vr_listener_dsp;
    FMOD::DSP* vr_player_source_dsp;  // Source DSP for player_sounds group
    bool vr_initialized;
    ListenerAttributes vr_listener_attributes;
    FMOD::ChannelGroup* vr_player_sounds_group;
    FMOD_VECTOR vr_player_position;
    FMOD_VECTOR vr_player_forward;
    FMOD_VECTOR vr_player_up;

public:
    AudioBackendContext();
    ~AudioBackendContext();

    void SetLastError(const std::string& error);
    std::string getLastError() const;

    bool isBackendInitialized() const;
    void setBackendInitialized(bool initialized);

    FMOD::System* GetFmodSystem() const;
    void SetFmodSystem(FMOD::System* system);

    FMOD::ChannelGroup* GetBgmChannelGroup() const;
    void SetBgmChannelGroup(FMOD::ChannelGroup* group);

    std::vector<BgmSlot>& GetBgmSlots();

    std::unordered_map<std::string, FMOD::Sound*>& GetSamplesMap();

    // VR audio related getters/setters
    unsigned int GetVrPluginHandle() const;
    void SetVrPluginHandle(unsigned int handle);

    unsigned int GetVrSourcePluginHandle() const;
    void SetVrSourcePluginHandle(unsigned int handle);

    FMOD::DSP* GetVrListenerDsp() const;
    void SetVrListenerDsp(FMOD::DSP* dsp);

    FMOD::DSP* GetVrPlayerSourceDsp() const;
    void SetVrPlayerSourceDsp(FMOD::DSP* dsp);

    bool isVrInitialized() const;
    void setVrInitialized(bool initialized);

    ListenerAttributes& GetVrListenerAttributes();
    void SetVrListenerAttributes(const ListenerAttributes& attributes);

    FMOD::ChannelGroup* GetVrPlayerSoundsGroup() const;
    void SetVrPlayerSoundsGroup(FMOD::ChannelGroup* group);

    FMOD_VECTOR& GetVrPlayerPosition();
    void SetVrPlayerPosition(const FMOD_VECTOR& position);

    FMOD_VECTOR& GetVrPlayerForward();
    void SetVrPlayerForward(const FMOD_VECTOR& forward);

    FMOD_VECTOR& GetVrPlayerUp();
    void SetVrPlayerUp(const FMOD_VECTOR& up);
};

// Global function to check if backend is initialized
bool isBackendInitialized();

#endif // CONTEXT_H
