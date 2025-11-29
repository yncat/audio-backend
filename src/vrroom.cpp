#include <iostream>
#include "context.h"
#include "adapter_resonance.h"
#include "fmod/fmod.hpp"
#include "fmod/fmod_errors.h"
#include <cstring>

// External declaration of global context
extern AudioBackendContext* g_context;

// Add a room definition and return its index
int vrRoomAdd(Position3D centerPosition, Size3D roomSize, WallMaterials* materials) {
    // Check if backend is initialized
    if (!isBackendInitialized()) {
        g_context->SetLastError("Backend is not initialized. Call audio_coreInitialize() first.");
        return -1;
    }

    // Check if VR is initialized
    if (!g_context->isVrInitialized()) {
        g_context->SetLastError("VR audio is not initialized. Call audio_vrInitialize() first.");
        return -1;
    }

    // Validate materials pointer
    if (!materials) {
        g_context->SetLastError("Materials pointer is null");
        return -1;
    }

    // Create a new StoredRoom
    StoredRoom newRoom;
    newRoom.centerPosition = centerPosition;
    newRoom.roomSize = roomSize;

    // Copy material strings to std::string (automatic memory management)
    if (materials->front) newRoom.materialFront = materials->front;
    if (materials->back) newRoom.materialBack = materials->back;
    if (materials->left) newRoom.materialLeft = materials->left;
    if (materials->right) newRoom.materialRight = materials->right;
    if (materials->floor) newRoom.materialFloor = materials->floor;
    if (materials->ceiling) newRoom.materialCeiling = materials->ceiling;

    std::vector<StoredRoom>& rooms = g_context->GetVrRooms();
    rooms.push_back(newRoom);
    std::cout << "vector pushed back" << std::endl;

    // Return the index of the newly added room
    return static_cast<int>(rooms.size() - 1);
}

// Change to a specific room by index
int vrRoomChange(int index) {
    // Check if backend is initialized
    if (!isBackendInitialized()) {
        g_context->SetLastError("Backend is not initialized. Call audio_coreInitialize() first.");
        return -1;
    }

    // Check if VR is initialized
    if (!g_context->isVrInitialized()) {
        g_context->SetLastError("VR audio is not initialized. Call audio_vrInitialize() first.");
        return -1;
    }

    std::vector<StoredRoom>& rooms = g_context->GetVrRooms();

    // Validate index
    if (index < 0 || index >= static_cast<int>(rooms.size())) {
        g_context->SetLastError("Invalid room index");
        return -1;
    }

    // Get the room
    const StoredRoom& room = rooms[index];

    // Convert to Resonance RoomProperties
    vraudio::RoomProperties roomProps = convertToResonanceRoomProperties(room);

    // Get the VR listener DSP
    FMOD::DSP* listenerDsp = g_context->GetVrListenerDsp();
    if (!listenerDsp) {
        g_context->SetLastError("VR listener DSP is not initialized");
        return -1;
    }

    // Set room properties on the listener DSP
    // According to FMOD Resonance Audio plugin documentation,
    // parameter index 1 is for room properties
    FMOD_RESULT result = listenerDsp->setParameterData(1, &roomProps, sizeof(vraudio::RoomProperties));
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to set room properties: ") + FMOD_ErrorString(result));
        return -1;
    }

    return 0;
}

// Clear all rooms and reset to default
int vrRoomClear() {
    // Check if backend is initialized
    if (!isBackendInitialized()) {
        g_context->SetLastError("Backend is not initialized. Call audio_coreInitialize() first.");
        return -1;
    }

    // Check if VR is initialized
    if (!g_context->isVrInitialized()) {
        g_context->SetLastError("VR audio is not initialized. Call audio_vrInitialize() first.");
        return -1;
    }

    std::vector<StoredRoom>& rooms = g_context->GetVrRooms();
    rooms.clear();

    // Reset listener DSP to default room properties
    FMOD::DSP* listenerDsp = g_context->GetVrListenerDsp();
    if (!listenerDsp) {
        g_context->SetLastError("VR listener DSP is not initialized");
        return -1;
    }

    // Create default room properties
    vraudio::RoomProperties defaultProps;  // Uses default constructor

    // Set default room properties on the listener DSP
    FMOD_RESULT result = listenerDsp->setParameterData(1, &defaultProps, sizeof(vraudio::RoomProperties));
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to reset room properties: ") + FMOD_ErrorString(result));
        return -1;
    }

    return 0;
}
