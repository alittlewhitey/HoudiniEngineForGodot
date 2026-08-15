#ifndef HE_PARAMETER_AUDIT_PLUGIN_H
#define HE_PARAMETER_AUDIT_PLUGIN_H

#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_set>

#include "HEPlugin.h"

// Observes parameter writes and tracks which nodes became dirty.
class HEParameterAuditPlugin : public HEPlugin {
public:
    const char* name() const override{
        return "parameter_audit";
    }

    void onParameterChanged(int nodeId, const std::string& name, const HEParameterService::Values& values) override;

    uint64_t changeCount() const;
    std::size_t dirtyNodeCount() const;
    bool isDirty(int nodeId) const;
    std::string lastChangedParameter() const;

    void clear();

private:
    mutable std::mutex mutex_;
    std::unordered_set<int> dirtyNodes_;
    uint64_t changeCount_ = 0;
    int lastNodeId_ = -1;
    std::string lastParameter_;
};

#endif // HE_PARAMETER_AUDIT_PLUGIN_H
