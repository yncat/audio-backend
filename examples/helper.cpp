#include "helper.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <limits>
#include "../src/audio_backend.h"

// Wait for specified seconds
void waitSeconds(int seconds) {
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

// Wait for specified milliseconds
void waitMilliseconds(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
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

// Clear input buffer
void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
