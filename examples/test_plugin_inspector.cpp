#include <iostream>
#include <string>
#include <fstream>
#include "helper.h"
#include "../src/audio_backend.h"

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
