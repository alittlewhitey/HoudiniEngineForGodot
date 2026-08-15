#ifndef HE_ASSET_SERVICE_H
#define HE_ASSET_SERVICE_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

// Host-independent asset identity registry.
//
// The frontend still owns the HEGAsset object references and HAPI asset
// metadata, while this service owns the pure C++ relations between asset ids,
// node ids, instance roots, and file paths.
class HEAssetService {
public:
    void clear(){
        assetNodes.clear();
        nodeAsset.clear();
        assetPaths.clear();
        assetInstances.clear();
    }

    // assetId -> registered node ids
    std::unordered_map<int, std::unordered_set<int>> assetNodes;

    // nodeId -> assetId
    std::unordered_map<int, int> nodeAsset;

    // assetId -> file path
    std::unordered_map<int, std::string> assetPaths;

    // instance root nodeId -> {assetId, operatorName}
    std::unordered_map<int, std::pair<int, std::string>> assetInstances;

    void unregisterNode(int nodeId){
        auto it = nodeAsset.find(nodeId);
        if(it == nodeAsset.end()){
            assetInstances.erase(nodeId);
            return;
        }

        const int assetId = it->second;
        auto nodesIt = assetNodes.find(assetId);
        if(nodesIt != assetNodes.end()){
            nodesIt->second.erase(nodeId);
            if(nodesIt->second.empty())
                assetNodes.erase(nodesIt);
        }
        nodeAsset.erase(it);
        assetInstances.erase(nodeId);
    }
};

#endif // HE_ASSET_SERVICE_H
