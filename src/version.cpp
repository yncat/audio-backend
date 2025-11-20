#include "version.h"

// Version constants following semantic versioning
const int MAJOR_VERSION = 1;
const int MINOR_VERSION = 0;
const int PATCH_VERSION = 0;

int getMajorVersion() {
    return MAJOR_VERSION;
}

int getMinorVersion() {
    return MINOR_VERSION;
}

int getPatchVersion() {
    return PATCH_VERSION;
}
