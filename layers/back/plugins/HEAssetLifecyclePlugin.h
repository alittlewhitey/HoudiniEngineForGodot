#ifndef HE_ASSET_LIFECYCLE_PLUGIN_H
#define HE_ASSET_LIFECYCLE_PLUGIN_H

#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_set>

#include "HEPlugin.h"

class HEAssetLifecyclePlugin : public HEPlugin {
public:
    const char* name() const override{
        return "asset_lifecycle";
    }

    void onAssetLoaded(const HEAssetRecord& asset) override;
    void onAssetUnloaded(int assetId) override;
    void onAssetReloaded(int oldAssetId, int newAssetId, const std::string& path) override;

    uint64_t loadedCount() const;
    uint64_t unloadedCount() const;
    uint64_t reloadedCount() const;
    bool isLoaded(int assetId) const;

private:
    mutable std::mutex mutex_;
    std::unordered_set<int> loadedAssetIds_;
    uint64_t loadedCount_ = 0;
    uint64_t unloadedCount_ = 0;
    uint64_t reloadedCount_ = 0;
};

#endif // HE_ASSET_LIFECYCLE_PLUGIN_H
