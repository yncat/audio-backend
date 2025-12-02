#include <iostream>
#include <vector>
#include "helper.h"
#include "../src/audio_backend.h"

void testBGMFunctions() {
    std::cout << "\n--- Testing BGM Functions ---\n";

    if (!initAudioBackend()) return;

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

    // Helper lambda for error checking
    auto checkError = [&](int result, const char* operation) -> bool {
        if (result == -1) {
            char errorBuffer[512];
            audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
            std::cout << "ERROR in " << operation << ": " << errorBuffer << "\n";
            audio_bgmFree(slot1);
            audio_bgmFree(slot2);
            audio_coreFree();
            return false;
        }
        return true;
    };

    // Test volume control
    std::cout << "\nSetting BGM volume to 0.7...\n";
    if (!checkError(audio_globalSetBgmVolume(0.7f), "audio_globalSetBgmVolume")) return;

    // Test bgmPlay
    std::cout << "Starting BGM 1 with bgmPlay...\n";
    if (!checkError(audio_bgmPlay(slot1), "audio_bgmPlay")) return;
    std::cout << "Playing... (wait 3 seconds)\n";
    waitSeconds(3);

    // Test pause
    std::cout << "\nPausing BGM 1...\n";
    if (!checkError(audio_bgmPause(slot1), "audio_bgmPause")) return;
    std::cout << "Paused for 1 second...\n";
    waitSeconds(1);

    // Test resume
    std::cout << "Resuming BGM 1...\n";
    if (!checkError(audio_bgmResume(slot1), "audio_bgmResume")) return;
    std::cout << "Playing... (wait 2 seconds)\n";
    waitSeconds(2);

    // Test crossfade
    std::cout << "\nCrossfading from BGM 1 to BGM 2 (2000ms)...\n";
    if (!checkError(audio_bgmCrossfade(slot1, slot2, 2000), "audio_bgmCrossfade")) return;
    std::cout << "Crossfading... (wait 3 seconds)\n";
    waitSeconds(3);

    // Test fadeout
    std::cout << "\nFading out BGM 2 (1500ms)...\n";
    if (!checkError(audio_bgmFadeout(slot2, 1500), "audio_bgmFadeout")) return;
    std::cout << "Fading out... (wait 2 seconds)\n";
    waitSeconds(2);

    // Test stop
    std::cout << "\nStopping all BGM...\n";
    if (!checkError(audio_bgmStop(slot1), "audio_bgmStop(slot1)")) return;
    if (!checkError(audio_bgmStop(slot2), "audio_bgmStop(slot2)")) return;

    // Free BGM
    std::cout << "Freeing BGM slots...\n";
    if (!checkError(audio_bgmFree(slot1), "audio_bgmFree(slot1)")) return;
    if (!checkError(audio_bgmFree(slot2), "audio_bgmFree(slot2)")) return;
    std::cout << "BGM slots freed\n";

    // Free audio backend
    std::cout << "\n";
    freeAudioBackend();

    std::cout << "\n--- BGM Test Completed ---\n";
}
