#ifndef HE_COOK_DIAGNOSTICS_PLUGIN_H
#define HE_COOK_DIAGNOSTICS_PLUGIN_H

#include <chrono>
#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "HEPlugin.h"

// Built-in diagnostics plugin. It is pure C++ and only observes HECore
// lifecycle events; it never touches Godot or HAPI types.
class HECookDiagnosticsPlugin : public HEPlugin {
public:
    const char* name() const override{
        return "cook_diagnostics";
    }

    void onCookStarted(int nodeId) override;
    void onCookFinished(int nodeId, bool success) override;

    uint64_t startedCount() const;
    uint64_t finishedCount() const;
    uint64_t failedCount() const;
    bool isCooking(int nodeId) const;

    double averageDurationMs() const;
    double lastDurationMs() const;

private:
    mutable std::mutex mutex_;
    std::unordered_map<int, std::chrono::steady_clock::time_point> active_;
    std::vector<double> durationsMs_;
    uint64_t startedCount_ = 0;
    uint64_t finishedCount_ = 0;
    uint64_t failedCount_ = 0;
    double lastDurationMs_ = 0.0;
};

#endif // HE_COOK_DIAGNOSTICS_PLUGIN_H
