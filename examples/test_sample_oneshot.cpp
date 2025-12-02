#include <iostream>
#include <vector>
#include "helper.h"
#include "../src/audio_backend.h"

void testSampleOneshot() {
    std::cout << "\n--- Testing Sample and Oneshot ---\n";

    if (!initAudioBackend()) return;

    // Load sample file from assets
    std::cout << "Loading sample (assets\\ding.ogg)...\n";
    std::vector<char> sample_data = loadFile("assets\\ding.ogg");
    if (sample_data.empty()) {
        std::cout << "FAILURE: Failed to load ding.ogg\n";
        audio_coreFree();
        return;
    }

    // Load sample with key
    std::cout << "Registering sample with key 'ding'...\n";
    int result = audio_sampleLoad(sample_data.data(), static_cast<int>(sample_data.size()), "ding");
    if (result != 0) {
        std::cout << "FAILURE: Failed to load sample\n";
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "Error: " << errorBuffer << "\n";
        audio_coreFree();
        return;
    }
    std::cout << "SUCCESS: Sample loaded\n";

    // Test: center, 100% volume, 100% pitch
    std::cout << "\nPlaying: center, volume=1.0, pitch=1.0\n";
    SoundAttributes attr = {0.0f, 1.0f, 1.0f};
    audio_sampleOneshot("ding", &attr);
    waitSeconds(1);

    // Test: left pan
    std::cout << "Playing: left pan (-1.0), volume=0.8, pitch=1.0\n";
    attr = {-1.0f, 0.8f, 1.0f};
    audio_sampleOneshot("ding", &attr);
    waitSeconds(1);

    // Test: right pan
    std::cout << "Playing: right pan (1.0), volume=0.8, pitch=1.0\n";
    attr = {1.0f, 0.8f, 1.0f};
    audio_sampleOneshot("ding", &attr);
    waitSeconds(1);

    // Test: low volume
    std::cout << "Playing: center, volume=0.3, pitch=1.0\n";
    attr = {0.0f, 0.3f, 1.0f};
    audio_sampleOneshot("ding", &attr);
    waitSeconds(1);

    // Test: high pitch
    std::cout << "Playing: center, volume=1.0, pitch=1.5 (higher)\n";
    attr = {0.0f, 1.0f, 1.5f};
    audio_sampleOneshot("ding", &attr);
    waitSeconds(1);

    // Test: low pitch
    std::cout << "Playing: center, volume=1.0, pitch=0.5 (lower)\n";
    attr = {0.0f, 1.0f, 0.5f};
    audio_sampleOneshot("ding", &attr);
    waitSeconds(2);

    // Free audio backend
    freeAudioBackend();

    std::cout << "\n--- Sample and Oneshot Test Completed ---\n";
}
