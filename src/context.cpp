#include "context.h"

// Global context variable (pointer)
// Not null when backend is initialized
AudioBackendContext* g_context = nullptr;

AudioBackendContext::AudioBackendContext() : last_error("") {
}

AudioBackendContext::~AudioBackendContext() {
}

void AudioBackendContext::SetLastError(const std::string& error) {
    last_error = error;
}

std::string AudioBackendContext::getLastError() const {
    return last_error;
}
