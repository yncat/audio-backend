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

// Add a new VR object with the specified key and properties
int vrObjectAdd(const char* key, VRObjectInfo* info) {
    // Check if VR is initialized
    if (!g_context->isVrInitialized()) {
        g_context->SetLastError("VR audio is not initialized. Call audio_vrInitialize() first.");
        return -1;
    }

    // Validate inputs
    if (key == nullptr || info == nullptr) {
        g_context->SetLastError("Invalid parameters: key and info cannot be null");
        return -1;
    }

    FMOD::System* system = g_context->GetFmodSystem();
    if (system == nullptr) {
        g_context->SetLastError("FMOD system is null");
        return -1;
    }

    // Check if key already exists
    auto& vr_objects = g_context->GetVrObjects();
    if (vr_objects.find(key) != vr_objects.end()) {
        g_context->SetLastError(std::string("VR object with key already exists: ") + key);
        return -1;
    }

    // Get the master channel group
    FMOD::ChannelGroup* masterGroup = nullptr;
    FMOD_RESULT result = system->getMasterChannelGroup(&masterGroup);
    if (result != FMOD_OK || masterGroup == nullptr) {
        g_context->SetLastError("Failed to get master channel group");
        return -1;
    }

    // Create a new VR object
    VRObject vrobj;
    vrobj.center = info->position;
    vrobj.size = info->size;

    // Create a channel group for this object
    result = system->createChannelGroup(key, &vrobj.channel_group);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to create channel group: ") + FMOD_ErrorString(result));
        return -1;
    }

    // Attach the channel group to master
    result = masterGroup->addGroup(vrobj.channel_group);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to add channel group to master: ") + FMOD_ErrorString(result));
        vrobj.channel_group->release();
        return -1;
    }

    // Create a Resonance Audio Source DSP for this object's channel group
    unsigned int source_plugin_handle = g_context->GetVrSourcePluginHandle();
    if (source_plugin_handle != 0) {
        FMOD::DSP* sourceDsp = nullptr;
        result = system->createDSPByPlugin(source_plugin_handle, &sourceDsp);
        if (result != FMOD_OK) {
            g_context->SetLastError(std::string("Failed to create Resonance Audio Source DSP: ") + FMOD_ErrorString(result));
            vrobj.channel_group->release();
            return -1;
        }

        result = vrobj.channel_group->addDSP(FMOD_CHANNELCONTROL_DSP_HEAD, sourceDsp);
        if (result != FMOD_OK) {
            g_context->SetLastError(std::string("Failed to add Source DSP to channel group: ") + FMOD_ErrorString(result));
            sourceDsp->release();
            vrobj.channel_group->release();
            return -1;
        }

        // Set distance attenuation parameters
        result = sourceDsp->setParameterFloat(2, 0.5f);  // Min Distance
        if (result != FMOD_OK) {
            g_context->SetLastError(std::string("Failed to set min distance: ") + FMOD_ErrorString(result));
            sourceDsp->release();
            vrobj.channel_group->release();
            return -1;
        }

        result = sourceDsp->setParameterFloat(3, 200.0f);  // Max Distance
        if (result != FMOD_OK) {
            g_context->SetLastError(std::string("Failed to set max distance: ") + FMOD_ErrorString(result));
            sourceDsp->release();
            vrobj.channel_group->release();
            return -1;
        }

        // Set 3D position for the channel group's DSP
        FMOD_VECTOR fmod_pos;
        fmod_pos.x = info->position.width;
        fmod_pos.y = info->position.height;
        fmod_pos.z = info->position.depth;

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
            vrobj.channel_group->release();
            return -1;
        }

        // Release our reference, the channel group now owns it
        sourceDsp->release();
    }

    // If looped_sample_key is specified, validate and store it
    if (info->looped_sample_key != nullptr && info->looped_sample_key[0] != '\0') {
        // Validate that the sample exists
        auto& samples = g_context->GetSamplesMap();
        auto it = samples.find(info->looped_sample_key);
        if (it == samples.end()) {
            g_context->SetLastError(std::string("Looped sample not found: ") + info->looped_sample_key);
            vrobj.channel_group->release();
            return -1;
        }

        // Store the sample key (no need to create a new sound)
        vrobj.looped_sample_key = info->looped_sample_key;
    }

    // Store the VR object in the context
    vr_objects[key] = vrobj;

    return 0;
}

// Remove a VR object by key
int vrObjectRemove(const char* key) {
    // Check if VR is initialized
    if (!g_context->isVrInitialized()) {
        g_context->SetLastError("VR audio is not initialized. Call audio_vrInitialize() first.");
        return -1;
    }

    // Validate input
    if (key == nullptr) {
        g_context->SetLastError("Invalid parameter: key cannot be null");
        return -1;
    }

    // Find the VR object
    auto& vr_objects = g_context->GetVrObjects();
    auto it = vr_objects.find(key);
    if (it == vr_objects.end()) {
        g_context->SetLastError(std::string("VR object not found: ") + key);
        return -1;
    }

    VRObject& vrobj = it->second;

    // Stop the looped channel if it exists
    if (vrobj.looped_channel != nullptr) {
        vrobj.looped_channel->stop();
        vrobj.looped_channel = nullptr;
    }

    // Release the channel group
    if (vrobj.channel_group != nullptr) {
        vrobj.channel_group->release();
        vrobj.channel_group = nullptr;
    }

    // Remove from map
    vr_objects.erase(it);

    return 0;
}

// Start looping the object's looped sound
int vrObjectStartLooping(const char* key) {
    // Check if VR is initialized
    if (!g_context->isVrInitialized()) {
        g_context->SetLastError("VR audio is not initialized. Call audio_vrInitialize() first.");
        return -1;
    }

    // Validate input
    if (key == nullptr) {
        g_context->SetLastError("Invalid parameter: key cannot be null");
        return -1;
    }

    FMOD::System* system = g_context->GetFmodSystem();
    if (system == nullptr) {
        g_context->SetLastError("FMOD system is null");
        return -1;
    }

    // Find the VR object
    auto& vr_objects = g_context->GetVrObjects();
    auto it = vr_objects.find(key);
    if (it == vr_objects.end()) {
        g_context->SetLastError(std::string("VR object not found: ") + key);
        return -1;
    }

    VRObject& vrobj = it->second;

    // Check if there's a looped sample key
    if (vrobj.looped_sample_key.empty()) {
        g_context->SetLastError(std::string("VR object has no looped sound: ") + key);
        return -1;
    }

    // Get the sample by key
    auto& samples = g_context->GetSamplesMap();
    auto sample_it = samples.find(vrobj.looped_sample_key);
    if (sample_it == samples.end()) {
        g_context->SetLastError(std::string("Looped sample not found: ") + vrobj.looped_sample_key);
        return -1;
    }
    FMOD::Sound* sound = sample_it->second;

    // If already playing, stop it first
    if (vrobj.looped_channel != nullptr) {
        vrobj.looped_channel->stop();
        vrobj.looped_channel = nullptr;
    }

    // Play the sound in the object's channel group
    FMOD_RESULT result = system->playSound(sound, vrobj.channel_group, false, &vrobj.looped_channel);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to play looped sound: ") + FMOD_ErrorString(result));
        return -1;
    }

    // Set loop mode on the channel
    result = vrobj.looped_channel->setMode(FMOD_LOOP_NORMAL);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to set loop mode: ") + FMOD_ErrorString(result));
        vrobj.looped_channel->stop();
        vrobj.looped_channel = nullptr;
        return -1;
    }

    return 0;
}

// Pause the object's looped sound
int vrObjectPauseLooping(const char* key) {
    // Check if VR is initialized
    if (!g_context->isVrInitialized()) {
        g_context->SetLastError("VR audio is not initialized. Call audio_vrInitialize() first.");
        return -1;
    }

    // Validate input
    if (key == nullptr) {
        g_context->SetLastError("Invalid parameter: key cannot be null");
        return -1;
    }

    // Find the VR object
    auto& vr_objects = g_context->GetVrObjects();
    auto it = vr_objects.find(key);
    if (it == vr_objects.end()) {
        g_context->SetLastError(std::string("VR object not found: ") + key);
        return -1;
    }

    VRObject& vrobj = it->second;

    // Check if there's a looped channel playing
    if (vrobj.looped_channel == nullptr) {
        g_context->SetLastError(std::string("VR object has no active looped channel: ") + key);
        return -1;
    }

    // Pause the channel
    FMOD_RESULT result = vrobj.looped_channel->setPaused(true);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to pause looped channel: ") + FMOD_ErrorString(result));
        return -1;
    }

    return 0;
}

// Resume the object's looped sound
int vrObjectResumeLooping(const char* key) {
    // Check if VR is initialized
    if (!g_context->isVrInitialized()) {
        g_context->SetLastError("VR audio is not initialized. Call audio_vrInitialize() first.");
        return -1;
    }

    // Validate input
    if (key == nullptr) {
        g_context->SetLastError("Invalid parameter: key cannot be null");
        return -1;
    }

    // Find the VR object
    auto& vr_objects = g_context->GetVrObjects();
    auto it = vr_objects.find(key);
    if (it == vr_objects.end()) {
        g_context->SetLastError(std::string("VR object not found: ") + key);
        return -1;
    }

    VRObject& vrobj = it->second;

    // Check if there's a looped channel
    if (vrobj.looped_channel == nullptr) {
        g_context->SetLastError(std::string("VR object has no active looped channel: ") + key);
        return -1;
    }

    // Resume the channel
    FMOD_RESULT result = vrobj.looped_channel->setPaused(false);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to resume looped channel: ") + FMOD_ErrorString(result));
        return -1;
    }

    return 0;
}

// Play a oneshot sound from the specified object
int vrObjectPlayOneshot(const char* object_key, const char* sample_key, SoundAttributes* attributes) {
    // Check if VR is initialized
    if (!g_context->isVrInitialized()) {
        g_context->SetLastError("VR audio is not initialized. Call audio_vrInitialize() first.");
        return -1;
    }

    // Validate inputs
    if (object_key == nullptr || sample_key == nullptr || attributes == nullptr) {
        g_context->SetLastError("Invalid parameters: object_key, sample_key, and attributes cannot be null");
        return -1;
    }

    FMOD::System* system = g_context->GetFmodSystem();
    if (system == nullptr) {
        g_context->SetLastError("FMOD system is null");
        return -1;
    }

    // Find the VR object
    auto& vr_objects = g_context->GetVrObjects();
    auto it = vr_objects.find(object_key);
    if (it == vr_objects.end()) {
        g_context->SetLastError(std::string("VR object not found: ") + object_key);
        return -1;
    }

    VRObject& vrobj = it->second;

    // Find the sample by key
    auto& samples = g_context->GetSamplesMap();
    auto sample_it = samples.find(sample_key);
    if (sample_it == samples.end()) {
        g_context->SetLastError(std::string("Sample not found: ") + sample_key);
        return -1;
    }
    FMOD::Sound* sound = sample_it->second;

    // Play the sound in the object's channel group (paused initially)
    FMOD::Channel* channel = nullptr;
    FMOD_RESULT result = system->playSound(sound, vrobj.channel_group, true, &channel);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to play sound: ") + FMOD_ErrorString(result));
        return -1;
    }

    // Apply sound attributes
    result = channel->setVolume(attributes->volume);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to set volume: ") + FMOD_ErrorString(result));
        channel->stop();
        return -1;
    }

    result = channel->setPitch(attributes->pitch);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to set pitch: ") + FMOD_ErrorString(result));
        channel->stop();
        return -1;
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

// Change the position of a VR object
int vrObjectChangePosition(const char* key, Position3D pos) {
    // Check if VR is initialized
    if (!g_context->isVrInitialized()) {
        g_context->SetLastError("VR audio is not initialized. Call audio_vrInitialize() first.");
        return -1;
    }

    // Validate input
    if (key == nullptr) {
        g_context->SetLastError("Invalid parameter: key cannot be null");
        return -1;
    }

    FMOD::System* system = g_context->GetFmodSystem();
    if (system == nullptr) {
        g_context->SetLastError("FMOD system is null");
        return -1;
    }

    // Find the VR object
    auto& vr_objects = g_context->GetVrObjects();
    auto it = vr_objects.find(key);
    if (it == vr_objects.end()) {
        g_context->SetLastError(std::string("VR object not found: ") + key);
        return -1;
    }

    VRObject& vrobj = it->second;

    // Update the stored position
    vrobj.center = pos;

    // Update the 3D position on the channel group's Resonance Audio Source DSP
    if (vrobj.channel_group != nullptr) {
        // Get the first DSP (which should be the Resonance Audio Source)
        FMOD::DSP* sourceDsp = nullptr;
        FMOD_RESULT result = vrobj.channel_group->getDSP(FMOD_CHANNELCONTROL_DSP_HEAD, &sourceDsp);
        if (result != FMOD_OK || sourceDsp == nullptr) {
            g_context->SetLastError(std::string("Failed to get Source DSP from channel group: ") + FMOD_ErrorString(result));
            return -1;
        }

        // Set 3D position for the channel group's DSP
        FMOD_VECTOR fmod_pos;
        fmod_pos.x = pos.width;
        fmod_pos.y = pos.height;
        fmod_pos.z = pos.depth;

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
            return -1;
        }
    }

    return 0;
}

} // extern "C"
