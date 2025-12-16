#include <iostream>
#include <vector>
#include "helper.h"
#include "../src/audio_backend.h"

void testVrObjectWide() {
    std::cout << "\n--- Testing VR Object Wide Positioning ---\n";

    if (!initAudioBackend()) return;

    // Initialize VR audio
    std::cout << "Initializing VR audio with resonanceaudio.dll...\n";
    int result = audio_vrInitialize("resonanceaudio.dll");
    if (result != 0) {
        std::cout << "FAILURE: VR audio failed to initialize\n";
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "Error: " << errorBuffer << "\n";
        audio_coreFree();
        return;
    }
    std::cout << "SUCCESS: VR audio initialized\n";

    // Load gunloop sample
    std::cout << "Loading gunloop.ogg sample...\n";
    std::vector<char> gunloop_data = loadFile("assets\\gunloop.ogg");
    if (gunloop_data.empty()) {
        std::cout << "FAILURE: Failed to load gunloop.ogg\n";
        audio_coreFree();
        return;
    }
    result = audio_sampleLoad(gunloop_data.data(), static_cast<int>(gunloop_data.size()), "gunloop");
    if (result != 0) {
        std::cout << "FAILURE: Failed to load gunloop sample\n";
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "Error: " << errorBuffer << "\n";
        audio_coreFree();
        return;
    }
    std::cout << "SUCCESS: gunloop.ogg loaded\n\n";

    // Test scenario: Wide object positioning test
    std::cout << "=== Starting Wide Object Positioning Test ===\n\n";

    // Create a wide VR object with center at (3, 0, 3) and size (3, 3, 3)
    // This creates an object spanning:
    // X: [0, 6], Y: [-3, 3], Z: [0, 6]
    std::cout << "1. Creating wide object with center (3, 0, 3) and size (3, 3, 3)...\n";
    std::cout << "   Object bounds: X[0, 6], Y[-3, 3], Z[0, 6]\n";
    VRObjectInfo objInfo;
    objInfo.position = {3.0f, 3.0f, 0.0f};  // width=3, height=0, depth=3
    objInfo.size = {3.0f, 3.0f, 3.0f};
    objInfo.looped_sample_key = "gunloop";

    result = audio_vrObjectAdd("wide_object", &objInfo);
    if (result != 0) {
        std::cout << "FAILURE: Failed to add wide object\n";
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "Error: " << errorBuffer << "\n";
        audio_coreFree();
        return;
    }
    std::cout << "SUCCESS: Wide object created\n";

    // Start looping the sound
    std::cout << "2. Starting loop sound...\n";
    result = audio_vrObjectStartLooping("wide_object");
    if (result != 0) {
        std::cout << "FAILURE: Failed to start looping\n";
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "Error: " << errorBuffer << "\n";
        audio_coreFree();
        return;
    }
    std::cout << "SUCCESS: Loop playing\n\n";

    // Position 1: Listener at (-1, 0, 0) - should hear object slightly from the right
    std::cout << "3. Setting listener position to (-1, 0, 0)...\n";
    std::cout << "   Expected: Object should sound slightly from the right\n";
    result = audio_vrPlayerSetPosition(-1.0f, 0.0f, 0.0f);
    if (result != 0) {
        std::cout << "FAILURE: Failed to set player position\n";
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "Error: " << errorBuffer << "\n";
        audio_coreFree();
        return;
    }
    std::cout << "SUCCESS: Listener at (-1, 0, 0)\n";
    std::cout << "   Waiting 3 seconds...\n";
    waitSeconds(3);

    // Position 2: Listener at (0, 0, 0) - adjacent to left corner, should sound centered
    std::cout << "\n4. Setting listener position to (0, 0, 0)...\n";
    std::cout << "   Expected: Adjacent to left corner, sound should be centered\n";
    result = audio_vrPlayerSetPosition(0.0f, 0.0f, 0.0f);
    if (result != 0) {
        std::cout << "FAILURE: Failed to set player position\n";
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "Error: " << errorBuffer << "\n";
        audio_coreFree();
        return;
    }
    std::cout << "SUCCESS: Listener at (0, 0, 0)\n";
    std::cout << "   Waiting 3 seconds...\n";
    waitSeconds(3);

    // Position 3: Listener at (3, 0, 0) - inside object, should still sound centered
    std::cout << "\n5. Setting listener position to (3, 0, 0)...\n";
    std::cout << "   Expected: Inside/near center of object, sound should be centered\n";
    result = audio_vrPlayerSetPosition(3.0f, 0.0f, 0.0f);
    if (result != 0) {
        std::cout << "FAILURE: Failed to set player position\n";
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "Error: " << errorBuffer << "\n";
        audio_coreFree();
        return;
    }
    std::cout << "SUCCESS: Listener at (3, 0, 0)\n";
    std::cout << "   Waiting 3 seconds...\n";
    waitSeconds(3);

    // Position 4: Listener at (7, 0, 0) - should hear object slightly from the left
    std::cout << "\n6. Setting listener position to (7, 0, 0)...\n";
    std::cout << "   Expected: Object should sound slightly from the left\n";
    result = audio_vrPlayerSetPosition(7.0f, 0.0f, 0.0f);
    if (result != 0) {
        std::cout << "FAILURE: Failed to set player position\n";
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "Error: " << errorBuffer << "\n";
        audio_coreFree();
        return;
    }
    std::cout << "SUCCESS: Listener at (7, 0, 0)\n";
    std::cout << "   Waiting 3 seconds...\n";
    waitSeconds(3);

    // Remove object and finish
    std::cout << "\n7. Removing wide object...\n";
    result = audio_vrObjectRemove("wide_object");
    if (result != 0) {
        std::cout << "FAILURE: Failed to remove object\n";
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "Error: " << errorBuffer << "\n";
        audio_coreFree();
        return;
    }
    std::cout << "SUCCESS: Wide object removed\n";

    // Free audio backend
    freeAudioBackend();

    std::cout << "\n=== Wide Object Positioning Test Completed Successfully ===\n";
}
