#include "HEParameterAuditPlugin.h"

void HEParameterAuditPlugin::onParameterChanged(int nodeId, const std::string& name, const HEParameterService::Values& values){
    std::lock_guard lock(mutex_);
    dirtyNodes_.insert(nodeId);
    ++changeCount_;
    lastNodeId_ = nodeId;
    lastParameter_ = name;
}

uint64_t HEParameterAuditPlugin::changeCount() const{
    std::lock_guard lock(mutex_);
    return changeCount_;
}

std::size_t HEParameterAuditPlugin::dirtyNodeCount() const{
    std::lock_guard lock(mutex_);
    return dirtyNodes_.size();
}

bool HEParameterAuditPlugin::isDirty(int nodeId) const{
    std::lock_guard lock(mutex_);
    return dirtyNodes_.find(nodeId) != dirtyNodes_.end();
}

std::string HEParameterAuditPlugin::lastChangedParameter() const{
    std::lock_guard lock(mutex_);
    return std::to_string(lastNodeId_) + ":" + lastParameter_;
}

void HEParameterAuditPlugin::clear(){
    std::lock_guard lock(mutex_);
    dirtyNodes_.clear();
    changeCount_ = 0;
    lastNodeId_ = -1;
    lastParameter_.clear();
}
