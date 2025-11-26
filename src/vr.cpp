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

    // Create a channel group for VR audio
    FMOD::ChannelGroup* vrGroup = nullptr;
    result = system->createChannelGroup("VR", &vrGroup);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to create VR channel group: ") + FMOD_ErrorString(result));
        system->unloadPlugin(plugin_handle);
        g_context->SetVrPluginHandle(0);
        return -1;
    }
    g_context->SetVrChannelGroup(vrGroup);

    // Get the Resonance Audio Listener plugin (nested plugin at index 0)
    unsigned int listener_plugin_handle = 0;
    result = system->getNestedPlugin(plugin_handle, 0, &listener_plugin_handle);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to get Resonance Audio Listener plugin (index 0): ") + FMOD_ErrorString(result));
        vrGroup->release();
        system->unloadPlugin(plugin_handle);
        g_context->SetVrChannelGroup(nullptr);
        g_context->SetVrPluginHandle(0);
        return -1;
    }

    // Create the Resonance Audio Listener DSP using the nested plugin handle
    FMOD::DSP* listenerDsp = nullptr;
    result = system->createDSPByPlugin(listener_plugin_handle, &listenerDsp);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to create Resonance Audio Listener DSP: ") + FMOD_ErrorString(result));
        vrGroup->release();
        system->unloadPlugin(plugin_handle);
        g_context->SetVrChannelGroup(nullptr);
        g_context->SetVrPluginHandle(0);
        return -1;
    }

    // Add the Listener DSP to the VR channel group
    // The Listener DSP processes spatial audio and room effects
    result = vrGroup->addDSP(FMOD_CHANNELCONTROL_DSP_TAIL, listenerDsp);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to add Listener DSP to VR channel group: ") + FMOD_ErrorString(result));
        listenerDsp->release();
        vrGroup->release();
        system->unloadPlugin(plugin_handle);
        g_context->SetVrListenerDsp(nullptr);
        g_context->SetVrChannelGroup(nullptr);
        g_context->SetVrPluginHandle(0);
        return -1;
    }
    g_context->SetVrListenerDsp(listenerDsp);

    // Set the initial 3D listener attributes
    ListenerAttributes& listener = g_context->GetVrListenerAttributes();
    result = system->set3DListenerAttributes(0, &listener.pos, &listener.vel, &listener.forward, &listener.up);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to set 3D listener attributes: ") + FMOD_ErrorString(result));
        listenerDsp->release();
        vrGroup->release();
        system->unloadPlugin(plugin_handle);
        g_context->SetVrListenerDsp(nullptr);
        g_context->SetVrChannelGroup(nullptr);
        g_context->SetVrPluginHandle(0);
        return -1;
    }

    g_context->setVrInitialized(true);

    return 0;
}

} // extern "C"
