#include <iostream>
#include "helper.h"
#include "../src/audio_backend.h"

void testVrInitialization() {
    std::cout << "\n--- Testing VR Initialization ---\n";

    if (!initAudioBackend()) return;

    // Test VR initialization
    std::cout << "Initializing VR audio with resonanceaudio.dll...\n";
    int result = audio_vrInitialize("resonanceaudio.dll");
    if (result == 0) {
        std::cout << "SUCCESS: VR audio initialized\n";
    } else {
        std::cout << "FAILURE: VR audio failed to initialize\n";
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "Error: " << errorBuffer << "\n";
        audio_coreFree();
        return;
    }

    // Test calling initialize again (should return 0 immediately)
    std::cout << "\nCalling audio_vrInitialize() again (should succeed immediately)...\n";
    result = audio_vrInitialize("resonanceaudio.dll");
    if (result == 0) {
        std::cout << "SUCCESS: Already initialized, returned 0\n";
    } else {
        std::cout << "FAILURE: Should have returned 0 for already initialized\n";
    }

    // Free audio backend
    freeAudioBackend();

    std::cout << "\n--- VR Initialization Test Completed ---\n";
}
