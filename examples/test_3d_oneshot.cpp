#include <iostream>
#include <vector>
#include "helper.h"
#include "../src/audio_backend.h"

void test3DOneshotSound() {
    std::cout << "\n--- Testing 3D Oneshot Sound ---\n";

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

    // Define positions around the listener in a circle
    struct TestPosition {
        const char* name;
        Position3D pos;
    };

    TestPosition positions[] = {
        {"Front (0, 0, 3)", {0, 3, 0}},
        {"Right-Front (2, 0, 2)", {2, 2, 0}},
        {"Right (3, 0, 0)", {3, 0, 0}},
        {"Right-Back (2, 0, -2)", {2, -2, 0}},
        {"Back (0, 0, -3)", {0, -3, 0}},
        {"Left-Back (-2, 0, -2)", {-2, -2, 0}},
        {"Left (-3, 0, 0)", {-3, 0, 0}},
        {"Left-Front (-2, 0, 2)", {-2, 2, 0}}
    };

    // Test 1: Play sounds around the listener with normal settings
    std::cout << "=== Test 1: Normal playback (volume=1.0, pitch=1.0) ===\n";
    SoundAttributes attr = {0.0f, 1.0f, 1.0f};
    for (const auto& test_pos : positions) {
        std::cout << "Playing at " << test_pos.name << "\n";
        result = audio_vrOneshotRelative("ding", &test_pos.pos, &attr, false);
        if (result != 0) {
            char errorBuffer[512];
            audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
            std::cout << "ERROR: " << errorBuffer << "\n";
        }
        waitSeconds(1);
    }

    std::cout << "\nWaiting a bit before next test...\n";
    waitSeconds(2);

    // Test 2: Play sounds with lower pitch
    std::cout << "\n=== Test 2: Lower pitch (volume=1.0, pitch=0.8) ===\n";
    attr = {0.0f, 1.0f, 0.8f};
    for (const auto& test_pos : positions) {
        std::cout << "Playing at " << test_pos.name << "\n";
        result = audio_vrOneshotRelative("ding", &test_pos.pos, &attr, false);
        if (result != 0) {
            char errorBuffer[512];
            audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
            std::cout << "ERROR: " << errorBuffer << "\n";
        }
        waitSeconds(1);
    }

    std::cout << "\nWaiting a bit before next test...\n";
    waitSeconds(2);

    // Test 3: Play sounds with lower volume
    std::cout << "\n=== Test 3: Lower volume (volume=0.3, pitch=1.0) ===\n";
    attr = {0.0f, 0.3f, 1.0f};
    for (const auto& test_pos : positions) {
        std::cout << "Playing at " << test_pos.name << "\n";
        result = audio_vrOneshotRelative("ding", &test_pos.pos, &attr, false);
        if (result != 0) {
            char errorBuffer[512];
            audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
            std::cout << "ERROR: " << errorBuffer << "\n";
        }
        waitSeconds(1);
    }

    // Free audio backend
    freeAudioBackend();

    std::cout << "\n--- 3D Oneshot Test Completed ---\n";
}
