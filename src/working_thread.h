#ifndef WORKING_THREAD_H
#define WORKING_THREAD_H

#include <Windows.h>

// Start the working thread
// Returns true on success, false on failure
bool startWorkingThread();

// Stop the working thread and wait for it to finish
void stopWorkingThread();

#endif // WORKING_THREAD_H
