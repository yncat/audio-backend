#include "context.h"
#include "working_thread.h"
#include "fmod/fmod.hpp"
#include "fmod/fmod_errors.h"

// External declaration of global context
extern AudioBackendContext* g_context;

extern "C" {

// Initialize FMOD and the audio backend
int coreInitialize() {
    // If already initialized, return success immediately
    if (isBackendInitialized()) {
        return 0;
    }

    // Create context if not exists
    if (g_context == nullptr) {
        g_context = new AudioBackendContext();
    }

    // Create FMOD system instance
    FMOD::System* system = nullptr;
    FMOD_RESULT result = FMOD::System_Create(&system);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to create FMOD system: ") + FMOD_ErrorString(result));
        return -1;
    }

    // Initialize FMOD system with default parameters
    // maxchannels: 512 channels should be sufficient for most games
    // flags: FMOD_INIT_NORMAL for standard initialization
    result = system->init(512, FMOD_INIT_NORMAL, nullptr);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to initialize FMOD system: ") + FMOD_ErrorString(result));
        system->release();
        return -1;
    }

    g_context->SetFmodSystem(system);

    // Create BGM channel group
    FMOD::ChannelGroup* bgmGroup = nullptr;
    result = system->createChannelGroup("BGM", &bgmGroup);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to create BGM channel group: ") + FMOD_ErrorString(result));
        system->release();
        g_context->SetFmodSystem(nullptr);
        return -1;
    }
    g_context->SetBgmChannelGroup(bgmGroup);

    g_context->setBackendInitialized(true);

    // Start working thread for automatic FMOD updates
    if (!startWorkingThread()) {
        g_context->SetLastError("Failed to start working thread");
        bgmGroup->release();
        system->release();
        g_context->SetBgmChannelGroup(nullptr);
        g_context->SetFmodSystem(nullptr);
        g_context->setBackendInitialized(false);
        return -1;
    }

    return 0;
}

// Free FMOD and the audio backend
void coreFree() {
    if (!isBackendInitialized()) {
        return;
    }

    // Stop working thread before closing FMOD
    stopWorkingThread();

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
