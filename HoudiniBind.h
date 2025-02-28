
#ifndef HOUDINI_BIND_GODOT
#define HOUDINI_BIND_GODOT

#include <HAPI/HAPI.h>
#include <cstdlib>
#include <string>
#include <fstream>
#include <chrono>
#include <memory>
#include <thread>
#include <map>
#include <execution>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/surface_tool.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/core/math.hpp>
#include "deps/HoudiniApi.h"
#include "deps/HoudiniEngineUtility.h"
#include "deps/HoudiniEnginePlatform.h"
#include "HDAImporter.h"
#include "Contact.h"

VARIANT_ENUM_CAST(HAPI_PackedPrimInstancingMode)
VARIANT_ENUM_CAST(HAPI_StorageType)
VARIANT_ENUM_CAST(HAPI_AttributeTypeInfo)
VARIANT_ENUM_CAST(HAPI_AttributeOwner)
VARIANT_ENUM_CAST(HAPI_NodeType)
VARIANT_ENUM_CAST(HAPI_GeoType)

enum SessionType
{
    InProcess = 1,
    NewNamedPipe = 2,
    NewTCPSocket = 3,
    ExistingNamedPipe = 4,
    ExistingTCPSocket = 5,
    ExistingSharedMemory = 6
};
VARIANT_ENUM_CAST(SessionType)
class Void{};
class NodeId: public godot::Object{
    GDCLASS(NodeId, godot::Object)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("set_id","id"),&NodeId::set_id);
        godot::ClassDB::bind_method(godot::D_METHOD("get_id"),&NodeId::get_id);
        godot::ClassDB::add_property("NodeId",godot::PropertyInfo(godot::Variant::INT,"id"),"set_id","get_id");
    }
    void _init(int id){
        (*this->id) = id;
    }
    void set_id(int id){
        (*this->id) = id;
    }
    int get_id(){
        return *id;
    }
public:
    NodeId(int id){
        (*this->id) = id;
    }
    NodeId(){
        (*this->id) = 0;
    }
    operator int(){
        return *id;
    }
    std::shared_ptr<int> id = std::make_shared<int>(0);
};
class PartId: public godot::Object{
    GDCLASS(PartId, godot::Object)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("set_id","id"),&PartId::set_id);
        godot::ClassDB::bind_method(godot::D_METHOD("get_id"),&PartId::get_id);
        godot::ClassDB::add_property("PartId",godot::PropertyInfo(godot::Variant::INT,"id"),"set_id","get_id");
    }
    void set_id(int id){
        (*this->id) = id;
    }
    int get_id(){
        return *id;
    }
    void _init(int id){
        (*this->id) = id;
    }
public:
    PartId(int id){
        (*this->id) = id;
    }
    PartId(){
        (*this->id) = 0;
    }
    operator int(){
        return *id;
    }
    std::shared_ptr<int> id = std::make_shared<int>(0);
};
class AssetLibId: public godot::Object{
    GDCLASS(AssetLibId, godot::Object)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("set_id","id"),&AssetLibId::set_id);
        godot::ClassDB::bind_method(godot::D_METHOD("get_id"),&AssetLibId::get_id);
        godot::ClassDB::add_property("AssetLibId",godot::PropertyInfo(godot::Variant::INT,"id"),"set_id","get_id");
    }
    void set_id(int id){
        (*this->id) = id;
    }
    int get_id(){
        return *id;
    }
    void _init(int id){
        (*this->id) = id;
    }
public:
    AssetLibId(int id){
        (*this->id) = id;
    }
    AssetLibId(){
        (*this->id) = 0;
    }
    operator int(){
        return *id;
    }
    std::shared_ptr<int> id = std::make_shared<int>(0);
};
class HDANode: public godot::Resource{
    GDCLASS(HDANode,godot::Resource)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("get_nodeId"),&HDANode::get_nodeId);
        godot::ClassDB::bind_method(godot::D_METHOD("set_nodeId","id"),&HDANode::set_nodeId);
        godot::ClassDB::add_property("HDANode",godot::PropertyInfo(godot::Variant::INT,"nodeId"),"set_nodeId","get_nodeId");
    }
    int get_nodeId(){
        return nodeId;
    }
    void set_nodeId(int id){
        nodeId = id;
    }
    godot::Dictionary get_nodeInfo();
    void set_nodeInfo(godot::Dictionary){}
public:
    int nodeId;
    godot::Dictionary nodeInfo;
};
class Action: public godot::Resource{
    GDCLASS(Action,godot::Resource)
    static void _bind_methods(){}
public:
    virtual ~Action(){}
};
class SessionAction: public Action{
    GDCLASS(SessionAction,Action)
    static void _bind_methods(){}
public:
    virtual ~SessionAction(){}
};
class AssetAction: public Action{
    GDCLASS(AssetAction,Action)
    static void _bind_methods(){}
public:
    virtual ~AssetAction(){}
};
class NodeAction: public Action{
    GDCLASS(NodeAction,Action)
    static void _bind_methods(){}
public:
    virtual ~NodeAction(){}
};
class StartSessionAction: public SessionAction{
    GDCLASS(StartSessionAction,SessionAction)
    static void _bind_methods(){}
};
class RestartSessionAction: public SessionAction{
    GDCLASS(RestartSessionAction,SessionAction)
    static void _bind_methods(){}
};
class StopSessionAction: public SessionAction{
    GDCLASS(StopSessionAction,SessionAction)
    static void _bind_methods(){}
};
class CookSessionAction: public SessionAction{
    GDCLASS(CookSessionAction,SessionAction)
    static void _bind_methods(){}
};
class CookAssetAction: public AssetAction{
    GDCLASS(CookAssetAction,AssetAction)
    static void _bind_methods(){}
};
class LoadAssetAction: public AssetAction{
    GDCLASS(LoadAssetAction,AssetAction)
    static void _bind_methods(){}
};
class CookNodeAction: public NodeAction{
    GDCLASS(CookNodeAction,NodeAction)
    static void _bind_methods(){}
};
class DeleteNodeAction: public NodeAction{
    GDCLASS(DeleteNodeAction,NodeAction)
    static void _bind_methods(){}
};
class CreateMeshAction: public NodeAction{
    GDCLASS(CreateMeshAction,NodeAction)
    static void _bind_methods(){}
};

constexpr const char* DefaultNamedPipe = "hapi";
constexpr const char* DefaultSharedMemoryName = "hapi";
constexpr const char* DefaultHostName = "127.0.0.1";
constexpr int DefaultTcpPort = 9090;
class HoudiniEngineManager: public godot::Node{
    GDCLASS(HoudiniEngineManager,godot::Node)
public:
    static HoudiniEngineManager* get_singleton(){
        return singleton;
    }
private:
    static HoudiniEngineManager* singleton;
    static void _bind_methods(){
        BIND_ENUM_CONSTANT(HAPI_PACKEDPRIM_INSTANCING_MODE_INVALID)
        BIND_ENUM_CONSTANT(HAPI_PACKEDPRIM_INSTANCING_MODE_DISABLED)
        BIND_ENUM_CONSTANT(HAPI_PACKEDPRIM_INSTANCING_MODE_HIERARCHY)
        BIND_ENUM_CONSTANT(HAPI_PACKEDPRIM_INSTANCING_MODE_FLAT)
        BIND_ENUM_CONSTANT(HAPI_PACKEDPRIM_INSTANCING_MODE_MAX)


        BIND_ENUM_CONSTANT(InProcess)
        BIND_ENUM_CONSTANT(NewNamedPipe)
        BIND_ENUM_CONSTANT(NewTCPSocket)
        BIND_ENUM_CONSTANT(ExistingNamedPipe)
        BIND_ENUM_CONSTANT(ExistingTCPSocket)
        BIND_ENUM_CONSTANT(ExistingSharedMemory)

        BIND_ENUM_CONSTANT(HAPI_STORAGETYPE_INVALID)
        BIND_ENUM_CONSTANT(HAPI_STORAGETYPE_INT)
        BIND_ENUM_CONSTANT(HAPI_STORAGETYPE_INT64)
        BIND_ENUM_CONSTANT(HAPI_STORAGETYPE_FLOAT)
        BIND_ENUM_CONSTANT(HAPI_STORAGETYPE_FLOAT64)
        BIND_ENUM_CONSTANT(HAPI_STORAGETYPE_STRING)
        BIND_ENUM_CONSTANT(HAPI_STORAGETYPE_UINT8)
        BIND_ENUM_CONSTANT(HAPI_STORAGETYPE_INT8)
        BIND_ENUM_CONSTANT(HAPI_STORAGETYPE_INT16)
        BIND_ENUM_CONSTANT(HAPI_STORAGETYPE_DICTIONARY)
        BIND_ENUM_CONSTANT(HAPI_STORAGETYPE_INT_ARRAY)
        BIND_ENUM_CONSTANT(HAPI_STORAGETYPE_INT64_ARRAY)
        BIND_ENUM_CONSTANT(HAPI_STORAGETYPE_FLOAT_ARRAY)
        BIND_ENUM_CONSTANT(HAPI_STORAGETYPE_FLOAT64_ARRAY)
        BIND_ENUM_CONSTANT(HAPI_STORAGETYPE_STRING_ARRAY)
        BIND_ENUM_CONSTANT(HAPI_STORAGETYPE_UINT8_ARRAY)
        BIND_ENUM_CONSTANT(HAPI_STORAGETYPE_INT8_ARRAY)
        BIND_ENUM_CONSTANT(HAPI_STORAGETYPE_INT16_ARRAY)
        BIND_ENUM_CONSTANT(HAPI_STORAGETYPE_DICTIONARY_ARRAY)

        BIND_ENUM_CONSTANT(HAPI_ATTRIBUTE_TYPE_INVALID)
        BIND_ENUM_CONSTANT(HAPI_ATTRIBUTE_TYPE_NONE)
        BIND_ENUM_CONSTANT(HAPI_ATTRIBUTE_TYPE_POINT)
        BIND_ENUM_CONSTANT(HAPI_ATTRIBUTE_TYPE_HPOINT)
        BIND_ENUM_CONSTANT(HAPI_ATTRIBUTE_TYPE_VECTOR)
        BIND_ENUM_CONSTANT(HAPI_ATTRIBUTE_TYPE_NORMAL)
        BIND_ENUM_CONSTANT(HAPI_ATTRIBUTE_TYPE_COLOR)
        BIND_ENUM_CONSTANT(HAPI_ATTRIBUTE_TYPE_QUATERNION)
        BIND_ENUM_CONSTANT(HAPI_ATTRIBUTE_TYPE_MATRIX3)
        BIND_ENUM_CONSTANT(HAPI_ATTRIBUTE_TYPE_MATRIX)
        BIND_ENUM_CONSTANT(HAPI_ATTRIBUTE_TYPE_ST)
        BIND_ENUM_CONSTANT(HAPI_ATTRIBUTE_TYPE_HIDDEN)
        BIND_ENUM_CONSTANT(HAPI_ATTRIBUTE_TYPE_BOX2)
        BIND_ENUM_CONSTANT(HAPI_ATTRIBUTE_TYPE_BOX)
        BIND_ENUM_CONSTANT(HAPI_ATTRIBUTE_TYPE_TEXTURE)
        BIND_ENUM_CONSTANT(HAPI_ATTRIBUTE_TYPE_MAX)

        BIND_ENUM_CONSTANT(HAPI_ATTROWNER_INVALID)
        BIND_ENUM_CONSTANT(HAPI_ATTROWNER_VERTEX)
        BIND_ENUM_CONSTANT(HAPI_ATTROWNER_POINT)
        BIND_ENUM_CONSTANT(HAPI_ATTROWNER_PRIM)
        BIND_ENUM_CONSTANT(HAPI_ATTROWNER_DETAIL)
        BIND_ENUM_CONSTANT(HAPI_ATTROWNER_MAX)

        BIND_ENUM_CONSTANT(HAPI_NODETYPE_ANY)
        BIND_ENUM_CONSTANT(HAPI_NODETYPE_NONE)
        BIND_ENUM_CONSTANT(HAPI_NODETYPE_OBJ)
        BIND_ENUM_CONSTANT(HAPI_NODETYPE_SOP)
        BIND_ENUM_CONSTANT(HAPI_NODETYPE_CHOP)
        BIND_ENUM_CONSTANT(HAPI_NODETYPE_ROP)
        BIND_ENUM_CONSTANT(HAPI_NODETYPE_SHOP)
        BIND_ENUM_CONSTANT(HAPI_NODETYPE_COP)
        BIND_ENUM_CONSTANT(HAPI_NODETYPE_VOP)
        BIND_ENUM_CONSTANT(HAPI_NODETYPE_DOP)
        BIND_ENUM_CONSTANT(HAPI_NODETYPE_TOP)

        BIND_ENUM_CONSTANT(HAPI_GEOTYPE_INVALID)
        BIND_ENUM_CONSTANT(HAPI_GEOTYPE_DEFAULT)
        BIND_ENUM_CONSTANT(HAPI_GEOTYPE_INTERMEDIATE)
        BIND_ENUM_CONSTANT(HAPI_GEOTYPE_INPUT)
        BIND_ENUM_CONSTANT(HAPI_GEOTYPE_CURVE)
        BIND_ENUM_CONSTANT(HAPI_GEOTYPE_MAX)
        


        godot::ClassDB::bind_method(godot::D_METHOD("startSession","type","use_cooking_thread"),&HoudiniEngineManager::startSession);
        godot::ClassDB::bind_method(godot::D_METHOD("restartSession","type","use_cooking_thread"),&HoudiniEngineManager::restartSession);
        godot::ClassDB::bind_method(godot::D_METHOD("stopSession"),&HoudiniEngineManager::stopSession);

        
    }
    template <typename ...T>
    void printFile(T... output){
        if(!logFilePath.empty()){
            logFile << std::chrono::system_clock::now() << " ### ";
            int arr[] = {((logFile << output),0)...};
            logFile << std::endl;
        }
    }
    template <typename ...T>
    void printLog(T... output){
        printFile(output...);
        godot::UtilityFunctions::push_warning(output...,'\n');
    }
    template <typename ...T>
    bool printLog(HAPI_Result result,HAPI_Result target,T... output){
        if(result != target)
            return false;
        printFile(output...);
        godot::UtilityFunctions::push_warning(output...,'\n');
        return true;
    }
    template <typename ...T>
    void printErr(T... output){
        printFile(output...);
        godot::UtilityFunctions::push_error(output...,'\n');
    }
    template <typename ...T>
    bool printErr(HAPI_Result result,HAPI_Result target,T... output){
        if(result != target)
            return false;
        printFile(output...);
        godot::UtilityFunctions::push_error(output...,'\n');
        return true;
    }


    //      NodeId          ParamName            ParamValues
    std::map<int,std::map<std::string,std::vector<std::variant<int64_t,double,std::string>>>> parameters;

    void _get_property_list(godot::List<godot::PropertyInfo>* list){
        list->clear();

        list->push_back(godot::PropertyInfo(godot::Variant::NIL,"Session Settings",godot::PROPERTY_HINT_NONE,"SessionSettings_",godot::PROPERTY_USAGE_GROUP));

        list->push_back(godot::PropertyInfo(godot::Variant::OBJECT,"SessionSettings_sessionAction",godot::PROPERTY_HINT_RESOURCE_TYPE,"SessionAction"));
        list->push_back(godot::PropertyInfo(godot::Variant::INT,"SessionSettings_sessionType",godot::PROPERTY_HINT_ENUM,"InProcess:1,NewNamedPipe:2,NewTCPSocket:3,ExistingNamedPipe:4,ExistingTCPSocket:5,ExistingSharedMemory:6"));
        list->push_back(godot::PropertyInfo(godot::Variant::BOOL,"SessionSettings_useCookingThread"));
        list->push_back(godot::PropertyInfo(godot::Variant::STRING,"SessionSettings_namedPipe"));
        list->push_back(godot::PropertyInfo(godot::Variant::STRING,"SessionSettings_hostName"));
        list->push_back(godot::PropertyInfo(godot::Variant::STRING,"SessionSettings_sharedMemoryName"));
        list->push_back(godot::PropertyInfo(godot::Variant::INT,"SessionSettings_tcpPort"));


        list->push_back(godot::PropertyInfo(godot::Variant::NIL,"Asset Settings",godot::PROPERTY_HINT_NONE,"AssetSettings_",godot::PROPERTY_USAGE_GROUP));
        
        list->push_back(godot::PropertyInfo(godot::Variant::OBJECT,"AssetSettings_assetAction",godot::PROPERTY_HINT_RESOURCE_TYPE,"AssetAction"));
        list->push_back(godot::PropertyInfo(godot::Variant::OBJECT,"AssetSettings_nowAsset",godot::PROPERTY_HINT_RESOURCE_TYPE,"HDAResource"));
        list->push_back(godot::PropertyInfo(godot::Variant::ARRAY,"AssetSettings_assets"));
        list->push_back(godot::PropertyInfo(godot::Variant::DICTIONARY,"cookOptions"));
        list->push_back(godot::PropertyInfo(godot::Variant::STRING,"logFilePath",godot::PROPERTY_HINT_SAVE_FILE));


        list->push_back(godot::PropertyInfo(godot::Variant::NIL,"Node Settings",godot::PROPERTY_HINT_NONE,"NodeSettings_",godot::PROPERTY_USAGE_GROUP));
        list->push_back(godot::PropertyInfo(godot::Variant::OBJECT,"NodeSettings_nodeAction",godot::PROPERTY_HINT_RESOURCE_TYPE,"NodeAction"));
        list->push_back(godot::PropertyInfo(godot::Variant::OBJECT,"NodeSettings_nowNode",godot::PROPERTY_HINT_RESOURCE_TYPE,"HDANode"));
        list->push_back(godot::PropertyInfo(godot::Variant::ARRAY,"NodeSettings_nodes"));
        

        

        if(HoudiniApi::IsSessionValid(&session)!=HAPI_RESULT_SUCCESS){
            return;
        }
        //ADD_GROUP("Parameters","Parameters_");
        list->push_back(godot::PropertyInfo(godot::Variant::NIL,"Parameters",godot::PROPERTY_HINT_NONE,"Parameters_",godot::PROPERTY_USAGE_GROUP));
        for(auto& a : nodeIds){
            int id = a.first;

            auto nodeInfo = getNodeInfo(a.first);
            std::string name = HoudiniEngineUtility::getString(&session,nodeInfo.nameSH);
            list->push_back(godot::PropertyInfo(godot::Variant::NIL,(name+' '+std::to_string(id)+" Parameters").c_str(),godot::PROPERTY_HINT_NONE,("Parameters_"+std::to_string(id)+"_Parameters_").c_str(),godot::PROPERTY_USAGE_SUBGROUP));
            //ADD_SUBGROUP((name+" Parameters").c_str(),("Parameters_"+std::to_string(id)+"_Parameters_").c_str());
            list->push_back(godot::PropertyInfo(godot::Variant::INT,("Parameters_"+std::to_string(id)+"_Parameters_id").c_str()));
            list->push_back(godot::PropertyInfo(godot::Variant::INT,("Parameters_"+std::to_string(id)+"_Parameters_assetId").c_str()));

            for(auto& b: parameters[id]){
                if(b.second.size() == 1){
                    if(std::holds_alternative<int64_t>(b.second[0])){
                        list->push_back(godot::PropertyInfo(godot::Variant::INT,("Parameters_"+std::to_string(id)+"_Parameters_"+b.first).c_str()));
                    }else if(std::holds_alternative<double>(b.second[0])){
                        list->push_back(godot::PropertyInfo(godot::Variant::FLOAT,("Parameters_"+std::to_string(id)+"_Parameters_"+b.first).c_str()));
                    }else if(std::holds_alternative<std::string>(b.second[0])){
                        list->push_back(godot::PropertyInfo(godot::Variant::STRING,("Parameters_"+std::to_string(id)+"_Parameters_"+b.first).c_str()));
                    }
                }else{
                    list->push_back(godot::PropertyInfo(godot::Variant::ARRAY,("Parameters_"+std::to_string(id)+"_Parameters_"+b.first).c_str()));
                }
            }
        }
    }
    bool _get(const godot::StringName& property, godot::Variant& ret){

        std::string propertyName = property.c_escape().utf8().get_data();

        if(propertyName == "SessionSettings_sessionAction"){
            ret = sessionAction;
            return true;
        }else if(propertyName == "SessionSettings_sessionType"){
            ret = sessionType;
            return true;
        }else if(propertyName == "SessionSettings_useCookingThread"){
            ret = useCookingThread;
            return true;
        }else if(propertyName == "SessionSettings_namedPipe"){
            ret = godot::String::utf8(namedPipe.c_str());
            return true;
        }else if(propertyName == "SessionSettings_hostName"){
            ret = godot::String::utf8(hostName.c_str());
            return true;
        }else if(propertyName == "SessionSettings_sharedMemoryName"){
            ret = godot::String::utf8(sharedMemoryName.c_str());
            return true;
        }else if(propertyName == "SessionSettings_tcpPort"){
            ret = tcpPort;
            return true;
        }else if(propertyName == "AssetSettings_assetAction"){
            ret = assetAction;
            return true;
        }else if(propertyName == "AssetSettings_nowAsset"){
            ret = nowAsset;
            return true;
        }else if(propertyName == "AssetSettings_assets"){
            ret = get_assets();
            return true;
        }else if(propertyName == "NodeSettings_nodeAction"){
            ret = nodeAction;
            return true;
        }else if(propertyName == "NodeSettings_nowNode"){
            ret = nowNode;
            return true;
        }else if(propertyName == "NodeSettings_nodes"){
            ret = get_nodes();
            return true;
        }else if(propertyName == "cookOptions"){
            ret = get_cookOptions();
            return true;
        }else if(propertyName == "logFilePath"){
            ret = godot::String::utf8(logFilePath.c_str());
            return true;
        }


        if(HoudiniApi::IsSessionValid(&session)!=HAPI_RESULT_SUCCESS){
            return false;
        }
        if(propertyName.find("Parameters_") == 0){
            int beg = strlen("Parameters_");
            int en = propertyName.find("_Parameters_",beg);
            int id = std::stoi(propertyName.substr(beg,en-beg));
            int beg2 = en+strlen("_Parameters_");
            std::string argName = propertyName.substr(beg2);
            if(argName == "id"){
                ret = id;
            }else if(argName == "assetId"){
                ret = nodeIds[id];
            }else{
                if(parameters.find(id)==parameters.end()||parameters[id].find(argName)==parameters[id].end()){
                    return false;
                }
                auto& res = parameters[id][argName];
                if(res.size() == 1){
                    if(std::holds_alternative<int64_t>(res[0])){
                        ret = std::get<int64_t>(res[0]);
                    }else if(std::holds_alternative<double>(res[0])){
                        ret = std::get<double>(res[0]);
                    }else if(std::holds_alternative<std::string>(res[0])){
                        ret = godot::String::utf8(std::get<std::string>(res[0]).c_str());
                    }
                }else{
                    godot::Array arr;
                    for(auto& a : res){
                        if(std::holds_alternative<int64_t>(res[0])){
                            arr.push_back(std::get<int64_t>(res[0]));
                        }else if(std::holds_alternative<double>(res[0])){
                            arr.push_back(std::get<double>(res[0]));
                        }else if(std::holds_alternative<std::string>(res[0])){
                            arr.push_back(godot::String::utf8(std::get<std::string>(res[0]).c_str()));
                        }
                    }
                    ret = arr;
                }
            }
        }
        return true;
    }
    bool _set(const godot::StringName& property, const godot::Variant& value){

        std::string propertyName = property.c_escape().utf8().get_data();

        if(propertyName == "SessionSettings_sessionAction"){
            set_sessionAction((godot::Ref<SessionAction>)(value));
            return true;
        }else if(propertyName == "SessionSettings_sessionType"){
            sessionType = (SessionType)(int)value;
            return true;
        }else if(propertyName == "SessionSettings_useCookingThread"){
            useCookingThread = (bool)value;
            return true;
        }else if(propertyName == "SessionSettings_namedPipe"){
            namedPipe = ((godot::String)value).utf8().get_data();
            return true;
        }else if(propertyName == "SessionSettings_hostName"){
            hostName = ((godot::String)value).utf8().get_data();
            return true;
        }else if(propertyName == "SessionSettings_sharedMemoryName"){
            sharedMemoryName = ((godot::String)value).utf8().get_data();
            return true;
        }else if(propertyName == "SessionSettings_tcpPort"){
            tcpPort = (int)value;
            return true;
        }else if(propertyName == "AssetSettings_assetAction"){
            set_assetAction((godot::Ref<AssetAction>)(value));
            return true;
        }else if(propertyName == "AssetSettings_nowAsset"){
            nowAsset = (godot::Ref<HDAResource>)value;
            return true;
        }else if(propertyName == "AssetSettings_assets"){
            
            return false;
        }else if(propertyName == "NodeSettings_nodeAction"){
            set_nodeAction((godot::Ref<NodeAction>)(value));
            return true;
        }else if(propertyName == "NodeSettings_nowNode"){
            nowNode = (godot::Ref<HDANode>)value;
            return true;
        }else if(propertyName == "NodeSettings_nodes"){

            return false;
        }else if(propertyName == "cookOptions"){
            set_cookOptions((godot::Dictionary)value);
            return true;
        }else if(propertyName == "logFilePath"){
            set_logFilePath((godot::String)value);
            return true;
        }




        if(HoudiniApi::IsSessionValid(&session)!=HAPI_RESULT_SUCCESS){
            return false;
        }
        if(propertyName.find("Parameters_") == 0){
            int beg = strlen("Parameters_");
            int en = propertyName.find("_Parameters_",beg);
            int id = std::stoi(propertyName.substr(beg,en-beg));
            int beg2 = en+strlen("_Parameters_");
            std::string argName = propertyName.substr(beg2);
            
            if(parameters.find(id)==parameters.end()||parameters[id].find(argName)==parameters[id].end()){
                return false;
            }
            auto& res = parameters[id][argName];
            if(res.size() == 1){
                if(std::holds_alternative<int64_t>(res[0])){
                    res[0] = (int64_t)value;
                    HoudiniApi::SetParmIntValue(&session,id,argName.c_str(),0,(int)value);
                }else if(std::holds_alternative<double>(res[0])){
                    res[0] = (double)value;
                    HoudiniApi::SetParmFloatValue(&session,id,argName.c_str(),0,(float)value);
                }else if(std::holds_alternative<std::string>(res[0])){
                    res[0] = (std::string)((godot::String)value).utf8().get_data();
                    HAPI_ParmId parmId;
                    HoudiniApi::GetParmIdFromName(&session,id,argName.c_str(),&parmId);
                    HoudiniApi::SetParmStringValue(&session,id,((godot::String)value).utf8().get_data(),parmId,0);
                }
            }else{
                res.clear();
                int size = ((godot::Array)value).size();
                for(int i = 0;i!=size;++i){
                    switch(((godot::Array)value)[i].get_type()){
                    case godot::Variant::INT:{
                        res.push_back((int64_t)((godot::Array)value)[i]);
                        HoudiniApi::SetParmIntValue(&session,id,argName.c_str(),i,(int)value);
                    }break;
                    case godot::Variant::FLOAT:{
                        res.push_back((double)((godot::Array)value)[i]);
                        HoudiniApi::SetParmFloatValue(&session,id,argName.c_str(),i,(float)value);
                    }break;
                    case godot::Variant::STRING:{
                        res.push_back(((godot::String)((godot::Array)value)[i]).utf8().get_data());
                        HAPI_ParmId parmId;
                        HoudiniApi::GetParmIdFromName(&session,id,argName.c_str(),&parmId);
                        HoudiniApi::SetParmStringValue(&session,id,((godot::String)value).utf8().get_data(),parmId,i);
                    }break;
                    }
                }
            }
            return true;
        }
        return false;
    }
    void _notification(int what){
        switch(what){
        case NOTIFICATION_ENTER_TREE:{
            init();
            set_process(1);
        }break;
        case NOTIFICATION_PROCESS:{
            Contact::process_call();
        }break;
        case NOTIFICATION_EXIT_TREE:{
            term();
        }break;
        }
    }
    void* libHAPIL = nullptr;
    void init(){
        singleton = this;
        if(putenv("HAPI_CLIENT_NAME=godot")){
            printLog("Failed to change env \"HAPI_CLIENT_NAME\" to \"godot\".\n");
        }
        libHAPIL = HoudiniEnginePlatform::LoadLibHAPIL();
        if(libHAPIL != nullptr){
            HoudiniApi::InitializeHAPI(libHAPIL);
        }
        if(!HoudiniApi::IsHAPIInitialized()){
            printErr("Failed to load and initialize the "
                        "Houdini Engine API from libHAPIL.\n");
        }
    }
    void term(){
        if(HoudiniApi::IsSessionValid(&session) == HAPI_RESULT_SUCCESS){
            if(!stopSession()){
                printErr("Failed to stop session.\n");
            }
            
        }
        // if(libHAPIL != nullptr){
        //     HoudiniApi::FinalizeHAPI();
        //     HoudiniEnginePlatform::FreeLibHAPIL(libHAPIL);
        // }
    }
    //      AssetId,Res
    std::map<int,godot::Ref<HDAResource>> assetIds;
    godot::Ref<HDAResource> nowAsset;
    godot::Array get_assets(){
        godot::Array arr;
        for(auto& a : assetIds){
            arr.push_back(a.second);
        }
        return arr;
    }
    //      NodeId,AssetId
    std::map<int,int> nodeIds;
    godot::Ref<HDANode> nowNode;
    godot::Array get_nodes(){
        godot::Array arr;
        for(auto& a : nodeIds){
            godot::Ref<HDANode> node;
            node.instantiate();
            node->nodeId = a.first;
            arr.push_back(node);
        }
        return arr;
    }

    void cookSession(){
        for(auto a : nodeIds){
            cookNode(a.first);
            getParameters(a.first);
        }
    }
    void cookAsset(){
        for(auto a : nodeIds){
            if(a.second == nowAsset->assetId){
                cookNode(a.first);
                getParameters(a.first);
            }
        }
    }
    godot::Ref<SessionAction> sessionAction;
    void set_sessionAction(godot::Ref<SessionAction> action){
        auto& type = typeid(*(action.ptr()));
        if(type == typeid(StartSessionAction)){
            this->sessionAction = action;
            std::jthread([this]{
                startSession(sessionType,useCookingThread);
                this->sessionAction.unref();
                Contact::add_call([this]{
                    notify_property_list_changed();
                });
            }).detach();
        }else if(type == typeid(RestartSessionAction)){
            this->sessionAction = action;
            std::jthread([this]{
                restartSession(sessionType,useCookingThread);
                this->sessionAction.unref();
                Contact::add_call([this]{
                    notify_property_list_changed();
                });
            }).detach();
        }else if(type == typeid(StopSessionAction)){
            this->sessionAction = action;
            std::jthread([this]{
                stopSession();
                this->sessionAction.unref();
                Contact::add_call([this]{
                    notify_property_list_changed();
                });
            }).detach();
        }else if(type == typeid(CookSessionAction)){
            this->sessionAction = action;
            std::jthread([this]{
                cookSession();
                this->sessionAction.unref();
                Contact::add_call([this]{
                    notify_property_list_changed();
                });
            }).detach();
        }else{
            return;
        }
    }
    godot::Ref<AssetAction> assetAction;
    void set_assetAction(godot::Ref<AssetAction> action){
        auto& type = typeid(*(action.ptr()));
        if(type == typeid(CookAssetAction)){
            this->assetAction = action;
            std::jthread([this]{
                cookAsset();
                this->assetAction.unref();
                Contact::add_call([this]{
                    notify_property_list_changed();
                });
            }).detach();
        }else if(type == typeid(LoadAssetAction)){
            this->assetAction = action;
            std::jthread([this]{
                if(nowAsset.is_null())
                    return;
                loadAssets(nowAsset,Void{});
                this->assetAction.unref();
                Contact::add_call([this]{
                    notify_property_list_changed();
                });
            }).detach();
        }else{
            return;
        }
    }
    godot::Ref<NodeAction> nodeAction;
    void set_nodeAction(godot::Ref<NodeAction> action){
        auto& type = typeid(*(action.ptr()));
        if(type == typeid(CookNodeAction)){
            this->nodeAction = action;
            std::jthread([this]{
                if(nowNode.is_null())
                    return;
                cookNode(nowNode->nodeId);
                getParameters(nowNode->nodeId);

                this->nodeAction.unref();
                Contact::add_call([this]{
                    notify_property_list_changed();
                });
            }).detach();
        }
        else if(type == typeid(DeleteNodeAction)){
            this->nodeAction = action;
            std::jthread([this]{
                if(nowNode.is_null())
                    return;
                deleteNode(nowNode->nodeId);
                this->nodeAction.unref();
                Contact::add_call([this]{
                    notify_property_list_changed();
                });
            }).detach();
        }
        else if(type == typeid(CreateMeshAction)){
            this->nodeAction = action;
            std::jthread([this]{
                if(nowNode.is_null())
                    return;
                createMeshNode(nowNode->nodeId);
                this->nodeAction.unref();
                Contact::add_call([this]{
                    notify_property_list_changed();
                });
            }).detach();
        }
    }

    std::string namedPipe = DefaultNamedPipe;

    std::string hostName = DefaultHostName;

    std::string sharedMemoryName = DefaultSharedMemoryName;

    int tcpPort = DefaultTcpPort;

    std::string logFilePath = "";
    std::ofstream logFile;
    void set_logFilePath(godot::String path){
        path = godot::ProjectSettings::get_singleton()->globalize_path(path);
        if(path.is_absolute_path()){
            if(!logFilePath.empty()){
                logFile.close();
                logFile.clear();
            }
            logFile.open(path.utf8().get_data(),std::ios::app|std::ios::out);
            logFilePath = path.utf8().get_data();
        }else if(path.is_empty()){
            logFile.close();
            logFile.clear();
            logFilePath.clear();
        }
    }

    HAPI_Session session;
    SessionType sessionType = InProcess;
    bool useCookingThread = true;


    HAPI_CookOptions cookOptions;
    void set_cookOptions(godot::Dictionary options){
        if(options.has("curveRefineLOD")){
            cookOptions.curveRefineLOD = (float)options["curveRefineLOD"];
        }
        if(options.has("clearErrorsAndWarnings")){
            cookOptions.clearErrorsAndWarnings = (bool)options["clearErrorsAndWarnings"];
        }
        if(options.has("maxVerticesPerPrimitive")){
            cookOptions.maxVerticesPerPrimitive = (int)options["maxVerticesPerPrimitive"];
        }
        if(options.has("splitGeosByGroup")){
            cookOptions.splitGeosByGroup = (bool)options["splitGeosByGroup"];
        }
        if(options.has("refineCurveToLinear")){
            cookOptions.refineCurveToLinear = (bool)options["refineCurveToLinear"];
        }
        if(options.has("handleBoxPartTypes")){
            cookOptions.handleBoxPartTypes = (bool)options["handleBoxPartTypes"];
        }
        if(options.has("handleSpherePartTypes")){
            cookOptions.handleSpherePartTypes = (bool)options["handleSpherePartTypes"];
        }
        if(options.has("splitPointsByVertexAttributes")){
            cookOptions.splitPointsByVertexAttributes = (bool)options["splitPointsByVertexAttributes"];
        }
        if(options.has("packedPrimInstancingMode")){
            cookOptions.packedPrimInstancingMode = (HAPI_PackedPrimInstancingMode)(int)options["packedPrimInstancingMode"];
        }
    }
    godot::Dictionary get_cookOptions(){
        godot::Dictionary dic;
        dic["curveRefineLOD"] = cookOptions.curveRefineLOD;
        dic["clearErrorsAndWarnings"] = cookOptions.clearErrorsAndWarnings;
        dic["maxVerticesPerPrimitive"] = cookOptions.maxVerticesPerPrimitive;
        dic["splitGeosByGroup"] = cookOptions.splitGeosByGroup;
        dic["refineCurveToLinear"] = cookOptions.refineCurveToLinear;
        dic["handleBoxPartTypes"] = cookOptions.handleBoxPartTypes;
        dic["handleSpherePartTypes"] = cookOptions.handleSpherePartTypes;
        dic["splitPointsByVertexAttributes"] = cookOptions.splitPointsByVertexAttributes;
        dic["packedPrimInstancingMode"] = (int)cookOptions.packedPrimInstancingMode;
        return dic;
    }
    //      nodeId        partId       Geo_Attrib    faces            P            vertexs             Cd                  N               uv
    std::map<int,std::map<int,std::tuple<std::vector<int>,std::vector<float>,std::vector<int>,std::vector<float>,std::vector<float>,std::vector<float>>>> geometries;
public:
    bool startSession(SessionType type,bool use_cooking_thread){
        if(HoudiniApi::IsSessionValid(&session) == HAPI_RESULT_SUCCESS){
            return true;
        }
        HoudiniApi::ClearConnectionError();
        HAPI_ThriftServerOptions server_options = HoudiniApi::ThriftServerOptions_Create();
        server_options.autoClose = true;
        server_options.timeoutMs = 3000.f;

        HAPI_Result SessionResult = HAPI_RESULT_FAILURE;
        sessionType = type;
        switch (type)
        {
        case SessionType::InProcess:{
            HAPI_SessionInfo sessionInfo = HoudiniApi::SessionInfo_Create();
            SessionResult = HoudiniApi::CreateInProcessSession(
                &session,&sessionInfo
            );
            if(SessionResult == HAPI_RESULT_SUCCESS){
                printFile("Successful create a HAPI in-process session\n");
            }else{
                printErr("Error create session: ",SessionResult);
            }
        }break;
        case SessionType::NewNamedPipe:{
            HAPI_ProcessId processID;
            HOUDINI_CHECK_ERROR(HoudiniApi::StartThriftNamedPipeServer(
                &server_options,namedPipe.c_str(),&processID,logFilePath.empty()?nullptr:logFilePath.c_str()
            ));
            HAPI_SessionInfo sessionInfo = HoudiniApi::SessionInfo_Create();
            SessionResult = HoudiniApi::CreateThriftNamedPipeSession(
                &session,namedPipe.c_str(),&sessionInfo
            );
            if(SessionResult == HAPI_RESULT_SUCCESS){
                printFile("Successful create a HAPI named-pipe session\n");
            }else{
                printErr("Error create session: ",SessionResult);
            }
        }break;
        case SessionType::NewTCPSocket:{
            HAPI_ProcessId processID;
            HOUDINI_CHECK_ERROR(HoudiniApi::StartThriftSocketServer(
                &server_options,tcpPort,&processID,logFilePath.empty()?nullptr:logFilePath.c_str()
            ));
            HAPI_SessionInfo sessionInfo = HoudiniApi::SessionInfo_Create();
            SessionResult = HoudiniApi::CreateThriftSocketSession(
                &session,hostName.c_str(), tcpPort, &sessionInfo
            );
            if(SessionResult == HAPI_RESULT_SUCCESS){
                printFile("Successful create a HAPI TCP socket session\n");
            }else{
                printErr("Error create session: ",SessionResult);
            }
        }break;
        case SessionType::ExistingNamedPipe:{
            HAPI_SessionInfo sessionInfo = HoudiniApi::SessionInfo_Create();
            SessionResult = HoudiniApi::CreateThriftNamedPipeSession(
                &session,namedPipe.c_str(),&sessionInfo
            );
            if(SessionResult == HAPI_RESULT_SUCCESS){
                printFile("Successful connect to an existint HAPI named-pipe session\n");
            }else{
                printErr("Error create session: ",SessionResult);
            }
        }break;
        case SessionType::ExistingTCPSocket:{
            HAPI_SessionInfo sessionInfo = HoudiniApi::SessionInfo_Create();
            SessionResult = HoudiniApi::CreateThriftSocketSession(
                &session,hostName.c_str(), tcpPort, &sessionInfo
            );
            if(SessionResult == HAPI_RESULT_SUCCESS){
                printFile("Successful connect to an existint HAPI TCP socket session\n");
            }else{
                printErr("Error create session: ",SessionResult);
            }
        }break;
        case SessionType::ExistingSharedMemory:{
            HAPI_SessionInfo sessionInfo = HoudiniApi::SessionInfo_Create();
            SessionResult = HoudiniApi::CreateThriftSharedMemorySession(
                &session,sharedMemoryName.c_str(), &sessionInfo
            );
            if(SessionResult == HAPI_RESULT_SUCCESS){
                printFile("Successful connect to an existint HAPI shared memory session\n");
            }else{
                printErr("Error create session: ",SessionResult);
            }
        }break;
        default:{
            printErr("Cannot connect to unknown session type (",sessionType,")");
            return false;
        }break;
        }

        if(SessionResult != HAPI_RESULT_SUCCESS){
            if (sessionType != SessionType::InProcess)
            {
                std::string connectionError = HoudiniEngineUtility::getConnectionError();
                if (!connectionError.empty())
                    printErr("Houdini Engine Session failed to connect - ",connectionError.c_str());
            }

            return false;
        }
        if(!initialize(use_cooking_thread)){
            printErr("Failed to start the Houdini Engine session - Failed to initialize HAPI");
            return false;
        }

        return true;
    }
    bool stopSession(){
        if(HoudiniApi::IsSessionValid(&session) == HAPI_RESULT_SUCCESS){
            HoudiniApi::Cleanup(&session);
            if(sessionType == InProcess){
                HoudiniApi::Shutdown(&session);
            }
            HoudiniApi::CloseSession(&session);
        }
        return true;
    }
    bool restartSession(SessionType sessionType, bool use_cooking_thread){
        bool success = false;
        stopSession();
        if(!startSession(sessionType,use_cooking_thread)){
            printErr("Failed to restart the Houdini Engine session - Failed to start the new Session");
        }else{
            if(HoudiniApi::IsSessionValid(&session) != HAPI_RESULT_SUCCESS){
                return false;
            }
            if(!initialize(use_cooking_thread)){
                printErr("Failed to restart the Houdini Engine session - Failed to initialize HAPI");
            }else{
                success = true;
            }
        }
        return success;
    }
    bool initialize(bool use_cooking_thread){
        if(HoudiniApi::IsSessionValid(&session) != HAPI_RESULT_SUCCESS){
            printErr("Failed to initialize HAPI: The session is invalid.");
            return false;
        }

        if(HoudiniApi::IsInitialized(&session) == HAPI_RESULT_NOT_INITIALIZED){
            HAPI_CookOptions cookOptions = HoudiniApi::CookOptions_Create();
            
            cookOptions.curveRefineLOD = 8.0f;
            cookOptions.clearErrorsAndWarnings = false;
            cookOptions.maxVerticesPerPrimitive = 3;
            cookOptions.splitGeosByGroup = false;
            cookOptions.refineCurveToLinear = true;
            cookOptions.handleBoxPartTypes = false;
            cookOptions.handleSpherePartTypes = false;
            cookOptions.splitPointsByVertexAttributes = false;
            cookOptions.packedPrimInstancingMode = HAPI_PACKEDPRIM_INSTANCING_MODE_FLAT;

            HAPI_Result Result = HoudiniApi::Initialize(
                &session,&cookOptions,use_cooking_thread,-1,"",nullptr,nullptr,nullptr,nullptr
            );
            this->cookOptions = cookOptions;
            if(Result == HAPI_RESULT_SUCCESS){
                printFile("Successfully initialized Houdini Engine.");
            }else if(Result == HAPI_RESULT_ALREADY_INITIALIZED){
                printFile("Successfully initialized Houdini Engine - HAPI was already initialized.");
            }else{
                printErr("Houdini Engine API initialization failed");
                return false;
            }
        }
        return true;
    }
    std::vector<int> loadAssets(godot::Ref<HDAResource> hdaRes,Void){
        
        if(auto a = HoudiniApi::IsSessionValid(&session);a != HAPI_RESULT_SUCCESS){
            printErr("Error load Asset with invalid session: ");
            return {};
        }
        int assetId = -1;
        if(auto a = HoudiniApi::LoadAssetLibraryFromFile(&session,hdaRes->path.c_str(),true,&assetId);a != HAPI_RESULT_SUCCESS){
            printErr("Error load Asset from file: ");
            return {};
        }
        int asset_count = 0;
        if(auto a = HoudiniApi::GetAvailableAssetCount(&session,assetId,&asset_count); a != HAPI_RESULT_SUCCESS){
            printErr("Error get available asset count: ",a);
            return {};
        }
        std::vector<HAPI_StringHandle> assetSH;
        assetSH.resize(asset_count);
        if(auto a = HoudiniApi::GetAvailableAssets(&session,assetId,assetSH.data(),asset_count);a != HAPI_RESULT_SUCCESS){
            printErr("Error get available assets: ",a);
            return {};
        }

        std::string temp;
        int rootId = -1;
        std::vector<int> result;
        result.reserve(asset_count);
        for(int i = 0;i!=asset_count;++i){
            temp = HoudiniEngineUtility::getString(&session,assetSH[i]);
            int id = -1;
            createNode(temp,temp,id,rootId,assetId);
            result.push_back(id);
        }
        hdaRes->assetId = assetId;
        assetIds.insert({assetId,hdaRes});
        return result;
    }
    godot::PackedInt32Array loadAssets(godot::Ref<HDAResource> hdaRes){
        auto arr = loadAssets(hdaRes,Void{});
        godot::PackedInt32Array res;
        for(auto a : arr){
            res.push_back(a);
        }
        return res;
    }
    bool createNode(std::string nodeLabel, std::string operatorName, int& id, int parentId, int assetId){
        if(HoudiniApi::IsSessionValid(&session) != HAPI_RESULT_SUCCESS){
            printErr("Failed to create node: The session is invalid.");
            return false;
        }
        if(HoudiniApi::CreateNode(&session,parentId,operatorName.c_str(),nodeLabel.c_str(),false,&id) != HAPI_RESULT_SUCCESS){
            printErr("Error create node: ",HoudiniEngineUtility::getLastError().c_str());
            return false;
        }else{
            printFile("Success create node, ID: ",id);
        }
        nodeIds.insert({id,assetId});
        return true;
    }
    bool createNode(godot::String nodeLabel,godot::String operatorName, NodeId* id, NodeId* parentId, int assetId){
        return createNode(std::string(nodeLabel.utf8().get_data()),std::string(operatorName.utf8().get_data()),*id->id.get(),(int)*parentId,assetId);
    }
    bool connectNode(int nodeId, int inputIndex,int node_to_connect,int outputIndex){
        if(HoudiniApi::IsSessionValid(&session) != HAPI_RESULT_SUCCESS){
            printErr("Failed to connect node: The session is invalid.");
            return false;
        }
        if(HoudiniApi::ConnectNodeInput(&session,nodeId,inputIndex,node_to_connect,outputIndex) != HAPI_RESULT_SUCCESS){
            printErr("Error connect node: ",HoudiniEngineUtility::getLastError().c_str());
            return false;
        }else{
            printFile("Success connect node ",nodeId," with ",node_to_connect);
        }
        return true;
    }
    bool connectNode(NodeId* nodeId,int inputIndex,NodeId* node_to_connect,int outputIndex){
        return connectNode(*nodeId,inputIndex,*node_to_connect,outputIndex);
    }
    bool cookNode(int id){
        if(HoudiniApi::IsSessionValid(&session) != HAPI_RESULT_SUCCESS){
            printErr("Failed to cook node: The session is invalid.");
            return false;
        }
        if(HoudiniApi::CookNode(&session,id,&cookOptions) != HAPI_RESULT_SUCCESS){
            printErr("Failed to cook node",HoudiniEngineUtility::getLastCookError().c_str());
            return false;
        }
        if(waitForCook()){
            printFile("Id: ",id," - Cook complete.");
        }
        return true;
    }
    bool cookNode(NodeId* id){
        return cookNode((int)*id);
    }
    bool deleteNode(int id){
        if(HoudiniApi::IsSessionValid(&session) != HAPI_RESULT_SUCCESS){
            printErr("Failed to cook node: The session is invalid.");
            return false;
        }
        if(auto a = HoudiniApi::DeleteNode(&session,id);a != HAPI_RESULT_SUCCESS){
            printErr("Failed to delete node: ",a," - ",HoudiniEngineUtility::getLastError().c_str());
            return false;
        }
        nodeIds.erase(id);
        parameters.erase(id);
        geometries.erase(id);

        return true;
    }
    bool deleteNode(NodeId* id){
        return deleteNode((int)*id);
    }
    bool waitForCook(){
        if(HoudiniApi::IsSessionValid(&session) != HAPI_RESULT_SUCCESS){
            printErr("Failed to cook node: The session is invalid.");
            return false;
        }
        int status;
        HAPI_Result result;
        do{
            result = HoudiniApi::GetStatus(&session,HAPI_STATUS_COOK_STATE,&status);
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }while(status > HAPI_STATE_MAX_READY_STATE && result == HAPI_RESULT_SUCCESS);
        if(status != HAPI_STATE_READY || result != HAPI_RESULT_SUCCESS){
            printErr("Cook failure: ",HoudiniEngineUtility::getLastCookError().c_str());
            return false;
        }
        return true;
    }
    bool createMeshNode(int nodeId){
        getGeometry(nodeId);
        if(geometries[nodeId].empty()){
            return false;
        }
        for(auto part : geometries[nodeId]){
            std::vector<int>& faces = std::get<0>(part.second);
            std::vector<float>& positions = std::get<1>(part.second);
            std::vector<int>& vertexs = std::get<2>(part.second);
            std::vector<float>& colors = std::get<3>(part.second);
            std::vector<float>& normals = std::get<4>(part.second);
            std::vector<float>& uvs = std::get<5>(part.second);
            std::vector<godot::Vector3> pos;
            std::vector<godot::Color> cols;
            int pos_num = positions.size()/3;
            int col_num = colors.size()/3;
            for(int i = 0;i!=pos_num;++i){
                godot::Vector3 vec;
                vec.x = positions[i*3];
                vec.y = positions[i*3+1];
                vec.z = positions[i*3+2];
                pos.push_back(vec);
                if(i+1 <= col_num){
                    godot::Color color;
                    color.r = colors[i*3];
                    color.g = colors[i*3+1];
                    color.b = colors[i*3+2];
                    cols.push_back(color);
                }
            }
            std::vector<godot::Vector3> nors;
            std::vector<godot::Vector2> uv_s;
            int vertex_num = vertexs.size()/3;
            vertex_num -= vertex_num%3;
            int nor_num = normals.size()/3;
            int uv_num = uvs.size()/3;
            for(int i = 0;i!=vertex_num;++i){
                if(i+1 <= nor_num){
                    godot::Vector3 nor;
                    nor.x = normals[i*3];
                    nor.y = normals[i*3+1];
                    nor.z = normals[i*3+2];
                    nors.push_back(nor);
                }
                if(i+1 <= uv_num){
                    godot::Vector2 vec;
                    vec.x = uvs[i*3];
                    vec.y = uvs[i*3+1];
                    uv_s.push_back(vec);
                }
            }
            godot::SurfaceTool* st = memnew(godot::SurfaceTool());
            st->begin(godot::Mesh::PRIMITIVE_TRIANGLES);
            for(int i = 0;i!=vertex_num;++i){
                if(vertexs[i]+1 > pos_num){
                    break;
                }
                if(i+1 <= nor_num)
                    st->set_normal(nors[i]);
                if(i+1 <= uv_num)
                    st->set_uv(uv_s[i]);
                if(vertexs[i]+1 <= col_num)
                    st->set_color(cols[vertexs[i]]);
                st->add_vertex(pos[vertexs[i]]);
            }
            godot::Ref<godot::ArrayMesh> arr_mesh = st->commit();
            godot::MeshInstance3D* instance = memnew(godot::MeshInstance3D());
            instance->set_mesh(arr_mesh);
            Contact::add_call([=,this]{
                add_child(instance,true);
                instance->set_owner(get_tree()->get_edited_scene_root());
            });
        }
        return true;
    }
    HAPI_NodeInfo getNodeInfo(int id){
        HAPI_NodeInfo info;
        if(HoudiniApi::GetNodeInfo(&session,id,&info) != HAPI_RESULT_SUCCESS){
            printErr(HoudiniEngineUtility::getLastError().c_str());
            return {};
        }
        return info;
    }
    godot::Dictionary getNodeInfo(NodeId* id){
        
        HAPI_NodeInfo info;
        if(HoudiniApi::GetNodeInfo(&session,*id,&info) != HAPI_RESULT_SUCCESS){
            printErr(HoudiniEngineUtility::getLastError().c_str());
            return {};
        }
        godot::Dictionary dic;
        dic["id"] = info.id;
        dic["parentId"] = info.parentId;
        auto name = HoudiniEngineUtility::getString(&session, info.nameSH);
        dic["name"] = godot::String::utf8(name.c_str());
        dic["type"] = info.type;
        dic["isValid"] = info.isValid;
        dic["totalCookCount"] = info.totalCookCount;
        dic["uniqueHoudiniNodeId"] = info.uniqueHoudiniNodeId;
        dic["parmCount"] = info.parmCount;
        dic["parmChoiceCount"] = info.parmChoiceCount;
        dic["childNodeCount"] = info.childNodeCount;
        dic["inputCount"] = info.inputCount;
        dic["outputCount"] = info.outputCount;
        dic["createdPostAssetLoad"] = info.createdPostAssetLoad;
        dic["isTimeDependent"] = info.isTimeDependent;
        dic["parmIntValueCount"] = info.parmIntValueCount;
        dic["parmFloatValueCount"] = info.parmFloatValueCount;
        dic["parmStringValueCount"] = info.parmStringValueCount;
        return dic;
    }
    HAPI_AssetInfo getAssetInfo(int id){
        HAPI_AssetInfo info;
        if(HoudiniApi::GetAssetInfo(&session,id,&info) != HAPI_RESULT_SUCCESS){
            printErr(HoudiniEngineUtility::getLastError().c_str());
            return {};
        }
        return info;
    }
    godot::Dictionary getAssetInfo(NodeId* id){
        
        HAPI_AssetInfo info;
        if(HoudiniApi::GetAssetInfo(&session,*id,&info) != HAPI_RESULT_SUCCESS){
            printErr(HoudiniEngineUtility::getLastError().c_str());
            return {};
        }
        godot::Dictionary dic;
        dic["nodeId"] = info.nodeId;
        dic["objectNodeId"] = info.objectNodeId;
        dic["hasEverCooked"] = info.hasEverCooked;
        auto name = HoudiniEngineUtility::getString(&session, info.nameSH);
        dic["name"] = godot::String::utf8(name.c_str());
        auto label = HoudiniEngineUtility::getString(&session, info.labelSH);
        dic["label"] = godot::String::utf8(label.c_str());
        auto filePath = HoudiniEngineUtility::getString(&session, info.filePathSH);
        dic["filePath"] = godot::String::utf8(filePath.c_str());
        auto version = HoudiniEngineUtility::getString(&session, info.versionSH);
        dic["version"] = godot::String::utf8(version.c_str());
        auto fullOpName = HoudiniEngineUtility::getString(&session, info.fullOpNameSH);
        dic["fullOpName"] = godot::String::utf8(fullOpName.c_str());
        auto helpText = HoudiniEngineUtility::getString(&session, info.helpTextSH);
        dic["helpText"] = godot::String::utf8(helpText.c_str());
        auto helpURL = HoudiniEngineUtility::getString(&session, info.helpURLSH);
        dic["helpURL"] = godot::String::utf8(helpURL.c_str());
        dic["objectCount"] = info.objectCount;
        dic["handleCount"] = info.handleCount;
        dic["transformInputCount"] = info.transformInputCount;
        dic["geoInputCount"] = info.geoInputCount;
        dic["geoOutputCount"] = info.geoOutputCount;
        dic["haveObjectsChanged"] = info.haveObjectsChanged;
        dic["haveMaterialsChanged"] = info.haveMaterialsChanged;
        return dic;
    }
    HAPI_ObjectInfo getObjectInfo(int id){
        HAPI_ObjectInfo info;
        if(HoudiniApi::GetObjectInfo(&session,id,&info) != HAPI_RESULT_SUCCESS){
            printErr(HoudiniEngineUtility::getLastError().c_str());
            return {};
        }
        return info;
    }
    godot::Dictionary getObjectInfo(NodeId* id){
        
        HAPI_ObjectInfo info;
        if(HoudiniApi::GetObjectInfo(&session,*id,&info) != HAPI_RESULT_SUCCESS){
            printErr(HoudiniEngineUtility::getLastError().c_str());
            return {};
        }
        godot::Dictionary dic;
        auto name = HoudiniEngineUtility::getString(&session, info.nameSH);
        dic["name"] = godot::String::utf8(name.c_str());
        auto objectInstancePath = HoudiniEngineUtility::getString(&session, info.objectInstancePathSH);
        dic["objectInstancePath"] = godot::String::utf8(objectInstancePath.c_str());
        dic["hasTransformChanged"] = info.hasTransformChanged;
        dic["haveGeosChanged"] = info.haveGeosChanged;
        dic["isVisible"] = info.isVisible;
        dic["isInstancer"] = info.isInstancer;
        dic["isInstanced"] = info.isInstanced;
        dic["geoCount"] = info.geoCount;
        dic["nodeId"] = info.nodeId;
        dic["objectToInstanceId"] = info.objectToInstanceId;
        return dic;
    }
    HAPI_GeoInfo getGeoInfo(int id){
        
        HAPI_GeoInfo info;
        if(HoudiniApi::GetGeoInfo(&session,id,&info) != HAPI_RESULT_SUCCESS){
            printErr(HoudiniEngineUtility::getLastError().c_str());
            return {};
        }
        return info;
    }
    godot::Dictionary getGeoInfo(NodeId* id){
        
        HAPI_GeoInfo info;
        if(HoudiniApi::GetGeoInfo(&session,*id,&info) != HAPI_RESULT_SUCCESS){
            printErr(HoudiniEngineUtility::getLastError().c_str());
            return {};
        }
        godot::Dictionary dic;
        dic["type"] = info.type;
        auto name = HoudiniEngineUtility::getString(&session, info.nameSH);
        dic["name"] = godot::String::utf8(name.c_str());
        dic["nodeId"] = info.nodeId;
        dic["isEditable"] = info.isEditable;
        dic["isTemplated"] = info.isTemplated;
        dic["isDisplayGeo"] = info.isDisplayGeo;
        dic["hasGeoChanged"] = info.hasGeoChanged;
        dic["hasMaterialChanged"] = info.hasMaterialChanged;
        dic["pointGroupCount"] = info.pointGroupCount;
        dic["primitiveGroupCount"] = info.primitiveGroupCount;
        dic["edgeGroupCount"] = info.edgeGroupCount;
        dic["partCount"] = info.partCount;
        return dic;
    }
    HAPI_MaterialInfo getMaterialInfo(int id){
        
        HAPI_MaterialInfo info;
        if(HoudiniApi::GetMaterialInfo(&session,id,&info) != HAPI_RESULT_SUCCESS){
            printErr(HoudiniEngineUtility::getLastError().c_str());
            return {};
        }
        return info;
    }
    godot::Dictionary getMaterialInfo(NodeId* id){
        
        HAPI_MaterialInfo info;
        if(HoudiniApi::GetMaterialInfo(&session,*id,&info) != HAPI_RESULT_SUCCESS){
            printErr(HoudiniEngineUtility::getLastError().c_str());
            return {};
        }
        godot::Dictionary dic;
        dic["nodeId"] = info.nodeId;
        dic["exists"] = info.exists;
        dic["hasChanged"] = info.hasChanged;
        return dic;
    }
    void getParameters(int id){
        if(HoudiniApi::IsSessionValid(&session) != HAPI_RESULT_SUCCESS){
            printErr("Failed to get parameters: The session is invalid.");
            return;
        }
        HAPI_NodeInfo info = getNodeInfo(id);
        std::vector<HAPI_ParmInfo> parm_infos;
        parm_infos.resize(info.parmCount);
        if(HoudiniApi::GetParameters(&session,id,parm_infos.data(),0,info.parmCount) != HAPI_RESULT_SUCCESS){
            printErr(HoudiniEngineUtility::getLastError().c_str());
            return;
        }
        for(int i = 0;i!=info.parmCount;++i){
            auto name = HoudiniEngineUtility::getString(&session, parm_infos[i].nameSH);
            std::vector<std::variant<int64_t,double,std::string>> arr;
            if(HoudiniApi::ParmInfo_IsInt(&parm_infos[i])){
                int parm_int_count = HoudiniApi::ParmInfo_GetIntValueCount(&parm_infos[i]);
                std::vector<int> parm_int_values;
                parm_int_values.resize(parm_int_count);

                if(HoudiniApi::GetParmIntValues(&session,id,parm_int_values.data(),parm_infos[i].intValuesIndex,parm_int_count) != HAPI_RESULT_SUCCESS){
                    printErr(HoudiniEngineUtility::getLastError().c_str());
                    continue;
                }
                for(int v = 0;v != parm_int_count;++v){
                    arr.push_back(parm_int_values[v]);
                }
            }else if(HoudiniApi::ParmInfo_IsFloat(&parm_infos[i])){
                int parm_float_count = HoudiniApi::ParmInfo_GetFloatValueCount(&parm_infos[i]);
                std::vector<float> parm_float_values;
                parm_float_values.resize(parm_float_count);

                if(HoudiniApi::GetParmFloatValues(&session,id,parm_float_values.data(),parm_infos[i].floatValuesIndex,parm_float_count) != HAPI_RESULT_SUCCESS){
                    printErr(HoudiniEngineUtility::getLastError().c_str());
                    continue;
                }
                for(int v = 0;v != parm_float_count;++v){
                    arr.push_back(parm_float_values[v]);
                }
            }else if(HoudiniApi::ParmInfo_IsString(&parm_infos[i])){
                int parm_string_count = HoudiniApi::ParmInfo_GetStringValueCount(&parm_infos[i]);
                std::vector<HAPI_StringHandle> parmSH_values;
                if(HoudiniApi::GetParmStringValues(&session,id,true,parmSH_values.data(),parm_infos[i].stringValuesIndex,parm_string_count)!=HAPI_RESULT_SUCCESS){
                    printErr(HoudiniEngineUtility::getLastError().c_str());
                    continue;
                }
                for(int v = 0;v != parm_string_count;++v){
                    arr.push_back(HoudiniEngineUtility::getString(&session,parmSH_values[v]));
                }
            }
            parameters[id][name] = arr;
        }
    }
    godot::Dictionary getParameters(NodeId* id){
        getParameters(*id);
        godot::Dictionary dict;
        auto& param = parameters[*id];
        for(auto& a : param){
            godot::Array arr;
            for(auto& b : a.second){
                if(std::holds_alternative<int64_t>(b)){
                    arr.push_back(std::get<int64_t>(b));
                }else if(std::holds_alternative<double>(b)){
                    arr.push_back(std::get<double>(b));
                }else if(std::holds_alternative<std::string>(b)){
                    arr.push_back(godot::String::utf8(std::get<std::string>(b).c_str()));
                }
            }
            dict[godot::String::utf8(a.first.c_str())] = arr;
        }
        return dict;
    }
    godot::Dictionary getAttributes(NodeId* nodeId,PartId* partId){
        if(HoudiniApi::IsSessionValid(&session) != HAPI_RESULT_SUCCESS){
            printErr("Failed to get attributes: The session is invalid.");
            return {};
        }
        HAPI_PartInfo part_info;
        HoudiniApi::PartInfo_Init(&part_info);
        if(HoudiniApi::GetPartInfo(&session,*nodeId, *partId, &part_info) != HAPI_RESULT_SUCCESS){
            printErr(HoudiniEngineUtility::getLastError().c_str());
            return {};
        }
        int vertex_attr_count = part_info.attributeCounts[HAPI_ATTROWNER_VERTEX];
        int point_attr_count = part_info.attributeCounts[HAPI_ATTROWNER_POINT];
        int prim_attr_count = part_info.attributeCounts[HAPI_ATTROWNER_PRIM];
        int detail_attr_count = part_info.attributeCounts[HAPI_ATTROWNER_DETAIL];

        std::vector<HAPI_StringHandle> attrNameSH;
        attrNameSH.resize(point_attr_count);
        if(HoudiniApi::GetAttributeNames(&session,*nodeId,*partId,HAPI_ATTROWNER_POINT,attrNameSH.data(),point_attr_count) != HAPI_RESULT_SUCCESS){
            printErr(HoudiniEngineUtility::getLastError().c_str());
            return {};
        }

        godot::Dictionary dict;
        godot::Array point_attrs;
        godot::Dictionary point_attr;
        for(int i = 0;i!=point_attr_count;++i){
            std::string attr_name = HoudiniEngineUtility::getString(&session, attrNameSH[i]);
            HAPI_AttributeInfo attr_info;
            HoudiniApi::AttributeInfo_Init(&attr_info);
            if(HoudiniApi::GetAttributeInfo(&session,*nodeId,*partId,attr_name.c_str(),HAPI_ATTROWNER_POINT,&attr_info) != HAPI_RESULT_SUCCESS){
                printErr(HoudiniEngineUtility::getLastError().c_str());
                return {};
            }
            point_attr["Name"] = godot::String::utf8(attr_name.c_str());
            point_attr["Count"] = attr_info.count;
            point_attr["Storage"] = attr_info.storage;
            point_attr["Exists"] = attr_info.exists;
            point_attr["TupleSize"] = attr_info.tupleSize;
            point_attr["TypeInfo"] = attr_info.typeInfo;
            point_attr["Owner"] = attr_info.owner;
            point_attr["OriginalOwner"] = attr_info.originalOwner;
            point_attrs.push_back(point_attr);
        }
        dict["Point"] = point_attrs;
        
        attrNameSH.clear();
        attrNameSH.resize(vertex_attr_count);
        if(HoudiniApi::GetAttributeNames(&session,*nodeId,*partId,HAPI_ATTROWNER_VERTEX,attrNameSH.data(),vertex_attr_count) != HAPI_RESULT_SUCCESS){
            printErr(HoudiniEngineUtility::getLastError().c_str());
            return {};
        }
        
        godot::Array vertex_attrs;
        godot::Dictionary vertex_attr;
        for(int i = 0;i!=point_attr_count;++i){
            std::string attr_name = HoudiniEngineUtility::getString(&session, attrNameSH[i]);
            HAPI_AttributeInfo attr_info;
            HoudiniApi::AttributeInfo_Init(&attr_info);
            if(HoudiniApi::GetAttributeInfo(&session,*nodeId,*partId,attr_name.c_str(),HAPI_ATTROWNER_VERTEX,&attr_info) != HAPI_RESULT_SUCCESS){
                printErr(HoudiniEngineUtility::getLastError().c_str());
                return {};
            }
            vertex_attr["Name"] = godot::String::utf8(attr_name.c_str());
            vertex_attr["Count"] = attr_info.count;
            vertex_attr["Storage"] = attr_info.storage;
            vertex_attr["Exists"] = attr_info.exists;
            vertex_attr["TupleSize"] = attr_info.tupleSize;
            vertex_attr["TypeInfo"] = attr_info.typeInfo;
            vertex_attr["Owner"] = attr_info.owner;
            vertex_attr["OriginalOwner"] = attr_info.originalOwner;
            vertex_attrs.push_back(point_attr);
        }
        dict["Vertex"] = point_attrs;

        attrNameSH.clear();
        attrNameSH.resize(prim_attr_count);
        if(HoudiniApi::GetAttributeNames(&session,*nodeId,*partId,HAPI_ATTROWNER_PRIM,attrNameSH.data(),prim_attr_count) != HAPI_RESULT_SUCCESS){
            printErr(HoudiniEngineUtility::getLastError().c_str());
            return {};
        }

        godot::Array prim_attrs;
        godot::Dictionary prim_attr;
        for(int i = 0;i!=point_attr_count;++i){
            std::string attr_name = HoudiniEngineUtility::getString(&session, attrNameSH[i]);
            HAPI_AttributeInfo attr_info;
            HoudiniApi::AttributeInfo_Init(&attr_info);
            if(HoudiniApi::GetAttributeInfo(&session,*nodeId,*partId,attr_name.c_str(),HAPI_ATTROWNER_PRIM,&attr_info) != HAPI_RESULT_SUCCESS){
                printErr(HoudiniEngineUtility::getLastError().c_str());
                return {};
            }
            prim_attr["Name"] = godot::String::utf8(attr_name.c_str());
            prim_attr["Count"] = attr_info.count;
            prim_attr["Storage"] = attr_info.storage;
            prim_attr["Exists"] = attr_info.exists;
            prim_attr["TupleSize"] = attr_info.tupleSize;
            prim_attr["TypeInfo"] = attr_info.typeInfo;
            prim_attr["Owner"] = attr_info.owner;
            prim_attr["OriginalOwner"] = attr_info.originalOwner;
            prim_attrs.push_back(point_attr);
        }
        dict["Prim"] = point_attrs;

        
        attrNameSH.clear();
        attrNameSH.resize(detail_attr_count);
        if(HoudiniApi::GetAttributeNames(&session,*nodeId,*partId,HAPI_ATTROWNER_DETAIL,attrNameSH.data(),detail_attr_count) != HAPI_RESULT_SUCCESS){
            printErr(HoudiniEngineUtility::getLastError().c_str());
            return {};
        }

        godot::Array detail_attrs;
        godot::Dictionary detail_attr;
        for(int i = 0;i!=point_attr_count;++i){
            std::string attr_name = HoudiniEngineUtility::getString(&session, attrNameSH[i]);
            HAPI_AttributeInfo attr_info;
            HoudiniApi::AttributeInfo_Init(&attr_info);
            if(HoudiniApi::GetAttributeInfo(&session,*nodeId,*partId,attr_name.c_str(),HAPI_ATTROWNER_DETAIL,&attr_info) != HAPI_RESULT_SUCCESS){
                printErr(HoudiniEngineUtility::getLastError().c_str());
                return {};
            }
            detail_attr["Name"] = godot::String::utf8(attr_name.c_str());
            detail_attr["Count"] = attr_info.count;
            detail_attr["Storage"] = attr_info.storage;
            detail_attr["Exists"] = attr_info.exists;
            detail_attr["TupleSize"] = attr_info.tupleSize;
            detail_attr["TypeInfo"] = attr_info.typeInfo;
            detail_attr["Owner"] = attr_info.owner;
            detail_attr["OriginalOwner"] = attr_info.originalOwner;
            detail_attrs.push_back(point_attr);
        }
        dict["Detail"] = point_attrs;

        return dict;
    }
    void getGeometry(int id){
        HAPI_GeoInfo mesh_geo_info;
        if(HoudiniApi::GetDisplayGeoInfo(&session, id, &mesh_geo_info) != HAPI_RESULT_SUCCESS){
            printErr(HoudiniEngineUtility::getLastError().c_str());
            return;
        }
        for(int partId = 0;partId!=mesh_geo_info.partCount;++partId){
            HAPI_PartInfo mesh_part_info;
            if(HoudiniApi::GetPartInfo(&session, mesh_geo_info.nodeId, partId, &mesh_part_info) != HAPI_RESULT_SUCCESS){
                printErr(HoudiniEngineUtility::getLastError().c_str());
                return;
            }
            std::vector<int> mesh_face_counts(mesh_part_info.faceCount);
            if(HoudiniApi::GetFaceCounts(&session,mesh_geo_info.nodeId,mesh_part_info.id,mesh_face_counts.data(),0,mesh_part_info.faceCount) != HAPI_RESULT_SUCCESS){
                printErr(HoudiniEngineUtility::getLastError().c_str());
                return;
            }
            std::vector<int> mesh_vertex_list(mesh_part_info.vertexCount);
            if(HoudiniApi::GetVertexList(&session,mesh_geo_info.nodeId,mesh_part_info.id,mesh_vertex_list.data(),0, mesh_part_info.vertexCount) != HAPI_RESULT_SUCCESS){
                printErr(HoudiniEngineUtility::getLastError().c_str());
                return;
            }
            std::for_each(std::execution::par_unseq,mesh_face_counts.begin(),mesh_face_counts.end(),[](int& data){
                //TODO: triangulate
            });
            auto fetchPointAttrib = [&](HAPI_AttributeOwner owner,const char* attrib_name,std::vector<float>& mesh_attrib_data)->bool{
                HAPI_AttributeInfo mesh_attrib_info;
                if(HoudiniApi::GetAttributeInfo(&session,mesh_geo_info.nodeId,mesh_part_info.id,attrib_name, owner,&mesh_attrib_info) != HAPI_RESULT_SUCCESS){
                    printFile(HoudiniEngineUtility::getLastError().c_str(),"Attribute name : ",attrib_name);
                    return false;
                }
                mesh_attrib_data.resize(mesh_attrib_info.count * mesh_attrib_info.tupleSize);
                if(HoudiniApi::GetAttributeFloatData(&session,mesh_geo_info.nodeId,mesh_part_info.id,attrib_name,&mesh_attrib_info,-1,mesh_attrib_data.data(),0,mesh_attrib_info.count) != HAPI_RESULT_SUCCESS){
                    printFile(HoudiniEngineUtility::getLastError().c_str(),"Attribute name : ",attrib_name);
                    return false;
                }
                return true;
            };
            std::vector<float> mesh_p_attrib_info;
            fetchPointAttrib(HAPI_ATTROWNER_POINT, "P", mesh_p_attrib_info);

            std::vector<float> mesh_cd_attrib_data;
            fetchPointAttrib(HAPI_ATTROWNER_POINT, "Cd", mesh_cd_attrib_data);

            std::vector<float> mesh_N_attrib_data;
            fetchPointAttrib(HAPI_ATTROWNER_VERTEX , "N", mesh_N_attrib_data);

            std::vector<float> mesh_uv_attrib_data;
            fetchPointAttrib(HAPI_ATTROWNER_VERTEX , "uv", mesh_uv_attrib_data);

            geometries[id][partId] = {std::move(mesh_face_counts),std::move(mesh_p_attrib_info),std::move(mesh_vertex_list),std::move(mesh_cd_attrib_data),std::move(mesh_N_attrib_data),std::move(mesh_uv_attrib_data)};
        }
    }
    int setGeometry(int id){
        for(auto part : geometries[id]){
            std::vector<int>& faces = std::get<0>(part.second);
            std::vector<float>& positions = std::get<1>(part.second);
            std::vector<int>& vertexs = std::get<2>(part.second);
            std::vector<float>& colors = std::get<3>(part.second);
            std::vector<float>& normals = std::get<4>(part.second);
            std::vector<float>& uvs = std::get<5>(part.second);

            HAPI_AttributeInfo pointInfo = HoudiniApi::AttributeInfo_Create();
            pointInfo.count = positions.size()/3;
            pointInfo.tupleSize = 3;
            pointInfo.exists = true;
            pointInfo.storage = HAPI_STORAGETYPE_FLOAT;
            pointInfo.owner = HAPI_ATTROWNER_POINT;
            if(HoudiniApi::AddAttribute(&session,id,part.first,"P",&pointInfo) != HAPI_RESULT_SUCCESS){
                printErr(HoudiniEngineUtility::getLastError().c_str());
                return -1;
            }
            if(HoudiniApi::SetAttributeFloatData(&session,id,part.first,"P",&pointInfo,positions.data(),0,positions.size()/3) != HAPI_RESULT_SUCCESS){
                printErr(HoudiniEngineUtility::getLastError().c_str());
                return -1;
            }
            if(HoudiniApi::SetVertexList(&session,id,part.first,vertexs.data(),0,vertexs.size()) != HAPI_RESULT_SUCCESS){
                printErr(HoudiniEngineUtility::getLastError().c_str());
                return -1;
            }
            if(HoudiniApi::SetFaceCounts(&session,id,part.first,faces.data(),0,faces.size()) != HAPI_RESULT_SUCCESS){
                printErr(HoudiniEngineUtility::getLastError().c_str());
                return -1;
            }
        }
        if(HoudiniApi::CommitGeo(&session,id) != HAPI_RESULT_SUCCESS){
            printErr(HoudiniEngineUtility::getLastError().c_str());
            return -1;
        }
        HAPI_NodeInfo nodeInfo = getNodeInfo(id);
        HAPI_NodeId parentId = nodeInfo.parentId;

        HAPI_NodeId colorNode = -1;
        createNode("Color","color",colorNode,parentId,nowAsset->assetId);
        connectNode(colorNode,0,id,0);
        HAPI_NodeId normalNode = -1;
        createNode("Normal","normal",normalNode,id,nowAsset->assetId);
        connectNode(normalNode,0,id,0);
        HAPI_NodeId uvProjectNode = -1;
        createNode("UV","uvproject",uvProjectNode,id,nowAsset->assetId);
        connectNode(uvProjectNode,0,id,0);
        HAPI_NodeId outputNode = -1;
        createNode("Output","output",outputNode,id,nowAsset->assetId);
        connectNode(outputNode,0,id,0);
        if(HoudiniApi::SetNodeDisplay(&session,outputNode,1)!=HAPI_RESULT_SUCCESS){
            printErr(HoudiniEngineUtility::getLastError().c_str());
            return -1;
        }
        return outputNode;
    }
};


#endif //!HOUDINI_BIND_GODOT
