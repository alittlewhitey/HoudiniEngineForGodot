#ifndef HE_CORE_H
#define HE_CORE_H

#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "HEAssetService.h"
#include "HEGeometryService.h"
#include "HECookService.h"
#include "HENodeService.h"
#include "HEParameterService.h"
#include "HEUndoRedoService.h"
#include "HEPlugin.h"
#include "HETypes.h"

// Host-independent Houdini Engine core.
//
// HECore owns cross-cutting state that does not depend on godot-cpp:
// - cook task ownership
// - session epoch
// - plugin registry
// - lifecycle event fan-out
// - pure C++ registries shared by the host frontend
//
// The Godot frontend (HEGCenter) keeps the Godot-facing object/reference
// caches and calls HECore::notify* at the correct lifecycle points.
class HECore {
public:
    HECore() = default;
    ~HECore();

    HECore(const HECore&) = delete;
    HECore& operator=(const HECore&) = delete;

    HECookService& cookService(){
        return cookService_;
    }

    HEAssetService& assetService(){
        return assetService_;
    }

    HENodeService& nodeService(){
        return nodeService_;
    }

    HEParameterService& parameterService(){
        return parameterService_;
    }

    HEGeometryService& geometryService(){
        return geometryService_;
    }

    HEUndoRedoService& undoRedo(){
        return undoRedo_;
    }

    uint64_t sessionEpoch() const{
        return sessionEpoch_;
    }

    HESessionType sessionType() const{
        return sessionType_;
    }

    bool isSessionActive() const{
        return sessionActive_;
    }

    void clearRuntimeState(){
        nodeService_.clear();
        assetService_.clear();
        parameterService_.clear();
        geometryService_.clear();
        undoRedo_.clear();
    }

    void registerPlugin(std::unique_ptr<HEPlugin> plugin);
    void unregisterPlugin(const std::string& name);
    HEPlugin* findPlugin(const std::string& name);
    void shutdownPlugins();

    // Lifecycle notifications. The frontend is responsible for calling these
    // after the corresponding HAPI operation succeeded or state changed.
    void notifySessionStarted(HESessionType type);
    void notifySessionStopped();
    void notifyAssetLoaded(const HEAssetRecord& asset);
    void notifyAssetUnloaded(int assetId);
    void notifyAssetReloaded(int oldAssetId, int newAssetId, const std::string& path);
    void notifyInstanceCreated(int rootNodeId, int assetId);
    void notifyInstanceDeleted(int rootNodeId);
    void notifyNodeCreated(int nodeId);
    void notifyNodeDeleted(int nodeId);
    void notifyParameterChanged(int nodeId, const std::string& name, const HEParameterService::Values& values);
    void notifyCookStarted(int nodeId);
    void notifyCookFinished(int nodeId, bool success);

private:
    std::vector<HEPlugin*> pluginSnapshot() const;

    mutable std::mutex pluginMutex_;
    std::vector<std::unique_ptr<HEPlugin>> plugins_;

    HECookService cookService_;
    HENodeService nodeService_;
    HEAssetService assetService_;
    HEParameterService parameterService_;
    HEGeometryService geometryService_;
    HEUndoRedoService undoRedo_;
    uint64_t sessionEpoch_ = 0;
    HESessionType sessionType_ = HESessionType::None;
    bool sessionActive_ = false;
};

#endif // HE_CORE_H
