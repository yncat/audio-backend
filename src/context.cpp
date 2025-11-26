#include "context.h"

// Global context variable (pointer)
// Not null when backend is initialized
AudioBackendContext* g_context = nullptr;

// Check if the backend is properly initialized
bool isBackendInitialized() {
    return g_context != nullptr && g_context->isBackendInitialized();
}

AudioBackendContext::AudioBackendContext() : last_error(""), backend_initialized(false), fmod_system(nullptr), bgm_channel_group(nullptr), vr_plugin_handle(0), vr_channel_group(nullptr), vr_listener_dsp(nullptr), vr_initialized(false) {
    // Initialize BGM slots (32 slots should be enough)
    bgm_slots.resize(32);

    // Initialize VR listener attributes to default values
    // pos=0,0,0 vel=0,0,0 forward=0,0,1 up=0,1,0
    vr_listener_attributes.pos = { 0.0f, 0.0f, 0.0f };
    vr_listener_attributes.vel = { 0.0f, 0.0f, 0.0f };
    vr_listener_attributes.forward = { 0.0f, 0.0f, 1.0f };
    vr_listener_attributes.up = { 0.0f, 1.0f, 0.0f };
}

AudioBackendContext::~AudioBackendContext() {
}

void AudioBackendContext::SetLastError(const std::string& error) {
    last_error = error;
}

std::string AudioBackendContext::getLastError() const {
    return last_error;
}

bool AudioBackendContext::isBackendInitialized() const {
    return backend_initialized;
}

void AudioBackendContext::setBackendInitialized(bool initialized) {
    backend_initialized = initialized;
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

std::unordered_map<std::string, FMOD::Sound*>& AudioBackendContext::GetSamplesMap() {
    return samples_map;
}

unsigned int AudioBackendContext::GetVrPluginHandle() const {
    return vr_plugin_handle;
}

void AudioBackendContext::SetVrPluginHandle(unsigned int handle) {
    vr_plugin_handle = handle;
}

FMOD::ChannelGroup* AudioBackendContext::GetVrChannelGroup() const {
    return vr_channel_group;
}

void AudioBackendContext::SetVrChannelGroup(FMOD::ChannelGroup* group) {
    vr_channel_group = group;
}

FMOD::DSP* AudioBackendContext::GetVrListenerDsp() const {
    return vr_listener_dsp;
}

void AudioBackendContext::SetVrListenerDsp(FMOD::DSP* dsp) {
    vr_listener_dsp = dsp;
}

bool AudioBackendContext::isVrInitialized() const {
    return vr_initialized;
}

void AudioBackendContext::setVrInitialized(bool initialized) {
    vr_initialized = initialized;
}

ListenerAttributes& AudioBackendContext::GetVrListenerAttributes() {
    return vr_listener_attributes;
}

void AudioBackendContext::SetVrListenerAttributes(const ListenerAttributes& attributes) {
    vr_listener_attributes = attributes;
}
