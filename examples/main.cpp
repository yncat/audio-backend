#include <iostream>
#include "helper.h"
#include "../src/audio_backend.h"

// Forward declarations of test functions
void testCoreInitFree();
void testBGMFunctions();
void testLoopPoint();
void testSampleOneshot();
void testVrInitialization();
void testPluginInspector();
void test3DOneshotSound();
void testVrPlayerPositionAndSound();
void testVrRoomEffects();

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
