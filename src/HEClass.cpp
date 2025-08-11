#include "HEClass.h"
#include "HECenter.h"
#include <exception>
#include <godot_cpp/classes/editor_interface.hpp>
#include <godot_cpp/classes/editor_file_system.hpp>

godot::Ref<HESession> HESession::switchSession(SessionType type){
    try{
        auto oldSession = HECenter::get_singleton()->getHESession();
        if(oldSession.is_valid()&&oldSession->valid())
            HECenter::get_singleton()->stopSession();
        if(type != SessionType::None)
            HECenter::get_singleton()->startSession((::SessionType)type);
        return getSession();
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
godot::Ref<HESession> HESession::getSession(){
    try{
        auto res = HECenter::get_singleton()->getHESession();
        if(!res->valid()){
            printWarning("Session isn't avaliable!");
            return {};
        }
        return res;
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
std::string HESession::getString(HAPI_StringHandle sh){
    try{
        return HECenter::get_singleton()->getString(sh);
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
godot::Ref<HEAsset> HESession::loadHDA(godot::Ref<HDAResource> hda){
    try{
        int id;
        HECenter::get_singleton()->loadAsset(hda->path,id);
        auto lib = HECenter::get_singleton()->findAssetRef(id);
        if(lib.is_null()) return {};
        hda->assetId = id;
        lib->res = hda;
        return lib;
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
godot::Ref<HEAsset> HESession::loadHDAExternal(godot::String hdaPath){
    try{
        int id;
        HECenter::get_singleton()->loadAsset(string_cast(hdaPath),id);
        auto lib = HECenter::get_singleton()->findAssetRef(id);
        return lib;
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
bool HESession::saveHIP(godot::String hipPath, bool lock){
    auto res = HECenter::get_singleton()->saveHIP(globalize_path(hipPath), lock);
    auto fs = godot::EditorInterface::get_singleton()->get_resource_filesystem();
    if(std::filesystem::exists(globalize_path(hipPath))){
        fs->update_file(hipPath);
        fs->reimport_files({hipPath});
    }
    return res;
}
bool HESession::loadHIP(godot::Ref<HIPResource> hip, bool append, bool cook){
    return HECenter::get_singleton()->loadHIP(hip->path, append, cook);
}
bool HESession::loadHIPExternal(godot::String hipPath, bool append, bool cook){
    return HECenter::get_singleton()->loadHIP(globalize_path(hipPath), append, cook);
}
godot::Ref<HENode> HENode::createNode(godot::String label, godot::String operatorName, godot::Ref<HENode> parentNode){
    try{
        int parentId = -1;
        if(parentNode.is_valid())
            parentId = parentNode->getId();
        int id;
        HECenter::get_singleton()->createNode(string_cast(label),string_cast(operatorName),id,parentId);
        auto node = HECenter::get_singleton()->findNodeRef(id);
        return node;
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
bool HENode::connectNodeTo(godot::Ref<HENode> resNode, int resInputIdx, int outputIdx){
    try{
        return HECenter::get_singleton()->connectNode(resNode->getId(), resInputIdx, id, outputIdx);
    }catch(const std::exception& e){
        printError(e.what());
        return false;
    }
}
bool HENode::disconnectNode(int inputIdx){
    try{
        return HECenter::get_singleton()->disconnectNode(id,inputIdx);
    }catch(const std::exception& e){
        printError(e.what());
        return false;
    }
}
godot::Ref<HENode> HENode::queryConnect(int inputIdx){
    try{
        int resId = HECenter::get_singleton()->queryConnectedNode(id,inputIdx);
        return HECenter::get_singleton()->findNodeRef(resId);
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
void HENode::deleteNode(){
    try{
        HECenter::get_singleton()->deleteNode(id);
    }catch(const std::exception& e){
        printError(e.what());
    }
}
godot::Ref<HESopNode> HESopNode::inputMeshNode(godot::String label, godot::Ref<godot::Mesh> mesh, godot::Ref<HENode> parentId){
    try{
        int id,pId;
        if(parentId.is_null()){
            pId = -1;
        }else{
            pId = parentId->getId();
        }
        HECenter::get_singleton()->createInputMeshNode(string_cast(label),id,pId,mesh);
        return HECenter::get_singleton()->findNodeRef(id);
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
godot::Ref<HESopNode> HESopNode::inputCurveNode(godot::String label, godot::Ref<godot::Curve3D> curve, godot::Ref<HENode> parentId){
    try{
        int id,pId;
        if(parentId.is_null()){
            pId = -1;
        }else{
            pId = parentId->getId();
        }
        HECenter::get_singleton()->createInputCurveNode(string_cast(label),id,pId,curve);
        return HECenter::get_singleton()->findNodeRef(id);
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
godot::String HENode::getName(){
    try{
        auto info = HECenter::get_singleton()->getNodeInfo(id);
        auto res = HECenter::get_singleton()->getString(info.nameSH);
        return string_cast(res);
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
void HENode::setName(godot::String name){
    try{
        HECenter::get_singleton()->renameNode(id, string_cast(name));
    }catch(const std::exception& e){
        printError(e.what());
    }
}
HAPI_NodeType HENode::getType(){
    try{
        auto info = HECenter::get_singleton()->getNodeInfo(id);
        return info.type;
    }catch(const std::exception& e){
        printError(e.what());
        return HAPI_NODETYPE_ANY;
    }
}
bool HENode::cook(){
    try{
        return HECenter::get_singleton()->cookNode(id,[this](bool success){
            emit_signal("cookFinished",success);
        });
    }catch(const std::exception& e){
        printError(e.what());
        return false;
    }
}
bool HENode::isCookFinished(){
    try{
        return HECenter::get_singleton()->getCookStatus(id);
    }catch(const std::exception& e){
        printError(e.what());
        return false;
    }
}
godot::Variant HENode::getParameter(godot::String name){
    try{
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
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
void HENode::setParameter(godot::String name, godot::Variant value){
    try{
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
    }catch(const std::exception& e){
        printError(e.what());
    }
}
godot::PackedStringArray HENode::getParameterList(){
    try{
        auto list = HECenter::get_singleton()->getParameterList(id);
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
godot::Array HENode::getChildList(godot::PackedInt32Array types, godot::PackedInt32Array flags, bool recursive){
    try{
        int64_t type_size = types.size();
        std::vector<HAPI_NodeType> v_types(type_size);
        for(int64_t i = 0;i!=type_size;++i){
            v_types[i] = (HAPI_NodeType)types[i];
        }
        int64_t flag_size = flags.size();
        std::vector<HAPI_NodeFlags> v_flags(type_size);
        for(int64_t i = 0;i!=flag_size;++i){
            v_flags[i] = (HAPI_NodeFlags)flags[i];
        }
        std::vector<int> children = HECenter::get_singleton()->getChildNodes(id,std::move(v_types),std::move(v_flags), recursive);
        godot::Array res;
        res.resize(children.size());
        for(int i = 0,size = children.size();i!=size;++i){
            res[i] = HECenter::get_singleton()->findNodeRef(children[i]);
        }
        return res;
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
godot::Ref<godot::Mesh> HEMesh::bakeAsMesh(){
    try{
        return HECenter::get_singleton()->getMeshRef(nodeId,partId)->duplicate(true);
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
godot::Dictionary HEMultiMesh::bakeAsMultiMesh(){
    try{
        godot::Dictionary res;
        auto arr = HECenter::get_singleton()->getMultiMeshRef(nodeId,partId);
        for(auto [id, ref]: arr){
            res[id] = (godot::Ref<godot::MultiMesh>)ref->duplicate(true);
        }
        return res;
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
godot::Array HECurve::bakeAsCurve(){
    try{
        godot::Array res;
        auto arr = HECenter::get_singleton()->getCurveRef(nodeId,partId);
        for(auto ref: arr){
            res.push_back(ref);
        }
        return res;
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
int HESopNode::getGeometryCount(){
    try{
        return HECenter::get_singleton()->getPartCount(id);
    }catch(const std::exception& e){
        printError(e.what());
        return 0;
    }
}
godot::Ref<HEGeometry> HESopNode::getGeometry(int partId){
    try{
        if(partId >= getGeometryCount()){
            printError("Error: Geometry id out of range");
            return {};
        }
        auto* core = HECenter::get_singleton();
        switch(core->getPartType(id,partId)){
            case PartType::Mesh:{
                return HEMesh::make_mesh(id,partId);
            }break;
            case PartType::Instancer:{
                return HEMultiMesh::make_multimesh(id,partId);
            }break;
            case PartType::Curve:{
                return HECurve::make_curve(id,partId);
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
godot::Ref<HEImage> HECopNode::getPNGImage(){
    try{
        return HEImage::make_image(id);
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
godot::Ref<godot::Image> HEImage::bakeAsImage(){
    try{
        return HECenter::get_singleton()->getImageRef(nodeId)->duplicate(true);
    }catch(const std::exception& e){
        printError(e.what());
        return {};
    }
}
