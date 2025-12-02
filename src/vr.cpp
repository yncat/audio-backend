#include "context.h"
#include "fmod/fmod.hpp"
#include "fmod/fmod_errors.h"

// External declaration of global context
extern AudioBackendContext* g_context;

extern "C" {

// Initialize VR audio (Google Resonance Audio plugin)
int vrInitialize(const char* plugin_path) {
    // Check if backend is initialized
    if (!isBackendInitialized()) {
        g_context->SetLastError("Backend is not initialized. Call audio_coreInitialize() first.");
        return -1;
    }

    // Check if VR is already initialized
    if (g_context->isVrInitialized()) {
        // Already initialized, return success
        return 0;
    }

    FMOD::System* system = g_context->GetFmodSystem();
    if (system == nullptr) {
        g_context->SetLastError("FMOD system is null");
        return -1;
    }

    // Load the Resonance Audio plugin
    unsigned int plugin_handle = 0;
    FMOD_RESULT result = system->loadPlugin(plugin_path, &plugin_handle);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to load VR plugin: ") + FMOD_ErrorString(result));
        return -1;
    }
    g_context->SetVrPluginHandle(plugin_handle);

    // Get the master channel group
    FMOD::ChannelGroup* masterGroup = nullptr;
    result = system->getMasterChannelGroup(&masterGroup);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to get master channel group: ") + FMOD_ErrorString(result));
        system->unloadPlugin(plugin_handle);
        g_context->SetVrPluginHandle(0);
        return -1;
    }

    // Get the Resonance Audio Listener plugin (nested plugin at index 0)
    unsigned int listener_plugin_handle = 0;
    result = system->getNestedPlugin(plugin_handle, 0, &listener_plugin_handle);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to get Resonance Audio Listener plugin (index 0): ") + FMOD_ErrorString(result));
        system->unloadPlugin(plugin_handle);
        g_context->SetVrPluginHandle(0);
        return -1;
    }

    // Create the Resonance Audio Listener DSP using the nested plugin handle
    FMOD::DSP* listenerDsp = nullptr;
    result = system->createDSPByPlugin(listener_plugin_handle, &listenerDsp);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to create Resonance Audio Listener DSP: ") + FMOD_ErrorString(result));
        system->unloadPlugin(plugin_handle);
        g_context->SetVrPluginHandle(0);
        return -1;
    }

    // Add the Listener DSP to the master channel group
    // The Listener DSP processes spatial audio and room effects
    result = masterGroup->addDSP(FMOD_CHANNELCONTROL_DSP_TAIL, listenerDsp);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to add Listener DSP to master channel group: ") + FMOD_ErrorString(result));
        listenerDsp->release();
        system->unloadPlugin(plugin_handle);
        g_context->SetVrListenerDsp(nullptr);
        g_context->SetVrPluginHandle(0);
        g_context->SetVrSourcePluginHandle(0);
        return -1;
    }
    g_context->SetVrListenerDsp(listenerDsp);

    // Get the Resonance Audio Source plugin (nested plugin at index 2)
    unsigned int source_plugin_handle = 0;
    result = system->getNestedPlugin(plugin_handle, 2, &source_plugin_handle);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to get Resonance Audio Source plugin (index 2): ") + FMOD_ErrorString(result));
        listenerDsp->release();
        system->unloadPlugin(plugin_handle);
        g_context->SetVrListenerDsp(nullptr);
        g_context->SetVrPluginHandle(0);
        g_context->SetVrSourcePluginHandle(0);
        return -1;
    }
    g_context->SetVrSourcePluginHandle(source_plugin_handle);

    // Set the initial 3D listener attributes
    ListenerAttributes& listener = g_context->GetVrListenerAttributes();
    result = system->set3DListenerAttributes(0, &listener.pos, &listener.vel, &listener.forward, &listener.up);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to set 3D listener attributes: ") + FMOD_ErrorString(result));
        listenerDsp->release();
        system->unloadPlugin(plugin_handle);
        g_context->SetVrListenerDsp(nullptr);
        g_context->SetVrPluginHandle(0);
        g_context->SetVrSourcePluginHandle(0);
        return -1;
    }

    // Create player_sounds channel group
    FMOD::ChannelGroup* playerSoundsGroup = nullptr;
    result = system->createChannelGroup("player_sounds", &playerSoundsGroup);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to create player_sounds channel group: ") + FMOD_ErrorString(result));
        listenerDsp->release();
        system->unloadPlugin(plugin_handle);
        g_context->SetVrListenerDsp(nullptr);
        g_context->SetVrPluginHandle(0);
        g_context->SetVrSourcePluginHandle(0);
        return -1;
    }

    // Attach the player_sounds group to master
    result = masterGroup->addGroup(playerSoundsGroup);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to add player_sounds group to master: ") + FMOD_ErrorString(result));
        playerSoundsGroup->release();
        listenerDsp->release();
        system->unloadPlugin(plugin_handle);
        g_context->SetVrListenerDsp(nullptr);
        g_context->SetVrPluginHandle(0);
        g_context->SetVrSourcePluginHandle(0);
        return -1;
    }

    // Create a Resonance Audio Source DSP for the player_sounds group
    // This allows player sounds to be positioned at the player's location
    FMOD::DSP* playerSourceDsp = nullptr;
    result = system->createDSPByPlugin(source_plugin_handle, &playerSourceDsp);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to create Resonance Audio Source DSP for player_sounds: ") + FMOD_ErrorString(result));
        playerSoundsGroup->release();
        listenerDsp->release();
        system->unloadPlugin(plugin_handle);
        g_context->SetVrListenerDsp(nullptr);
        g_context->SetVrPluginHandle(0);
        g_context->SetVrSourcePluginHandle(0);
        return -1;
    }

    // Attach resonance audio source DSP to player_sounds channel group
    // This allows player sounds to be positioned and go through resonance audio
    result = playerSoundsGroup->addDSP(FMOD_CHANNELCONTROL_DSP_HEAD, playerSourceDsp);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to add Source DSP to player_sounds group: ") + FMOD_ErrorString(result));
        playerSourceDsp->release();
        playerSoundsGroup->release();
        listenerDsp->release();
        system->unloadPlugin(plugin_handle);
        g_context->SetVrListenerDsp(nullptr);
        g_context->SetVrPluginHandle(0);
        g_context->SetVrSourcePluginHandle(0);
        return -1;
    }

    // Set distance attenuation parameters for player source DSP
    // Parameter [2]: Min Distance
    result = playerSourceDsp->setParameterFloat(2, 0.5f);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to set min distance for player source DSP: ") + FMOD_ErrorString(result));
        playerSourceDsp->release();
        playerSoundsGroup->release();
        listenerDsp->release();
        system->unloadPlugin(plugin_handle);
        g_context->SetVrListenerDsp(nullptr);
        g_context->SetVrPluginHandle(0);
        g_context->SetVrSourcePluginHandle(0);
        return -1;
    }

    // Parameter [3]: Max Distance
    result = playerSourceDsp->setParameterFloat(3, 200.0f);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to set max distance for player source DSP: ") + FMOD_ErrorString(result));
        playerSourceDsp->release();
        playerSoundsGroup->release();
        listenerDsp->release();
        system->unloadPlugin(plugin_handle);
        g_context->SetVrListenerDsp(nullptr);
        g_context->SetVrPluginHandle(0);
        g_context->SetVrSourcePluginHandle(0);
        return -1;
    }

    g_context->SetVrPlayerSoundsGroup(playerSoundsGroup);
    g_context->SetVrPlayerSourceDsp(playerSourceDsp);

    g_context->setVrInitialized(true);

    return 0;
}

} // extern "C"
