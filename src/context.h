#ifndef CONTEXT_H
#define CONTEXT_H

#include <string>
#include <vector>
#include <unordered_map>
#include "fmod/fmod.hpp"

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
    FMOD::ChannelGroup* vr_channel_group;
    FMOD::DSP* vr_listener_dsp;
    bool vr_initialized;

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

    FMOD::ChannelGroup* GetVrChannelGroup() const;
    void SetVrChannelGroup(FMOD::ChannelGroup* group);

    FMOD::DSP* GetVrListenerDsp() const;
    void SetVrListenerDsp(FMOD::DSP* dsp);

    bool isVrInitialized() const;
    void setVrInitialized(bool initialized);
};

// Global function to check if backend is initialized
bool isBackendInitialized();

#endif // CONTEXT_H
