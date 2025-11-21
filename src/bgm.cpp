#include "bgm.h"
#include "context.h"
#include "fmod/fmod.hpp"
#include "fmod/fmod_errors.h"
#include <cstring>

// External declaration of global context
extern AudioBackendContext* g_context;

// Set global BGM volume
void globalSetBgmVolume(float volume) {
    if (g_context == nullptr) {
        return;
    }

    FMOD::ChannelGroup* bgmGroup = g_context->GetBgmChannelGroup();
    if (bgmGroup == nullptr) {
        return;
    }

    bgmGroup->setVolume(volume);
}

// Load BGM from memory and return slot number
int bgmLoad(const void* address, int size) {
    if (g_context == nullptr) {
        return -1;
    }

    FMOD::System* system = g_context->GetFmodSystem();
    if (system == nullptr) {
        return -1;
    }

    // Find an empty slot
    std::vector<BgmSlot>& slots = g_context->GetBgmSlots();
    int slot_index = -1;
    for (size_t i = 0; i < slots.size(); i++) {
        if (!slots[i].is_used) {
            slot_index = static_cast<int>(i);
            break;
        }
    }

    if (slot_index == -1) {
        g_context->SetLastError("No available BGM slots");
        return -1;
    }

    // Copy the memory buffer since FMOD_OPENMEMORY will duplicate it
    // but we want to be safe and manage our own copy
    void* buffer_copy = malloc(size);
    if (buffer_copy == nullptr) {
        g_context->SetLastError("Failed to allocate memory for BGM buffer");
        return -1;
    }
    memcpy(buffer_copy, address, size);

    // Create sound from memory
    FMOD::Sound* sound = nullptr;
    FMOD_CREATESOUNDEXINFO exinfo;
    memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
    exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
    exinfo.length = size;

    // Use FMOD_LOOP_NORMAL for looping BGM, FMOD_OPENMEMORY to load from memory
    FMOD_RESULT result = system->createSound(
        static_cast<const char*>(buffer_copy),
        FMOD_OPENMEMORY | FMOD_LOOP_NORMAL,
        &exinfo,
        &sound
    );

    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to create BGM sound: ") + FMOD_ErrorString(result));
        free(buffer_copy);
        return -1;
    }

    // Store in slot
    slots[slot_index].sound = sound;
    slots[slot_index].buffer = buffer_copy;
    slots[slot_index].is_used = true;
    slots[slot_index].channel = nullptr;
    slots[slot_index].loop_point_ms = -1;

    return slot_index;
}

// Pause BGM
void bgmPause(int slot) {
    if (g_context == nullptr || slot < 0) {
        return;
    }

    std::vector<BgmSlot>& slots = g_context->GetBgmSlots();
    if (slot >= static_cast<int>(slots.size()) || !slots[slot].is_used) {
        return;
    }

    if (slots[slot].channel != nullptr) {
        slots[slot].channel->setPaused(true);
    }
}

// Resume BGM
void bgmResume(int slot) {
    if (g_context == nullptr || slot < 0) {
        return;
    }

    std::vector<BgmSlot>& slots = g_context->GetBgmSlots();
    if (slot >= static_cast<int>(slots.size()) || !slots[slot].is_used) {
        return;
    }

    if (slots[slot].channel != nullptr) {
        slots[slot].channel->setPaused(false);
    } else if (slots[slot].sound != nullptr) {
        // If channel doesn't exist, start playing
        FMOD::System* system = g_context->GetFmodSystem();
        FMOD::ChannelGroup* bgmGroup = g_context->GetBgmChannelGroup();
        FMOD::Channel* channel = nullptr;

        system->playSound(slots[slot].sound, bgmGroup, false, &channel);
        slots[slot].channel = channel;
    }
}

// Stop BGM
void bgmStop(int slot) {
    if (g_context == nullptr || slot < 0) {
        return;
    }

    std::vector<BgmSlot>& slots = g_context->GetBgmSlots();
    if (slot >= static_cast<int>(slots.size()) || !slots[slot].is_used) {
        return;
    }

    if (slots[slot].channel != nullptr) {
        slots[slot].channel->stop();
        slots[slot].channel = nullptr;
    }
}

// Fadeout BGM over specified milliseconds
void bgmFadeout(int slot, int ms) {
    if (g_context == nullptr || slot < 0) {
        return;
    }

    std::vector<BgmSlot>& slots = g_context->GetBgmSlots();
    if (slot >= static_cast<int>(slots.size()) || !slots[slot].is_used) {
        return;
    }

    if (slots[slot].channel != nullptr) {
        // Get current volume
        float current_volume = 1.0f;
        slots[slot].channel->getVolume(&current_volume);

        // Set fade from current volume to 0
        unsigned long long dspclock;
        int rate;
        g_context->GetFmodSystem()->getSoftwareFormat(&rate, nullptr, nullptr);
        slots[slot].channel->getDSPClock(nullptr, &dspclock);

        unsigned long long fade_length = (static_cast<unsigned long long>(ms) * rate) / 1000;
        slots[slot].channel->addFadePoint(dspclock, current_volume);
        slots[slot].channel->addFadePoint(dspclock + fade_length, 0.0f);
    }
}

// Fadein BGM over specified milliseconds
void bgmFadein(int slot, int ms) {
    if (g_context == nullptr || slot < 0) {
        return;
    }

    std::vector<BgmSlot>& slots = g_context->GetBgmSlots();
    if (slot >= static_cast<int>(slots.size()) || !slots[slot].is_used) {
        return;
    }

    FMOD::System* system = g_context->GetFmodSystem();
    FMOD::ChannelGroup* bgmGroup = g_context->GetBgmChannelGroup();

    if (slots[slot].channel == nullptr && slots[slot].sound != nullptr) {
        // Start playing if not already playing
        FMOD::Channel* channel = nullptr;
        system->playSound(slots[slot].sound, bgmGroup, true, &channel);
        slots[slot].channel = channel;

        if (channel != nullptr) {
            // Set initial volume to 0
            channel->setVolume(0.0f);

            // Set up fade
            unsigned long long dspclock;
            int rate;
            system->getSoftwareFormat(&rate, nullptr, nullptr);
            channel->getDSPClock(nullptr, &dspclock);

            unsigned long long fade_length = (static_cast<unsigned long long>(ms) * rate) / 1000;
            channel->addFadePoint(dspclock, 0.0f);
            channel->addFadePoint(dspclock + fade_length, 1.0f);

            // Unpause to start playback
            channel->setPaused(false);
        }
    } else if (slots[slot].channel != nullptr) {
        // Already playing, just fade from current volume to 1.0
        float current_volume = 0.0f;
        slots[slot].channel->getVolume(&current_volume);

        unsigned long long dspclock;
        int rate;
        system->getSoftwareFormat(&rate, nullptr, nullptr);
        slots[slot].channel->getDSPClock(nullptr, &dspclock);

        unsigned long long fade_length = (static_cast<unsigned long long>(ms) * rate) / 1000;
        slots[slot].channel->addFadePoint(dspclock, current_volume);
        slots[slot].channel->addFadePoint(dspclock + fade_length, 1.0f);
    }
}

// Crossfade between two BGM tracks
void bgmCrossfade(int slot1, int slot2, int ms) {
    // Fade out slot1 and fade in slot2
    bgmFadeout(slot1, ms);
    bgmFadein(slot2, ms);
}

// Set loop point for BGM (in milliseconds)
void bgmSetLoopPoint(int slot, int ms) {
    if (g_context == nullptr || slot < 0) {
        return;
    }

    std::vector<BgmSlot>& slots = g_context->GetBgmSlots();
    if (slot >= static_cast<int>(slots.size()) || !slots[slot].is_used) {
        return;
    }

    slots[slot].loop_point_ms = ms;

    if (slots[slot].sound != nullptr) {
        // Convert milliseconds to PCM samples
        float frequency;
        int channels, bits;
        slots[slot].sound->getDefaults(&frequency, nullptr);
        slots[slot].sound->getFormat(nullptr, nullptr, &channels, &bits);

        unsigned int loop_start = static_cast<unsigned int>((ms / 1000.0f) * frequency);
        unsigned int loop_end = 0;
        slots[slot].sound->getLength(&loop_end, FMOD_TIMEUNIT_PCM);

        // Set loop points
        slots[slot].sound->setLoopPoints(loop_start, FMOD_TIMEUNIT_PCM, loop_end - 1, FMOD_TIMEUNIT_PCM);
    }
}

// Free BGM slot
void bgmFree(int slot) {
    if (g_context == nullptr || slot < 0) {
        return;
    }

    std::vector<BgmSlot>& slots = g_context->GetBgmSlots();
    if (slot >= static_cast<int>(slots.size()) || !slots[slot].is_used) {
        return;
    }

    // Stop and release channel
    if (slots[slot].channel != nullptr) {
        slots[slot].channel->stop();
        slots[slot].channel = nullptr;
    }

    // Release sound
    if (slots[slot].sound != nullptr) {
        slots[slot].sound->release();
        slots[slot].sound = nullptr;
    }

    // Free buffer
    if (slots[slot].buffer != nullptr) {
        free(slots[slot].buffer);
        slots[slot].buffer = nullptr;
    }

    // Mark slot as unused
    slots[slot].is_used = false;
    slots[slot].loop_point_ms = -1;
}
