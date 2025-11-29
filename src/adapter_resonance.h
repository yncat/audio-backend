#ifndef ADAPTER_RESONANCE_H
#define ADAPTER_RESONANCE_H

#include "vrstructs.h"
#include "resonance_room_properties.h"

// Convert material name string to Resonance Audio MaterialName enum
// Returns -1 if the material name is not recognized
int convertToResonanceMaterialEnum(const char* material_name);

// Convert StoredRoom to Resonance Audio RoomProperties
vraudio::RoomProperties convertToResonanceRoomProperties(const StoredRoom& room);

#endif // ADAPTER_RESONANCE_H
