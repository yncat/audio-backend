#ifndef CONTEXT_H
#define CONTEXT_H

#include <string>

class AudioBackendContext {
private:
    std::string last_error;

public:
    AudioBackendContext();
    ~AudioBackendContext();

    void SetLastError(const std::string& error);
    std::string getLastError() const;
};

#endif // CONTEXT_H
