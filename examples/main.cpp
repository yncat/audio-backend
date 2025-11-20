#include <iostream>
#include <string>
#include <limits>
#include <vector>
#include <cmath>
#include <thread>
#include <chrono>
#include "../src/audio_backend.h"

void displayMenu() {
    std::cout << "\n================================\n";
    std::cout << "Audio Backend Examples\n";
    std::cout << "================================\n";
    std::cout << "1: Test Core Init/Free\n";
    std::cout << "2: Test BGM Functions\n";
    std::cout << "0: Quit\n";
    std::cout << "================================\n";
    std::cout << "Select an option: ";
}

void testCoreInitFree() {
    std::cout << "\n--- Testing Core Init/Free ---\n";

    // Test initialization
    std::cout << "Calling audio_coreInitialize()...\n";
    int result = audio_coreInitialize();
    if (result == 1) {
        std::cout << "SUCCESS: Audio backend initialized\n";
    } else {
        std::cout << "FAILURE: Audio backend failed to initialize\n";
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "Error: " << errorBuffer << "\n";
        return;
    }

    // Test calling initialize again (should return 1 immediately)
    std::cout << "\nCalling audio_coreInitialize() again (should succeed immediately)...\n";
    result = audio_coreInitialize();
    if (result == 1) {
        std::cout << "SUCCESS: Already initialized, returned 1\n";
    } else {
        std::cout << "FAILURE: Should have returned 1 for already initialized\n";
    }

    // Test free
    std::cout << "\nCalling audio_coreFree()...\n";
    audio_coreFree();
    std::cout << "Audio backend freed\n";

    // Test calling free again (should be safe)
    std::cout << "\nCalling audio_coreFree() again (should be safe)...\n";
    audio_coreFree();
    std::cout << "Second free completed safely\n";

    std::cout << "\n--- Test Completed ---\n";
}

// Generate a simple WAV file in memory with a sine wave tone
std::vector<char> generateSineWaveWAV(float frequency, float duration_seconds, int sample_rate = 44100) {
    std::vector<char> wav_data;

    // Calculate sizes
    int num_samples = static_cast<int>(sample_rate * duration_seconds);
    int data_size = num_samples * 2; // 16-bit mono
    int file_size = 44 + data_size; // WAV header is 44 bytes

    // Reserve space
    wav_data.reserve(file_size);

    // Write WAV header
    // RIFF header
    wav_data.push_back('R'); wav_data.push_back('I'); wav_data.push_back('F'); wav_data.push_back('F');
    // File size - 8
    int chunk_size = file_size - 8;
    wav_data.push_back(chunk_size & 0xFF);
    wav_data.push_back((chunk_size >> 8) & 0xFF);
    wav_data.push_back((chunk_size >> 16) & 0xFF);
    wav_data.push_back((chunk_size >> 24) & 0xFF);
    // WAVE
    wav_data.push_back('W'); wav_data.push_back('A'); wav_data.push_back('V'); wav_data.push_back('E');
    // fmt chunk
    wav_data.push_back('f'); wav_data.push_back('m'); wav_data.push_back('t'); wav_data.push_back(' ');
    // fmt chunk size (16 for PCM)
    wav_data.push_back(16); wav_data.push_back(0); wav_data.push_back(0); wav_data.push_back(0);
    // Audio format (1 = PCM)
    wav_data.push_back(1); wav_data.push_back(0);
    // Number of channels (1 = mono)
    wav_data.push_back(1); wav_data.push_back(0);
    // Sample rate
    wav_data.push_back(sample_rate & 0xFF);
    wav_data.push_back((sample_rate >> 8) & 0xFF);
    wav_data.push_back((sample_rate >> 16) & 0xFF);
    wav_data.push_back((sample_rate >> 24) & 0xFF);
    // Byte rate (sample_rate * num_channels * bits_per_sample / 8)
    int byte_rate = sample_rate * 2;
    wav_data.push_back(byte_rate & 0xFF);
    wav_data.push_back((byte_rate >> 8) & 0xFF);
    wav_data.push_back((byte_rate >> 16) & 0xFF);
    wav_data.push_back((byte_rate >> 24) & 0xFF);
    // Block align (num_channels * bits_per_sample / 8)
    wav_data.push_back(2); wav_data.push_back(0);
    // Bits per sample
    wav_data.push_back(16); wav_data.push_back(0);
    // data chunk
    wav_data.push_back('d'); wav_data.push_back('a'); wav_data.push_back('t'); wav_data.push_back('a');
    // data chunk size
    wav_data.push_back(data_size & 0xFF);
    wav_data.push_back((data_size >> 8) & 0xFF);
    wav_data.push_back((data_size >> 16) & 0xFF);
    wav_data.push_back((data_size >> 24) & 0xFF);

    // Generate sine wave samples
    const float PI = 3.14159265359f;
    for (int i = 0; i < num_samples; i++) {
        float t = static_cast<float>(i) / sample_rate;
        float sample = std::sin(2.0f * PI * frequency * t);
        short sample_16bit = static_cast<short>(sample * 32767.0f * 0.5f); // 50% volume

        wav_data.push_back(sample_16bit & 0xFF);
        wav_data.push_back((sample_16bit >> 8) & 0xFF);
    }

    return wav_data;
}

void testBGMFunctions() {
    std::cout << "\n--- Testing BGM Functions ---\n";

    // Initialize audio backend first
    std::cout << "Initializing audio backend...\n";
    int result = audio_coreInitialize();
    if (result != 1) {
        std::cout << "FAILURE: Audio backend failed to initialize\n";
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "Error: " << errorBuffer << "\n";
        return;
    }
    std::cout << "Audio backend initialized successfully\n\n";

    // Generate test BGM data (440Hz sine wave for 2 seconds - A note)
    std::cout << "Generating test BGM 1 (440Hz A note)...\n";
    std::vector<char> bgm1_data = generateSineWaveWAV(440.0f, 2.0f);

    // Generate second test BGM data (523Hz sine wave for 2 seconds - C note)
    std::cout << "Generating test BGM 2 (523Hz C note)...\n";
    std::vector<char> bgm2_data = generateSineWaveWAV(523.25f, 2.0f);

    // Test loading BGM
    std::cout << "\nLoading BGM 1...\n";
    int slot1 = audio_bgmLoad(bgm1_data.data(), static_cast<int>(bgm1_data.size()));
    if (slot1 < 0) {
        std::cout << "FAILURE: Failed to load BGM 1\n";
        audio_coreFree();
        return;
    }
    std::cout << "SUCCESS: BGM 1 loaded to slot " << slot1 << "\n";

    std::cout << "Loading BGM 2...\n";
    int slot2 = audio_bgmLoad(bgm2_data.data(), static_cast<int>(bgm2_data.size()));
    if (slot2 < 0) {
        std::cout << "FAILURE: Failed to load BGM 2\n";
        audio_bgmFree(slot1);
        audio_coreFree();
        return;
    }
    std::cout << "SUCCESS: BGM 2 loaded to slot " << slot2 << "\n";

    // Test volume control
    std::cout << "\nSetting BGM volume to 0.7...\n";
    audio_globalSetBgmVolume(0.7f);

    // Test fadein
    std::cout << "Starting BGM 1 with 1000ms fadein...\n";
    audio_bgmFadein(slot1, 1000);
    std::cout << "Playing... (wait 3 seconds)\n";
    std::this_thread::sleep_for(std::chrono::seconds(3));

    // Test pause
    std::cout << "\nPausing BGM 1...\n";
    audio_bgmPause(slot1);
    std::cout << "Paused for 1 second...\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Test resume
    std::cout << "Resuming BGM 1...\n";
    audio_bgmResume(slot1);
    std::cout << "Playing... (wait 2 seconds)\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Test crossfade
    std::cout << "\nCrossfading from BGM 1 to BGM 2 (2000ms)...\n";
    audio_bgmCrossfade(slot1, slot2, 2000);
    std::cout << "Crossfading... (wait 3 seconds)\n";
    std::this_thread::sleep_for(std::chrono::seconds(3));

    // Test fadeout
    std::cout << "\nFading out BGM 2 (1500ms)...\n";
    audio_bgmFadeout(slot2, 1500);
    std::cout << "Fading out... (wait 2 seconds)\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Test stop
    std::cout << "\nStopping all BGM...\n";
    audio_bgmStop(slot1);
    audio_bgmStop(slot2);

    // Free BGM
    std::cout << "Freeing BGM slots...\n";
    audio_bgmFree(slot1);
    audio_bgmFree(slot2);
    std::cout << "BGM slots freed\n";

    // Free audio backend
    std::cout << "\nFreeing audio backend...\n";
    audio_coreFree();
    std::cout << "Audio backend freed\n";

    std::cout << "\n--- BGM Test Completed ---\n";
}

void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main() {
    int choice = -1;

    std::cout << "Audio Backend Example Program\n";

    // Get and display DLL version
    int major = audio_versionGetMajor();
    int minor = audio_versionGetMinor();
    int patch = audio_versionGetPatch();
    std::cout << "DLL Version: " << major << "." << minor << "." << patch << "\n";

    while (true) {
        displayMenu();

        if (!(std::cin >> choice)) {
            std::cout << "Invalid input. Please enter a number.\n";
            clearInput();
            continue;
        }

        clearInput();

        switch (choice) {
            case 0:
                std::cout << "Exiting...\n";
                return 0;

            case 1:
                testCoreInitFree();
                break;

            case 2:
                testBGMFunctions();
                break;

            default:
                std::cout << "Invalid option. Please try again.\n";
                break;
        }
    }

    return 0;
}
