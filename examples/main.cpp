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
    std::cout << "3: Test Loop Point\n";
    std::cout << "4: Test Sample and Oneshot\n";
    std::cout << "5: Test VR Initialization\n";
    std::cout << "6: Test Plugin Inspector\n";
    std::cout << "7: Test 3D Oneshot\n";
    std::cout << "8: Test VR Player Position & Sound\n";
    std::cout << "9: Test VR Room Effects\n";
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

// Wait for specified seconds
void waitSeconds(int seconds) {
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
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

// Common initialization for tests
bool initAudioBackend() {
    std::cout << "Initializing audio backend...\n";
    int result = audio_coreInitialize();
    if (result != 0) {
        std::cout << "FAILURE: Audio backend failed to initialize\n";
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "Error: " << errorBuffer << "\n";
        return false;
    }
    std::cout << "Audio backend initialized successfully\n\n";
    return true;
}

// Common cleanup for tests
void freeAudioBackend() {
    std::cout << "Freeing audio backend...\n";
    audio_coreFree();
    std::cout << "Audio backend freed\n";
}

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

void testVrInitialization() {
    std::cout << "\n--- Testing VR Initialization ---\n";

    if (!initAudioBackend()) return;

    // Test VR initialization
    std::cout << "Initializing VR audio with resonanceaudio.dll...\n";
    int result = audio_vrInitialize("resonanceaudio.dll");
    if (result == 0) {
        std::cout << "SUCCESS: VR audio initialized\n";
    } else {
        std::cout << "FAILURE: VR audio failed to initialize\n";
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "Error: " << errorBuffer << "\n";
        audio_coreFree();
        return;
    }

    // Test calling initialize again (should return 0 immediately)
    std::cout << "\nCalling audio_vrInitialize() again (should succeed immediately)...\n";
    result = audio_vrInitialize("resonanceaudio.dll");
    if (result == 0) {
        std::cout << "SUCCESS: Already initialized, returned 0\n";
    } else {
        std::cout << "FAILURE: Should have returned 0 for already initialized\n";
    }

    // Free audio backend
    freeAudioBackend();

    std::cout << "\n--- VR Initialization Test Completed ---\n";
}

void testPluginInspector() {
    std::cout << "\n--- Testing Plugin Inspector ---\n";

    if (!initAudioBackend()) return;

    // Get plugin path from user
    std::string plugin_path;
    std::cout << "Enter plugin path (e.g., lib\\resonanceaudio.dll): ";
    std::getline(std::cin, plugin_path);

    if (plugin_path.empty()) {
        std::cout << "No plugin path specified. Using default: resonanceaudio.dll\n";
        plugin_path = "resonanceaudio.dll";
    }

    // Get output file path from user
    std::string output_path;
    std::cout << "Enter output file path (e.g., plugin_info.txt): ";
    std::getline(std::cin, output_path);

    if (output_path.empty()) {
        std::cout << "No output path specified. Using default: plugin_info.txt\n";
        output_path = "plugin_info.txt";
    }

    // Run plugin inspector
    std::cout << "\nInspecting plugin: " << plugin_path << "\n";
    std::cout << "Output file: " << output_path << "\n";

    int result = audio_corePluginInspect(plugin_path.c_str(), output_path.c_str());
    if (result == 0) {
        std::cout << "SUCCESS: Plugin inspection completed\n";
        std::cout << "Plugin information has been written to " << output_path << "\n";

        // Display the output file content
        std::cout << "\n--- Plugin Information ---\n";
        std::ifstream infile(output_path);
        if (infile.is_open()) {
            std::string line;
            while (std::getline(infile, line)) {
                std::cout << line << "\n";
            }
            infile.close();
        }
        std::cout << "--- End of Plugin Information ---\n";
    } else {
        std::cout << "FAILURE: Plugin inspection failed\n";
        char errorBuffer[512];
        audio_errorGetLast(errorBuffer, sizeof(errorBuffer));
        std::cout << "Error: " << errorBuffer << "\n";
    }

    // Free audio backend
    freeAudioBackend();

    std::cout << "\n--- Plugin Inspector Test Completed ---\n";
}

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
    Position3D room1Pos = {0, 0, 0};
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
    Position3D room2Pos = {0, 0, 0};
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
    Position3D soundPos = {0, 3, 0};  // In front of listener
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

            case 3:
                testLoopPoint();
                break;

            case 4:
                testSampleOneshot();
                break;

            case 5:
                testVrInitialization();
                break;

            case 6:
                testPluginInspector();
                break;

            case 7:
                test3DOneshotSound();
                break;

            case 8:
                testVrPlayerPositionAndSound();
                break;

            case 9:
                testVrRoomEffects();
                break;

            default:
                std::cout << "Invalid option. Please try again.\n";
                break;
        }
    }

    return 0;
}
