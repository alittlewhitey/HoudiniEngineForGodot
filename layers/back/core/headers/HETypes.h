#ifndef HE_TYPES_H
#define HE_TYPES_H

#include <cstdint>
#include <string>
#include <vector>

// Pure C++ session type. The Godot frontend mirrors this as HEGSessionType.
enum class HESessionType : int {
    None = 0,
    InProcess = 1,
    NewNamedPipe = 2,
    NewTCPSocket = 3,
    ExistingNamedPipe = 4,
    ExistingTCPSocket = 5,
    ExistingSharedMemory = 6
};

struct HENodeHandle {
    int id = -1;
    uint64_t epoch = 0;
};

struct HEAssetHandle {
    int id = -1;
    uint64_t epoch = 0;
};

struct HEAssetRecord {
    HEAssetHandle handle;
    std::string path;
    std::vector<std::string> operators;
};

#endif // HE_TYPES_H
