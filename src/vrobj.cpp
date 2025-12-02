#include "context.h"
#include "vrstructs.h"
#include "sound_attributes.h"
#include "fmod/fmod.hpp"
#include "fmod/fmod_errors.h"
#include "fmod/fmod_dsp.h"

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

    // Get the master channel group for VR audio
    FMOD::ChannelGroup* masterGroup = nullptr;
    FMOD_RESULT result = system->getMasterChannelGroup(&masterGroup);
    if (result != FMOD_OK || masterGroup == nullptr) {
        g_context->SetLastError("Failed to get master channel group");
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

    // Create a channel for this sound (paused initially)
    FMOD::Channel* channel = nullptr;
    result = system->playSound(sound, masterGroup, true, &channel);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to play sound: ") + FMOD_ErrorString(result));
        return -1;
    }

    // Calculate world position for DSP
    FMOD_VECTOR fmod_pos;
    if (follow) {
        // For head-relative sounds, use the position as-is (relative to listener)
        fmod_pos.x = position3d->width;
        fmod_pos.y = position3d->height;
        fmod_pos.z = position3d->depth;
    } else {
        // For world-relative sounds, add listener position to relative position
        ListenerAttributes& listener = g_context->GetVrListenerAttributes();
        fmod_pos.x = listener.pos.x + position3d->width;
        fmod_pos.y = listener.pos.y + position3d->height;
        fmod_pos.z = listener.pos.z + position3d->depth;
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

        // Set distance attenuation parameters (similar to AGPP implementation)
        // Parameter [2]: Min Distance
        result = sourceDsp->setParameterFloat(2, 0.5f);
        if (result != FMOD_OK) {
            g_context->SetLastError(std::string("Failed to set min distance: ") + FMOD_ErrorString(result));
            sourceDsp->release();
            channel->stop();
            return -1;
        }

        // Parameter [3]: Max Distance
        result = sourceDsp->setParameterFloat(3, 200.0f);
        if (result != FMOD_OK) {
            g_context->SetLastError(std::string("Failed to set max distance: ") + FMOD_ErrorString(result));
            sourceDsp->release();
            channel->stop();
            return -1;
        }

        // Set 3D attributes on the Resonance Audio Source DSP (parameter index 8)
        FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };
        FMOD_DSP_PARAMETER_3DATTRIBUTES dsp_3d_attrs = {};
        dsp_3d_attrs.relative.position = fmod_pos;
        dsp_3d_attrs.relative.velocity = vel;
        dsp_3d_attrs.relative.forward = { 0.0f, 0.0f, 1.0f };
        dsp_3d_attrs.relative.up = { 0.0f, 1.0f, 0.0f };
        dsp_3d_attrs.absolute.position = fmod_pos;
        dsp_3d_attrs.absolute.velocity = vel;
        dsp_3d_attrs.absolute.forward = { 0.0f, 0.0f, 1.0f };
        dsp_3d_attrs.absolute.up = { 0.0f, 1.0f, 0.0f };

        result = sourceDsp->setParameterData(8, &dsp_3d_attrs, sizeof(dsp_3d_attrs));
        if (result != FMOD_OK) {
            g_context->SetLastError(std::string("Failed to set 3D attributes on Source DSP: ") + FMOD_ErrorString(result));
            sourceDsp->release();
            channel->stop();
            return -1;
        }

        // Release our reference, the channel now owns it
        sourceDsp->release();
    }

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

    // Get the master channel group for VR audio
    FMOD::ChannelGroup* masterGroup = nullptr;
    FMOD_RESULT result = system->getMasterChannelGroup(&masterGroup);
    if (result != FMOD_OK || masterGroup == nullptr) {
        g_context->SetLastError("Failed to get master channel group");
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

    // Create a channel for this sound (paused initially)
    FMOD::Channel* channel = nullptr;
    result = system->playSound(sound, masterGroup, true, &channel);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to play sound: ") + FMOD_ErrorString(result));
        return -1;
    }

    // Set absolute world position for DSP
    FMOD_VECTOR fmod_pos;
    fmod_pos.x = position3d->width;
    fmod_pos.y = position3d->height;
    fmod_pos.z = position3d->depth;

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

        // Set distance attenuation parameters (similar to AGPP implementation)
        // Parameter [2]: Min Distance
        result = sourceDsp->setParameterFloat(2, 0.5f);
        if (result != FMOD_OK) {
            g_context->SetLastError(std::string("Failed to set min distance: ") + FMOD_ErrorString(result));
            sourceDsp->release();
            channel->stop();
            return -1;
        }

        // Parameter [3]: Max Distance
        result = sourceDsp->setParameterFloat(3, 200.0f);
        if (result != FMOD_OK) {
            g_context->SetLastError(std::string("Failed to set max distance: ") + FMOD_ErrorString(result));
            sourceDsp->release();
            channel->stop();
            return -1;
        }

        // Set 3D attributes on the Resonance Audio Source DSP (parameter index 8)
        FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };
        FMOD_DSP_PARAMETER_3DATTRIBUTES dsp_3d_attrs = {};
        dsp_3d_attrs.relative.position = fmod_pos;
        dsp_3d_attrs.relative.velocity = vel;
        dsp_3d_attrs.relative.forward = { 0.0f, 0.0f, 1.0f };
        dsp_3d_attrs.relative.up = { 0.0f, 1.0f, 0.0f };
        dsp_3d_attrs.absolute.position = fmod_pos;
        dsp_3d_attrs.absolute.velocity = vel;
        dsp_3d_attrs.absolute.forward = { 0.0f, 0.0f, 1.0f };
        dsp_3d_attrs.absolute.up = { 0.0f, 1.0f, 0.0f };

        result = sourceDsp->setParameterData(8, &dsp_3d_attrs, sizeof(dsp_3d_attrs));
        if (result != FMOD_OK) {
            g_context->SetLastError(std::string("Failed to set 3D attributes on Source DSP: ") + FMOD_ErrorString(result));
            sourceDsp->release();
            channel->stop();
            return -1;
        }

        // Release our reference, the channel now owns it
        sourceDsp->release();
    }

    // Unpause and play
    result = channel->setPaused(false);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to unpause channel: ") + FMOD_ErrorString(result));
        channel->stop();
        return -1;
    }

    return 0;
}

// Play a oneshot sound at the player's position (for player-emitted sounds)
int vrOneshotPlayer(const char* sample_key, SoundAttributes* sound_attributes) {
    // Check if VR is initialized
    if (!g_context->isVrInitialized()) {
        g_context->SetLastError("VR audio is not initialized. Call audio_vrInitialize() first.");
        return -1;
    }

    // Validate inputs
    if (sample_key == nullptr || sound_attributes == nullptr) {
        g_context->SetLastError("Invalid parameters: sample_key and sound_attributes cannot be null");
        return -1;
    }

    FMOD::System* system = g_context->GetFmodSystem();
    if (system == nullptr) {
        g_context->SetLastError("FMOD system is null");
        return -1;
    }

    // Get the player_sounds channel group
    FMOD::ChannelGroup* playerSoundsGroup = g_context->GetVrPlayerSoundsGroup();
    if (playerSoundsGroup == nullptr) {
        g_context->SetLastError("player_sounds channel group is not initialized");
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

    // Create a channel for this sound (paused initially) in the player_sounds group
    FMOD::Channel* channel = nullptr;
    FMOD_RESULT result = system->playSound(sound, playerSoundsGroup, true, &channel);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to play sound: ") + FMOD_ErrorString(result));
        return -1;
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

    // Note: pan is ignored for player sounds as per spec
    // Note: The player_sounds group already has a Resonance Audio Source DSP attached,
    // so individual channels don't need their own Source DSP.
    // The position is managed by the group's DSP via setPlayerPosition().

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
