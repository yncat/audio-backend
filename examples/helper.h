#ifndef HELPER_H
#define HELPER_H

#include <string>
#include <vector>

// Wait for specified seconds
void waitSeconds(int seconds);

// Load a file into memory
std::vector<char> loadFile(const std::string& path);

// Common initialization for tests
bool initAudioBackend();

// Common cleanup for tests
void freeAudioBackend();

// Clear input buffer
void clearInput();

#endif // HELPER_H
