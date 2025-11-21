#include <iostream>
#include <string>
#include <limits>
#include <vector>
#include <cmath>
#include <thread>
#include <chrono>
#include <fstream>
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
    if (result == 0) {
        std::cout << "SUCCESS: Audio backend initialized\n";
    } else {
        std::cout << "FAILURE: Audio backend failed to initialize\n";
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "Error: " << errorBuffer << "\n";
        return;
    }

    // Test calling initialize again (should return 0 immediately)
    std::cout << "\nCalling audio_coreInitialize() again (should succeed immediately)...\n";
    result = audio_coreInitialize();
    if (result == 0) {
        std::cout << "SUCCESS: Already initialized, returned 0\n";
    } else {
        std::cout << "FAILURE: Should have returned 0 for already initialized\n";
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

// Load a file into memory
std::vector<char> loadFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path << "\n";
        return {};
    }
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        std::cerr << "Failed to read file: " << path << "\n";
        return {};
    }
    return buffer;
}

void testBGMFunctions() {
    std::cout << "\n--- Testing BGM Functions ---\n";

    // Initialize audio backend first
    std::cout << "Initializing audio backend...\n";
    int result = audio_coreInitialize();
    if (result != 0) {
        std::cout << "FAILURE: Audio backend failed to initialize\n";
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "Error: " << errorBuffer << "\n";
        return;
    }
    std::cout << "Audio backend initialized successfully\n\n";

    // Load BGM files from assets
    std::cout << "Loading BGM 1 (assets\\bgm_full.ogg)...\n";
    std::vector<char> bgm1_data = loadFile("assets\\bgm_full.ogg");
    if (bgm1_data.empty()) {
        std::cout << "FAILURE: Failed to load bgm_full.ogg\n";
        audio_coreFree();
        return;
    }

    std::cout << "Loading BGM 2 (assets\\bgm_hats.ogg)...\n";
    std::vector<char> bgm2_data = loadFile("assets\\bgm_hats.ogg");
    if (bgm2_data.empty()) {
        std::cout << "FAILURE: Failed to load bgm_hats.ogg\n";
        audio_coreFree();
        return;
    }

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
