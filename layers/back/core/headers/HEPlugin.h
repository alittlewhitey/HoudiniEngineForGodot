#ifndef HE_PLUGIN_H
#define HE_PLUGIN_H

#include <string>
#include <vector>

#include "HEParameterService.h"
#include "HETypes.h"

class HECore;

// Pure C++ plugin interface.
//
// Plugins never see godot-cpp, HEG* classes, Variant, or Godot signals.
// They receive normalized lifecycle events and may query HECore state.
class HEPlugin {
public:
    virtual ~HEPlugin() = default;

    virtual const char* name() const = 0;

    virtual std::vector<std::string> dependencies() const{
        return {};
    }

    virtual void initialize(HECore& core){
    }

    virtual void shutdown(){
    }

    virtual void onSessionStarted(HESessionType type){
    }

    virtual void onSessionStopped(){
    }

    virtual void onAssetLoaded(const HEAssetRecord& asset){
    }

    virtual void onAssetUnloaded(int assetId){
    }

    virtual void onAssetReloaded(int oldAssetId, int newAssetId, const std::string& path){
    }

    virtual void onInstanceCreated(int rootNodeId, int assetId){
    }

    virtual void onInstanceDeleted(int rootNodeId){
    }

    virtual void onNodeCreated(int nodeId){
    }

    virtual void onNodeDeleted(int nodeId){
    }

    virtual void onParameterChanged(int nodeId, const std::string& name, const HEParameterService::Values& values){
    }

    virtual void onCookStarted(int nodeId){
    }

    virtual void onCookFinished(int nodeId, bool success){
    }
};

#endif // HE_PLUGIN_H
