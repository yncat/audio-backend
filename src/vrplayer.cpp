#include "context.h"
#include "vrstructs.h"
#include "fmod/fmod.hpp"
#include "fmod/fmod_errors.h"
#include "fmod/fmod_dsp.h"

// External declaration of global context
extern AudioBackendContext* g_context;

extern "C" {

// Set player position in 3D space
int setPlayerPosition(float width, float depth, float height) {
    // Check if VR is initialized
    if (!g_context->isVrInitialized()) {
        g_context->SetLastError("VR audio is not initialized. Call audio_vrInitialize() first.");
        return -1;
    }

    // Convert from game coordinates to FMOD coordinates
    // width->X, depth->Z, height->Y
    FMOD_VECTOR position;
    position.x = width;
    position.y = height;
    position.z = depth;

    // Update the player position in context
    g_context->SetVrPlayerPosition(position);

    FMOD::System* system = g_context->GetFmodSystem();
    if (system == nullptr) {
        g_context->SetLastError("FMOD system is null");
        return -1;
    }

    // Update the 3D listener position (the player's ears move with the player)
    FMOD_VECTOR& forward = g_context->GetVrPlayerForward();
    FMOD_VECTOR& up = g_context->GetVrPlayerUp();
    FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };

    FMOD_RESULT result = system->set3DListenerAttributes(0, &position, &vel, &forward, &up);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to set 3D listener position: ") + FMOD_ErrorString(result));
        return -1;
    }

    // Update VR listener attributes in context
    ListenerAttributes& listener = g_context->GetVrListenerAttributes();
    listener.pos = position;
    listener.vel = vel;
    listener.forward = forward;
    listener.up = up;

    return 0;
}

// Set player rotation (orientation) in 3D space
int setPlayerRotation(const UnitVector3D* front, const UnitVector3D* up) {
    // Check if VR is initialized
    if (!g_context->isVrInitialized()) {
        g_context->SetLastError("VR audio is not initialized. Call audio_vrInitialize() first.");
        return -1;
    }

    // Validate inputs
    if (front == nullptr || up == nullptr) {
        g_context->SetLastError("Invalid parameters: front and up vectors cannot be null");
        return -1;
    }

    // Convert from game coordinates to FMOD coordinates
    // width->X, depth->Z, height->Y
    FMOD_VECTOR forward;
    forward.x = front->width;
    forward.y = front->height;
    forward.z = front->depth;

    FMOD_VECTOR upVector;
    upVector.x = up->width;
    upVector.y = up->height;
    upVector.z = up->depth;

    // Update the player orientation in context
    g_context->SetVrPlayerForward(forward);
    g_context->SetVrPlayerUp(upVector);

    FMOD::System* system = g_context->GetFmodSystem();
    if (system == nullptr) {
        g_context->SetLastError("FMOD system is null");
        return -1;
    }

    // Update the 3D listener orientation
    FMOD_VECTOR& position = g_context->GetVrPlayerPosition();
    FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };

    FMOD_RESULT result = system->set3DListenerAttributes(0, &position, &vel, &forward, &upVector);
    if (result != FMOD_OK) {
        g_context->SetLastError(std::string("Failed to set 3D listener rotation: ") + FMOD_ErrorString(result));
        return -1;
    }

    // Update VR listener attributes in context
    ListenerAttributes& listener = g_context->GetVrListenerAttributes();
    listener.pos = position;
    listener.vel = vel;
    listener.forward = forward;
    listener.up = upVector;

    return 0;
}

} // extern "C"
