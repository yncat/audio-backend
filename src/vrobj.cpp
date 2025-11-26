#include "context.h"
#include "vrstructs.h"
#include "sound_attributes.h"
#include "fmod/fmod.hpp"
#include "fmod/fmod_errors.h"

// External declaration of global context
extern AudioBackendContext* g_context;

extern "C" {

// Play a oneshot sound at a relative position to the listener
int vrOneshotRelative(const char* sample_key, const Position3D* position3d, SoundAttributes* sound_attributes, bool follow) {
    // Check if VR is initialized
    if (!g_context->isVrInitialized()) {
        g_context->SetLastError("VR audio is not initialized. Call audio_vrInitialize() first.");
        return -1;
    }

    // Validate inputs
    if (sample_key == nullptr || position3d == nullptr || sound_attributes == nullptr) {
        g_context->SetLastError("Invalid parameters: sample_key, position3d, and sound_attributes cannot be null");
        return -1;
    }

    FMOD::System* system = g_context->GetFmodSystem();
    if (system == nullptr) {
        g_context->SetLastError("FMOD system is null");
        return -1;
    }

    // Get the VR channel group
    FMOD::ChannelGroup* vrGroup = g_context->GetVrChannelGroup();
    if (vrGroup == nullptr) {
        g_context->SetLastError("VR channel group is null");
        return -1;
    }

    // Find the sample by key
    auto& samples = g_context->GetSamplesMap();
    auto it = samples.find(sample_key);
    if (it == samples.end()) {
        g_context->SetLastError(std::string("Sample not found: ") + sample_key);
        return -1;
    }
    FMOD::Sound* sound = it->second;

    // Create a channel for this sound
    FMOD::Channel* channel = nullptr;
    FMOD_RESULT result = system->playSound(sound, vrGroup, true, &channel);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to play sound: ") + FMOD_ErrorString(result));
        return -1;
    }

    // Set 3D mode
    FMOD_MODE mode = FMOD_3D | FMOD_3D_WORLDRELATIVE;
    if (follow) {
        mode |= FMOD_3D_HEADRELATIVE;
    }
    result = channel->setMode(mode);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to set 3D mode: ") + FMOD_ErrorString(result));
        channel->stop();
        return -1;
    }

    // Calculate world position
    FMOD_VECTOR fmod_pos;
    if (follow) {
        // For head-relative sounds, use the position as-is (relative to listener)
        fmod_pos.x = static_cast<float>(position3d->width);
        fmod_pos.y = static_cast<float>(position3d->height);
        fmod_pos.z = static_cast<float>(position3d->depth);
    } else {
        // For world-relative sounds, add listener position to relative position
        ListenerAttributes& listener = g_context->GetVrListenerAttributes();
        fmod_pos.x = listener.pos.x + static_cast<float>(position3d->width);
        fmod_pos.y = listener.pos.y + static_cast<float>(position3d->height);
        fmod_pos.z = listener.pos.z + static_cast<float>(position3d->depth);
    }

    // Set 3D position
    FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };
    result = channel->set3DAttributes(&fmod_pos, &vel);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to set 3D attributes: ") + FMOD_ErrorString(result));
        channel->stop();
        return -1;
    }

    // Attach Resonance Audio Source DSP to the channel
    unsigned int source_plugin_handle = g_context->GetVrSourcePluginHandle();
    if (source_plugin_handle != 0) {
        FMOD::DSP* sourceDsp = nullptr;
        result = system->createDSPByPlugin(source_plugin_handle, &sourceDsp);
        if (result != FMOD_OK) {
            g_context->SetLastError(std::string("Failed to create Resonance Audio Source DSP: ") + FMOD_ErrorString(result));
            channel->stop();
            return -1;
        }

        result = channel->addDSP(FMOD_CHANNELCONTROL_DSP_HEAD, sourceDsp);
        if (result != FMOD_OK) {
            g_context->SetLastError(std::string("Failed to add Source DSP to channel: ") + FMOD_ErrorString(result));
            sourceDsp->release();
            channel->stop();
            return -1;
        }
        // Release our reference, the channel now owns it
        sourceDsp->release();
    }

    // Apply sound attributes
    // Set volume
    result = channel->setVolume(sound_attributes->volume);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to set volume: ") + FMOD_ErrorString(result));
        channel->stop();
        return -1;
    }

    // Set pitch
    result = channel->setPitch(sound_attributes->pitch);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to set pitch: ") + FMOD_ErrorString(result));
        channel->stop();
        return -1;
    }

    // Note: pan is ignored for 3D sounds as per spec

    // Unpause and play
    result = channel->setPaused(false);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to unpause channel: ") + FMOD_ErrorString(result));
        channel->stop();
        return -1;
    }

    return 0;
}

// Play a oneshot sound at an absolute world position
int vrOneshotAbsolute(const char* sample_key, const Position3D* position3d, SoundAttributes* sound_attributes) {
    // Check if VR is initialized
    if (!g_context->isVrInitialized()) {
        g_context->SetLastError("VR audio is not initialized. Call audio_vrInitialize() first.");
        return -1;
    }

    // Validate inputs
    if (sample_key == nullptr || position3d == nullptr || sound_attributes == nullptr) {
        g_context->SetLastError("Invalid parameters: sample_key, position3d, and sound_attributes cannot be null");
        return -1;
    }

    FMOD::System* system = g_context->GetFmodSystem();
    if (system == nullptr) {
        g_context->SetLastError("FMOD system is null");
        return -1;
    }

    // Get the VR channel group
    FMOD::ChannelGroup* vrGroup = g_context->GetVrChannelGroup();
    if (vrGroup == nullptr) {
        g_context->SetLastError("VR channel group is null");
        return -1;
    }

    // Find the sample by key
    auto& samples = g_context->GetSamplesMap();
    auto it = samples.find(sample_key);
    if (it == samples.end()) {
        g_context->SetLastError(std::string("Sample not found: ") + sample_key);
        return -1;
    }
    FMOD::Sound* sound = it->second;

    // Create a channel for this sound
    FMOD::Channel* channel = nullptr;
    FMOD_RESULT result = system->playSound(sound, vrGroup, true, &channel);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to play sound: ") + FMOD_ErrorString(result));
        return -1;
    }

    // Set 3D mode (world-relative)
    FMOD_MODE mode = FMOD_3D | FMOD_3D_WORLDRELATIVE;
    result = channel->setMode(mode);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to set 3D mode: ") + FMOD_ErrorString(result));
        channel->stop();
        return -1;
    }

    // Set absolute world position
    FMOD_VECTOR fmod_pos;
    fmod_pos.x = static_cast<float>(position3d->width);
    fmod_pos.y = static_cast<float>(position3d->height);
    fmod_pos.z = static_cast<float>(position3d->depth);

    // Set 3D position
    FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };
    result = channel->set3DAttributes(&fmod_pos, &vel);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to set 3D attributes: ") + FMOD_ErrorString(result));
        channel->stop();
        return -1;
    }

    // Attach Resonance Audio Source DSP to the channel
    unsigned int source_plugin_handle = g_context->GetVrSourcePluginHandle();
    if (source_plugin_handle != 0) {
        FMOD::DSP* sourceDsp = nullptr;
        result = system->createDSPByPlugin(source_plugin_handle, &sourceDsp);
        if (result != FMOD_OK) {
            g_context->SetLastError(std::string("Failed to create Resonance Audio Source DSP: ") + FMOD_ErrorString(result));
            channel->stop();
            return -1;
        }

        result = channel->addDSP(FMOD_CHANNELCONTROL_DSP_HEAD, sourceDsp);
        if (result != FMOD_OK) {
            g_context->SetLastError(std::string("Failed to add Source DSP to channel: ") + FMOD_ErrorString(result));
            sourceDsp->release();
            channel->stop();
            return -1;
        }
        // Release our reference, the channel now owns it
        sourceDsp->release();
    }

    // Apply sound attributes
    // Set volume
    result = channel->setVolume(sound_attributes->volume);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to set volume: ") + FMOD_ErrorString(result));
        channel->stop();
        return -1;
    }

    // Set pitch
    result = channel->setPitch(sound_attributes->pitch);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to set pitch: ") + FMOD_ErrorString(result));
        channel->stop();
        return -1;
    }

    // Note: pan is ignored for 3D sounds as per spec

    // Unpause and play
    result = channel->setPaused(false);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to unpause channel: ") + FMOD_ErrorString(result));
        channel->stop();
        return -1;
    }

    return 0;
}

} // extern "C"
