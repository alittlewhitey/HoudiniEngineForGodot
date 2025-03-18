#include "HoudiniBind.h"
HoudiniEngineManager* HoudiniEngineManager::singleton = nullptr;
godot::Dictionary HDANode::get_nodeInfo(){
    if(nodeInfo.is_empty()){
        godot::Ref<NodeId> id;
        id.instantiate();
        *(id->id) = nodeId;
        if(HoudiniEngineManager::get_singleton() == nullptr)
            return {};
        nodeInfo = HoudiniEngineManager::get_singleton()->getNodeInfo(id);
    }
    return nodeInfo;
}