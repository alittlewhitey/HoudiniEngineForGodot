#include "HEClass.h"
#include "HECenter.h"
godot::Ref<HESession> HESession::switchSession(SessionType type){
    auto oldSession = getSession();
    if(oldSession.is_valid()&&oldSession->valid()){
        HECenter::get_singleton()->stopSession();
    }
    HECenter::get_singleton()->startSession((::SessionType)type);
    return getSession();
}
godot::Ref<HESession> HESession::getSession(){
    return HECenter::get_singleton()->getHESession();
}
std::string HESession::getString(HAPI_StringHandle sh){
    return HECenter::get_singleton()->getString(sh);
}
godot::Ref<HEAsset> HESession::loadHDA(godot::Ref<HDAResource> hda){
    int id;
    HECenter::get_singleton()->loadAsset(hda->path,id);
    auto lib = HECenter::get_singleton()->findAssetRef(id);
    hda->assetId = id;
    lib->res = hda;
    return lib;
}
godot::Ref<HEAsset> HESession::loadHDAExternal(godot::String hdaPath){
    int id;
    HECenter::get_singleton()->loadAsset(string_cast(hdaPath),id);
    auto lib = HECenter::get_singleton()->findAssetRef(id);
    return lib;
}
godot::Ref<HENode> HESession::createNode(godot::String label, godot::String operatorName, godot::Ref<HENode> parentNode){
    int parentId = -1;
    if(parentNode.is_valid())
        parentId = parentNode->getId();
    int id;
    HECenter::get_singleton()->createNode(string_cast(label),string_cast(operatorName),id,parentId);
    auto node = HECenter::get_singleton()->findNodeRef(id);
    return node;
}
bool HESession::connectNode(godot::Ref<HENode> connectingNode, int inputIdx, godot::Ref<HENode> connectedNode, int connectedOutputIdx){
    return HECenter::get_singleton()->connectNode(connectingNode->getId(), inputIdx, connectedNode->getId(), connectedOutputIdx);
}
void HESession::deleteNode(godot::Ref<HENode> node){
    HECenter::get_singleton()->deleteNode(node->getId());
}
godot::Ref<HENode> HESession::inputMeshNode(godot::String nodeLabel, godot::Ref<godot::Mesh> mesh, godot::Ref<HENode> parentId){
    int id,pId;
    if(parentId.is_null()){
        pId = -1;
    }else{
        pId = parentId->getId();
    }
    HECenter::get_singleton()->createInputNode(string_cast(nodeLabel),id,pId,mesh);
    return HECenter::get_singleton()->findNodeRef(id);
}
HAPI_NodeType HENode::getType(){
    auto info = HECenter::get_singleton()->getNodeInfo(id);
    return info.type;
}
void HENode::cook(){
    HECenter::get_singleton()->cookNode(id);
}
bool HENode::isCookFinished(){
    return HECenter::get_singleton()->getCookStatus(id);
}
godot::Variant HENode::getParameter(godot::String name){
    auto value = HECenter::get_singleton()->getParameter(id,string_cast(name));
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
}
void HENode::setParameter(godot::String name, godot::Variant value){
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
        case godot::Variant::Type::ARRAY:{
            godot::Array&& arr = (godot::Array)value;
            auto size = arr.size();
            res.reserve(size);
            for(int i = 0;i!=size;++i){
                switch(arr[i].get_type()){
                    case godot::Variant::Type::INT:{
                        res.push_back((int64_t)value);
                    }break;
                    case godot::Variant::Type::FLOAT:{
                        res.push_back((double)value);
                    }break;
                    case godot::Variant::Type::STRING:{
                        res.push_back(string_cast((godot::String)value));
                    }break;
                    default:{
                        printWarning("Unsupported type: ", arr[i].get_type());
                    }break;
                }
            }
        }break;
        default:{
            printWarning("Unsupported type: ", value.get_type());
        }break;
    }
    HECenter::get_singleton()->setParameter(id,string_cast(name),res);
}
godot::PackedStringArray HENode::getParameterList(){
    auto list = HECenter::get_singleton()->getParameterList(id);
    auto list_size = list.size();
    godot::PackedStringArray arr;
    arr.resize(list_size);
    for(int i = 0,size = list_size;i!=size;++i){
        arr[i] = string_cast(std::move(list[i]));
    }
    return arr;
}
godot::Ref<godot::Mesh> HEMesh::bakeAsMesh(){
    return HECenter::get_singleton()->getMeshRef(nodeId,partId);
}
godot::Dictionary HEMultiMesh::bakeAsMultiMesh(){
    godot::Dictionary res;
    auto arr = HECenter::get_singleton()->getMultiMeshRef(nodeId,partId);
    for(auto [id, ref]: arr){
        res[id] = ref;
    }
    return res;
}
int HESopNode::getGeometryCount(){
    return HECenter::get_singleton()->getPartCount(id);
}
godot::Ref<HEGeometry> HESopNode::getGeometry(int partId){
    auto* core = HECenter::get_singleton();
    switch(core->getPartType(id,partId)){
        case PartType::Mesh:{
            return HEMesh::make_mesh(id,partId);
        }break;
        case PartType::Instancer:{
            return HEMultiMesh::make_multimesh(id,partId);
        }break;
        // case PartType::Curve:{

        // }break;
        default:{
            printWarning("Unsupported part type: ", (int)core->getPartType(id,partId));
        }break;
    }
    return {};
}
godot::Ref<HEImage> HECopNode::getPNGImage(){
    return HEImage::make_image(id);
}
godot::Ref<godot::Image> HEImage::bakeAsImage(){
    return HECenter::get_singleton()->getImageRef(nodeId);
}