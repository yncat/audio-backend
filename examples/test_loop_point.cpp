#include <iostream>
#include <vector>
#include "helper.h"
#include "../src/audio_backend.h"

void testLoopPoint() {
    std::cout << "\n--- Testing Loop Point ---\n";

    if (!initAudioBackend()) return;

    // Load BGM file from assets
    std::cout << "Loading BGM (assets\\cat_music.ogg)...\n";
    std::vector<char> bgm_data = loadFile("assets\\cat_music.ogg");
    if (bgm_data.empty()) {
        std::cout << "FAILURE: Failed to load cat_music.ogg\n";
        audio_coreFree();
        return;
    }

    // Load BGM to slot
    int slot = audio_bgmLoad(bgm_data.data(), static_cast<int>(bgm_data.size()));
    if (slot < 0) {
        std::cout << "FAILURE: Failed to load BGM\n";
        audio_coreFree();
        return;
    }
    std::cout << "SUCCESS: BGM loaded to slot " << slot << "\n";

    // Helper lambda for error checking
    auto checkError = [&](int result, const char* operation) -> bool {
        if (result == -1) {
            char errorBuffer[512];
            audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
            std::cout << "ERROR in " << operation << ": " << errorBuffer << "\n";
            audio_bgmFree(slot);
            audio_coreFree();
            return false;
        }
        return true;
    };

    // Set volume to 0.4 (cat song is loud)
    std::cout << "Setting BGM volume to 0.4...\n";
    if (!checkError(audio_globalSetBgmVolume(0.4f), "audio_globalSetBgmVolume")) return;

    // Set loop point to 15.50 seconds (15500ms)
    std::cout << "Setting loop point to 15500ms...\n";
    if (!checkError(audio_bgmSetLoopPoint(slot, 15500), "audio_bgmSetLoopPoint")) return;

    // Start playback
    std::cout << "Starting BGM playback...\n";
    if (!checkError(audio_bgmPlay(slot), "audio_bgmPlay")) return;

    std::cout << "\nBGM is now playing with loop point at 15.50 seconds.\n";
    std::cout << "Press Enter to fade out and exit...\n";

    // Wait for Enter key
    std::cin.get();

    // Fadeout
    std::cout << "Fading out...\n";
    if (!checkError(audio_bgmFadeout(slot, 1500), "audio_bgmFadeout")) return;
    waitSeconds(2);

    // Free BGM
    std::cout << "Freeing BGM slot...\n";
    if (!checkError(audio_bgmFree(slot), "audio_bgmFree")) return;

    // Free audio backend
    freeAudioBackend();

    std::cout << "\n--- Loop Point Test Completed ---\n";
}
