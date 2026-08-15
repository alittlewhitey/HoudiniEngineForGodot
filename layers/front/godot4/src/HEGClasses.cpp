#include "HEGClasses.h"
#include "HEGCenter.h"
#include "HEGHapi.h"
#include <exception>
#include <godot_cpp/classes/editor_interface.hpp>
#include <godot_cpp/classes/editor_file_system.hpp>

bool HEGNode::isValid() const{
    HEGCenter* core = HEGCenter::get_singleton();
    return core != nullptr
        && valid
        && sessionEpoch == core->getSessionEpoch();
}

bool HEGAsset::isValid() const{
    HEGCenter* core = HEGCenter::get_singleton();
    return core != nullptr
        && valid
        && sessionEpoch == core->getSessionEpoch();
}

godot::Ref<HEGSession> HEGSession::switchSession(HEGSessionType type){
    try{
        HEGCenter* core = HEGCenter::get_singleton();

        // Always clear local state before switching sessions. If the old
        // session is already invalid but wasn't cleaned up, starting a new
        // session would otherwise leave stale node/asset references behind.
        core->stopSession();

        if(type != HEGSessionType::None)
            core->startSession((::HEGSessionType)type);

        if(type == HEGSessionType::None)
            return {};
        return getSession();
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
godot::Ref<HEGSession> HEGSession::getSession(){
    try{
        // Always return the HEGSession object. It may be inactive, but the
        // caller needs the instance to call stop()/restart() after a session
        // has become invalid.
        return HEGCenter::get_singleton()->getHESession();
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
int HEGSession::getSessionType() const{
    return (int)type;
}
bool HEGSession::stop(){
    try{
        return HEGCenter::get_singleton()->stopSession();
    }catch(const std::exception& e){
        printError(e.what());
        return false;
    }
}
bool HEGSession::restart(){
    try{
        return HEGCenter::get_singleton()->restartSession();
    }catch(const std::exception& e){
        printError(e.what());
        return false;
    }
}
bool HEGSession::cookSession(){
    try{
        return HEGCenter::get_singleton()->cookSession();
    }catch(const std::exception& e){
        printError(e.what());
        return false;
    }
}
godot::Array HEGSession::getAssets(){
    try{
        HEGCenter* core = HEGCenter::get_singleton();
        godot::Array res;
        for(int assetId : core->getAssetIds()){
            godot::Ref<HEGAsset> asset = core->findAssetRef(assetId);
            if(asset.is_valid())
                res.push_back(asset);
        }
        return res;
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
godot::Array HEGSession::getNodes(){
    try{
        HEGCenter* core = HEGCenter::get_singleton();
        godot::Array res;
        for(int nodeId : core->getNodeIds()){
            godot::Ref<HEGNode> node = core->findNodeRef(nodeId);
            if(node.is_valid())
                res.push_back(node);
        }
        return res;
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
godot::Ref<HEGAsset> HEGSession::getAsset(int assetId){
    try{
        return HEGCenter::get_singleton()->findAssetRef(assetId);
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
godot::Ref<HEGNode> HEGSession::getNode(int nodeId){
    try{
        return HEGCenter::get_singleton()->findNodeRef(nodeId);
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
std::string HEGSession::getString(HAPI_StringHandle sh){
    try{
        return HEGCenter::get_singleton()->getString(sh);
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
godot::Ref<HEGAsset> HEGSession::loadHDA(godot::Ref<HEGHdaResource> hda){
    try{
        if(hda.is_null()){
            printWarning("loadHDA: HEGHdaResource is null.");
            return {};
        }

        HEGCenter* core = HEGCenter::get_singleton();
        int id = -1;
        if(!core->loadAsset(hda->path,id)){
            printWarning("loadHDA failed: ",hda->path.c_str());
            return {};
        }

        auto lib = core->findAssetRef(id);
        if(lib.is_null())
            return {};

        hda->assetId = id;
        lib->res = hda;
        return lib;
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
godot::Ref<HEGAsset> HEGSession::loadHDAExternal(godot::String hdaPath){
    try{
        HEGCenter* core = HEGCenter::get_singleton();
        int id = -1;
        if(!core->loadAsset(globalize_path(hdaPath),id)){
            printWarning("loadHDAExternal failed: ",hdaPath);
            return {};
        }

        return core->findAssetRef(id);
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
bool HEGSession::saveHIP(godot::String hipPath, bool lock){
    auto res = HEGCenter::get_singleton()->saveHIP(globalize_path(hipPath), lock);
    auto fs = godot::EditorInterface::get_singleton()->get_resource_filesystem();
    if(std::filesystem::exists(globalize_path(hipPath))){
        fs->update_file(hipPath);
        fs->reimport_files({hipPath});
    }
    return res;
}
godot::Array HEGSession::loadHIP(godot::Ref<HEGHipResource> hip, bool append, bool cook){
    std::vector<int> newIds = HEGCenter::get_singleton()->loadHIP(hip->path, append, cook);
    godot::Array res;
    for(auto a : newIds){
        res.push_back(HEGCenter::get_singleton()->findNodeRef(a));
    }
    return res;
}
godot::Array HEGSession::loadHIPExternal(godot::String hipPath, bool append, bool cook){
    std::vector<int> newIds = HEGCenter::get_singleton()->loadHIP(globalize_path(hipPath), append, cook);
    godot::Array res;
    for(auto a : newIds){
        res.push_back(HEGCenter::get_singleton()->findNodeRef(a));
    }
    return res;
}
godot::String HEGAsset::getPath(){
    try{
        if(!isValid())
            return {};
        return string_cast(HEGCenter::get_singleton()->getAssetPath(id));
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
godot::Dictionary HEGAsset::getInfo(){
    try{
        if(!isValid()){
            printWarning("HEGAsset is no longer valid. Load the HDA again.");
            return {};
        }
        return HEGHapi::AssetInfoToDict(HEGCenter::get_singleton()->getAssetInfo(id));
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
godot::Array HEGAsset::getInstances(){
    try{
        if(!isValid()){
            printWarning("HEGAsset is no longer valid. Load the HDA again.");
            return {};
        }
        godot::Array res;
        for(int rootId : HEGCenter::get_singleton()->getAssetInstances(id)){
            res.push_back(HEGCenter::get_singleton()->findNodeRef(rootId));
        }
        return res;
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
godot::Array HEGAsset::getOutputNodes(){
    try{
        if(!isValid()){
            printWarning("HEGAsset is no longer valid. Load the HDA again.");
            return {};
        }

        godot::Array result;
        std::function<void(godot::Ref<HEGNode>)> collect;
        collect = [&](godot::Ref<HEGNode> node){
            if(node.is_null())
                return;

            if(godot::Object::cast_to<HEGSopNode>(node.ptr())){
                godot::Node3D* output = godot::Object::cast_to<HEGSopNode>(node.ptr())->createOutputNode();
                if(output)
                    result.push_back(output);
            }

            for(int i = 0; i < node->getChildList().size(); ++i){
                godot::Variant child = node->getChildList()[i];
                godot::Ref<HEGNode> childNode = child;
                collect(childNode);
            }
        };

        for(int rootId : HEGCenter::get_singleton()->getAssetInstances(id))
            collect(HEGCenter::get_singleton()->findNodeRef(rootId));

        return result;
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
godot::Node3D* HEGAsset::createOutputNode(){
    try{
        if(!isValid()){
            printWarning("HEGAsset is no longer valid. Load the HDA again.");
            return nullptr;
        }

        godot::Node3D* root = memnew(godot::Node3D);
        root->set_name("HEGAsset_" + godot::String::num_int64(id));

        for(int rootId : HEGCenter::get_singleton()->getAssetInstances(id)){
            godot::Ref<HEGNode> instanceRoot = HEGCenter::get_singleton()->findNodeRef(rootId);
            std::function<void(godot::Ref<HEGNode>)> collect;
            collect = [&](godot::Ref<HEGNode> node){
                if(node.is_null())
                    return;

                HEGSopNode* sop = godot::Object::cast_to<HEGSopNode>(node.ptr());
                if(sop){
                    godot::Node3D* output = sop->createOutputNode();
                    if(output){
                        output->set_name(node->getName() + "_output");
                        root->add_child(output, false, godot::Node::INTERNAL_MODE_BACK);
                    }
                }

                godot::Array children = node->getChildList();
                for(int i = 0; i < children.size(); ++i){
                    godot::Ref<HEGNode> child = children[i];
                    collect(child);
                }
            };
            collect(instanceRoot);
        }

        return root;
    }catch(const std::exception& e){
        printError(e.what());
        return nullptr;
    }
}
void HEGAsset::setMaterial(godot::String materialPath, godot::Ref<godot::Material> material){
    try{
        if(!isValid()){
            printWarning("HEGAsset is no longer valid.");
            return;
        }
        HEGCenter::get_singleton()->setMaterialOverride(string_cast(materialPath), material);
    }catch(const std::exception& e){
        printError(e.what());
    }
}
godot::Ref<godot::Material> HEGAsset::getMaterial(godot::String materialPath){
    try{
        if(!isValid())
            return {};
        return HEGCenter::get_singleton()->getMaterialOverride(string_cast(materialPath));
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
godot::Ref<HEGNode> HEGAsset::instantiate(godot::String operatorName, godot::Ref<HEGNode> parentNode){
    try{
        if(!isValid()){
            printWarning("HEGAsset is no longer valid. Load the HDA again.");
            return {};
        }

        int parentId = -1;
        if(parentNode.is_valid()){
            if(!parentNode->isValid()){
                printWarning("Parent HEGNode is no longer valid.");
                return {};
            }
            parentId = parentNode->getId();
        }

        int rootId = HEGCenter::get_singleton()->instantiateAsset(id,string_cast(operatorName),parentId);
        return HEGCenter::get_singleton()->findNodeRef(rootId);
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
bool HEGAsset::cook(){
    try{
        if(!isValid()){
            printWarning("HEGAsset is no longer valid. Load the HDA again.");
            return false;
        }
        return HEGCenter::get_singleton()->cookAsset(id);
    }catch(const std::exception& e){
        printError(e.what());
        return false;
    }
}
bool HEGAsset::unload(){
    try{
        if(!isValid()){
            printWarning("HEGAsset is no longer valid.");
            return false;
        }
        return HEGCenter::get_singleton()->unloadAsset(id);
    }catch(const std::exception& e){
        printError(e.what());
        return false;
    }
}
bool HEGAsset::reload(){
    try{
        if(!isValid()){
            printWarning("HEGAsset is no longer valid. Load the HDA again.");
            return false;
        }
        return HEGCenter::get_singleton()->reloadAsset(id);
    }catch(const std::exception& e){
        printError(e.what());
        return false;
    }
}
godot::Ref<HEGNode> HEGNode::createNode(godot::String label, godot::String operatorName, godot::Ref<HEGNode> parentNode){
    try{
        int parentId = -1;
        if(parentNode.is_valid()){
            if(!parentNode->isValid()){
                printWarning("Parent HEGNode is no longer valid.");
                return {};
            }
            parentId = parentNode->getId();
        }
        int id = -1;
        if(!HEGCenter::get_singleton()->createNode(string_cast(label),string_cast(operatorName),id,parentId))
            return {};
        return HEGCenter::get_singleton()->findNodeRef(id);
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
bool HEGNode::connectNodeTo(godot::Ref<HEGNode> resNode, int resInputIdx, int outputIdx){
    try{
        if(!isValid()){
            printWarning("HEGNode is no longer valid.");
            return false;
        }
        if(resNode.is_null() || !resNode->isValid()){
            printWarning("Target HEGNode is no longer valid.");
            return false;
        }
        return HEGCenter::get_singleton()->connectNode(resNode->getId(), resInputIdx, id, outputIdx);
    }catch(const std::exception& e){
        printError(e.what());
        return false;
    }
}
bool HEGNode::disconnectNode(int inputIdx){
    try{
        if(!isValid()){
            printWarning("HEGNode is no longer valid.");
            return false;
        }
        return HEGCenter::get_singleton()->disconnectNode(id,inputIdx);
    }catch(const std::exception& e){
        printError(e.what());
        return false;
    }
}
godot::Ref<HEGNode> HEGNode::queryConnect(int inputIdx){
    try{
        if(!isValid()){
            printWarning("HEGNode is no longer valid.");
            return {};
        }
        int resId = HEGCenter::get_singleton()->queryConnectedNode(id,inputIdx);
        if(resId < 0)
            return {};
        HEGCenter* core = HEGCenter::get_singleton();
        return core->findOrRegisterNode(resId, core->getNodeAssetId(id));
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
void HEGNode::deleteNode(){
    try{
        if(!isValid()){
            printWarning("HEGNode is no longer valid.");
            return;
        }
        HEGCenter::get_singleton()->deleteNode(id);
    }catch(const std::exception& e){
        printError(e.what());
    }
}
godot::Ref<HEGSopNode> HEGSopNode::inputMeshNode(godot::String label, godot::Ref<godot::Mesh> mesh, godot::Ref<HEGNode> parentId){
    try{
        int id,pId;
        if(parentId.is_null()){
            pId = -1;
        }else{
            pId = parentId->getId();
        }
        HEGCenter::get_singleton()->createInputMeshNode(string_cast(label),id,pId,mesh);
        return HEGCenter::get_singleton()->findNodeRef(id);
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
godot::Ref<HEGSopNode> HEGSopNode::inputCurveNode(godot::String label, godot::Ref<godot::Curve3D> curve, godot::Ref<HEGNode> parentId){
    try{
        int id,pId;
        if(parentId.is_null()){
            pId = -1;
        }else{
            pId = parentId->getId();
        }
        HEGCenter::get_singleton()->createInputCurveNode(string_cast(label),id,pId,curve);
        return HEGCenter::get_singleton()->findNodeRef(id);
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
godot::String HEGNode::getName(){
    try{
        auto info = HEGCenter::get_singleton()->getNodeInfo(id);
        auto res = HEGCenter::get_singleton()->getString(info.nameSH);
        return string_cast(res);
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
void HEGNode::setName(godot::String name){
    try{
        HEGCenter::get_singleton()->renameNode(id, string_cast(name));
    }catch(const std::exception& e){
        printError(e.what());
    }
}
HAPI_NodeType HEGNode::getType(){
    try{
        auto info = HEGCenter::get_singleton()->getNodeInfo(id);
        return info.type;
    }catch(const std::exception& e){
        printError(e.what());
        return HAPI_NODETYPE_ANY;
    }
}
godot::Dictionary HEGNode::getNodeInfo(){
    try{
        if(!isValid())
            return {};
        return HEGHapi::NodeInfoToDict(HEGCenter::get_singleton()->getNodeInfo(id));
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
godot::Ref<HEGNode> HEGNode::getParent(){
    try{
        if(!isValid())
            return {};
        int parentId = HEGCenter::get_singleton()->getNodeParentId(id);
        return HEGCenter::get_singleton()->findNodeRef(parentId);
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
int HEGNode::getParentId(){
    try{
        if(!isValid())
            return -1;
        return HEGCenter::get_singleton()->getNodeParentId(id);
    }catch(const std::exception& e){
        printError(e.what());
        return -1;
    }
}
godot::String HEGNode::getPath(){
    try{
        if(!isValid())
            return {};
        return string_cast(HEGCenter::get_singleton()->getNodePathString(id));
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
int HEGNode::getInputCount(){
    try{
        if(!isValid())
            return 0;
        return HEGCenter::get_singleton()->getNodeInfo(id).inputCount;
    }catch(const std::exception& e){
        printError(e.what());
        return 0;
    }
}
int HEGNode::getOutputCount(){
    try{
        if(!isValid())
            return 0;
        return HEGCenter::get_singleton()->getNodeInfo(id).outputCount;
    }catch(const std::exception& e){
        printError(e.what());
        return 0;
    }
}
godot::String HEGNode::getInputName(int inputIndex){
    try{
        if(!isValid())
            return {};
        return string_cast(HEGCenter::get_singleton()->getNodeInputName(id,inputIndex));
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
godot::String HEGNode::getOutputName(int outputIndex){
    try{
        if(!isValid())
            return {};
        return string_cast(HEGCenter::get_singleton()->getNodeOutputName(id,outputIndex));
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
bool HEGNode::isNodeValid(){
    try{
        if(!isValid())
            return false;
        return HEGCenter::get_singleton()->isNodeValidLive(id);
    }catch(const std::exception& e){
        printError(e.what());
        return false;
    }
}
bool HEGNode::setDisplay(bool display){
    try{
        if(!isValid())
            return false;
        return HEGCenter::get_singleton()->setNodeDisplay(id,display);
    }catch(const std::exception& e){
        printError(e.what());
        return false;
    }
}
bool HEGNode::isTimeDependent(){
    try{
        if(!isValid())
            return false;
        return HEGCenter::get_singleton()->getNodeInfo(id).isTimeDependent;
    }catch(const std::exception& e){
        printError(e.what());
        return false;
    }
}
godot::Ref<HEGAsset> HEGNode::getAsset(){
    try{
        if(!isValid())
            return {};
        int assetId = HEGCenter::get_singleton()->getNodeAssetId(id);
        return HEGCenter::get_singleton()->findAssetRef(assetId);
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
bool HEGNode::cook(){
    try{
        if(!isValid()){
            printWarning("HEGNode is no longer valid.");
            return false;
        }
        int nodeId = id;
        return HEGCenter::get_singleton()->cookNode(nodeId,[nodeId](bool success){
            auto node = HEGCenter::get_singleton()->findNodeRef(nodeId);
            if(node.is_valid()){
                node->emit_signal("cookFinished",success);
            }
        });
    }catch(const std::exception& e){
        printError(e.what());
        return false;
    }
}
bool HEGNode::isCookFinished(){
    try{
        if(!isValid())
            return false;
        return HEGCenter::get_singleton()->getCookStatus(id) == 2;
    }catch(const std::exception& e){
        printError(e.what());
        return false;
    }
}
int HEGNode::getCookState(){
    try{
        if(!isValid())
            return COOK_STATE_NODE_INVALID;
        return HEGCenter::get_singleton()->getCookStatus(id);
    }catch(const std::exception& e){
        printError(e.what());
        return COOK_STATE_NODE_INVALID;
    }
}
godot::String HEGNode::getLastCookError(){
    try{
        if(!isValid())
            return {};
        return string_cast(HEGCenter::get_singleton()->getLastCookError(id));
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
godot::Variant HEGNode::getParameter(godot::String name){
    try{
        HEGCenter* core = HEGCenter::get_singleton();
        auto info = core->getParmInfo(id,string_cast(name));
        auto value = core->getParameter(id,string_cast(name));

        if(value.empty())
            return godot::Variant();

        auto isDouble = [](const auto& v){
            return std::holds_alternative<double>(v);
        };

        if(info.type == HAPI_PARMTYPE_TOGGLE){
            if(std::holds_alternative<int64_t>(value[0]))
                return (bool)std::get<int64_t>(value[0]);
        }

        if(info.type == HAPI_PARMTYPE_COLOR
           && value.size() >= 3
           && isDouble(value[0]) && isDouble(value[1]) && isDouble(value[2])){
            float alpha = (value.size() >= 4 && isDouble(value[3]))
                ? (float)std::get<double>(value[3])
                : 1.0f;
            return godot::Color(
                (float)std::get<double>(value[0]),
                (float)std::get<double>(value[1]),
                (float)std::get<double>(value[2]),
                alpha
            );
        }

        if(value.size() >= 2 && isDouble(value[0]) && isDouble(value[1])){
            switch(info.scriptType){
                case HAPI_PRM_SCRIPT_TYPE_VECTOR2:
                    return godot::Vector2(
                        (float)std::get<double>(value[0]),
                        (float)std::get<double>(value[1]));
                case HAPI_PRM_SCRIPT_TYPE_VECTOR3:
                    if(value.size() >= 3 && isDouble(value[2]))
                        return godot::Vector3(
                            (float)std::get<double>(value[0]),
                            (float)std::get<double>(value[1]),
                            (float)std::get<double>(value[2]));
                    break;
                case HAPI_PRM_SCRIPT_TYPE_VECTOR4:
                    if(value.size() >= 4 && isDouble(value[2]) && isDouble(value[3]))
                        return godot::Vector4(
                            (float)std::get<double>(value[0]),
                            (float)std::get<double>(value[1]),
                            (float)std::get<double>(value[2]),
                            (float)std::get<double>(value[3]));
                    break;
                default:
                    break;
            }
        }

        if(value.size() == 1){
            if(std::holds_alternative<int64_t>(value[0]))
                return std::get<int64_t>(value[0]);
            else if(std::holds_alternative<double>(value[0]))
                return std::get<double>(value[0]);
            else if(std::holds_alternative<std::string>(value[0]))
                return string_cast(std::get<std::string>(value[0]));
        }else{
            godot::Array arr;
            for(auto& a : value){
                if(std::holds_alternative<int64_t>(a)){
                    arr.push_back(std::get<int64_t>(a));
                }else if(std::holds_alternative<double>(a)){
                    arr.push_back(std::get<double>(a));
                }else if(std::holds_alternative<std::string>(a)){
                    arr.push_back(string_cast(std::get<std::string>(a)));
                }
            }
            return arr;
        }
        return godot::Variant();
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
void HEGNode::setParameter(godot::String name, godot::Variant value, bool autoCook){
    try{
        std::string parmName = string_cast(name);
        HEGCenter* core = HEGCenter::get_singleton();
        std::vector<std::variant<int64_t,double,std::string>> res;
        switch(value.get_type()){
            case godot::Variant::Type::INT:{
                res.push_back((int64_t)value);
            }break;
            case godot::Variant::Type::FLOAT:{
                res.push_back((double)value);
            }break;
            case godot::Variant::Type::STRING:{
                res.push_back(string_cast((godot::String)value));
            }break;
            case godot::Variant::Type::BOOL:{
                res.push_back((int64_t)(bool)value);
            }break;
            case godot::Variant::Type::COLOR:{
                godot::Color c = (godot::Color)value;
                int componentCount = 4;
                HAPI_ParmInfo info = core->getParmInfo(id,parmName);
                if(info.type == HAPI_PARMTYPE_COLOR){
                    int count = HoudiniApi::ParmInfo_GetFloatValueCount(&info);
                    if(count == 3)
                        componentCount = 3;
                    else if(count == 4)
                        componentCount = 4;
                    else if(count > 0)
                        componentCount = count;
                }
                res.push_back((double)c.r);
                res.push_back((double)c.g);
                res.push_back((double)c.b);
                if(componentCount >= 4)
                    res.push_back((double)c.a);
            }break;
            case godot::Variant::Type::VECTOR2:{
                godot::Vector2 v = (godot::Vector2)value;
                res.push_back((double)v.x);
                res.push_back((double)v.y);
            }break;
            case godot::Variant::Type::VECTOR3:{
                godot::Vector3 v = (godot::Vector3)value;
                res.push_back((double)v.x);
                res.push_back((double)v.y);
                res.push_back((double)v.z);
            }break;
            case godot::Variant::Type::VECTOR4:{
                godot::Vector4 v = (godot::Vector4)value;
                res.push_back((double)v.x);
                res.push_back((double)v.y);
                res.push_back((double)v.z);
                res.push_back((double)v.w);
            }break;
            case godot::Variant::Type::ARRAY:{
                godot::Array arr = (godot::Array)value;
                auto size = arr.size();
                res.reserve(size);
                for(int i = 0;i!=size;++i){
                    switch(arr[i].get_type()){
                        case godot::Variant::Type::INT:{
                            res.push_back((int64_t)arr[i]);
                        }break;
                        case godot::Variant::Type::FLOAT:{
                            res.push_back((double)arr[i]);
                        }break;
                        case godot::Variant::Type::STRING:{
                            res.push_back(string_cast((godot::String)arr[i]));
                        }break;
                        default:{
                            printWarning("Unsupported array element type for parameter: ", arr[i].get_type());
                            return;
                        }break;
                    }
                }
            }break;
            default:{
                printWarning("Unsupported type for parameter: ", value.get_type());
                return;
            }break;
        }
        core->setParameter(id,parmName,std::move(res),autoCook);
    }catch(const std::exception& e){
        printError(e.what());
    }
}
godot::PackedStringArray HEGNode::getParameterList(){
    try{
        auto list = HEGCenter::get_singleton()->getParameterList(id);
        auto list_size = list.size();
        godot::PackedStringArray arr;
        arr.resize(list_size);
        for(int i = 0,size = list_size;i!=size;++i){
            arr[i] = string_cast(std::move(list[i]));
        }
        return arr;
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
bool HEGNode::refreshParameters(){
    try{
        if(!isValid()){
            printWarning("HEGNode is no longer valid.");
            return false;
        }
        return HEGCenter::get_singleton()->refreshNodeParameters(id);
    }catch(const std::exception& e){
        printError(e.what());
        return false;
    }
}
godot::Dictionary HEGNode::getParameterInfo(godot::String name){
    try{
        auto info = HEGCenter::get_singleton()->getParmInfo(id,string_cast(name));
        godot::Dictionary d = HEGHapi::ParmInfoToDict(info);
        if(info.labelSH > 0)
            d["label"] = string_cast(HEGCenter::get_singleton()->getString(info.labelSH));
        if(info.helpSH > 0)
            d["help"] = string_cast(HEGCenter::get_singleton()->getString(info.helpSH));
        return d;
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
godot::Array HEGNode::getParameterInfos(){
    try{
        godot::Array res;
        auto list = HEGCenter::get_singleton()->getParameterList(id);
        for(auto& name : list){
            auto info = HEGCenter::get_singleton()->getParmInfo(id,name);
            godot::Dictionary d = HEGHapi::ParmInfoToDict(info);
            d["name"] = string_cast(name);
            if(info.labelSH > 0)
                d["label"] = string_cast(HEGCenter::get_singleton()->getString(info.labelSH));
            if(info.helpSH > 0)
                d["help"] = string_cast(HEGCenter::get_singleton()->getString(info.helpSH));
            res.push_back(d);
        }
        return res;
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
godot::Array HEGNode::getParameterChoices(godot::String name){
    try{
        godot::Array res;
        auto choices = HEGCenter::get_singleton()->getParmChoices(id,string_cast(name));
        for(auto& [label, value] : choices){
            godot::Dictionary d;
            d["label"] = string_cast(label);
            d["value"] = string_cast(value);
            res.push_back(d);
        }
        return res;
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
bool HEGNode::insertMultiparmInstance(godot::String name, int instancePosition){
    try{
        return HEGCenter::get_singleton()->insertMultiparmInstance(id,string_cast(name),instancePosition);
    }catch(const std::exception& e){
        printError(e.what());
        return false;
    }
}
bool HEGNode::removeMultiparmInstance(godot::String name, int instancePosition){
    try{
        return HEGCenter::get_singleton()->removeMultiparmInstance(id,string_cast(name),instancePosition);
    }catch(const std::exception& e){
        printError(e.what());
        return false;
    }
}
bool HEGNode::setParameterNodeValue(godot::String name, int nodeValue){
    try{
        return HEGCenter::get_singleton()->setParmNodeValue(id,string_cast(name),nodeValue);
    }catch(const std::exception& e){
        printError(e.what());
        return false;
    }
}
int HEGNode::getParameterNodeValue(godot::String name){
    try{
        return HEGCenter::get_singleton()->getParmNodeValue(id,string_cast(name));
    }catch(const std::exception& e){
        printError(e.what());
        return -1;
    }
}
bool HEGNode::revertParameter(godot::String name){
    try{
        return HEGCenter::get_singleton()->revertParm(id,string_cast(name));
    }catch(const std::exception& e){
        printError(e.what());
        return false;
    }
}
godot::Array HEGNode::getChildList(godot::PackedInt32Array types, godot::PackedInt32Array flags, bool recursive){
    try{
        int64_t type_size = types.size();
        std::vector<HAPI_NodeType> v_types(type_size);
        for(int64_t i = 0;i!=type_size;++i){
            v_types[i] = (HAPI_NodeType)types[i];
        }
        int64_t flag_size = flags.size();
        std::vector<HAPI_NodeFlags> v_flags(flag_size);
        for(int64_t i = 0;i!=flag_size;++i){
            v_flags[i] = (HAPI_NodeFlags)flags[i];
        }
        HEGCenter* core = HEGCenter::get_singleton();
        const int assetId = core->getNodeAssetId(id);
        std::vector<int> children = core->getChildNodes(id,std::move(v_types),std::move(v_flags), recursive);
        godot::Array res;
        res.resize(children.size());
        for(int i = 0,size = children.size();i!=size;++i){
            res[i] = core->findOrRegisterNode(children[i], assetId);
        }
        return res;
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
int HEGNode::getAssetId(){
    try{
        if(!isValid())
            return -1;
        return HEGCenter::get_singleton()->getNodeAssetId(id);
    }catch(const std::exception& e){
        printError(e.what());
        return -1;
    }
}
bool HEGNode::isAssetRoot(){
    try{
        if(!isValid())
            return false;
        return HEGCenter::get_singleton()->isAssetInstanceRoot(id);
    }catch(const std::exception& e){
        printError(e.what());
        return false;
    }
}
godot::Ref<HEGNode> HEGNode::duplicate(){
    try{
        if(!isValid()){
            printWarning("HEGNode is no longer valid.");
            return {};
        }
        return HEGCenter::get_singleton()->findNodeRef(HEGCenter::get_singleton()->duplicateAsset(id));
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
godot::Ref<HEGNode> HEGNode::rebuild(){
    try{
        if(!isValid()){
            printWarning("HEGNode is no longer valid.");
            return {};
        }
        return HEGCenter::get_singleton()->findNodeRef(HEGCenter::get_singleton()->rebuildAsset(id));
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
bool HEGNode::resetParameters(){
    try{
        if(!isValid()){
            printWarning("HEGNode is no longer valid.");
            return false;
        }
        return HEGCenter::get_singleton()->resetAssetParams(id);
    }catch(const std::exception& e){
        printError(e.what());
        return false;
    }
}
//TODO: When mesh wraped and used in main tree, threading write makes error.
godot::Node3D* HEGMesh::wrap(){
    auto node = HEGGeometry::wrap();
    godot::MeshInstance3D* mesh3d = memnew(godot::MeshInstance3D);
    mesh3d->set_mesh(HEGCenter::get_singleton()->getMeshRef(nodeId,partId));
    node->add_child(mesh3d,false,godot::Node::INTERNAL_MODE_BACK);
    return node;
}
godot::Ref<godot::Mesh> HEGMesh::bakeAsMesh(){
    try{
        godot::Ref<godot::Mesh> mesh = HEGCenter::get_singleton()->getMeshRef(nodeId,partId);
        if(mesh.is_null())
            return {};
        return mesh->duplicate(true);
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
bool HEGMesh::setMaterial(int surfaceIndex, godot::Ref<godot::Material> material){
    try{
        return HEGCenter::get_singleton()->setMeshSurfaceMaterial(nodeId,partId,surfaceIndex,material);
    }catch(const std::exception& e){
        printError(e.what());
        return false;
    }
}
godot::Ref<godot::Material> HEGMesh::getMaterial(int surfaceIndex){
    try{
        return HEGCenter::get_singleton()->getMeshSurfaceMaterial(nodeId,partId,surfaceIndex);
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}

godot::Node3D* HEGMultiMesh::wrap(){
    auto node = HEGGeometry::wrap();
    for(auto ref : HEGCenter::get_singleton()->getMultiMeshRef(nodeId,partId)){
        godot::MultiMeshInstance3D* mesh3d = memnew(godot::MultiMeshInstance3D);
        mesh3d->set_multimesh(ref.second);
        node->add_child(mesh3d,false,godot::Node::INTERNAL_MODE_BACK);
    }
    return node;
}
godot::Dictionary HEGMultiMesh::bakeAsMultiMesh(){
    try{
        godot::Dictionary res;
        auto arr = HEGCenter::get_singleton()->getMultiMeshRef(nodeId,partId);
        for(auto [id, ref]: arr){
            res[id] = (godot::Ref<godot::MultiMesh>)ref->duplicate(true);
        }
        return res;
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
godot::Node3D* HEGCurve::wrap(){
    auto node = HEGGeometry::wrap();
    for(auto curve : HEGCenter::get_singleton()->getCurveRef(nodeId,partId)){
        godot::Path3D* path3d = memnew(godot::Path3D);
        path3d->set_curve(curve);
        node->add_child(path3d,false,godot::Node::INTERNAL_MODE_BACK);
    }
    return node;
}
godot::Array HEGCurve::bakeAsCurve(){
    try{
        godot::Array res;
        auto arr = HEGCenter::get_singleton()->getCurveRef(nodeId,partId);
        for(auto ref: arr){
            res.push_back(ref);
        }
        return res;
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
int HEGSopNode::getGeometryCount(){
    try{
        return HEGCenter::get_singleton()->getPartCount(id);
    }catch(const std::exception& e){
        printError(e.what());
        return 0;
    }
}
godot::Ref<HEGGeometry> HEGSopNode::getGeometry(int partId){
    try{
        if(partId >= getGeometryCount()){
            printError("Error: Geometry id out of range");
            return {};
        }
        auto* core = HEGCenter::get_singleton();
        switch(core->getPartType(id,partId)){
            case HEPartType::Mesh:{
                return HEGMesh::make_mesh(id,partId);
            }break;
            case HEPartType::Instancer:{
                return HEGMultiMesh::make_multimesh(id,partId);
            }break;
            case HEPartType::Curve:{
                return HEGCurve::make_curve(id,partId);
            }break;
            default:{
                printWarning("Unsupported part type: ", (int)core->getPartType(id,partId));
            }break;
        }
        return {};
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
godot::Array HEGSopNode::getGeometries(){
    try{
        godot::Array res;
        const int count = getGeometryCount();
        for(int partId = 0; partId != count; ++partId){
            godot::Ref<HEGGeometry> geometry = getGeometry(partId);
            if(geometry.is_valid())
                res.push_back(geometry);
        }
        return res;
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
godot::Node3D* HEGSopNode::createOutputNode(){
    try{
        if(!isValid()){
            printWarning("HEGSopNode is no longer valid.");
            return nullptr;
        }

        godot::Node3D* root = memnew(godot::Node3D);
        root->set_name(getName() + "_output");

        const int count = getGeometryCount();
        for(int partId = 0; partId != count; ++partId){
            godot::Ref<HEGGeometry> geometry = getGeometry(partId);
            if(geometry.is_null())
                continue;

            godot::Node3D* partNode = geometry->wrap();
            if(partNode){
                partNode->set_name(godot::String::num_int64(partId));
                root->add_child(partNode, false, godot::Node::INTERNAL_MODE_BACK);
            }
        }

        return root;
    }catch(const std::exception& e){
        printError(e.what());
        return nullptr;
    }
}
godot::Ref<HEGImage> HEGCopNode::getPNGImage(){
    try{
        return HEGImage::make_image(id);
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
godot::Ref<godot::Image> HEGImage::bakeAsImage(){
    try{
        return HEGCenter::get_singleton()->getImageRef(nodeId)->duplicate(true);
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
