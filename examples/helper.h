#ifndef HELPER_H
#define HELPER_H

#include <string>
#include <vector>

// Wait for specified seconds
void waitSeconds(int seconds);

// Wait for specified milliseconds
void waitMilliseconds(int milliseconds);

// Load a file into memory
std::vector<char> loadFile(const std::string& path);

// Common initialization for tests
bool initAudioBackend();

// Common cleanup for tests
void freeAudioBackend();

// Clear input buffer
void clearInput();

#endif // HELPER_H
