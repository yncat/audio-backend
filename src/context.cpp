#include "context.h"

// Global context variable (pointer)
// Not null when backend is initialized
AudioBackendContext* g_context = nullptr;

AudioBackendContext::AudioBackendContext() : last_error(""), fmod_system(nullptr), bgm_channel_group(nullptr) {
    // Initialize BGM slots (32 slots should be enough)
    bgm_slots.resize(32);
}

AudioBackendContext::~AudioBackendContext() {
}

void AudioBackendContext::SetLastError(const std::string& error) {
    last_error = error;
}

std::string AudioBackendContext::getLastError() const {
    return last_error;
}

FMOD::System* AudioBackendContext::GetFmodSystem() const {
    return fmod_system;
}

void AudioBackendContext::SetFmodSystem(FMOD::System* system) {
    fmod_system = system;
}

FMOD::ChannelGroup* AudioBackendContext::GetBgmChannelGroup() const {
    return bgm_channel_group;
}

void AudioBackendContext::SetBgmChannelGroup(FMOD::ChannelGroup* group) {
    bgm_channel_group = group;
}

std::vector<BgmSlot>& AudioBackendContext::GetBgmSlots() {
    return bgm_slots;
}
