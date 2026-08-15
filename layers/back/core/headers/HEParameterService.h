#ifndef HE_PARAMETER_SERVICE_H
#define HE_PARAMETER_SERVICE_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

// Host-independent parameter cache.
//
// Values use a neutral variant representation. HAPI-specific ParmInfo details
// still live in the frontend for now and will be migrated into a neutral
// HEParameterInfo model next.
class HEParameterService {
public:
    using Value = std::variant<int64_t, double, std::string>;
    using Values = std::vector<Value>;
    using NodeParameters = std::unordered_map<std::string, Values>;

    void clear(){
        parameters.clear();
        parameterOrder.clear();
    }

    void clearNode(int nodeId){
        parameters.erase(nodeId);
        parameterOrder.erase(nodeId);
    }

    Values get(int nodeId, const std::string& name) const{
        auto nodeIt = parameters.find(nodeId);
        if(nodeIt == parameters.end())
            return {};

        auto parmIt = nodeIt->second.find(name);
        if(parmIt == nodeIt->second.end())
            return {};
        return parmIt->second;
    }

    void set(int nodeId, const std::string& name, Values values){
        parameters[nodeId][name] = std::move(values);
    }

    void remove(int nodeId, const std::string& name){
        auto nodeIt = parameters.find(nodeId);
        if(nodeIt == parameters.end())
            return;
        nodeIt->second.erase(name);
    }

    std::vector<std::string> names(int nodeId) const{
        auto it = parameterOrder.find(nodeId);
        if(it == parameterOrder.end())
            return {};
        return it->second;
    }

    void setNames(int nodeId, std::vector<std::string> names){
        parameterOrder[nodeId] = std::move(names);
    }

    // nodeId -> parameter name -> values
    std::unordered_map<int, NodeParameters> parameters;

    // nodeId -> ordered parameter names
    std::unordered_map<int, std::vector<std::string>> parameterOrder;
};

#endif // HE_PARAMETER_SERVICE_H
