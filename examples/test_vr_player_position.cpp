#include <iostream>
#include <vector>
#include "helper.h"
#include "../src/audio_backend.h"

void testVrPlayerPositionAndSound() {
    std::cout << "\n--- Testing VR Player Position & Sound ---\n";

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

    // Load sample file
    std::cout << "Loading sample (assets\\ding.ogg)...\n";
    std::vector<char> sample_data = loadFile("assets\\ding.ogg");
    if (sample_data.empty()) {
        std::cout << "FAILURE: Failed to load ding.ogg\n";
        audio_coreFree();
        return;
    }

    // Load sample with key
    std::cout << "Registering sample with key 'ding'...\n";
    result = audio_sampleLoad(sample_data.data(), static_cast<int>(sample_data.size()), "ding");
    if (result != 0) {
        std::cout << "FAILURE: Failed to load sample\n";
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "Error: " << errorBuffer << "\n";
        audio_coreFree();
        return;
    }
    std::cout << "SUCCESS: Sample loaded\n\n";

    // Test 1: Set player position and play player sound
    std::cout << "=== Test 1: Player at origin (0, 0, 0) ===\n";
    result = audio_vrPlayerSetPosition(0.0f, 0.0f, 0.0f);
    if (result != 0) {
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "ERROR setting position: " << errorBuffer << "\n";
    } else {
        std::cout << "Player position set to (0, 0, 0)\n";
    }

    std::cout << "Playing player sound (footstep)...\n";
    SoundAttributes attr = {0.0f, 1.0f, 1.0f};
    result = audio_vrOneshotPlayer("ding", &attr);
    if (result != 0) {
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "ERROR: " << errorBuffer << "\n";
    }
    waitSeconds(2);

    // Test 2: Move player to the right
    std::cout << "\n=== Test 2: Player at (5, 0, 0) - Right ===\n";
    result = audio_vrPlayerSetPosition(5.0f, 0.0f, 0.0f);
    if (result != 0) {
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "ERROR setting position: " << errorBuffer << "\n";
    } else {
        std::cout << "Player position set to (5, 0, 0)\n";
    }

    std::cout << "Playing player sound...\n";
    result = audio_vrOneshotPlayer("ding", &attr);
    if (result != 0) {
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "ERROR: " << errorBuffer << "\n";
    }
    waitSeconds(2);

    // Test 3: Move player to the left
    std::cout << "\n=== Test 3: Player at (-5, 0, 0) - Left ===\n";
    result = audio_vrPlayerSetPosition(-5.0f, 0.0f, 0.0f);
    if (result != 0) {
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "ERROR setting position: " << errorBuffer << "\n";
    } else {
        std::cout << "Player position set to (-5, 0, 0)\n";
    }

    std::cout << "Playing player sound...\n";
    result = audio_vrOneshotPlayer("ding", &attr);
    if (result != 0) {
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "ERROR: " << errorBuffer << "\n";
    }
    waitSeconds(2);

    // Test 4: Move player forward
    std::cout << "\n=== Test 4: Player at (0, 5, 0) - Forward ===\n";
    result = audio_vrPlayerSetPosition(0.0f, 5.0f, 0.0f);
    if (result != 0) {
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "ERROR setting position: " << errorBuffer << "\n";
    } else {
        std::cout << "Player position set to (0, 5, 0)\n";
    }

    std::cout << "Playing player sound...\n";
    result = audio_vrOneshotPlayer("ding", &attr);
    if (result != 0) {
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "ERROR: " << errorBuffer << "\n";
    }
    waitSeconds(2);

    // Test 5: Test player rotation (turning around)
    std::cout << "\n=== Test 5: Testing player rotation ===\n";
    result = audio_vrPlayerSetPosition(0.0f, 0.0f, 0.0f);
    std::cout << "Player position reset to (0, 0, 0)\n";

    // Set rotation to look backwards
    UnitVector3D frontBackward = {0.0f, -1.0f, 0.0f};  // Looking backward
    UnitVector3D up = {0.0f, 0.0f, 1.0f};  // Up is still up
    result = audio_vrPlayerSetRotation(&frontBackward, &up);
    if (result != 0) {
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "ERROR setting rotation: " << errorBuffer << "\n";
    } else {
        std::cout << "Player rotation set to look backward\n";
    }

    std::cout << "Playing player sound...\n";
    result = audio_vrOneshotPlayer("ding", &attr);
    if (result != 0) {
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "ERROR: " << errorBuffer << "\n";
    }
    waitSeconds(2);

    // Test 6: Multiple quick sounds at different positions
    std::cout << "\n=== Test 6: Walking simulation (multiple positions) ===\n";
    float positions[][3] = {
        {0.0f, 0.0f, 0.0f},
        {0.5f, 1.0f, 0.0f},
        {1.0f, 2.0f, 0.0f},
        {1.5f, 3.0f, 0.0f},
        {2.0f, 4.0f, 0.0f}
    };

    for (int i = 0; i < 5; i++) {
        result = audio_vrPlayerSetPosition(positions[i][0], positions[i][1], positions[i][2]);
        std::cout << "Player at (" << positions[i][0] << ", " << positions[i][1] << ", " << positions[i][2] << ")\n";

        result = audio_vrOneshotPlayer("ding", &attr);
        if (result != 0) {
            char errorBuffer[512];
            audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
            std::cout << "ERROR: " << errorBuffer << "\n";
        }
        waitSeconds(1);
    }

    // Free audio backend
    freeAudioBackend();

    std::cout << "\n--- VR Player Position & Sound Test Completed ---\n";
}
