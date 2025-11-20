#ifndef CONTEXT_H
#define CONTEXT_H

#include <string>
#include "fmod/fmod.hpp"

class AudioBackendContext {
private:
    std::string last_error;
    FMOD::System* fmod_system;

public:
    AudioBackendContext();
    ~AudioBackendContext();

    void SetLastError(const std::string& error);
    std::string getLastError() const;

    FMOD::System* GetFmodSystem() const;
    void SetFmodSystem(FMOD::System* system);
};

#endif // CONTEXT_H
