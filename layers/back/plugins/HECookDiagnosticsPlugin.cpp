#include "HECookDiagnosticsPlugin.h"

void HECookDiagnosticsPlugin::onCookStarted(int nodeId){
    std::lock_guard lock(mutex_);
    active_[nodeId] = std::chrono::steady_clock::now();
    ++startedCount_;
}

void HECookDiagnosticsPlugin::onCookFinished(int nodeId, bool success){
    std::lock_guard lock(mutex_);
    auto it = active_.find(nodeId);
    if(it == active_.end())
        return;

    const auto end = std::chrono::steady_clock::now();
    const double ms = std::chrono::duration<double, std::milli>(end - it->second).count();
    active_.erase(it);

    durationsMs_.push_back(ms);
    lastDurationMs_ = ms;
    ++finishedCount_;
    if(!success)
        ++failedCount_;
}

uint64_t HECookDiagnosticsPlugin::startedCount() const{
    std::lock_guard lock(mutex_);
    return startedCount_;
}

uint64_t HECookDiagnosticsPlugin::finishedCount() const{
    std::lock_guard lock(mutex_);
    return finishedCount_;
}

uint64_t HECookDiagnosticsPlugin::failedCount() const{
    std::lock_guard lock(mutex_);
    return failedCount_;
}

bool HECookDiagnosticsPlugin::isCooking(int nodeId) const{
    std::lock_guard lock(mutex_);
    return active_.find(nodeId) != active_.end();
}

double HECookDiagnosticsPlugin::averageDurationMs() const{
    std::lock_guard lock(mutex_);
    if(durationsMs_.empty())
        return 0.0;

    double total = 0.0;
    for(double ms : durationsMs_)
        total += ms;
    return total / static_cast<double>(durationsMs_.size());
}

double HECookDiagnosticsPlugin::lastDurationMs() const{
    std::lock_guard lock(mutex_);
    return lastDurationMs_;
}
