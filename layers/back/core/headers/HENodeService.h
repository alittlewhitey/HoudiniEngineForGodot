#ifndef HE_NODE_SERVICE_H
#define HE_NODE_SERVICE_H

#include <atomic>
#include <string>
#include <unordered_map>
#include <unordered_set>

// Host-independent node identity and cook state registry.
class HENodeService {
public:
    void clear(){
        nodeIds.clear();
        cookStatus.clear();
        cookCounts.clear();
        cookErrors.clear();
    }

    bool contains(int nodeId) const{
        return nodeIds.find(nodeId) != nodeIds.end();
    }

    void clearNode(int nodeId){
        nodeIds.erase(nodeId);
        cookStatus.erase(nodeId);
        cookCounts.erase(nodeId);
        cookErrors.erase(nodeId);
    }

    void setCookError(int nodeId, const std::string& error){
        cookErrors[nodeId] = error;
    }

    void clearCookError(int nodeId){
        cookErrors.erase(nodeId);
    }

    std::string cookError(int nodeId) const{
        auto it = cookErrors.find(nodeId);
        return it == cookErrors.end() ? std::string() : it->second;
    }

    std::unordered_set<int> nodeIds;

    // nodeId -> cook state
    std::unordered_map<int, int> cookStatus;

    // nodeId -> cook generation count
    std::unordered_map<int, std::atomic<int>> cookCounts;

    // nodeId -> last cook error message
    std::unordered_map<int, std::string> cookErrors;
};

#endif // HE_NODE_SERVICE_H
