#include "context.h"
#include "fmod/fmod.hpp"
#include "fmod/fmod_errors.h"

// External declaration of global context
extern AudioBackendContext* g_context;

extern "C" {

// Initialize FMOD and the audio backend
__declspec(dllexport) int audio_coreInitialize() {
    // If already initialized, return success immediately
    if (g_context != nullptr) {
        return 1;
    }

    // Create FMOD system instance
    FMOD::System* system = nullptr;
    FMOD_RESULT result = FMOD::System_Create(&system);
    if (result != FMOD_OK) {
        // Create temporary context to store error
        AudioBackendContext* temp_context = new AudioBackendContext();
        temp_context->SetLastError(std::string("Failed to create FMOD system: ") + FMOD_ErrorString(result));
        g_context = temp_context;
        return 0;
    }

    // Initialize FMOD system with default parameters
    // maxchannels: 512 channels should be sufficient for most games
    // flags: FMOD_INIT_NORMAL for standard initialization
    result = system->init(512, FMOD_INIT_NORMAL, nullptr);
    if (result != FMOD_OK) {
        // Create temporary context to store error
        AudioBackendContext* temp_context = new AudioBackendContext();
        temp_context->SetLastError(std::string("Failed to initialize FMOD system: ") + FMOD_ErrorString(result));
        g_context = temp_context;

        // Release the system since initialization failed
        system->release();
        return 0;
    }

    // Create and configure the AudioBackendContext
    AudioBackendContext* context = new AudioBackendContext();
    context->SetFmodSystem(system);

    // Create BGM channel group
    FMOD::ChannelGroup* bgmGroup = nullptr;
    result = system->createChannelGroup("BGM", &bgmGroup);
    if (result != FMOD_OK) {
        context->SetLastError(std::string("Failed to create BGM channel group: ") + FMOD_ErrorString(result));
        g_context = context;
        system->release();
        return 0;
    }
    context->SetBgmChannelGroup(bgmGroup);

    g_context = context;

    return 1;
}

// Free FMOD and the audio backend
__declspec(dllexport) void audio_coreFree() {
    if (g_context == nullptr) {
        return;
    }

    // Get FMOD system and close it
    FMOD::System* system = g_context->GetFmodSystem();
    if (system != nullptr) {
        system->close();
        system->release();
    }

    // Delete the context
    delete g_context;
    g_context = nullptr;
}

} // extern "C"
