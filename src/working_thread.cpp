#include "working_thread.h"
#include "context.h"
#include "fmod/fmod.hpp"

extern AudioBackendContext* g_context;

// Thread handle
static HANDLE g_workerThread = NULL;
// Event to signal thread to stop
static HANDLE g_stopEvent = NULL;

// Worker thread function
static DWORD WINAPI workerThreadProc(LPVOID lpParam) {
    const DWORD UPDATE_INTERVAL_MS = 30;

    FMOD::System* system = g_context->GetFmodSystem();

    while (true) {
        // Wait for stop event or timeout (30ms)
        DWORD waitResult = WaitForSingleObject(g_stopEvent, UPDATE_INTERVAL_MS);

        if (waitResult == WAIT_OBJECT_0) {
            // Stop event signaled, exit thread
            break;
        }

        // Timeout occurred, perform FMOD update
        system->update();
    }

    return 0;
}

bool startWorkingThread() {
    // Create stop event (manual reset, initially non-signaled)
    g_stopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (g_stopEvent == NULL) {
        return false;
    }

    // Create worker thread
    g_workerThread = CreateThread(NULL, 0, workerThreadProc, NULL, 0, NULL);
    if (g_workerThread == NULL) {
        CloseHandle(g_stopEvent);
        g_stopEvent = NULL;
        return false;
    }

    return true;
}

void stopWorkingThread() {
    if (g_workerThread == NULL) {
        return;
    }

    // Signal the thread to stop
    SetEvent(g_stopEvent);

    // Wait for thread to finish
    WaitForSingleObject(g_workerThread, INFINITE);

    // Cleanup handles
    CloseHandle(g_workerThread);
    CloseHandle(g_stopEvent);
    g_workerThread = NULL;
    g_stopEvent = NULL;
}
