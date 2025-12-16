#include "vrpositioning.h"
#include <algorithm>
#include <cmath>

extern "C" {

void updateObjectSoundPosition(VRObject* obj, Position3D* playerPos) {
    // If object is not wide, simply use center position
    if (!obj->is_wide) {
        obj->sound_position = obj->center;
        return;
    }

    // Calculate object bounds
    // min = center - size, max = center + size
    float min_x = obj->center.width - obj->size.width;
    float max_x = obj->center.width + obj->size.width;
    float min_y = obj->center.height - obj->size.height;
    float max_y = obj->center.height + obj->size.height;
    float min_z = obj->center.depth - obj->size.depth;
    float max_z = obj->center.depth + obj->size.depth;

    // Check if player is inside the object bounds
    bool inside_x = (playerPos->width >= min_x && playerPos->width <= max_x);
    bool inside_y = (playerPos->height >= min_y && playerPos->height <= max_y);
    bool inside_z = (playerPos->depth >= min_z && playerPos->depth <= max_z);

    // If player is inside the object, use player position as sound position
    if (inside_x && inside_y && inside_z) {
        obj->sound_position = *playerPos;
        return;
    }

    // Calculate closest point on object to player for each axis
    float sound_x, sound_y, sound_z;

    // X axis
    if (playerPos->width < min_x) {
        sound_x = min_x;
    } else if (playerPos->width > max_x) {
        sound_x = max_x;
    } else {
        sound_x = playerPos->width;
    }

    // Y axis
    if (playerPos->height < min_y) {
        sound_y = min_y;
    } else if (playerPos->height > max_y) {
        sound_y = max_y;
    } else {
        sound_y = playerPos->height;
    }

    // Z axis
    if (playerPos->depth < min_z) {
        sound_z = min_z;
    } else if (playerPos->depth > max_z) {
        sound_z = max_z;
    } else {
        sound_z = playerPos->depth;
    }

    // Set the sound position
    obj->sound_position.width = sound_x;
    obj->sound_position.height = sound_y;
    obj->sound_position.depth = sound_z;
}

} // extern "C"
