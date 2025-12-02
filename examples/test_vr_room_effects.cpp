#include <iostream>
#include <vector>
#include "helper.h"
#include "../src/audio_backend.h"

void testVrRoomEffects() {
    std::cout << "\n--- Testing VR Room Effects ---\n";

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

    // Load clap sample
    std::cout << "Loading sample (assets\\clap.wav)...\n";
    std::vector<char> sample_data = loadFile("assets\\clap.wav");
    if (sample_data.empty()) {
        std::cout << "FAILURE: Failed to load clap.wav\n";
        audio_coreFree();
        return;
    }

    // Load sample with key
    std::cout << "Registering sample with key 'clap'...\n";
    result = audio_sampleLoad(sample_data.data(), static_cast<int>(sample_data.size()), "clap");
    if (result != 0) {
        std::cout << "FAILURE: Failed to load sample\n";
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "Error: " << errorBuffer << "\n";
        audio_coreFree();
        return;
    }
    std::cout << "SUCCESS: Sample loaded\n\n";

    // Define Room 1: Small concrete room (5m x 5m x 3m)
    std::cout << "=== Creating Room 1: Small concrete room (5m x 5m x 3m) ===\n";
    Position3D room1Pos = {0.0f, 0.0f, 0.0f};
    Size3D room1Size = {5.0f, 5.0f, 3.0f};  // width, depth, height
    WallMaterials room1Materials = {
        "concrete_block_coarse",  // front
        "concrete_block_coarse",  // back
        "concrete_block_coarse",  // left
        "concrete_block_coarse",  // right
        "concrete_block_coarse",  // floor
        "concrete_block_coarse"   // ceiling
    };

    int room1Index = audio_vrRoomAdd(room1Pos, room1Size, &room1Materials);
    if (room1Index < 0) {
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "ERROR creating room 1: " << errorBuffer << "\n";
        audio_coreFree();
        return;
    }
    std::cout << "Room 1 created with index " << room1Index << "\n";

    // Define Room 2: Large carpeted room with curtains (10m x 8m x 4m)
    std::cout << "\n=== Creating Room 2: Large carpeted room with curtains (10m x 8m x 4m) ===\n";
    Position3D room2Pos = {0.0f, 0.0f, 0.0f};
    Size3D room2Size = {5.0f, 8.0f, 4.0f};  // width, depth, height
    WallMaterials room2Materials = {
        "curtain_heavy",           // front
        "curtain_heavy",           // back
        "plywood_panel",           // left
        "plywood_panel",           // right
        "grass",                   // floor (soft material like carpet)
        "acoustic_ceiling_tiles"   // ceiling
    };

    int room2Index = audio_vrRoomAdd(room2Pos, room2Size, &room2Materials);
    if (room2Index < 0) {
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "ERROR creating room 2: " << errorBuffer << "\n";
        audio_coreFree();
        return;
    }
    std::cout << "Room 2 created with index " << room2Index << "\n";

    // Test 1: Play without room effect
    std::cout << "\n=== Test 1: Playing clap without room effect ===\n";
    SoundAttributes attr = {0.0f, 1.0f, 1.0f};
    Position3D soundPos = {0.0f, 3.0f, 0.0f};  // In front of listener
    result = audio_vrOneshotRelative("clap", &soundPos, &attr, false);
    if (result != 0) {
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "ERROR: " << errorBuffer << "\n";
    }
    std::cout << "Playing... (wait 3 seconds)\n";
    waitSeconds(3);

    // Test 2: Switch to Room 1 and play
    std::cout << "\n=== Test 2: Switching to Room 1 (concrete room) ===\n";
    result = audio_vrRoomChange(room1Index);
    if (result != 0) {
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "ERROR changing room: " << errorBuffer << "\n";
    } else {
        std::cout << "Switched to Room 1\n";
    }

    std::cout << "Playing clap in concrete room...\n";
    result = audio_vrOneshotRelative("clap", &soundPos, &attr, false);
    if (result != 0) {
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "ERROR: " << errorBuffer << "\n";
    }
    std::cout << "Playing... (wait 3 seconds)\n";
    waitSeconds(3);

    // Test 3: Switch to Room 2 and play
    std::cout << "\n=== Test 3: Switching to Room 2 (carpeted room) ===\n";
    result = audio_vrRoomChange(room2Index);
    if (result != 0) {
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "ERROR changing room: " << errorBuffer << "\n";
    } else {
        std::cout << "Switched to Room 2\n";
    }

    std::cout << "Playing clap in carpeted room...\n";
    result = audio_vrOneshotRelative("clap", &soundPos, &attr, false);
    if (result != 0) {
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "ERROR: " << errorBuffer << "\n";
    }
    std::cout << "Playing... (wait 3 seconds)\n";
    waitSeconds(3);

    // Test 4: Clear rooms and play again
    std::cout << "\n=== Test 4: Clearing rooms and playing ===\n";
    result = audio_vrRoomClear();
    if (result != 0) {
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "ERROR clearing rooms: " << errorBuffer << "\n";
    } else {
        std::cout << "Rooms cleared (back to default)\n";
    }

    std::cout << "Playing clap without room effect again...\n";
    result = audio_vrOneshotRelative("clap", &soundPos, &attr, false);
    if (result != 0) {
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "ERROR: " << errorBuffer << "\n";
    }
    std::cout << "Playing... (wait 3 seconds)\n";
    waitSeconds(3);

    // Free audio backend
    freeAudioBackend();

    std::cout << "\n--- VR Room Effects Test Completed ---\n";
}
