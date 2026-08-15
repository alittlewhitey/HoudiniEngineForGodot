#include "HECore.h"

HECore::~HECore(){
    shutdownPlugins();
}

void HECore::registerPlugin(std::unique_ptr<HEPlugin> plugin){
    if(!plugin)
        return;

    const std::string pluginName = plugin->name();
    HEPlugin* raw = plugin.get();
    {
        std::lock_guard lock(pluginMutex_);
        for(auto& existing : plugins_){
            if(existing->name() == pluginName)
                return;
        }
        plugins_.push_back(std::move(plugin));
    }
    raw->initialize(*this);
}

HEPlugin* HECore::findPlugin(const std::string& name){
    std::lock_guard lock(pluginMutex_);
    for(auto& plugin : plugins_){
        if(plugin->name() == name)
            return plugin.get();
    }
    return nullptr;
}

void HECore::unregisterPlugin(const std::string& name){
    std::unique_ptr<HEPlugin> removed;
    {
        std::lock_guard lock(pluginMutex_);
        for(auto it = plugins_.begin(); it != plugins_.end(); ++it){
            if((*it)->name() == name){
                removed = std::move(*it);
                plugins_.erase(it);
                break;
            }
        }
    }

    if(removed)
        removed->shutdown();
}

void HECore::shutdownPlugins(){
    std::vector<std::unique_ptr<HEPlugin>> plugins;
    {
        std::lock_guard lock(pluginMutex_);
        plugins = std::move(plugins_);
        plugins_.clear();
    }

    for(auto& plugin : plugins)
        plugin->shutdown();
}

std::vector<HEPlugin*> HECore::pluginSnapshot() const{
    std::vector<HEPlugin*> snapshot;
    std::lock_guard lock(pluginMutex_);
    snapshot.reserve(plugins_.size());
    for(auto& plugin : plugins_)
        snapshot.push_back(plugin.get());
    return snapshot;
}

void HECore::notifySessionStarted(HESessionType type){
    sessionType_ = type;
    sessionActive_ = true;
    ++sessionEpoch_;

    for(HEPlugin* plugin : pluginSnapshot())
        plugin->onSessionStarted(type);
}

void HECore::notifySessionStopped(){
    sessionActive_ = false;
    sessionType_ = HESessionType::None;

    for(HEPlugin* plugin : pluginSnapshot())
        plugin->onSessionStopped();
}

void HECore::notifyAssetLoaded(const HEAssetRecord& asset){
    for(HEPlugin* plugin : pluginSnapshot())
        plugin->onAssetLoaded(asset);
}

void HECore::notifyAssetUnloaded(int assetId){
    for(HEPlugin* plugin : pluginSnapshot())
        plugin->onAssetUnloaded(assetId);
}

void HECore::notifyAssetReloaded(int oldAssetId, int newAssetId, const std::string& path){
    for(HEPlugin* plugin : pluginSnapshot())
        plugin->onAssetReloaded(oldAssetId,newAssetId,path);
}

void HECore::notifyInstanceCreated(int rootNodeId, int assetId){
    for(HEPlugin* plugin : pluginSnapshot())
        plugin->onInstanceCreated(rootNodeId,assetId);
}

void HECore::notifyInstanceDeleted(int rootNodeId){
    for(HEPlugin* plugin : pluginSnapshot())
        plugin->onInstanceDeleted(rootNodeId);
}

void HECore::notifyNodeCreated(int nodeId){
    for(HEPlugin* plugin : pluginSnapshot())
        plugin->onNodeCreated(nodeId);
}

void HECore::notifyNodeDeleted(int nodeId){
    for(HEPlugin* plugin : pluginSnapshot())
        plugin->onNodeDeleted(nodeId);
}

void HECore::notifyParameterChanged(int nodeId, const std::string& name, const HEParameterService::Values& values){
    for(HEPlugin* plugin : pluginSnapshot())
        plugin->onParameterChanged(nodeId,name,values);
}

void HECore::notifyCookStarted(int nodeId){
    for(HEPlugin* plugin : pluginSnapshot())
        plugin->onCookStarted(nodeId);
}

void HECore::notifyCookFinished(int nodeId, bool success){
    for(HEPlugin* plugin : pluginSnapshot())
        plugin->onCookFinished(nodeId,success);
}
