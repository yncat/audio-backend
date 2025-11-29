#include "adapter_resonance.h"
#include <cstring>
#include <cctype>
#include <string>

// Helper function to convert string to lowercase for case-insensitive comparison
std::string toLower(const char* str) {
    if (!str) return "";
    std::string result = str;
    for (char& c : result) {
        c = std::tolower(static_cast<unsigned char>(c));
    }
    return result;
}

// Convert material name string to Resonance Audio MaterialName enum
int convertToResonanceMaterialEnum(const char* material_name) {
    if (!material_name) {
        return -1;
    }

    std::string name = toLower(material_name);

    if (name == "transparent") {
        return vraudio::MaterialName::kTransparent;
    } else if (name == "acoustic_ceiling_tiles") {
        return vraudio::MaterialName::kAcousticCeilingTiles;
    } else if (name == "brick_bare") {
        return vraudio::MaterialName::kBrickBare;
    } else if (name == "brick_painted") {
        return vraudio::MaterialName::kBrickPainted;
    } else if (name == "concrete_block_coarse") {
        return vraudio::MaterialName::kConcreteBlockCoarse;
    } else if (name == "concrete_block_painted") {
        return vraudio::MaterialName::kConcreteBlockPainted;
    } else if (name == "curtain_heavy") {
        return vraudio::MaterialName::kCurtainHeavy;
    } else if (name == "fiber_glass_insulation") {
        return vraudio::MaterialName::kFiberGlassInsulation;
    } else if (name == "glass_thin") {
        return vraudio::MaterialName::kGlassThin;
    } else if (name == "glass_thick") {
        return vraudio::MaterialName::kGlassThick;
    } else if (name == "grass") {
        return vraudio::MaterialName::kGrass;
    } else if (name == "linoleum_on_concrete") {
        return vraudio::MaterialName::kLinoleumOnConcrete;
    } else if (name == "marble") {
        return vraudio::MaterialName::kMarble;
    } else if (name == "metal") {
        return vraudio::MaterialName::kMetal;
    } else if (name == "parquet_on_concrete") {
        return vraudio::MaterialName::kParquetOnConcrete;
    } else if (name == "plaster_rough") {
        return vraudio::MaterialName::kPlasterRough;
    } else if (name == "plaster_smooth") {
        return vraudio::MaterialName::kPlasterSmooth;
    } else if (name == "plywood_panel") {
        return vraudio::MaterialName::kPlywoodPanel;
    } else if (name == "polished_concrete_or_tile") {
        return vraudio::MaterialName::kPolishedConcreteOrTile;
    } else if (name == "sheetrock") {
        return vraudio::MaterialName::kSheetrock;
    } else if (name == "water_or_ice_surface") {
        return vraudio::MaterialName::kWaterOrIceSurface;
    } else if (name == "wood_ceiling") {
        return vraudio::MaterialName::kWoodCeiling;
    } else if (name == "wood_panel") {
        return vraudio::MaterialName::kWoodPanel;
    } else if (name == "uniform") {
        return vraudio::MaterialName::kUniform;
    } else {
        return -1;  // Unknown material
    }
}

// Convert StoredRoom to Resonance Audio RoomProperties
vraudio::RoomProperties convertToResonanceRoomProperties(const StoredRoom& room) {
    vraudio::RoomProperties props;

    // Set center position
    // Convert from Position3D (width, depth, height) to RoomProperties position (x, y, z)
    // Our coordinate system: width->X, depth->Z, height->Y
    // Resonance coordinate system: position[0]=x, position[1]=y, position[2]=z
    props.position[0] = static_cast<float>(room.centerPosition.width);
    props.position[1] = static_cast<float>(room.centerPosition.height);
    props.position[2] = static_cast<float>(room.centerPosition.depth);

    // Set rotation to no rotation (identity quaternion: w=1, x=0, y=0, z=0)
    props.rotation[0] = 0.0f;  // x
    props.rotation[1] = 0.0f;  // y
    props.rotation[2] = 0.0f;  // z
    props.rotation[3] = 1.0f;  // w

    // Set room dimensions
    // Convert from Size3D (width, depth, height) to RoomProperties dimensions (x, y, z)
    // Our coordinate system: width->X, depth->Z, height->Y
    // Resonance coordinate system: dimensions[0]=x, dimensions[1]=y, dimensions[2]=z
    props.dimensions[0] = room.roomSize.width;
    props.dimensions[1] = room.roomSize.height;
    props.dimensions[2] = room.roomSize.depth;

    // Convert and set wall materials
    // Resonance material_names order:
    // [0] left (-)ive x-axis wall
    // [1] right (+)ive x-axis wall
    // [2] floor (-)ive y-axis wall (bottom)
    // [3] ceiling (+)ive y-axis wall (top)
    // [4] front (-)ive z-axis wall
    // [5] back (+)ive z-axis wall

    int leftMat = convertToResonanceMaterialEnum(room.materialLeft.c_str());
    int rightMat = convertToResonanceMaterialEnum(room.materialRight.c_str());
    int floorMat = convertToResonanceMaterialEnum(room.materialFloor.c_str());
    int ceilingMat = convertToResonanceMaterialEnum(room.materialCeiling.c_str());
    int frontMat = convertToResonanceMaterialEnum(room.materialFront.c_str());
    int backMat = convertToResonanceMaterialEnum(room.materialBack.c_str());

    // Use transparent as default for invalid materials
    props.material_names[0] = leftMat >= 0 ? static_cast<vraudio::MaterialName>(leftMat) : vraudio::MaterialName::kTransparent;
    props.material_names[1] = rightMat >= 0 ? static_cast<vraudio::MaterialName>(rightMat) : vraudio::MaterialName::kTransparent;
    props.material_names[2] = floorMat >= 0 ? static_cast<vraudio::MaterialName>(floorMat) : vraudio::MaterialName::kTransparent;
    props.material_names[3] = ceilingMat >= 0 ? static_cast<vraudio::MaterialName>(ceilingMat) : vraudio::MaterialName::kTransparent;
    props.material_names[4] = frontMat >= 0 ? static_cast<vraudio::MaterialName>(frontMat) : vraudio::MaterialName::kTransparent;
    props.material_names[5] = backMat >= 0 ? static_cast<vraudio::MaterialName>(backMat) : vraudio::MaterialName::kTransparent;

    // Leave other properties (reflection_scalar, reverb_gain, reverb_time, reverb_brightness) at default values
    // They are already initialized to defaults by RoomProperties constructor

    return props;
}
