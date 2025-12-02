#include <iostream>
#include <vector>
#include "helper.h"
#include "../src/audio_backend.h"

void testVrObject() {
    std::cout << "\n--- Testing VR Object ---\n";

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

    // Load sample files
    std::cout << "Loading samples...\n";

    std::vector<char> missile_data = loadFile("assets\\missile.ogg");
    if (missile_data.empty()) {
        std::cout << "FAILURE: Failed to load missile.ogg\n";
        audio_coreFree();
        return;
    }
    result = audio_sampleLoad(missile_data.data(), static_cast<int>(missile_data.size()), "missile");
    if (result != 0) {
        std::cout << "FAILURE: Failed to load missile sample\n";
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "Error: " << errorBuffer << "\n";
        audio_coreFree();
        return;
    }
    std::cout << "SUCCESS: missile.ogg loaded\n";

    std::vector<char> explosion_data = loadFile("assets\\explosion.ogg");
    if (explosion_data.empty()) {
        std::cout << "FAILURE: Failed to load explosion.ogg\n";
        audio_coreFree();
        return;
    }
    result = audio_sampleLoad(explosion_data.data(), static_cast<int>(explosion_data.size()), "explosion");
    if (result != 0) {
        std::cout << "FAILURE: Failed to load explosion sample\n";
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "Error: " << errorBuffer << "\n";
        audio_coreFree();
        return;
    }
    std::cout << "SUCCESS: explosion.ogg loaded\n";

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
    std::cout << "SUCCESS: gunloop.ogg loaded\n";

    std::vector<char> gunend_data = loadFile("assets\\gunend.ogg");
    if (gunend_data.empty()) {
        std::cout << "FAILURE: Failed to load gunend.ogg\n";
        audio_coreFree();
        return;
    }
    result = audio_sampleLoad(gunend_data.data(), static_cast<int>(gunend_data.size()), "gunend");
    if (result != 0) {
        std::cout << "FAILURE: Failed to load gunend sample\n";
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "Error: " << errorBuffer << "\n";
        audio_coreFree();
        return;
    }
    std::cout << "SUCCESS: gunend.ogg loaded\n\n";

    // Test scenario: Gun with loop sound and missile object
    std::cout << "=== Starting VR Object Test Scenario ===\n\n";

    // Create gun object at position (5, 3, 0) with looped sound
    std::cout << "1. Creating 'gun' object at position (5, 3, 0) with gunloop...\n";
    VRObjectInfo gunInfo;
    gunInfo.position = {5.0f, 0.0f, 3.0f};  // width=5, depth=0, height=3
    gunInfo.size = {1.0f, 1.0f, 1.0f};
    gunInfo.looped_sample_key = "gunloop";

    result = audio_vrObjectAdd("gun", &gunInfo);
    if (result != 0) {
        std::cout << "FAILURE: Failed to add gun object\n";
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "Error: " << errorBuffer << "\n";
        audio_coreFree();
        return;
    }
    std::cout << "SUCCESS: Gun object created\n";

    // Start looping the gun sound
    std::cout << "2. Starting gun loop sound...\n";
    result = audio_vrObjectStartLooping("gun");
    if (result != 0) {
        std::cout << "FAILURE: Failed to start gun looping\n";
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "Error: " << errorBuffer << "\n";
        audio_coreFree();
        return;
    }
    std::cout << "SUCCESS: Gun loop playing\n";

    // Wait ~3 seconds
    std::cout << "3. Waiting 3 seconds...\n";
    waitSeconds(3);

    // Create missile object at same position as gun (no loop sound)
    std::cout << "4. Creating 'missile' object at same position as gun (5, 3, 0)...\n";
    VRObjectInfo missileInfo;
    missileInfo.position = {5.0f, 0.0f, 3.0f};
    missileInfo.size = {0.5f, 0.5f, 0.5f};
    missileInfo.looped_sample_key = nullptr;  // No looped sound

    result = audio_vrObjectAdd("missile", &missileInfo);
    if (result != 0) {
        std::cout << "FAILURE: Failed to add missile object\n";
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "Error: " << errorBuffer << "\n";
        audio_coreFree();
        return;
    }
    std::cout << "SUCCESS: Missile object created\n";

    // Play missile sound from missile object
    std::cout << "5. Playing missile.ogg from missile object...\n";
    SoundAttributes missileAttr = {0.0f, 1.0f, 1.0f};
    result = audio_vrObjectPlayOneshot("missile", "missile", &missileAttr);
    if (result != 0) {
        std::cout << "FAILURE: Failed to play missile oneshot\n";
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "Error: " << errorBuffer << "\n";
        audio_coreFree();
        return;
    }
    std::cout << "SUCCESS: Missile sound playing\n";

    // Move missile object 10 times, decreasing x coordinate
    std::cout << "6. Moving missile object (x coordinate decreasing from 5 to -5)...\n";
    for (int i = 0; i < 10; i++) {
        // Calculate new position - x decreases from 5 to -5 over 10 steps
        float newX = 5.0f - (i * 1.0f);
        Position3D newPos = {newX, 0.0f, 3.0f};

        // Update missile position
        result = audio_vrObjectChangePosition("missile", newPos);
        if (result != 0) {
            std::cout << "FAILURE: Failed to change missile position\n";
            char errorBuffer[512];
            audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
            std::cout << "Error: " << errorBuffer << "\n";
            audio_coreFree();
            return;
        }

        std::cout << "   Missile moved to x=" << newX << " (frame " << (i+1) << "/10)\n";

        // Wait 150ms between moves
        waitMilliseconds(150);
    }

    // Play explosion at position (-5, 3, 0) using absolute positioning
    std::cout << "7. Playing explosion.ogg at position (-5, 3, 0) using absolute oneshot...\n";
    Position3D explosionPos = {-5.0f, 0.0f, 3.0f};
    SoundAttributes explosionAttr = {0.0f, 1.0f, 1.0f};
    result = audio_vrOneshotAbsolute("explosion", &explosionPos, &explosionAttr);
    if (result != 0) {
        std::cout << "FAILURE: Failed to play explosion\n";
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "Error: " << errorBuffer << "\n";
        audio_coreFree();
        return;
    }
    std::cout << "SUCCESS: Explosion sound playing\n";

    // Remove missile object
    std::cout << "8. Removing missile object...\n";
    result = audio_vrObjectRemove("missile");
    if (result != 0) {
        std::cout << "FAILURE: Failed to remove missile object\n";
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "Error: " << errorBuffer << "\n";
        audio_coreFree();
        return;
    }
    std::cout << "SUCCESS: Missile object removed\n";

    // Wait 3 seconds
    std::cout << "9. Waiting 3 seconds...\n";
    waitSeconds(3);

    // Play gunend sound from gun object
    std::cout << "10. Playing gunend.ogg from gun object...\n";
    SoundAttributes gunendAttr = {0.0f, 1.0f, 1.0f};
    result = audio_vrObjectPlayOneshot("gun", "gunend", &gunendAttr);
    if (result != 0) {
        std::cout << "FAILURE: Failed to play gunend\n";
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "Error: " << errorBuffer << "\n";
        audio_coreFree();
        return;
    }
    std::cout << "SUCCESS: Gunend sound playing\n";

    // Pause gun loop
    std::cout << "11. Pausing gun loop...\n";
    result = audio_vrObjectPauseLooping("gun");
    if (result != 0) {
        std::cout << "FAILURE: Failed to pause gun loop\n";
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "Error: " << errorBuffer << "\n";
        audio_coreFree();
        return;
    }
    std::cout << "SUCCESS: Gun loop paused\n";

    // Wait 2 seconds
    std::cout << "12. Waiting 2 seconds...\n";
    waitSeconds(2);

    // Remove gun object
    std::cout << "13. Removing gun object...\n";
    result = audio_vrObjectRemove("gun");
    if (result != 0) {
        std::cout << "FAILURE: Failed to remove gun object\n";
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "Error: " << errorBuffer << "\n";
        audio_coreFree();
        return;
    }
    std::cout << "SUCCESS: Gun object removed\n";

    // Free audio backend
    freeAudioBackend();

    std::cout << "\n=== VR Object Test Completed Successfully ===\n";
}
