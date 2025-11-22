#include "sample.h"
#include "context.h"
#include "fmod/fmod.hpp"
#include <string>

extern AudioBackendContext* g_context;

int sampleLoad(const void* address, int size, const char* key) {
    if (!isBackendInitialized()) {
        return -1;
    }

    std::string keyStr(key);
    auto& samples_map = g_context->GetSamplesMap();

    // Check if key already exists
    if (samples_map.find(keyStr) != samples_map.end()) {
        g_context->SetLastError("Sample with key '" + keyStr + "' already exists");
        return -1;
    }

    // Create FMOD_CREATESOUNDEXINFO for memory loading
    FMOD_CREATESOUNDEXINFO exinfo = {};
    exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
    exinfo.length = static_cast<unsigned int>(size);

    // Load sound with FMOD_CREATESAMPLE (pre-decode into memory)
    FMOD::Sound* sound = nullptr;
    FMOD_RESULT result = g_context->GetFmodSystem()->createSound(
        static_cast<const char*>(address),
        FMOD_OPENMEMORY | FMOD_CREATESAMPLE,
        &exinfo,
        &sound
    );

    if (result != FMOD_OK) {
        g_context->SetLastError("Failed to load sample: FMOD error " + std::to_string(result));
        return -1;
    }

    samples_map[keyStr] = sound;
    return 0;
}

int sampleOneshot(const char* key, SoundAttributes* attributes) {
    if (!isBackendInitialized()) {
        return -1;
    }

    std::string keyStr(key);
    auto& samples_map = g_context->GetSamplesMap();

    // Find sample by key
    auto it = samples_map.find(keyStr);
    if (it == samples_map.end()) {
        g_context->SetLastError("Sample with key '" + keyStr + "' not found");
        return -1;
    }

    FMOD::Sound* sound = it->second;
    FMOD::Channel* channel = nullptr;

    // Play sound (paused initially to set attributes)
    FMOD_RESULT result = g_context->GetFmodSystem()->playSound(sound, nullptr, true, &channel);
    if (result != FMOD_OK) {
        g_context->SetLastError("Failed to play sample: FMOD error " + std::to_string(result));
        return -1;
    }

    // Set attributes
    if (attributes) {
        channel->setPan(attributes->pan);
        channel->setVolume(attributes->volume);
        channel->setPitch(attributes->pitch);
    }

    // Unpause to start playback
    channel->setPaused(false);

    return 0;
}
