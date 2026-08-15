#include "HEAssetLifecyclePlugin.h"

void HEAssetLifecyclePlugin::onAssetLoaded(const HEAssetRecord& asset){
    std::lock_guard lock(mutex_);
    loadedAssetIds_.insert(asset.handle.id);
    ++loadedCount_;
}

void HEAssetLifecyclePlugin::onAssetUnloaded(int assetId){
    std::lock_guard lock(mutex_);
    loadedAssetIds_.erase(assetId);
    ++unloadedCount_;
}

void HEAssetLifecyclePlugin::onAssetReloaded(int oldAssetId, int newAssetId, const std::string& path){
    std::lock_guard lock(mutex_);
    loadedAssetIds_.erase(oldAssetId);
    loadedAssetIds_.insert(newAssetId);
    ++reloadedCount_;
}

uint64_t HEAssetLifecyclePlugin::loadedCount() const{
    std::lock_guard lock(mutex_);
    return loadedCount_;
}

uint64_t HEAssetLifecyclePlugin::unloadedCount() const{
    std::lock_guard lock(mutex_);
    return unloadedCount_;
}

uint64_t HEAssetLifecyclePlugin::reloadedCount() const{
    std::lock_guard lock(mutex_);
    return reloadedCount_;
}

bool HEAssetLifecyclePlugin::isLoaded(int assetId) const{
    std::lock_guard lock(mutex_);
    return loadedAssetIds_.find(assetId) != loadedAssetIds_.end();
}
