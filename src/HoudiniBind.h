
#ifndef HOUDINI_BIND_GODOT
#define HOUDINI_BIND_GODOT

#include <HAPI/HAPI.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <fstream>
#include <chrono>
#include <memory>
#include <thread>
#include <map>
#include <set>
#include <any>
#include <queue>
#include <filesystem>
#include <execution>
#include <variant>
#include <source_location>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/material.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/surface_tool.hpp>
#include <godot_cpp/classes/multi_mesh.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/multi_mesh_instance3d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/transform3d.hpp>
#include <godot_cpp/core/math.hpp>
#include <HoudiniApi.h>
#include <HoudiniEngineUtility.h>
#include <HoudiniEnginePlatform.h>
#include "Utility.h"
#include "Contact.h"
#include "HDAImporter.h"

VARIANT_ENUM_CAST(HAPI_PackedPrimInstancingMode)
VARIANT_ENUM_CAST(HAPI_StorageType)
VARIANT_ENUM_CAST(HAPI_AttributeTypeInfo)
VARIANT_ENUM_CAST(HAPI_AttributeOwner)
VARIANT_ENUM_CAST(HAPI_NodeType)
VARIANT_ENUM_CAST(HAPI_GeoType)
VARIANT_ENUM_CAST(HAPI_Result)
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
class Void{
public:
    Void(godot::Variant){}
    Void(std::any){}
    Void(){}
};
class Int: public godot::RefCounted{
GDCLASS(Int, godot::RefCounted)
    void set_value(int64_t value){
        this->value = value;
    }
    int64_t get_value(){
        return value;
    }
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("set_value","value"),&Int::set_value);
        godot::ClassDB::bind_method(godot::D_METHOD("get_value"),&Int::get_value);
        godot::ClassDB::add_property("Int",godot::PropertyInfo(godot::Variant::INT,"value"),"set_value","get_value");
    }
public:
    intmax_t value;
    Int(){}
    template<typename T> requires std::integral<T>
    Int(T value){
        this->value = value;
    }
};
class RefDictionary: public godot::RefCounted{
    GDCLASS(RefDictionary,RefCounted)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("set_value","value"),&RefDictionary::set_value);
        godot::ClassDB::bind_method(godot::D_METHOD("get_value"),&RefDictionary::get_value);
        godot::ClassDB::add_property("RefDictionary",godot::PropertyInfo(godot::Variant::DICTIONARY,"value"),"set_value","get_value");
    }
    void set_value(godot::Dictionary value){
        this->value = value;
    }
    godot::Dictionary get_value(){
        return value;
    }
public:
    godot::Dictionary value;
    template<typename ...T>
    RefDictionary(T&&... v){
        value = godot::Dictionary(std::forward<decltype(v)>(v)...);
    }
};
class NodeId: public godot::RefCounted{
    GDCLASS(NodeId, godot::RefCounted)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("set_id","id"),&NodeId::set_id);
        godot::ClassDB::bind_method(godot::D_METHOD("get_id"),&NodeId::get_id);
        godot::ClassDB::add_property("NodeId",godot::PropertyInfo(godot::Variant::INT,"id"),"set_id","get_id");
    }
    void _init(int id){
        (*this->id) = id;
    }
public:
    NodeId(int id){
        (*this->id) = id;
    }
    NodeId(){
        (*this->id) = 0;
    }
    void set_id(int id){
        (*this->id) = id;
    }
    int get_id(){
        return *id;
    }
    operator int(){
        return *id;
    }
    std::shared_ptr<int> id = std::make_shared<int>(0);
};
class PartId: public godot::RefCounted{
    GDCLASS(PartId, godot::RefCounted)
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
class AssetLibId: public godot::RefCounted{
    GDCLASS(AssetLibId, godot::RefCounted)
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
class HoudiniEngineManager;
class HoudiniEngine: public godot::Object{
    GDCLASS(HoudiniEngine,godot::Object)
    static void _bind_methods(){
        BIND_ENUM_CONSTANT(HAPI_RESULT_SUCCESS);
        BIND_ENUM_CONSTANT(HAPI_RESULT_FAILURE);
        BIND_ENUM_CONSTANT(HAPI_RESULT_ALREADY_INITIALIZED);
        BIND_ENUM_CONSTANT(HAPI_RESULT_NOT_INITIALIZED);
        BIND_ENUM_CONSTANT(HAPI_RESULT_CANT_LOADFILE);
        BIND_ENUM_CONSTANT(HAPI_RESULT_PARM_SET_FAILED);
        BIND_ENUM_CONSTANT(HAPI_RESULT_INVALID_ARGUMENT);
        BIND_ENUM_CONSTANT(HAPI_RESULT_CANT_LOAD_GEO);
        BIND_ENUM_CONSTANT(HAPI_RESULT_CANT_GENERATE_PRESET);
        BIND_ENUM_CONSTANT(HAPI_RESULT_CANT_LOAD_PRESET);
        BIND_ENUM_CONSTANT(HAPI_RESULT_ASSET_DEF_ALREADY_LOADED);
        BIND_ENUM_CONSTANT(HAPI_RESULT_NO_LICENSE_FOUND);
        BIND_ENUM_CONSTANT(HAPI_RESULT_DISALLOWED_NC_LICENSE_FOUND);
        BIND_ENUM_CONSTANT(HAPI_RESULT_DISALLOWED_NC_ASSET_WITH_C_LICENSE);
        BIND_ENUM_CONSTANT(HAPI_RESULT_DISALLOWED_NC_ASSET_WITH_LC_LICENSE);
        BIND_ENUM_CONSTANT(HAPI_RESULT_DISALLOWED_LC_ASSET_WITH_C_LICENSE);
        BIND_ENUM_CONSTANT(HAPI_RESULT_DISALLOWED_HENGINEINDIE_W_3PARTY_PLUGIN);
        BIND_ENUM_CONSTANT(HAPI_RESULT_ASSET_INVALID);
        BIND_ENUM_CONSTANT(HAPI_RESULT_NODE_INVALID);
        BIND_ENUM_CONSTANT(HAPI_RESULT_USER_INTERRUPTED);
        BIND_ENUM_CONSTANT(HAPI_RESULT_INVALID_SESSION);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetObjectInfo","session","nodeId"),&HoudiniEngine::GetObjectInfo);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetAssetInfo","session","nodeId"),&HoudiniEngine::GetAssetInfo);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetNodeInfo","session","nodeId"),&HoudiniEngine::GetNodeInfo);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetGeoInfo","session","nodeId"),&HoudiniEngine::GetGeoInfo);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetMaterialInfo","session","nodeId"),&HoudiniEngine::GetMaterialInfo);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetAttrInfo","session","nodeId","partId","name","owner"),&HoudiniEngine::GetAttrInfo);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("AddAttribute","session","nodeId","partId","name","attrInfo"),&HoudiniEngine::AddAttribute);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("SetCustomString","session","string_value","handle_value"),&HoudiniEngine::SetCustomString);

    }
public:
    GDE_EXPORT static HAPI_ObjectInfo  DictToObjectInfo(HoudiniEngineManager* session, godot::Dictionary objectInfo);
    GDE_EXPORT static godot::Dictionary ObjectInfoToDict(HoudiniEngineManager* session, HAPI_ObjectInfo info);
    GDE_EXPORT static HAPI_AssetInfo DictToAssetInfo(HoudiniEngineManager* session, godot::Dictionary assetInfo);
    GDE_EXPORT static godot::Dictionary AssetInfoToDict(HoudiniEngineManager* session, HAPI_AssetInfo info);
    GDE_EXPORT static HAPI_NodeInfo DictToNodeInfo(HoudiniEngineManager* session, godot::Dictionary nodeInfo);
    GDE_EXPORT static godot::Dictionary NodeInfoToDict(HoudiniEngineManager* session, HAPI_NodeInfo info);
    GDE_EXPORT static HAPI_GeoInfo DictToGeoInfo(HoudiniEngineManager* session, godot::Dictionary geoInfo);
    GDE_EXPORT static godot::Dictionary GeoInfoToDict(HoudiniEngineManager* session, HAPI_GeoInfo info);
    GDE_EXPORT static HAPI_MaterialInfo DictToMaterialInfo(HoudiniEngineManager* session, godot::Dictionary matInfo);
    GDE_EXPORT static godot::Dictionary MaterialInfoToDict(HoudiniEngineManager* session, HAPI_MaterialInfo info);
    GDE_EXPORT static HAPI_AttributeInfo DictToAttrInfo(HoudiniEngineManager* session, godot::Dictionary attrInfo);
    GDE_EXPORT static godot::Dictionary AttrInfoToDict(HoudiniEngineManager* session, HAPI_AttributeInfo info);
    GDE_EXPORT static HAPI_CookOptions DictToCookOptions(godot::Dictionary cookOptions);
    GDE_EXPORT static godot::Dictionary CookOptionsToDict(HAPI_CookOptions options);


    GDE_EXPORT static godot::Dictionary GetObjectInfo(HoudiniEngineManager* session,int nodeId);
    GDE_EXPORT static godot::Dictionary GetAssetInfo(HoudiniEngineManager* session, int nodeId);
    GDE_EXPORT static godot::Dictionary GetNodeInfo(HoudiniEngineManager* session, int nodeId);
    GDE_EXPORT static godot::Dictionary GetGeoInfo(HoudiniEngineManager* session, int nodeId);
    GDE_EXPORT static godot::Dictionary GetMaterialInfo(HoudiniEngineManager* session, int nodeId);
    GDE_EXPORT static godot::Dictionary GetAttrInfo(HoudiniEngineManager* session,int nodeId,int partId,godot::String name,HAPI_AttributeOwner owner);
    GDE_EXPORT static HAPI_Result AddAttribute(HoudiniEngineManager* session,int nodeId,int partId,godot::String name,godot::Ref<RefDictionary> attrInfo);
    GDE_EXPORT static HAPI_Result Cleanup(HoudiniEngineManager* session);
    GDE_EXPORT static HAPI_Result ClearConnectionError();
    GDE_EXPORT static HAPI_Result CloseSession(HoudiniEngineManager* session);
    GDE_EXPORT static HAPI_Result CommitGeo(HoudiniEngineManager* session,int nodeId);
    GDE_EXPORT static HAPI_Result ConnectNodeInput(HoudiniEngineManager* session, int nodeId, int inputIndex, int nodeId_toConnect, int outputIndex);
    GDE_EXPORT static HAPI_Result CookNode(HoudiniEngineManager* session, int nodeId, godot::Ref<RefDictionary> cookOptions);
    GDE_EXPORT static godot::Dictionary CookOptions_Create();
    GDE_EXPORT static HAPI_Result SetCustomString(HoudiniEngineManager* session, godot::String string_value, godot::Ref<Int> handle_value);
};

class HDANode: public godot::Resource{
    GDCLASS(HDANode,godot::Resource)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("get_nodeId"),&HDANode::get_nodeId);
        godot::ClassDB::bind_method(godot::D_METHOD("set_nodeId","id"),&HDANode::set_nodeId);
        godot::ClassDB::add_property("HDANode",godot::PropertyInfo(godot::Variant::INT,"nodeId"),"set_nodeId","get_nodeId");
        godot::ClassDB::bind_method(godot::D_METHOD("get_name"),&HDANode::get_name);
        godot::ClassDB::bind_method(godot::D_METHOD("set_name","name"),&HDANode::set_name);
        godot::ClassDB::add_property("HDANode",godot::PropertyInfo(godot::Variant::STRING,"name"),"set_name","get_name");
        godot::ClassDB::bind_method(godot::D_METHOD("get_nodeInfo"),&HDANode::get_nodeInfo);
        godot::ClassDB::bind_method(godot::D_METHOD("set_nodeInfo","id"),&HDANode::set_nodeInfo);
        godot::ClassDB::add_property("HDANode",godot::PropertyInfo(godot::Variant::DICTIONARY,"nodeInfo"),"set_nodeInfo","get_nodeInfo");
    }
    friend class HoudiniEngineManager;
    int get_nodeId(){
        return nodeId;
    }
    void set_nodeId(int id){}
    godot::String get_name(){
        if(name.is_empty())
            name = get_nodeInfo()["name"];
        return name;
    }
    void set_name(godot::String name){}
    godot::Dictionary get_nodeInfo(){
        if(nodeInfo.is_empty()){
            if(session == nullptr)
                return {};
            nodeInfo = HoudiniEngine::GetNodeInfo(session,nodeId);
        }
        return nodeInfo;
    }
    void set_nodeInfo(godot::Dictionary){}
    HoudiniEngineManager* session = nullptr;
public:
    int nodeId;
    godot::String name;
    godot::Dictionary nodeInfo;
};
// class InputNode: public godot::Resource{
//     GDCLASS(InputNode,godot::Resource)
//     static void _bind_methods(){
//         godot::ClassDB::bind_method(godot::D_METHOD("get_nodeId"),&InputNode::get_nodeId);
//         godot::ClassDB::bind_method(godot::D_METHOD("set_nodeId","id"),&InputNode::set_nodeId);
//         godot::ClassDB::add_property("InputNode",godot::PropertyInfo(godot::Variant::INT,"nodeId"),"set_nodeId","get_nodeId");
//         godot::ClassDB::bind_method(godot::D_METHOD("get_name"),&InputNode::get_name);
//         godot::ClassDB::bind_method(godot::D_METHOD("set_name","name"),&InputNode::set_name);
//         godot::ClassDB::add_property("InputNode",godot::PropertyInfo(godot::Variant::STRING,"name"),"set_name","get_name");
//         godot::ClassDB::bind_method(godot::D_METHOD("get_nodeInfo"),&InputNode::get_nodeInfo);
//         godot::ClassDB::bind_method(godot::D_METHOD("set_nodeInfo","id"),&InputNode::set_nodeInfo);
//         godot::ClassDB::add_property("InputNode",godot::PropertyInfo(godot::Variant::DICTIONARY,"nodeInfo"),"set_nodeInfo","get_nodeInfo");
//     }
//     int get_nodeId(){
//         return nodeId;
//     }
//     void set_nodeId(int id){}
//     godot::String get_name(){
//         if(name.is_empty())
//             name = get_nodeInfo()["name"];
//         return name;
//     }
//     void set_name(godot::String name){}
//     godot::Dictionary get_nodeInfo();
//     void set_nodeInfo(godot::Dictionary){}
// public:
//     int nodeId;
//     godot::String name;
//     godot::Dictionary nodeInfo;
// };
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
class LoadInputNodeAction: public NodeAction{
    GDCLASS(LoadInputNodeAction,NodeAction)
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
class HoudiniEngineManager: public godot::Node3D{
    GDCLASS(HoudiniEngineManager,godot::Node3D)
    friend class HoudiniEngine;
private:
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
        

        godot::ClassDB::bind_method(godot::D_METHOD("_settings_changed"),&HoudiniEngineManager::_settings_changed);
        godot::ClassDB::bind_method(godot::D_METHOD("get_assets"),&HoudiniEngineManager::get_assets);
        godot::ClassDB::bind_method(godot::D_METHOD("get_nodes"),&HoudiniEngineManager::get_nodes);
        godot::ClassDB::bind_method(godot::D_METHOD("cookSession"),&HoudiniEngineManager::cookSession);
        godot::ClassDB::bind_method(godot::D_METHOD("cookAsset"),&HoudiniEngineManager::cookAsset);
        godot::ClassDB::bind_method(godot::D_METHOD("set_sessionAction","action"),&HoudiniEngineManager::set_sessionAction);
        godot::ClassDB::bind_method(godot::D_METHOD("set_assetAction","action"),&HoudiniEngineManager::set_assetAction);
        godot::ClassDB::bind_method(godot::D_METHOD("set_nodeAction","action"),&HoudiniEngineManager::set_nodeAction);
        godot::ClassDB::bind_method(godot::D_METHOD("set_logFilePath","path"),&HoudiniEngineManager::set_logFilePath);
        godot::ClassDB::bind_method(godot::D_METHOD("set_cookOptions","options"),&HoudiniEngineManager::set_cookOptions);
        godot::ClassDB::bind_method(godot::D_METHOD("get_cookOptions"),&HoudiniEngineManager::get_cookOptions);
        godot::ClassDB::bind_method(godot::D_METHOD("startSession","type","use_cooking_thread"),&HoudiniEngineManager::startSession);
        godot::ClassDB::bind_method(godot::D_METHOD("stopSession"),&HoudiniEngineManager::stopSession);
        godot::ClassDB::bind_method(godot::D_METHOD("initialize","use_cooking_thread"),&HoudiniEngineManager::initialize);
        godot::ClassDB::bind_method(godot::D_METHOD("loadAssets","hdaRes"),static_cast<godot::PackedInt32Array(HoudiniEngineManager::*)(godot::Ref<HDAResource>)>(&HoudiniEngineManager::loadAssets));
        godot::ClassDB::bind_method(godot::D_METHOD("createNode","nodeLabel","operatorName","id","parentId","assetId"),static_cast<bool(HoudiniEngineManager::*)(godot::String,godot::String, godot::Ref<NodeId>, godot::Ref<NodeId>, int)>(&HoudiniEngineManager::createNode));
        godot::ClassDB::bind_method(godot::D_METHOD("connectNode","nodeId","inputIndex","node_to_connect","outputIndex"),static_cast<bool(HoudiniEngineManager::*)(godot::Ref<NodeId>, int, godot::Ref<NodeId>, int)>(&HoudiniEngineManager::connectNode));
        godot::ClassDB::bind_method(godot::D_METHOD("disconnectNode","nodeId","inputIndex"),static_cast<bool(HoudiniEngineManager::*)(godot::Ref<NodeId>, int)>(&HoudiniEngineManager::disconnectNode));
        godot::ClassDB::bind_method(godot::D_METHOD("cookNode","nodeId"),static_cast<bool(HoudiniEngineManager::*)(godot::Ref<NodeId>)>(&HoudiniEngineManager::cookNode));
        godot::ClassDB::bind_method(godot::D_METHOD("deleteNode","nodeId"),static_cast<bool(HoudiniEngineManager::*)(godot::Ref<NodeId>)>(&HoudiniEngineManager::deleteNode));
        godot::ClassDB::bind_method(godot::D_METHOD("createMeshInstance","nodeId"),static_cast<bool(HoudiniEngineManager::*)(godot::Ref<NodeId>)>(&HoudiniEngineManager::createMeshInstance));
        godot::ClassDB::bind_method(godot::D_METHOD("freeGDNode","node"),static_cast<bool(HoudiniEngineManager::*)(godot::Node*)>(&HoudiniEngineManager::freeGDNode));
        godot::ClassDB::bind_method(godot::D_METHOD("stopFreeGDNode","node"),static_cast<bool(HoudiniEngineManager::*)(godot::Node*)>(&HoudiniEngineManager::stopFreeGDNode));
        godot::ClassDB::bind_method(godot::D_METHOD("getParameters","nodeId"),static_cast<godot::Dictionary(HoudiniEngineManager::*)(godot::Ref<NodeId>)>(&HoudiniEngineManager::getParameters));
        //godot::ClassDB::bind_method(godot::D_METHOD("getAttributes","nodeId","partId"),static_cast<godot::Dictionary(HoudiniEngineManager::*)(godot::Ref<NodeId>,godot::Ref<PartId>)>(&HoudiniEngineManager::getAttributes));

        godot::ClassDB::add_signal("HoudiniEngineManager",godot::MethodInfo("materialChanged",godot::PropertyInfo(godot::Variant::INT,"nodeId")));
        godot::ClassDB::add_signal("HoudiniEngineManager",godot::MethodInfo("geometryChanged",godot::PropertyInfo(godot::Variant::INT,"nodeId")));
    }


    void _get_property_list(godot::List<godot::PropertyInfo>* list){
        list->clear();


        list->push_back(godot::PropertyInfo(godot::Variant::NIL,"Asset Settings",godot::PROPERTY_HINT_NONE,"AssetSettings_",godot::PROPERTY_USAGE_GROUP));
        
        list->push_back(godot::PropertyInfo(godot::Variant::OBJECT,"AssetSettings_assetAction",godot::PROPERTY_HINT_RESOURCE_TYPE,"AssetAction"));
        list->push_back(godot::PropertyInfo(godot::Variant::OBJECT,"AssetSettings_nowAsset",godot::PROPERTY_HINT_RESOURCE_TYPE,"HDAResource"));
        list->push_back(godot::PropertyInfo(godot::Variant::ARRAY,"AssetSettings_assets"));


        list->push_back(godot::PropertyInfo(godot::Variant::NIL,"Node Settings",godot::PROPERTY_HINT_NONE,"NodeSettings_",godot::PROPERTY_USAGE_GROUP));
        list->push_back(godot::PropertyInfo(godot::Variant::BOOL,"NodeSettings_showModel"));
        list->push_back(godot::PropertyInfo(godot::Variant::OBJECT,"NodeSettings_nodeAction",godot::PROPERTY_HINT_RESOURCE_TYPE,"NodeAction"));
        list->push_back(godot::PropertyInfo(godot::Variant::OBJECT,"NodeSettings_nowNode",godot::PROPERTY_HINT_RESOURCE_TYPE,"HDANode"));
        list->push_back(godot::PropertyInfo(godot::Variant::OBJECT,"NodeSettings_inputMesh",godot::PROPERTY_HINT_RESOURCE_TYPE,"Mesh"));
        list->push_back(godot::PropertyInfo(godot::Variant::OBJECT,"NodeSettings_inputMeshTreeRoot",godot::PROPERTY_HINT_NODE_TYPE));
        list->push_back(godot::PropertyInfo(godot::Variant::ARRAY,"NodeSettings_nodes"));
        

        list->push_back(godot::PropertyInfo(godot::Variant::OBJECT,"sessionAction",godot::PROPERTY_HINT_RESOURCE_TYPE,"SessionAction"));
        list->push_back(godot::PropertyInfo(godot::Variant::BOOL,"autoCook"));
        
        if(!sessionOpened){
            return;
        }
        list->push_back(godot::PropertyInfo(godot::Variant::NIL,"Parameters",godot::PROPERTY_HINT_NONE,"Parameters_",godot::PROPERTY_USAGE_GROUP));
        for(auto& a : nodeIds){
            int id = a.first;

            auto nodeInfo = getNodeInfo(a.first);
            std::string name = HoudiniEngineUtility::getString(get_session(),nodeInfo.nameSH);
            list->push_back(godot::PropertyInfo(godot::Variant::NIL,(name+' '+std::to_string(id)+" Parameters").c_str(),godot::PROPERTY_HINT_NONE,("Parameters_"+std::to_string(id)+"_Parameters_").c_str(),godot::PROPERTY_USAGE_SUBGROUP));
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

        if(propertyName == "AssetSettings_assetAction"){
            ret = assetAction;
            return true;
        }else if(propertyName == "AssetSettings_nowAsset"){
            ret = nowAsset;
            return true;
        }else if(propertyName == "AssetSettings_assets"){
            ret = get_assets();
            return true;
        }else if(propertyName == "NodeSettings_showModel"){
            ret = showModel;
            return true;
        }else if(propertyName == "NodeSettings_nodeAction"){
            ret = nodeAction;
            return true;
        }else if(propertyName == "NodeSettings_nowNode"){
            ret = nowNode;
            return true;
        }else if(propertyName == "NodeSettings_inputMesh"){
            ret = inputMesh;
            return true;
        }else if(propertyName == "NodeSettings_inputMeshTreeRoot"){
            ret = inputMeshTreeRoot;
            return true;
        }else if(propertyName == "NodeSettings_nodes"){
            ret = get_nodes();
            return true;
        }else if(propertyName == "autoCook"){
            ret = autoCook;
            return true;
        }else if(propertyName == "sessionAction"){
            ret = sessionAction;
            return true;
        }


        if(!sessionOpened){
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
        
        if(propertyName == "AssetSettings_assetAction"){
            set_assetAction((godot::Ref<AssetAction>)(value));
            return true;
        }else if(propertyName == "AssetSettings_nowAsset"){
            nowAsset = (godot::Ref<HDAResource>)value;
            return true;
        }else if(propertyName == "AssetSettings_assets"){
            
            return true;
        }else if(propertyName == "NodeSettings_showModel"){
            showModel = (bool)value;
            if(nowNode.is_valid()){
                updateInternalModel();
            }
            for(auto a : internalModels)
                a.second->set_visible(showModel);
            return true;
        }else if(propertyName == "NodeSettings_nodeAction"){
            set_nodeAction((godot::Ref<NodeAction>)(value));
            return true;
        }else if(propertyName == "NodeSettings_nowNode"){
            nowNode = (godot::Ref<HDANode>)value;
            if(!sessionOpened)
                return false;
            clearInternalModels();
            if(showModel){
                updateInternalModel();
                cookNode(nowNode);
            }
            return true;
        }else if(propertyName == "NodeSettings_inputMesh"){
            auto a = (godot::Ref<godot::Mesh>)(value);
            if(a.is_valid())
                inputMeshTreeRoot = nullptr;
            inputMesh = a;
            return true;
        }else if(propertyName == "NodeSettings_inputMeshTreeRoot"){
            auto a = (godot::Node*)(godot::Object*)(value);
            if(a != nullptr)
                inputMesh.unref();
            inputMeshTreeRoot = a;
            return true;
        }else if(propertyName == "NodeSettings_nodes"){

            return true;
        }else if(propertyName == "autoCook"){
            autoCook = (bool)value;
            cookNode(nowNode);
            return true;
        }else if(propertyName == "sessionAction"){
            set_sessionAction((godot::Ref<SessionAction>)(value));
            return true;
        }




        if(!sessionOpened){
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
                    HoudiniApi::SetParmIntValue(get_session(),id,argName.c_str(),0,(int)value);
                }else if(std::holds_alternative<double>(res[0])){
                    res[0] = (double)value;
                    HoudiniApi::SetParmFloatValue(get_session(),id,argName.c_str(),0,(float)value);
                }else if(std::holds_alternative<std::string>(res[0])){
                    res[0] = (std::string)((godot::String)value).utf8().get_data();
                    HAPI_ParmId parmId;
                    HoudiniApi::GetParmIdFromName(get_session(),id,argName.c_str(),&parmId);
                    HoudiniApi::SetParmStringValue(get_session(),id,((godot::String)value).utf8().get_data(),parmId,0);
                }
            }else{
                res.clear();
                int size = ((godot::Array)value).size();
                for(int i = 0;i!=size;++i){
                    switch(((godot::Array)value)[i].get_type()){
                    case godot::Variant::INT:{
                        res.push_back((int64_t)((godot::Array)value)[i]);
                        HoudiniApi::SetParmIntValue(get_session(),id,argName.c_str(),i,(int)value);
                    }break;
                    case godot::Variant::FLOAT:{
                        res.push_back((double)((godot::Array)value)[i]);
                        HoudiniApi::SetParmFloatValue(get_session(),id,argName.c_str(),i,(float)value);
                    }break;
                    case godot::Variant::STRING:{
                        res.push_back(((godot::String)((godot::Array)value)[i]).utf8().get_data());
                        HAPI_ParmId parmId;
                        HoudiniApi::GetParmIdFromName(get_session(),id,argName.c_str(),&parmId);
                        HoudiniApi::SetParmStringValue(get_session(),id,((godot::String)value).utf8().get_data(),parmId,i);
                    }break;
                    default:{
                        printError("Not supported type!");
                    }break;
                    }
                }
            }
            if(autoCook&&nowNode.is_valid()){
                cookNode(nowNode->nodeId);
            }
            return true;
        }
        return false;
    }
    GDE_EXPORT 
    void _init_hserver(){
        if(!findproc("hserver")){
            printError("Can't find hserver. Try to restart it by hkey.");
            printError("Please restart the hserver to manually.");
            printWarning("Run ",(houdiniRootPath+"/bin/hkey").c_str()," to restart hserver");
            if(houdiniRootPath.empty()){
                printWarning("Run Houdini License Administrator (hkey) to restart hserver");
            }else{
                printWarning("Run ",(houdiniRootPath+"/bin/hkey").c_str()," to restart hserver");
            }
        }
    }
    GDE_EXPORT 
    void _init_settings(){
        
        auto tempDic = godot::Dictionary();
        godot::ProjectSettings* settings = godot::ProjectSettings::get_singleton();
        auto addSetting = [&tempDic,settings](godot::String name,godot::Variant value,godot::Variant::Type type,godot::PropertyHint hint = godot::PROPERTY_HINT_NONE,godot::String hint_string = ""){
            if(!settings->has_setting(name)){
                settings->set_setting(name,value);
            }
            tempDic["name"] = name;
            tempDic["type"] = type;
            tempDic["hint"] = hint;
            tempDic["hint_string"] = hint_string;
            settings->add_property_info(tempDic);
            settings->set_initial_value(name,value);
            tempDic.clear();
        };
        addSetting("houdini/config/useEnvLibPath",true,godot::Variant::BOOL);

        settings->set_restart_if_changed("houdini/config/useEnvLibPath",true);

        addSetting("houdini/config/houdiniRootPath","",godot::Variant::STRING,godot::PROPERTY_HINT_GLOBAL_DIR);

        settings->set_restart_if_changed("houdini/config/houdiniRootPath",true);

        addSetting("houdini/config/houdiniLibPath","",godot::Variant::STRING,godot::PROPERTY_HINT_GLOBAL_DIR);
        
        settings->set_restart_if_changed("houdini/config/houdiniLibPath",true);

        addSetting("houdini/config/logFilePath","",godot::Variant::STRING,godot::PROPERTY_HINT_SAVE_FILE);

        addSetting("houdini/config/cookOptions",default_cookOptions(),godot::Variant::DICTIONARY);

        addSetting("houdini/config/session/sessionType",1,godot::Variant::INT,godot::PROPERTY_HINT_ENUM,"InProcess:1,NewNamedPipe:2,NewTCPSocket:3,ExistingNamedPipe:4,ExistingTCPSocket:5,ExistingSharedMemory:6");

        addSetting("houdini/config/session/useCookingThread",true,godot::Variant::BOOL);

        addSetting("houdini/config/session/namedPipe","hapi",godot::Variant::STRING);

        addSetting("houdini/config/session/sharedMemoryName","hapi",godot::Variant::STRING);

        addSetting("houdini/config/session/hostName","127.0.0.1",godot::Variant::STRING);

        addSetting("houdini/config/session/tcpPort",9090,godot::Variant::INT);

    }
    GDE_EXPORT 
    void _update_settings(){
        using namespace _houdini_engine_log;
        godot::Variant value;
        godot::String tempStr;

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/useEnvLibPath");
        if(useEnvLibPath != (bool)value){
            godot::ProjectSettings::get_singleton()->set_setting("houdini/config/useEnvLibPath",useEnvLibPath);
        }

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/houdiniRootPath");
        tempStr = godot::String::utf8(houdiniRootPath.c_str());
        if(tempStr != value){
            godot::ProjectSettings::get_singleton()->set_setting("houdini/config/houdiniRootPath",tempStr);
        }

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/houdiniLibPath");
        tempStr = godot::String::utf8(houdiniLibPath.c_str());
        if(tempStr != value){
            godot::ProjectSettings::get_singleton()->set_setting("houdini/config/houdiniLibPath",tempStr);
        }

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/logFilePath");
        tempStr = godot::String::utf8(logFilePath.c_str());
        if(tempStr != value){
            set_logFilePath((godot::String)value);
            godot::ProjectSettings::get_singleton()->set_setting("houdini/config/logFilePath",tempStr);
        }

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/cookOptions");
        if(auto options = get_cookOptions();options != (godot::Dictionary)value){
            godot::ProjectSettings::get_singleton()->set_setting("houdini/config/cookOptions",options);
        }

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/session/sessionType");
        if((int)value != (int)sessionType){
            godot::ProjectSettings::get_singleton()->set_setting("houdini/config/session/sessionType",(int)sessionType);
        }

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/session/useCookingThread");
        if((bool)value != useCookingThread){
            godot::ProjectSettings::get_singleton()->set_setting("houdini/config/session/useCookingThread",useCookingThread);
        }

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/session/namedPipe");
        tempStr = godot::String::utf8(namedPipe.c_str());
        if(tempStr != value){
            godot::ProjectSettings::get_singleton()->set_setting("houdini/config/session/namedPipe",tempStr);
        }

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/session/sharedMemoryName");
        tempStr = godot::String::utf8(sharedMemoryName.c_str());
        if(tempStr != value){
            godot::ProjectSettings::get_singleton()->set_setting("houdini/config/session/sharedMemoryName",tempStr);
        }

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/session/hostName");
        tempStr = godot::String::utf8(hostName.c_str());
        if(tempStr != value){
            godot::ProjectSettings::get_singleton()->set_setting("houdini/config/session/hostName",tempStr);
        }

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/session/tcpPort");
        if((int)value != tcpPort){
            godot::ProjectSettings::get_singleton()->set_setting("houdini/config/session/tcpPort",tcpPort);
        }
    }
    GDE_EXPORT 
    void _settings_changed(){
        using namespace _houdini_engine_log;
        godot::Variant value;
        std::string tempStr;

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/useEnvLibPath");
        if(useEnvLibPath != (bool)value){
            useEnvLibPath = (bool)value;
        }

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/houdiniRootPath");
        tempStr = ((godot::String)value).utf8().get_data();
        if(tempStr != houdiniRootPath){
            set_houdiniRootPath((godot::String)value);
        }

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/houdiniLibPath");
        tempStr = ((godot::String)value).utf8().get_data();
        if(tempStr != houdiniRootPath){
            set_houdiniLibPath((godot::String)value);
        }

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/logFilePath");
        tempStr = ((godot::String)value).utf8().get_data();
        if(tempStr != logFilePath){
            set_logFilePath((godot::String)value);
        }

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/cookOptions");
        if((godot::Dictionary)value != get_cookOptions()){
            set_cookOptions((godot::Dictionary)value);
        }

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/session/sessionType");
        if((int)value != (int)sessionType){
            sessionType = (SessionType)(int)value;
        }

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/session/useCookingThread");
        if((bool)value != useCookingThread){
            useCookingThread = (bool)value;
        }

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/session/namedPipe");
        tempStr = ((godot::String)value).utf8().get_data();
        if(tempStr != namedPipe){
            namedPipe = tempStr;
        }

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/session/sharedMemoryName");
        tempStr = ((godot::String)value).utf8().get_data();
        if(tempStr != sharedMemoryName){
            sharedMemoryName = tempStr;
        }

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/session/hostName");
        tempStr = ((godot::String)value).utf8().get_data();
        if(tempStr != hostName){
            hostName = tempStr;
        }

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/session/tcpPort");
        if((int)value != tcpPort){
            tcpPort = (int)value;
        }
    }
    GDE_EXPORT 
    void _notification(int what){
        switch(what){
        case NOTIFICATION_POSTINITIALIZE:{
            _init_settings();
        }break;
        case NOTIFICATION_ENTER_TREE:{
            init();
            set_process(1);
        }break;
        case NOTIFICATION_PROCESS:{
            Contact::process_call();
            process();
        }break;
        case NOTIFICATION_EXIT_TREE:{
            term();
        }break;
        case NOTIFICATION_PREDELETE:{
            predel();
        }break;
        }
    }
    void* libHAPIL = nullptr;
    bool sessionOpened = false;

    //      partId  mesh
    std::map<int,godot::MeshInstance3D*> internalModels;
    int internalNodeId = -1;
    void clearInternalModels(){
        internalNodeId = -1;
        for(auto a : internalModels){
            remove_child(a.second);
        }
        internalModels.clear();
    }
    std::set<godot::Node*> createdGDNodes;
    std::map<godot::Node*, std::shared_ptr<std::jthread>> freeGDNodeTasks;
    std::chrono::milliseconds freeTimeout = defaultFreeTimeout;
    constexpr static std::chrono::milliseconds defaultFreeTimeout = std::chrono::milliseconds(100);
    GDE_EXPORT
    void init(){
        godot::OS::get_singleton()->set_low_processor_usage_mode(true);

        godot::Ref<godot::StandardMaterial3D> defaultMaterial;
        defaultMaterial.instantiate();
        defaultMaterial->set_flag(godot::BaseMaterial3D::Flags::FLAG_ALBEDO_FROM_VERTEX_COLOR,true);
        materialRes[""] = defaultMaterial;

        
        get_tree()->connect("node_removed",godot::Callable(this,"freeGDNode"));
        get_tree()->connect("node_added",godot::Callable(this,"stopFreeGDNode"));
        godot::ProjectSettings::get_singleton()->connect("settings_changed",godot::Callable(this,"_settings_changed"));
        _settings_changed();

        sessionAction.unref();
        assetAction.unref();
        nodeAction.unref();
        nowAsset.unref();
        nowNode.unref();
    }
    GDE_EXPORT
    void process(){
        if(!sessionOpened)
            for(auto a : nodeIds){
                // if(checkMaterialChange(a.first)){
                //     emit_signal("materialChanged",a.first);
                // }
                if(checkGeometryChange(a.first)){
                    emit_signal("geometryChanged",a.first);
                }
            }
    }
    GDE_EXPORT
    void term(){
        get_tree()->disconnect("node_removed",godot::Callable(this,"freeGDNode"));
        get_tree()->disconnect("node_added",godot::Callable(this,"stopFreeGDNode"));
        godot::ProjectSettings::get_singleton()->disconnect("settings_changed",godot::Callable(this,"_settings_changed"));
    }
    GDE_EXPORT
    void predel(){
        if(sessionOpened){
            if(!stopSession()){
                printError("Failed to stop session.\n");
            }
        }
        using namespace std::chrono_literals;
        for(auto it : internalModels){
            it.second->get_mesh().unref();
        }
        freeTimeout = 0ms;
        clearInternalModels();
        std::this_thread::sleep_for(defaultFreeTimeout<1s?defaultFreeTimeout:1s);
        // if(libHAPIL != nullptr){
        //     HoudiniApi::FinalizeHAPI();
        //     HoudiniEnginePlatform::FreeLibHAPIL(libHAPIL);
        // }
    }
    godot::Ref<HDAResource> nowAsset;
    GDE_EXPORT
    godot::Array get_assets(){
        godot::Array arr;
        for(auto& a : assetIds){
            arr.push_back(a.second);
        }
        return arr;
    }
    godot::Ref<HDANode> nowNode;
    GDE_EXPORT
    godot::Array get_nodes(){
        godot::Array arr;
        for(auto& a : nodeIds){
            godot::Ref<HDANode> node;
            node.instantiate();
            node->nodeId = a.first;
            node->session = this;
            arr.push_back(node);
        }
        return arr;
    }
    GDE_EXPORT
    void cookSession(){
        for(auto a : nodeIds){
            cookNode(a.first);
        }
    }
    GDE_EXPORT
    void cookAsset(){
        for(auto a : nodeIds){
            if(a.second == nowAsset->assetId){
                cookNode(a.first);
            }
        }
    }
    godot::Ref<SessionAction> sessionAction;
    GDE_EXPORT
    void set_sessionAction(godot::Ref<SessionAction> action){
        if(action.is_null())
            return;
        const auto& type = typeid(*(action.ptr()));
        if(type == typeid(StartSessionAction)){
            this->sessionAction = action;
            std::jthread([this]{
                startSession(sessionType,useCookingThread);
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
    GDE_EXPORT
    void set_assetAction(godot::Ref<AssetAction> action){
        if(action.is_null()||nowAsset.is_null())
            return;
        const auto& type = typeid(*(action.ptr()));
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
                printLog(" ");
                loadAssets(nowAsset,Void{});
                printLog(" ");
                this->assetAction.unref();
                Contact::add_call([this]{
                    printLog(" ");
                    notify_property_list_changed();
                });
            }).detach();
        }else{
            return;
        }
    }
    godot::Ref<NodeAction> nodeAction;
    GDE_EXPORT
    void set_nodeAction(godot::Ref<NodeAction> action){
        if(action.is_null())
            return;
        const auto& type = typeid(*(action.ptr()));
        if(type == typeid(LoadInputNodeAction)){
            if(inputMesh.is_null()&&inputMeshTreeRoot == nullptr)
                return;
            this->nodeAction = action;
            std::jthread([this]{
                int id = -1;
                if(inputMesh.is_valid()){
                    createInputNode("Mesh",id,-1,inputMesh);
                }else if(inputMeshTreeRoot != nullptr){
                    std::function<void(godot::Node*)> func = [&,this](godot::Node* root){
                        if(auto a = root;a->get_class() == godot::MeshInstance3D::get_class_static()){
                            auto mesh = static_cast<godot::MeshInstance3D*>(a)->get_mesh();
                            createInputNode(a->get_name().c_escape().utf8().get_data(),id,-1,mesh);
                        }else if(a->get_class() == godot::MultiMeshInstance3D::get_class_static()){
                            auto mesh = static_cast<godot::MultiMeshInstance3D*>(a)->get_multimesh()->get_mesh();
                            createInputNode(a->get_name().c_escape().utf8().get_data(),id,-1,mesh);
                        }

                        godot::TypedArray<godot::Node> children;
                        auto id = Contact::add_call([&,this]{
                            children = root->get_children();
                        });
                        while(Contact::find_if(id)){
                            using namespace std::chrono_literals;
                            std::this_thread::sleep_for(1ms);
                        }
                        for(int i = 0,sz = children.size();i!=sz;++i){
                            func(static_cast<godot::Node*>((godot::Object*)children[i]));
                        }
                    };
                    func(inputMeshTreeRoot);
                }

                this->nodeAction.unref();
                Contact::add_call([this]{
                    notify_property_list_changed();
                });
            }).detach();
        }
        else if(type == typeid(CookNodeAction)){
            if(nowNode.is_null())
                return;
            this->nodeAction = action;
            std::jthread([this]{
                if(nowNode.is_null())
                    return;
                cookNode(nowNode->nodeId);

                this->nodeAction.unref();
                Contact::add_call([this]{
                    notify_property_list_changed();
                });
            }).detach();
        }
        else if(type == typeid(DeleteNodeAction)){
            if(nowNode.is_null())
                return;
            this->nodeAction = action;
            std::jthread([this]{
                if(nowNode.is_null())
                    return;
                deleteNode(nowNode->nodeId);
                this->nowNode.unref();
                this->nodeAction.unref();
                Contact::add_call([this]{
                    notify_property_list_changed();
                });
            }).detach();
        }
        else if(type == typeid(CreateMeshAction)){
            if(nowNode.is_null())
                return;
            this->nodeAction = action;
            std::jthread([this]{
                if(nowNode.is_null())
                    return;
                    createMeshInstance();
                this->nodeAction.unref();
                Contact::add_call([this]{
                    notify_property_list_changed();
                });
            }).detach();
        }
    }

    bool autoCook = 0;
    bool showModel = 0;
    godot::Ref<godot::Mesh> inputMesh;
    godot::Node* inputMeshTreeRoot = nullptr;

    bool useEnvLibPath = true;

    std::string houdiniRootPath = "";
    void set_houdiniRootPath(godot::String path){
        path = godot::ProjectSettings::get_singleton()->globalize_path(path);
        if(path == ""){
            houdiniRootPath.clear();
            return;
        }
        std::string houdiniPath = path.utf8().get_data();
        std::string hconfigPath = houdiniPath+"/bin/hconfig";
        if(!std::filesystem::exists(hconfigPath)){
            hconfigPath += ".exe";
            if(!std::filesystem::exists(hconfigPath)){
                printError("Invalid houdini root path. Make sure \"${HoudiniRootPath}/bin/hconfig\" is exist.");
                return;
            }
        }
        if(findenv("HFS"))
            return;
        // Add Environment
        std::string output = exec_output(hconfigPath.c_str());
        if(output.empty()){
            printError("Houdini environment is null");
            return;
        }
        std::istringstream iss(output);
        std::string envLine,envKey,envValue,temp;
        while(std::getline(iss,envLine)){
            if(!iss)
                break;
            std::istringstream iss2(envLine);
            iss2 >> envKey >> temp >> envValue;
            if(envKey.empty())
                continue;
            envValue.erase(0,1);
            envValue.erase(envValue.size()-1,1);
            addenv(envKey,envValue);
        }
        houdiniRootPath = houdiniPath;
        if(useEnvLibPath)
            initHoudini();
    }

    std::string houdiniLibPath = "";

    void set_houdiniLibPath(godot::String path){
        path = godot::ProjectSettings::get_singleton()->globalize_path(path);
        if(path == ""){
            houdiniLibPath.clear();
            return;
        }
        std::string houdiniPath = path.utf8().get_data();
        if(!std::filesystem::exists(houdiniPath)){
            printError("Invalid houdini lib path. Make sure the dir exists.");
            return;
        }
        houdiniLibPath = houdiniPath;
        if(!useEnvLibPath)
            initHoudini();
    }
    void initHoudini(){
        if(putenv((char*)"HAPI_CLIENT_NAME=godot")){
            printWarning("Failed to change env \"HAPI_CLIENT_NAME\" to \"godot\".\n");
        }
        if(useEnvLibPath)
            libHAPIL = HoudiniEnginePlatform::LoadLibHAPIL();
        else 
            libHAPIL = HoudiniEnginePlatform::LoadLibHAPIL(houdiniLibPath);
        if(libHAPIL != nullptr){
            HoudiniApi::InitializeHAPI(libHAPIL);
        }else{
            printError("Failed to initialize hapi");
        }
        if(!HoudiniApi::IsHAPIInitialized()){
            printError("Failed to load and initialize the "
                        "Houdini Engine API from libHAPIL.\n");
        }
    }

    GDE_EXPORT
    void set_logFilePath(godot::String path){
        using namespace _houdini_engine_log;
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

    std::string namedPipe = DefaultNamedPipe;

    std::string hostName = DefaultHostName;

    std::string sharedMemoryName = DefaultSharedMemoryName;

    int tcpPort = DefaultTcpPort;


    HAPI_CookOptions cookOptions;
    GDE_EXPORT
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
    GDE_EXPORT
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
    GDE_EXPORT
    godot::Dictionary default_cookOptions(){
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
        this->cookOptions = cookOptions;
        return get_cookOptions();
    }
    enum class AttribOwner{
        Invalid = HAPI_ATTROWNER_INVALID,
        Point = HAPI_ATTROWNER_POINT,
        Vertex = HAPI_ATTROWNER_VERTEX,
        Prim = HAPI_ATTROWNER_PRIM,
        Detail = HAPI_ATTROWNER_DETAIL
    };
    enum class PartType{
        Invalid = HAPI_PARTTYPE_INVALID,
        Mesh = HAPI_PARTTYPE_MESH,
        Curve = HAPI_PARTTYPE_CURVE,
        Volume = HAPI_PARTTYPE_VOLUME,
        Instancer = HAPI_PARTTYPE_INSTANCER,
        Box = HAPI_PARTTYPE_BOX,
        Sphere = HAPI_PARTTYPE_SPHERE,
    };
    //      NodeId,AssetId
    std::map<int,int> nodeIds;
    //      AssetId,Res
    std::map<int,godot::Ref<HDAResource>> assetIds;
    //      NodeId          ParamName            ParamValues
    std::map<int,std::map<std::string,std::vector<std::variant<int64_t,double,std::string>>>> parameters;
    //      nodeId      partId  type
    std::map<int,std::map<int,PartType>> partType;
    //      nodeId    meshPartId       Geo_Attrib    faces            P            vertexs                          Cd                                      N                                           uv                                  uv2
    std::map<int,std::map<int,std::tuple<std::vector<int>,std::vector<float>,std::vector<int>,std::pair<AttribOwner,std::vector<float>>,std::pair<AttribOwner,std::vector<float>>,std::pair<AttribOwner,std::vector<float>>,std::pair<AttribOwner,std::vector<float>>>>> geometries;
    //      nodeId  instancerPartId     transform
    std::map<int,std::map<int,std::vector<HAPI_Transform>>> instanceTransforms;
    //      nodeId      partId      sharedMesh
    std::map<int,std::map<int,godot::Ref<godot::ArrayMesh>>> meshRef;
    //      nodeId       partId                 Point-Attrib                Vertex-Attrib               Prim-Attrib                     Detail-Attrib    
    std::map<int,std::map<int,std::tuple<std::vector<HAPI_AttributeInfo>,std::vector<HAPI_AttributeInfo>,std::vector<HAPI_AttributeInfo>,std::vector<HAPI_AttributeInfo>>>> attributes;
    //      nodeId      partId      materialResPath
    std::map<int,std::map<int,std::vector<std::string>>> materials;
    //      nodeId      partId       if-allSame     material-nodeIds
    std::map<int,std::map<int,std::pair<bool,std::vector<int>>>> materialIds;

    // made by user
    //      godot path          Material instance
    std::map<std::string,godot::Ref<godot::Material>> materialRes;


public:
    HAPI_Session* get_session(){
        return get_session();
    }
    GDE_EXPORT
    bool startSession(SessionType type,bool use_cooking_thread){
        using namespace _houdini_engine_log;
        if(sessionOpened){
            printFile("Now session is valid.\n");
            return true;
        }


        _init_hserver();

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
                get_session(),&sessionInfo
            );
            if(SessionResult == HAPI_RESULT_SUCCESS){
                printFile("Successful create a HAPI in-process session\n");
            }else{
                printError("Error create session: ",SessionResult);
            }
        }break;
        case SessionType::NewNamedPipe:{
            HAPI_ProcessId processID;
            HOUDINI_CHECK_ERROR(HoudiniApi::StartThriftNamedPipeServer(
                &server_options,namedPipe.c_str(),&processID,logFilePath.empty()?nullptr:logFilePath.c_str()
            ));
            HAPI_SessionInfo sessionInfo = HoudiniApi::SessionInfo_Create();
            SessionResult = HoudiniApi::CreateThriftNamedPipeSession(
                get_session(),namedPipe.c_str(),&sessionInfo
            );
            if(SessionResult == HAPI_RESULT_SUCCESS){
                printFile("Successful create a HAPI named-pipe session\n");
            }else{
                printError("Error create session: ",SessionResult);
            }
        }break;
        case SessionType::NewTCPSocket:{
            HAPI_ProcessId processID;
            HOUDINI_CHECK_ERROR(HoudiniApi::StartThriftSocketServer(
                &server_options,tcpPort,&processID,logFilePath.empty()?nullptr:logFilePath.c_str()
            ));
            HAPI_SessionInfo sessionInfo = HoudiniApi::SessionInfo_Create();
            SessionResult = HoudiniApi::CreateThriftSocketSession(
                get_session(),hostName.c_str(), tcpPort, &sessionInfo
            );
            if(SessionResult == HAPI_RESULT_SUCCESS){
                printFile("Successful create a HAPI TCP socket session\n");
            }else{
                printError("Error create session: ",SessionResult);
            }
        }break;
        case SessionType::ExistingNamedPipe:{
            HAPI_SessionInfo sessionInfo = HoudiniApi::SessionInfo_Create();
            SessionResult = HoudiniApi::CreateThriftNamedPipeSession(
                get_session(),namedPipe.c_str(),&sessionInfo
            );
            if(SessionResult == HAPI_RESULT_SUCCESS){
                printFile("Successful connect to an existint HAPI named-pipe session\n");
            }else{
                printError("Error create session: ",SessionResult);
            }
        }break;
        case SessionType::ExistingTCPSocket:{
            HAPI_SessionInfo sessionInfo = HoudiniApi::SessionInfo_Create();
            SessionResult = HoudiniApi::CreateThriftSocketSession(
                get_session(),hostName.c_str(), tcpPort, &sessionInfo
            );
            if(SessionResult == HAPI_RESULT_SUCCESS){
                printFile("Successful connect to an existint HAPI TCP socket session\n");
            }else{
                printError("Error create session: ",SessionResult);
            }
        }break;
        case SessionType::ExistingSharedMemory:{
            HAPI_SessionInfo sessionInfo = HoudiniApi::SessionInfo_Create();
            SessionResult = HoudiniApi::CreateThriftSharedMemorySession(
                get_session(),sharedMemoryName.c_str(), &sessionInfo
            );
            if(SessionResult == HAPI_RESULT_SUCCESS){
                printFile("Successful connect to an existint HAPI shared memory session\n");
            }else{
                printError("Error create session: ",SessionResult);
            }
        }break;
        default:{
            printError("Cannot connect to unknown session type (",sessionType,")");
            return false;
        }break;
        }

        if(SessionResult != HAPI_RESULT_SUCCESS){
            if (sessionType != SessionType::InProcess)
            {
                std::string connectionError = HoudiniEngineUtility::getConnectionError();
                if (!connectionError.empty())
                    printError("Houdini Engine Session failed to connect - ",connectionError.c_str());
            }
            printError("Houdini Engine Session failed to start");
            return false;
        }
        sessionOpened = true;
        if(!initialize(use_cooking_thread)){
            printError("Failed to start the Houdini Engine session - Failed to initialize HAPI");
            return false;
        }

        return true;
    }
    GDE_EXPORT
    bool stopSession(){
        if(sessionOpened){
            if(HoudiniApi::Cleanup(get_session()) != HAPI_RESULT_SUCCESS){
                printError("Failed to stop the Houdini Engine session - Clean up failed.");
                sessionOpened = false;
                return false;
            }
            if(HoudiniApi::CloseSession(get_session()) != HAPI_RESULT_SUCCESS){
                printError("Failed to stop the Houdini Engine session - Close session failed.");
                sessionOpened = false;
                return false;
            }
        }else{
            printError("Failed to stop the Houdini Engine session - Session is invalid.");
            return false;
        }
        sessionOpened = false;
        sessionAction.unref();
        assetAction.unref();
        nodeAction.unref();
        nowAsset.unref();
        nowNode.unref();
        nodeIds.clear();
        assetIds.clear();
        parameters.clear();
        partType.clear();
        geometries.clear();
        meshRef.clear();
        materials.clear();
        materialIds.clear();
        attributes.clear();
        Contact::add_call([this]{
            clearInternalModels();
        });
        internalNodeId = -1;
        return true;
    }
    GDE_EXPORT
    bool initialize(bool use_cooking_thread){
        if(!sessionOpened){
            printError("Failed to initialize HAPI: The session is invalid.");
            return false;
        }

        if(HoudiniApi::IsInitialized(get_session()) == HAPI_RESULT_NOT_INITIALIZED){

            HAPI_Result Result = HoudiniApi::Initialize(
                get_session(),&cookOptions,use_cooking_thread,-1,"",nullptr,nullptr,nullptr,nullptr
            );
            _update_settings();
            if(Result == HAPI_RESULT_SUCCESS){
                printFile("Successfully initialized Houdini Engine.");
            }else if(Result == HAPI_RESULT_ALREADY_INITIALIZED){
                printFile("Successfully initialized Houdini Engine - HAPI was already initialized.");
            }else{
                printError("Houdini Engine API initialization failed: ",Result);
                return false;
            }
        }
        return true;
    }
    GDE_EXPORT
    std::vector<int> loadAssets(godot::Ref<HDAResource> hdaRes,Void){
        printLog(" ");
        if(!sessionOpened){
            printError("Error load Asset with invalid session");
            return {};
        }
        printLog(" ");
        int assetId = -1;
        try{

        if(auto a = HoudiniApi::LoadAssetLibraryFromFile(get_session(),hdaRes->path.c_str(),true,&assetId);a != HAPI_RESULT_SUCCESS){
            printError("Error load Asset from file: ");
            return {};
        }

        }catch(std::exception& e){
        }
        int asset_count = 0;
        if(auto a = HoudiniApi::GetAvailableAssetCount(get_session(),assetId,&asset_count); a != HAPI_RESULT_SUCCESS){
            printError("Error get available asset count: ",a);
            return {};
        }
        std::vector<HAPI_StringHandle> assetSH;
        assetSH.resize(asset_count);
        if(auto a = HoudiniApi::GetAvailableAssets(get_session(),assetId,assetSH.data(),asset_count);a != HAPI_RESULT_SUCCESS){
            printError("Error get available assets: ",a);
            return {};
        }

        printLog(" ");
        std::string temp;
        int rootId = -1;
        std::vector<int> result;
        result.reserve(asset_count);
        for(int i = 0;i!=asset_count;++i){
            temp = HoudiniEngineUtility::getString(get_session(),assetSH[i]);
            int id = -1;
            createNode(temp,temp,id,rootId,assetId);
            result.push_back(id);
        }
        hdaRes->assetId = assetId;
        assetIds.insert({assetId,hdaRes});
        printLog(" ");
        return result;
    }
    GDE_EXPORT
    godot::PackedInt32Array loadAssets(godot::Ref<HDAResource> hdaRes){
        auto arr = loadAssets(hdaRes,Void{});
        godot::PackedInt32Array res;
        for(auto a : arr){
            res.push_back(a);
        }
        return res;
    }
    GDE_EXPORT
    bool createNode(std::string nodeLabel, std::string operatorName, int& id, int parentId, int assetId){
        if(!sessionOpened){
            printError("Failed to create node: The session is invalid.");
            return false;
        }
        if(HoudiniApi::CreateNode(get_session(),parentId,operatorName.c_str(),nodeLabel.c_str(),false,&id) != HAPI_RESULT_SUCCESS){
            printError("Error create node: ",HoudiniEngineUtility::getLastError().c_str());
            return false;
        }else{
            printFile("Success create node, ID: ",id);
        }
        nodeIds.insert({id,assetId});
        return true;
    }
    GDE_EXPORT
    bool createNode(godot::String nodeLabel,godot::String operatorName, godot::Ref<NodeId> id, godot::Ref<NodeId> parentId, int assetId){
        return createNode(std::string(nodeLabel.utf8().get_data()),std::string(operatorName.utf8().get_data()),*id->id.get(),(int)**parentId,assetId);
    }
    GDE_EXPORT
    bool connectNode(int nodeId, int inputIndex,int node_to_connect,int outputIndex){
        if(!sessionOpened){
            printError("Failed to connect node: The session is invalid.");
            return false;
        }
        if(HoudiniApi::ConnectNodeInput(get_session(),nodeId,inputIndex,node_to_connect,outputIndex) != HAPI_RESULT_SUCCESS){
            printError("Error connect node: ",HoudiniEngineUtility::getLastError().c_str());
            return false;
        }else{
            printFile("Success connect node ",nodeId," with ",node_to_connect);
        }
        return true;
    }
    GDE_EXPORT
    bool connectNode(godot::Ref<NodeId> nodeId,int inputIndex,godot::Ref<NodeId> node_to_connect,int outputIndex){
        return connectNode(*nodeId,inputIndex,*node_to_connect,outputIndex);
    }
    GDE_EXPORT
    bool disconnectNode(int nodeId, int inputIndex){
        if(!sessionOpened){
            printError("Failed to disconnect node: The session is invalid.");
            return false;
        }
        if(HoudiniApi::DisconnectNodeInput(get_session(),nodeId,inputIndex) != HAPI_RESULT_SUCCESS){
            printError("Error disconnect node: ",HoudiniEngineUtility::getLastError().c_str());
            return false;
        }else{
            printFile("Success dicconnect node ",nodeId,"'s port ",inputIndex);
        }
        return true;
    }
    GDE_EXPORT
    bool disconnectNode(godot::Ref<NodeId> nodeId,int inputIndex){
        return disconnectNode(*nodeId,inputIndex);
    }
    GDE_EXPORT
    bool cookNode(int id){
        if(!sessionOpened){
            printError("Failed to cook node: The session is invalid.");
            return false;
        }
        if(id == -1)
            return false;
        if(nodeIds.find(id) == nodeIds.end())
            return false;
        if(HoudiniApi::CookNode(get_session(),id,&cookOptions) != HAPI_RESULT_SUCCESS){
            printError("Failed to cook node",HoudiniEngineUtility::getLastCookError().c_str());
            return false;
        }
        waitForCook();
        if(showModel && nowNode.is_valid() && id == nowNode->nodeId)
            updateInternalModel();
            
        getParameters(id);
        return true;
    }
    GDE_EXPORT
    bool cookNode(godot::Ref<NodeId> id){
        if(id.is_null())
            return false;
        return cookNode((int)**id);
    }
    GDE_EXPORT
    bool deleteNode(int id){
        if(!sessionOpened){
            printError("Failed to cook node: The session is invalid.");
            return false;
        }
        if(auto a = HoudiniApi::DeleteNode(get_session(),id);a != HAPI_RESULT_SUCCESS){
            printError("Failed to delete node: ",a," - ",HoudiniEngineUtility::getLastError().c_str());
            return false;
        }
        Contact::add_call([=,this]{
            if(internalNodeId == id){
                clearInternalModels();
            }
        });
        if(nowNode.is_valid()&&nowNode->nodeId == id){
            nowNode.unref();
        }
        nodeIds.erase(id);
        parameters.erase(id);
        partType.erase(id);
        geometries.erase(id);
        instanceTransforms.erase(id);
        meshRef.erase(id);
        materials.erase(id);
        materialIds.erase(id);
        attributes.erase(id);

        return true;
    }
    GDE_EXPORT
    bool deleteNode(godot::Ref<NodeId> id){
        return deleteNode((int)**id);
    }
    GDE_EXPORT
    bool waitForCook(){
        if(!sessionOpened){
            printError("Failed to cook node: The session is invalid.");
            return false;
        }
        int status;
        HAPI_Result result;
        do{
            result = HoudiniApi::GetStatus(get_session(),HAPI_STATUS_COOK_STATE,&status);
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }while(status > HAPI_STATE_MAX_READY_STATE && result == HAPI_RESULT_SUCCESS);
        if(status != HAPI_STATE_READY || result != HAPI_RESULT_SUCCESS){
            printError("Cook failure: ",HoudiniEngineUtility::getLastCookError().c_str());
            return false;
        }
        return true;
    }
    GDE_EXPORT
    bool updateInternalModel(){
        if(nowNode.is_null())
            return false;
        int nodeId = nowNode->nodeId;
        getParameters(nodeId);
        getGeometry(nodeId);
        getMaterial(nodeId);
        if(geometries.find(nodeId) == geometries.end()||geometries[nodeId].empty()){
            return false;
        }
         
        for(auto part : geometries[nodeId]){
            std::vector<int>& faces = std::get<0>(part.second);
            std::vector<float>& positions = std::get<1>(part.second);
            std::vector<int>& vertexs = std::get<2>(part.second);
            std::pair<AttribOwner,std::vector<float>>& color_Attrib = std::get<3>(part.second);
            std::pair<AttribOwner,std::vector<float>>& normal_Attrib = std::get<4>(part.second);
            std::pair<AttribOwner,std::vector<float>>& uv_Attrib = std::get<5>(part.second);
            std::pair<AttribOwner,std::vector<float>>& uv2_Attrib = std::get<6>(part.second);
            std::vector<float>& colors = color_Attrib.second;
            std::vector<float>& normals = normal_Attrib.second;
            std::vector<float>& uvs = uv_Attrib.second;
            std::vector<float>& uv2s = uv2_Attrib.second;
            static std::vector<std::string> voidMaterialPaths;
            std::vector<std::string>& materialPaths = voidMaterialPaths;
            if(materials.find(nodeId)!=materials.end()&&materials[nodeId].find(part.first)!=materials[nodeId].end()){
                materialPaths = materials[nodeId][part.first];
            }
         
            std::vector<godot::Vector3> pos;
            std::vector<godot::Color> cols;
            std::vector<godot::Vector3> nors;
            std::vector<godot::Vector2> uv_s;
            std::vector<godot::Vector2> uv2_s;
            int pos_num = positions.size();
            int vertex_num = vertexs.size();
            int col_num = colors.size();
            int nor_num = normals.size();
            int uv_num = uvs.size();
            int uv2_num = uv2s.size();
            int mat_num = materialPaths.size();
            std::string lastMatPath;
            if(!materialPaths.empty())
                lastMatPath = materialPaths[0];
            for(int i = 0;i!=pos_num;i+=3){
                godot::Vector3 vec;
                vec.x = positions[i];
                vec.y = positions[i+1];
                vec.z = positions[i+2];
                pos.push_back(vec);
            }
            for(int i = 0;i!=col_num;i+=3){
                godot::Color color;
                color.r = colors[i];
                color.g = colors[i+1];
                color.b = colors[i+2];
                cols.push_back(color);
            }
            for(int i = 0;i!=nor_num;i+=3){
                godot::Vector3 nor;
                nor.x = normals[i];
                nor.y = normals[i+1];
                nor.z = normals[i+2];
                nors.push_back(nor);
            }
            for(int i = 0;i!=uv_num;i+=3){
                godot::Vector2 uv;
                uv.x = uvs[i];
                uv.y = uvs[i+1];
                uv_s.push_back(uv);
            }
            for(int i = 0;i!=uv2_num;i+=3){
                godot::Vector2 uv2;
                uv2.x = uv2s[i];
                uv2.y = uv2s[i+1];
                uv2_s.push_back(uv2);
            }


            godot::Ref<godot::ArrayMesh> arr_mesh;
            arr_mesh.instantiate();
            
            godot::SurfaceTool* st = memnew(godot::SurfaceTool());
         
            st->begin(godot::Mesh::PRIMITIVE_TRIANGLES);
            st->set_material(materialRes[lastMatPath]);

            for(int i = 0;i!=vertex_num;++i){
                if(vertexs[i] >= pos_num)
                    break;
                if(i/3 < mat_num){
                    std::string newPath = materialPaths[i/3];
                    if(newPath != lastMatPath){
                        st->commit(arr_mesh);
                        st->clear();
                        st->begin(godot::Mesh::PRIMITIVE_TRIANGLES);
                        st->set_material(materialRes[newPath]);
                        lastMatPath = newPath;
                    }
                }else{
                    if(!lastMatPath.empty()){
                        st->commit(arr_mesh);
                        st->clear();
                        st->begin(godot::Mesh::PRIMITIVE_TRIANGLES);
                        st->set_material(materialRes[""]);
                        lastMatPath.clear();
                    }
                }
                if(color_Attrib.first == AttribOwner::Vertex){
                    if(i < col_num){
                        st->set_color(cols[i]);
                    }
                }else if(color_Attrib.first == AttribOwner::Point){
                    if(vertexs[i] < col_num){
                        st->set_color(cols[vertexs[i]]);
                    }
                }
                if(normal_Attrib.first == AttribOwner::Vertex){
                    if(i < nor_num){
                        st->set_normal(nors[i]);
                    }
                }else if(normal_Attrib.first == AttribOwner::Point){
                    if(vertexs[i] < nor_num){
                        st->set_normal(nors[vertexs[i]]);
                    }
                }
                if(uv_Attrib.first == AttribOwner::Vertex){
                    if(i < uv_num){
                        st->set_uv(uv_s[i]);
                    }
                }else if(uv_Attrib.first == AttribOwner::Point){
                    if(vertexs[i] < uv_num){
                        st->set_uv(uv_s[vertexs[i]]);
                    }
                }
                if(uv2_Attrib.first == AttribOwner::Vertex){
                    if(i < uv2_num){
                        st->set_uv2(uv2_s[i]);
                    }
                }else if(uv2_Attrib.first == AttribOwner::Point){
                    if(vertexs[i] < uv2_num){
                        st->set_uv2(uv2_s[vertexs[i]]);
                    }
                }
                st->add_vertex(pos[vertexs[i]]);
            }
            st->commit(arr_mesh);
            auto& ref = meshRef[nodeId][part.first];
            if(ref.is_null())
                ref.instantiate();
            ref->clear_surfaces();
            
            for(auto i = 0,count = arr_mesh->get_surface_count();i!=count;++i){
                ref->add_surface_from_arrays(godot::Mesh::PRIMITIVE_TRIANGLES,arr_mesh->surface_get_arrays(i));
                ref->surface_set_material(i,arr_mesh->surface_get_material(i));
            }
            Contact::add_call([=,this]{
                if(internalModels.find(part.first)==internalModels.end()){
                    internalModels[part.first] = memnew(godot::MeshInstance3D());
                    createdGDNodes.insert(internalModels[part.first]);
                    add_child(internalModels[part.first],false,godot::Node::INTERNAL_MODE_FRONT);
                }
                internalModels[part.first]->set_mesh(ref);
            });
            godot::memdelete(st);
        }
        internalNodeId = nodeId;
        return true;
    }
    GDE_EXPORT
    void createMeshInstance(){
        if(!showModel)
            updateInternalModel();
        Contact::add_call([=,this]{


        if(instanceTransforms.find(internalNodeId) == instanceTransforms.end()||instanceTransforms[internalNodeId].empty()){
            godot::Node3D* father = this;
            if(internalModels.size() == 0){
                return;
            }else if(internalModels.size() == 1){
                ;
            }else{
                godot::Node3D* group = memnew(godot::Node3D());
                group->set_name("group");
                add_child(group,true);
                group->set_owner(get_tree()->get_edited_scene_root());
                group->set_visible(true);
                createdGDNodes.insert(group);
                father = group;
            }
            for(auto a : internalModels){
                auto internelModel = a.second;

                godot::MeshInstance3D* instance = (godot::MeshInstance3D*)internelModel->duplicate();
                instance->set_mesh(internelModel->get_mesh()->duplicate());
                father->add_child(instance,true);
                instance->set_owner(get_tree()->get_edited_scene_root());
                instance->set_visible(true);
                createdGDNodes.insert(instance);
                internelModel->set_visible(showModel);
            }
        }else{
            
            for(auto& a : instanceTransforms[internalNodeId]){
                godot::Node3D* father = this;
                if(auto size = internalModels.size();size == 0){
                    break;
                }else if(size == 1){
                    ;
                }else{
                    godot::Node3D* group = memnew(godot::Node3D());
                    group->set_name("Group");
                    father->add_child(group);
                    group->set_owner(get_tree()->get_edited_scene_root());
                    group->set_visible(true);
                    createdGDNodes.insert(group);
                    father = group;
                }

                std::vector<godot::Transform3D> transforms;
                godot::Node3D* transformer = memnew(godot::Node3D());
                for(auto& b : a.second){
                    transformer->set_position(godot::Vector3(b.position[0],b.position[1],b.position[2]));
                    transformer->set_rotation(godot::Vector3(b.rotationQuaternion[0],b.rotationQuaternion[1],b.rotationQuaternion[2]));
                    transformer->set_scale(godot::Vector3(b.scale[0],b.scale[1],b.scale[2]));
                    transforms.push_back(transformer->get_transform());
                }
                godot::memdelete(transformer);
                for(auto internalModel : internalModels){
                    int size = transforms.size();
                    godot::Ref<godot::MultiMesh> multiMesh;
                    multiMesh.instantiate();
                    multiMesh->set_use_colors(true);
                    multiMesh->set_transform_format(godot::MultiMesh::TRANSFORM_3D);
                    multiMesh->set_instance_count(size);
                    multiMesh->set_mesh(internalModel.second->get_mesh());
                    for(int i = 0;i!=size;++i)
                        multiMesh->set_instance_transform(i,transforms[i]);

                    
                    godot::MultiMeshInstance3D* node = memnew(godot::MultiMeshInstance3D());
                    node->set_multimesh(multiMesh);
                    father->add_child(node);
                    node->set_owner(get_tree()->get_edited_scene_root());
                    node->set_visible(true);
                    createdGDNodes.insert(node);
                }
            }
        }
            
            
        });
    }
    GDE_EXPORT
    bool createMeshInstance(int nodeId){
         
        
        getParameters(nodeId);
        getGeometry(nodeId);
        getMaterial(nodeId);
        if(geometries[nodeId].empty()){
            return false;
        }
         
        for(auto part : geometries[nodeId]){
            std::vector<int>& faces = std::get<0>(part.second);
            std::vector<float>& positions = std::get<1>(part.second);
            std::vector<int>& vertexs = std::get<2>(part.second);
            std::pair<AttribOwner,std::vector<float>>& color_Attrib = std::get<3>(part.second);
            std::pair<AttribOwner,std::vector<float>>& normal_Attrib = std::get<4>(part.second);
            std::pair<AttribOwner,std::vector<float>>& uv_Attrib = std::get<5>(part.second);
            std::pair<AttribOwner,std::vector<float>>& uv2_Attrib = std::get<6>(part.second);
            std::vector<float>& colors = color_Attrib.second;
            std::vector<float>& normals = normal_Attrib.second;
            std::vector<float>& uvs = uv_Attrib.second;
            std::vector<float>& uv2s = uv2_Attrib.second;
            std::vector<std::string> voidMaterialPaths;
            std::vector<std::string>& materialPaths = voidMaterialPaths;
            if(materials.find(nodeId)!=materials.end()&&materials[nodeId].find(part.first)!=materials[nodeId].end()){
                materialPaths = materials[nodeId][part.first];
            }
         
            std::vector<godot::Vector3> pos;
            std::vector<godot::Color> cols;
            std::vector<godot::Vector3> nors;
            std::vector<godot::Vector2> uv_s;
            std::vector<godot::Vector2> uv2_s;
            int pos_num = positions.size();
            int vertex_num = vertexs.size();
            int col_num = colors.size();
            int nor_num = normals.size();
            int uv_num = uvs.size();
            int uv2_num = uv2s.size();
            int mat_num = materialPaths.size();
            std::string lastMatPath;
            if(!materialPaths.empty())
                lastMatPath = materialPaths[0];
            for(int i = 0;i!=pos_num;i+=3){
                godot::Vector3 vec;
                vec.x = positions[i];
                vec.y = positions[i+1];
                vec.z = positions[i+2];
                pos.push_back(vec);
            }
            for(int i = 0;i!=col_num;i+=3){
                godot::Color color;
                color.r = colors[i];
                color.g = colors[i+1];
                color.b = colors[i+2];
                cols.push_back(color);
            }
            for(int i = 0;i!=nor_num;i+=3){
                godot::Vector3 nor;
                nor.x = normals[i];
                nor.y = normals[i+1];
                nor.z = normals[i+2];
                nors.push_back(nor);
            }
            for(int i = 0;i!=uv_num;i+=3){
                godot::Vector2 uv;
                uv.x = uvs[i];
                uv.y = uvs[i+1];
                uv_s.push_back(uv);
            }
            for(int i = 0;i!=uv2_num;i+=3){
                godot::Vector2 uv2;
                uv2.x = uv2s[i];
                uv2.y = uv2s[i+1];
                uv2_s.push_back(uv2);
            }


            godot::Ref<godot::ArrayMesh> arr_mesh;
            arr_mesh.instantiate();
            
            godot::SurfaceTool* st = memnew(godot::SurfaceTool());
         
            st->begin(godot::Mesh::PRIMITIVE_TRIANGLES);
            st->set_material(materialRes[lastMatPath]);

            for(int i = 0;i!=vertex_num;++i){
                if(vertexs[i] >= pos_num)
                    break;
                if(i/3 < mat_num){
                    std::string newPath = materialPaths[i/3];
                    if(newPath != lastMatPath){
                        st->commit(arr_mesh);
                        st->clear();
                        st->begin(godot::Mesh::PRIMITIVE_TRIANGLES);
                        st->set_material(materialRes[newPath]);
                        lastMatPath = newPath;
                    }else{
                        if(!lastMatPath.empty()){
                            st->commit(arr_mesh);
                            st->clear();
                            st->begin(godot::Mesh::PRIMITIVE_TRIANGLES);
                            st->set_material(materialRes[""]);
                            lastMatPath.clear();
                        }
                    }
                }
                if(color_Attrib.first == AttribOwner::Vertex){
                    if(i < col_num){
                        st->set_color(cols[i]);
                    }
                }else if(color_Attrib.first == AttribOwner::Point){
                    if(vertexs[i] < col_num){
                        st->set_color(cols[vertexs[i]]);
                    }
                }
                if(normal_Attrib.first == AttribOwner::Vertex){
                    if(i < nor_num){
                        st->set_normal(nors[i]);
                    }
                }else if(normal_Attrib.first == AttribOwner::Point){
                    if(vertexs[i] < nor_num){
                        st->set_normal(nors[vertexs[i]]);
                    }
                }
                if(uv_Attrib.first == AttribOwner::Vertex){
                    if(i < uv_num){
                        st->set_uv(uv_s[i]);
                    }
                }else if(uv_Attrib.first == AttribOwner::Point){
                    if(vertexs[i] < uv_num){
                        st->set_uv(uv_s[vertexs[i]]);
                    }
                }
                if(uv2_Attrib.first == AttribOwner::Vertex){
                    if(i < uv2_num){
                        st->set_uv2(uv2_s[i]);
                    }
                }else if(uv2_Attrib.first == AttribOwner::Point){
                    if(vertexs[i] < uv2_num){
                        st->set_uv2(uv2_s[vertexs[i]]);
                    }
                }
                st->add_vertex(pos[vertexs[i]]);
            }
            st->commit(arr_mesh);
            
            godot::MeshInstance3D* instance = memnew(godot::MeshInstance3D());
            instance->set_mesh(arr_mesh);
            Contact::add_call([=,this]{
                add_child(instance,true);
                instance->set_owner(get_tree()->get_edited_scene_root());
                createdGDNodes.insert(instance);
            });
            godot::memdelete(st);
        }
        return true;
    }
    GDE_EXPORT
    bool createMeshInstance(godot::Ref<NodeId> id){
        return createMeshInstance(*id);
    }
    GDE_EXPORT
    bool freeGDNode(godot::Node* node){
        if(createdGDNodes.find(node) != createdGDNodes.end()){
            freeGDNodeTasks[node] = std::make_shared<std::jthread>([this,node](std::stop_token st){
                std::shared_ptr<std::jthread> nowThread = freeGDNodeTasks[node];
                std::this_thread::sleep_for(freeTimeout);
                freeGDNodeTasks.erase(node);
                if(st.stop_requested())
                    return;
                Contact::add_call([=]{
                    node->queue_free();
                });
            });
            freeGDNodeTasks[node]->detach();
            return true;
        }
        return false;
    }
    GDE_EXPORT
    bool stopFreeGDNode(godot::Node* node){
        if(freeGDNodeTasks.find(node)!=freeGDNodeTasks.end()){
            freeGDNodeTasks[node]->request_stop();
            return true;
        }
        return false;
    }
    GDE_EXPORT
    HAPI_NodeInfo getNodeInfo(int id){
        HAPI_NodeInfo info;
        if(HoudiniApi::GetNodeInfo(get_session(),id,&info) != HAPI_RESULT_SUCCESS){
            printError(HoudiniEngineUtility::getLastError().c_str());
            return {};
        }
        return info;
    }
    GDE_EXPORT
    HAPI_GeoInfo getGeoInfo(int id){
        
        HAPI_GeoInfo info;
        if(HoudiniApi::GetGeoInfo(get_session(),id,&info) != HAPI_RESULT_SUCCESS){
            printError(HoudiniEngineUtility::getLastError().c_str());
            return {};
        }
        return info;
    }
    GDE_EXPORT
    HAPI_MaterialInfo getMaterialInfo(int id){
        
        HAPI_MaterialInfo info;
        if(HoudiniApi::GetMaterialInfo(get_session(),id,&info) != HAPI_RESULT_SUCCESS){
            printError(HoudiniEngineUtility::getLastError().c_str());
            return {};
        }
        return info;
    }
    GDE_EXPORT
    void getParameters(int id){
        if(!sessionOpened){
            printError("Failed to get parameters: The session is invalid.");
            return;
        }
        HAPI_NodeInfo info = getNodeInfo(id);
        std::vector<HAPI_ParmInfo> parm_infos;
        parm_infos.resize(info.parmCount);
        if(auto a = HoudiniApi::GetParameters(get_session(),id,parm_infos.data(),0,info.parmCount);a != HAPI_RESULT_SUCCESS){
            printFile(HoudiniEngineUtility::getLastError().c_str(), " No parm got.");
            return;
        }
        for(int i = 0;i!=info.parmCount;++i){
            auto name = HoudiniEngineUtility::getString(get_session(), parm_infos[i].nameSH);
            std::vector<std::variant<int64_t,double,std::string>> arr;
            if(HoudiniApi::ParmInfo_IsInt(&parm_infos[i])){
                int parm_int_count = HoudiniApi::ParmInfo_GetIntValueCount(&parm_infos[i]);
                std::vector<int> parm_int_values;
                parm_int_values.resize(parm_int_count);

                if(auto a = HoudiniApi::GetParmIntValues(get_session(),id,parm_int_values.data(),parm_infos[i].intValuesIndex,parm_int_count);a != HAPI_RESULT_SUCCESS){
                    printFile(HoudiniEngineUtility::getLastError().c_str()," ",a);
                    continue;
                }
                for(int v = 0;v != parm_int_count;++v){
                    arr.push_back(parm_int_values[v]);
                }
            }else if(HoudiniApi::ParmInfo_IsFloat(&parm_infos[i])){
                int parm_float_count = HoudiniApi::ParmInfo_GetFloatValueCount(&parm_infos[i]);
                std::vector<float> parm_float_values;
                parm_float_values.resize(parm_float_count);

                if(HoudiniApi::GetParmFloatValues(get_session(),id,parm_float_values.data(),parm_infos[i].floatValuesIndex,parm_float_count) != HAPI_RESULT_SUCCESS){
                    printFile(HoudiniEngineUtility::getLastError().c_str());
                    continue;
                }
                for(int v = 0;v != parm_float_count;++v){
                    arr.push_back(parm_float_values[v]);
                }
            }else if(HoudiniApi::ParmInfo_IsString(&parm_infos[i])){
                int parm_string_count = HoudiniApi::ParmInfo_GetStringValueCount(&parm_infos[i]);
                std::vector<HAPI_StringHandle> parmSH_values;
                if(HoudiniApi::GetParmStringValues(get_session(),id,true,parmSH_values.data(),parm_infos[i].stringValuesIndex,parm_string_count)!=HAPI_RESULT_SUCCESS){
                    printFile(HoudiniEngineUtility::getLastError().c_str());
                    continue;
                }
                for(int v = 0;v != parm_string_count;++v){
                    arr.push_back(HoudiniEngineUtility::getString(get_session(),parmSH_values[v]));
                }
            }
            parameters[id][name] = arr;
        }
    }
    GDE_EXPORT
    godot::Dictionary getParameters(godot::Ref<NodeId> id){
        getParameters(*id);
        godot::Dictionary dict;
        auto& param = parameters[**id];
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
    GDE_EXPORT
    godot::Dictionary getAttributes(godot::Ref<NodeId> nodeId,godot::Ref<PartId> partId){
        if(!sessionOpened){
            printError("Failed to get attributes: The session is invalid.");
            return {};
        }
        HAPI_PartInfo part_info;
        HoudiniApi::PartInfo_Init(&part_info);
        if(HoudiniApi::GetPartInfo(get_session(),**nodeId, **partId, &part_info) != HAPI_RESULT_SUCCESS){
            printError(HoudiniEngineUtility::getLastError().c_str());
            return {};
        }
        int point_attr_count = part_info.attributeCounts[HAPI_ATTROWNER_POINT];
        int vertex_attr_count = part_info.attributeCounts[HAPI_ATTROWNER_VERTEX];
        int prim_attr_count = part_info.attributeCounts[HAPI_ATTROWNER_PRIM];
        int detail_attr_count = part_info.attributeCounts[HAPI_ATTROWNER_DETAIL];

        std::vector<HAPI_StringHandle> attrNameSH;
        attrNameSH.resize(point_attr_count);
        if(HoudiniApi::GetAttributeNames(get_session(),**nodeId,**partId,HAPI_ATTROWNER_POINT,attrNameSH.data(),point_attr_count) != HAPI_RESULT_SUCCESS){
            printError(HoudiniEngineUtility::getLastError().c_str());
            return {};
        }

        godot::Dictionary dict;
        godot::Array point_attrs;
        godot::Dictionary point_attr;
        for(int i = 0;i!=point_attr_count;++i){
            std::string attr_name = HoudiniEngineUtility::getString(get_session(), attrNameSH[i]);
            HAPI_AttributeInfo attr_info;
            HoudiniApi::AttributeInfo_Init(&attr_info);
            if(HoudiniApi::GetAttributeInfo(get_session(),**nodeId,**partId,attr_name.c_str(),HAPI_ATTROWNER_POINT,&attr_info) != HAPI_RESULT_SUCCESS){
                printError(HoudiniEngineUtility::getLastError().c_str());
                continue;
            }
            if(!attr_info.exists)
                continue;
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
        if(HoudiniApi::GetAttributeNames(get_session(),**nodeId,**partId,HAPI_ATTROWNER_VERTEX,attrNameSH.data(),vertex_attr_count) != HAPI_RESULT_SUCCESS){
            printError(HoudiniEngineUtility::getLastError().c_str());
            return {};
        }
        
        godot::Array vertex_attrs;
        godot::Dictionary vertex_attr;
        for(int i = 0;i!=point_attr_count;++i){
            std::string attr_name = HoudiniEngineUtility::getString(get_session(), attrNameSH[i]);
            HAPI_AttributeInfo attr_info;
            HoudiniApi::AttributeInfo_Init(&attr_info);
            if(HoudiniApi::GetAttributeInfo(get_session(),**nodeId,**partId,attr_name.c_str(),HAPI_ATTROWNER_VERTEX,&attr_info) != HAPI_RESULT_SUCCESS){
                printError(HoudiniEngineUtility::getLastError().c_str());
                continue;
            }
            if(!attr_info.exists)
                continue;
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
        if(HoudiniApi::GetAttributeNames(get_session(),**nodeId,**partId,HAPI_ATTROWNER_PRIM,attrNameSH.data(),prim_attr_count) != HAPI_RESULT_SUCCESS){
            printError(HoudiniEngineUtility::getLastError().c_str());
            return {};
        }

        godot::Array prim_attrs;
        godot::Dictionary prim_attr;
        for(int i = 0;i!=point_attr_count;++i){
            std::string attr_name = HoudiniEngineUtility::getString(get_session(), attrNameSH[i]);
            HAPI_AttributeInfo attr_info;
            HoudiniApi::AttributeInfo_Init(&attr_info);
            if(HoudiniApi::GetAttributeInfo(get_session(),**nodeId,**partId,attr_name.c_str(),HAPI_ATTROWNER_PRIM,&attr_info) != HAPI_RESULT_SUCCESS){
                printError(HoudiniEngineUtility::getLastError().c_str());
                continue;
            }
            if(!attr_info.exists)
                continue;
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
        if(HoudiniApi::GetAttributeNames(get_session(),**nodeId,**partId,HAPI_ATTROWNER_DETAIL,attrNameSH.data(),detail_attr_count) != HAPI_RESULT_SUCCESS){
            printError(HoudiniEngineUtility::getLastError().c_str());
            return {};
        }

        godot::Array detail_attrs;
        godot::Dictionary detail_attr;
        for(int i = 0;i!=point_attr_count;++i){
            std::string attr_name = HoudiniEngineUtility::getString(get_session(), attrNameSH[i]);
            HAPI_AttributeInfo attr_info;
            HoudiniApi::AttributeInfo_Init(&attr_info);
            if(HoudiniApi::GetAttributeInfo(get_session(),**nodeId,**partId,attr_name.c_str(),HAPI_ATTROWNER_DETAIL,&attr_info) != HAPI_RESULT_SUCCESS){
                printError(HoudiniEngineUtility::getLastError().c_str());
                continue;
            }
            if(!attr_info.exists)
                continue;
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
    GDE_EXPORT
    bool checkGeometryChange(int nodeId){
        HAPI_GeoInfo geoInfo;
        if(HoudiniApi::GetGeoInfo(get_session(),nodeId,&geoInfo) != HAPI_RESULT_SUCCESS){
            printError(HoudiniEngineUtility::getLastError().c_str());
            return false;
        }
        return geoInfo.hasGeoChanged;
    }
    GDE_EXPORT
    void getGeometry(int id){

        HAPI_GeoInfo mesh_geo_info;
        if(HoudiniApi::GetDisplayGeoInfo(get_session(), id, &mesh_geo_info) != HAPI_RESULT_SUCCESS){
            printError(HoudiniEngineUtility::getLastError().c_str());
            return;
        }
        for(int partId = 0;partId!=mesh_geo_info.partCount;++partId){
            HAPI_PartInfo partInfo;
            HoudiniApi::PartInfo_Init(&partInfo);
            if(HoudiniApi::GetPartInfo(get_session(), mesh_geo_info.nodeId, partId, &partInfo) != HAPI_RESULT_SUCCESS){
                printError(HoudiniEngineUtility::getLastError().c_str());
                return;
            }
            auto type = partType[id][partId] = (PartType)partInfo.type;
            switch(type){
            case PartType::Mesh:{
                std::vector<int> mesh_face_counts(partInfo.faceCount);
                if(HoudiniApi::GetFaceCounts(get_session(),mesh_geo_info.nodeId,partInfo.id,mesh_face_counts.data(),0,partInfo.faceCount) != HAPI_RESULT_SUCCESS){
                    printError(HoudiniEngineUtility::getLastError().c_str());
                    return;
                }
                std::vector<int> mesh_vertex_list(partInfo.vertexCount);
                if(HoudiniApi::GetVertexList(get_session(),mesh_geo_info.nodeId,partInfo.id,mesh_vertex_list.data(),0, partInfo.vertexCount) != HAPI_RESULT_SUCCESS){
                    printError(HoudiniEngineUtility::getLastError().c_str());
                    return;
                }
                
                // std::for_each(std::execution::par_unseq,mesh_face_counts.begin(),mesh_face_counts.end(),[](int& data){
                //     //TODO: triangulate
                // });
                auto fetchPointAttrib = [&](HAPI_AttributeOwner owner,const char* attrib_name,std::vector<float>& mesh_attrib_data)->bool{
                    HAPI_AttributeInfo mesh_attrib_info;
                    if(HoudiniApi::GetAttributeInfo(get_session(),mesh_geo_info.nodeId,partInfo.id,attrib_name, owner,&mesh_attrib_info) != HAPI_RESULT_SUCCESS){
                        printFile(HoudiniEngineUtility::getLastError().c_str());
                        return false;
                    }
                    std::cerr << "mesh_attrib_info.exists: " << mesh_attrib_info.exists << std::endl;
                    if(!mesh_attrib_info.exists)
                        return false;
                    std::size_t dataSize = mesh_attrib_info.count * mesh_attrib_info.tupleSize;
                    std::cerr << "dataSize" << dataSize << std::endl;

                    std::cerr << "mesh_attrib_info.count: " << mesh_attrib_info.count << std::endl;
                    mesh_attrib_data.resize(dataSize);
                    if(HoudiniApi::GetAttributeFloatData(get_session(),mesh_geo_info.nodeId,partInfo.id,attrib_name,&mesh_attrib_info,-1,mesh_attrib_data.data(),0,mesh_attrib_info.count) != HAPI_RESULT_SUCCESS){
                        printFile(HoudiniEngineUtility::getLastError().c_str()," Attribute name : ",attrib_name);
                        return false;
                    }
                    return true;
                };
                std::vector<float> mesh_p_attrib_info;
                fetchPointAttrib(HAPI_ATTROWNER_POINT, "P", mesh_p_attrib_info);
                std::cout << "get: mesh_p_attrib_info.size()" << mesh_p_attrib_info.size() << std::endl;

                std::vector<float> mesh_cd_attrib_data;
                AttribOwner mesh_cd_attrib_owner = AttribOwner::Vertex;
                if(!fetchPointAttrib(HAPI_ATTROWNER_VERTEX, "Cd", mesh_cd_attrib_data)){
                    fetchPointAttrib(HAPI_ATTROWNER_POINT, "Cd", mesh_cd_attrib_data);
                    mesh_cd_attrib_owner = AttribOwner::Point;
                }
                std::cout << "get: mesh_cd_attrib_data.size()" << mesh_cd_attrib_data.size() << std::endl;

                std::vector<float> mesh_N_attrib_data;
                AttribOwner mesh_N_attrib_owner = AttribOwner::Vertex;
                if(!fetchPointAttrib(HAPI_ATTROWNER_VERTEX , "N", mesh_N_attrib_data)){
                    fetchPointAttrib(HAPI_ATTROWNER_POINT , "N", mesh_N_attrib_data);
                    mesh_N_attrib_owner = AttribOwner::Point;
                }
                std::cout << "get: mesh_N_attrib_data.size()" << mesh_N_attrib_data.size() << std::endl;
                
                std::vector<float> mesh_uv_attrib_data;
                AttribOwner mesh_uv_attrib_owner = AttribOwner::Vertex;
                if(!fetchPointAttrib(HAPI_ATTROWNER_VERTEX , "uv", mesh_uv_attrib_data)){
                    fetchPointAttrib(HAPI_ATTROWNER_POINT , "uv", mesh_uv_attrib_data);
                    mesh_uv_attrib_owner = AttribOwner::Point;
                }
                std::cout << "get: mesh_uv_attrib_data.size()" << mesh_uv_attrib_data.size() << std::endl;
                
                std::vector<float> mesh_uv2_attrib_data;
                AttribOwner mesh_uv2_attrib_owner = AttribOwner::Vertex;
                if(!fetchPointAttrib(HAPI_ATTROWNER_VERTEX , "uv2", mesh_uv2_attrib_data)){
                    fetchPointAttrib(HAPI_ATTROWNER_POINT , "uv2", mesh_uv2_attrib_data);
                    mesh_uv2_attrib_owner = AttribOwner::Point;
                }
                std::cout << "get: mesh_uv2_attrib_data.size()" << mesh_uv2_attrib_data.size() << std::endl;
                
                geometries[id][partId] = {std::move(mesh_face_counts),std::move(mesh_p_attrib_info),std::move(mesh_vertex_list),{mesh_cd_attrib_owner,std::move(mesh_cd_attrib_data)},{mesh_N_attrib_owner,std::move(mesh_N_attrib_data)},{mesh_uv_attrib_owner,std::move(mesh_uv_attrib_data)},{mesh_uv2_attrib_owner,std::move(mesh_uv2_attrib_data)}};
            }break;
            case PartType::Instancer:{
                std::vector<HAPI_Transform> instancer_transforms(partInfo.instanceCount);
                auto result = HoudiniApi::GetInstancerPartTransforms(get_session(),id,partId,HAPI_SRT,instancer_transforms.data(),0,partInfo.instanceCount);
                if(result != HAPI_RESULT_SUCCESS){
                    printFile(HoudiniEngineUtility::getLastError().c_str());
                    continue;
                }
                instanceTransforms[id][partId] = std::move(instancer_transforms);
            }break;
            case PartType::Invalid:{
                printError("This part is invalid.");
            }break;
            default:{
                printError("Not yet supported.");
            }break;
            }
        }
    }
    GDE_EXPORT
    void getMaterial(int id){
        HAPI_GeoInfo mesh_geo_info;
        if(HoudiniApi::GetDisplayGeoInfo(get_session(), id, &mesh_geo_info) != HAPI_RESULT_SUCCESS){
            printError(HoudiniEngineUtility::getLastError().c_str());
            return;
        }

        for(int partId = 0;partId!=mesh_geo_info.partCount;++partId){
            HAPI_PartInfo mesh_part_info;
            HoudiniApi::PartInfo_Init(&mesh_part_info);
            if(HoudiniApi::GetPartInfo(get_session(), mesh_geo_info.nodeId, partId, &mesh_part_info) != HAPI_RESULT_SUCCESS){
                printError(HoudiniEngineUtility::getLastError().c_str());
                continue;
            }
            bool all_the_same = false;
            std::vector<HAPI_NodeId> matIds(mesh_part_info.faceCount);
            if(HoudiniApi::GetMaterialNodeIdsOnFaces(get_session(),id,partId,&all_the_same,matIds.data(),0,mesh_part_info.faceCount)!=HAPI_RESULT_SUCCESS){
                printError(HoudiniEngineUtility::getLastError().c_str());
                continue;
            }

            materialIds[id][partId].first = all_the_same;
            materialIds[id][partId].second = std::move(matIds);
        }
        

        if(HoudiniApi::GetDisplayGeoInfo(get_session(), id, &mesh_geo_info) != HAPI_RESULT_SUCCESS){
            printError(HoudiniEngineUtility::getLastError().c_str());
            return;
        }
        for(int partId = 0;partId!=mesh_geo_info.partCount;++partId){
            HAPI_PartInfo mesh_part_info;
            HoudiniApi::PartInfo_Init(&mesh_part_info);
            if(HoudiniApi::GetPartInfo(get_session(), mesh_geo_info.nodeId, partId, &mesh_part_info) != HAPI_RESULT_SUCCESS){
                printError(HoudiniEngineUtility::getLastError().c_str());
                continue;
            }
            std::vector<HAPI_StringHandle> materialPaths;
            std::vector<std::string> resTypePaths;
            HAPI_AttributeInfo materialPathAttribInfo;
            
            if(HoudiniApi::GetAttributeInfo(get_session(),mesh_geo_info.nodeId,mesh_part_info.id,"gd_mat_path", HAPI_ATTROWNER_PRIM,&materialPathAttribInfo) != HAPI_RESULT_SUCCESS){
                printFile(HoudiniEngineUtility::getLastError().c_str());
                continue;
            }
            if(!materialPathAttribInfo.exists)
                continue;
            materialPaths.resize(materialPathAttribInfo.count * materialPathAttribInfo.tupleSize);
            if(HoudiniApi::GetAttributeStringData(get_session(),mesh_geo_info.nodeId,mesh_part_info.id,"gd_mat_path",&materialPathAttribInfo,materialPaths.data(),0,materialPathAttribInfo.count) != HAPI_RESULT_SUCCESS){
                printFile(HoudiniEngineUtility::getLastError().c_str()," Attribute name : gd_mat_path");
                continue;
            }
            for(auto a : materialPaths){
                std::string path = HoudiniEngineUtility::getString(get_session(),a);
                if(materialRes.find(path) == materialRes.end()){
                    materialRes[path] = godot::ResourceLoader::get_singleton()->load(godot::String::utf8(path.c_str()));
                }
                resTypePaths.emplace_back(std::move(path));
            }
            materials[id][partId] = std::move(resTypePaths);
        }
    }
    GDE_EXPORT
    bool checkMaterialChange(int nodeId){
        for(auto& a : materialIds[nodeId]){
            if(a.second.first){
                HAPI_MaterialInfo materialInfo;
                if(HoudiniApi::GetMaterialInfo(get_session(),a.second.second[0],&materialInfo)!=HAPI_RESULT_SUCCESS){
                    printError(HoudiniEngineUtility::getLastError().c_str());
                }else{
                    return materialInfo.hasChanged;
                }
            }
            for(auto& b : a.second.second){
                HAPI_MaterialInfo materialInfo;
                if(HoudiniApi::GetMaterialInfo(get_session(),a.second.second[0],&materialInfo)!=HAPI_RESULT_SUCCESS){
                    printError(HoudiniEngineUtility::getLastError().c_str());
                }else{
                    if(materialInfo.hasChanged)
                        return true;
                }
            }
        }
        return false;
    }
    GDE_EXPORT
    bool createInputNode(std::string nodeLabel, int& id, int parentId, godot::Ref<godot::Mesh> mesh){
        if(!sessionOpened){
            printError("Failed to create input node: The session is invalid.");
            return false;
        }
        if(HoudiniApi::CreateInputNode(get_session(),parentId,&id,nodeLabel.c_str()) != HAPI_RESULT_SUCCESS){
            printError("Error create input node: ",HoudiniEngineUtility::getLastError().c_str());
            return false;
        }else{
            printFile("Success create node, ID: ",id);
        }
        if(initInputNode(id,mesh) != -1){
            nodeIds.insert({id,-1});
            cookNode(id);
        }
        else{
            deleteNode(id);
            printError("Failed to create input node.");
        }
        return true;
    }
    GDE_EXPORT
    int initInputNode(int id,godot::Ref<godot::Mesh> mesh){
        auto geoInfo = getGeoInfo(id);
        if(geoInfo.isTemplated)
            return -1;
        std::vector<int> allFaces;
        std::vector<float> allPositions;
        std::vector<int> allVertexs;
        std::vector<float> allColors;
        std::vector<float> allNormals;
        std::vector<float> allUVs;
        std::vector<float> allUV2s;
        std::vector<const char*> allMatPaths;
        for(int i = 0,size = mesh->get_surface_count();i!=size;++i){
            godot::Array rawData = mesh->surface_get_arrays(i);
            godot::PackedVector3Array rawVertexs;
            godot::PackedVector3Array rawNormals;
            godot::PackedColorArray rawColors;
            godot::PackedVector2Array rawUVs;
            godot::PackedVector2Array rawUV2s;
            godot::PackedInt32Array rawIndexs;
            godot::String rawMatPath = "";
            if(auto mat = mesh->surface_get_material(i);mat.is_valid()){
                rawMatPath = mat->get_path();
            }
            if(rawData[godot::Mesh::ARRAY_VERTEX].get_type() == godot::Variant::Type::PACKED_VECTOR3_ARRAY)
                rawVertexs = rawData[godot::Mesh::ARRAY_VERTEX];
            if(rawData[godot::Mesh::ARRAY_NORMAL].get_type() == godot::Variant::Type::PACKED_VECTOR3_ARRAY)
                rawNormals = rawData[godot::Mesh::ARRAY_NORMAL];
            if(rawData[godot::Mesh::ARRAY_COLOR].get_type() == godot::Variant::Type::PACKED_COLOR_ARRAY)
                rawColors = rawData[godot::Mesh::ARRAY_COLOR];
            if(rawData[godot::Mesh::ARRAY_TEX_UV].get_type() == godot::Variant::Type::PACKED_VECTOR2_ARRAY)
                rawUVs = rawData[godot::Mesh::ARRAY_TEX_UV];
            if(rawData[godot::Mesh::ARRAY_TEX_UV2].get_type() == godot::Variant::Type::PACKED_VECTOR2_ARRAY)
                rawUV2s = rawData[godot::Mesh::ARRAY_TEX_UV2];
            if(rawData[godot::Mesh::ARRAY_INDEX].get_type() == godot::Variant::Type::PACKED_INT32_ARRAY)
                rawIndexs = rawData[godot::Mesh::ARRAY_INDEX];
            std::vector<int> faces;
            std::vector<float> positions;
            std::vector<int> vertexs;
            std::vector<float> colors;
            std::vector<float> normals;
            std::vector<float> uvs;
            std::vector<float> uv2s;
            std::vector<const char*> matPaths;
            if(rawIndexs.is_empty()){
                std::vector<godot::Vector3> collectPos;
                collectPos.reserve(rawVertexs.size()/3);
                for(decltype(rawVertexs.size()) i = 0,size = rawVertexs.size();i!=size;++i){
                    if(auto it = std::find(collectPos.begin(),collectPos.end(),rawVertexs[i]);it != collectPos.end()){
                        vertexs.push_back(it-collectPos.begin());
                    }else{
                        vertexs.push_back(collectPos.size());
                        collectPos.push_back(rawVertexs[i]);
                    }
                }
                positions.reserve(collectPos.size()*3);
                for(decltype(collectPos.size()) i = 0,sz = collectPos.size();i!=sz;++i){
                    godot::Vector3 pos = collectPos[i];
                    positions.push_back(pos.x);
                    positions.push_back(pos.y);
                    positions.push_back(pos.z);
                }
                colors.reserve(rawColors.size()*3);
                for(decltype(rawColors.size()) i = 0,sz = rawColors.size();i!=sz;++i){
                    godot::Color col = rawColors[i];
                    colors.push_back(col.r);
                    colors.push_back(col.g);
                    colors.push_back(col.b);
                }
                normals.reserve(rawNormals.size()*3);
                for(decltype(rawNormals.size()) i = 0,sz = rawNormals.size();i!=sz;++i){
                    godot::Vector3 vec = rawNormals[i];
                    normals.push_back(vec.x);
                    normals.push_back(vec.y);
                    normals.push_back(vec.z);
                }
                uvs.reserve(rawUVs.size()*3);
                for(decltype(rawUVs.size()) i = 0,sz = rawUVs.size();i!=sz;++i){
                    godot::Vector2 vec = rawUVs[i];
                    uvs.push_back(vec.x);
                    uvs.push_back(vec.y);
                    uvs.push_back(0);
                }
                uv2s.reserve(rawUV2s.size()*3);
                for(decltype(rawUV2s.size()) i = 0,sz = rawUV2s.size();i!=sz;++i){
                    godot::Vector2 vec = rawUV2s[i];
                    uv2s.push_back(vec.x);
                    uv2s.push_back(vec.y);
                    uv2s.push_back(0);
                }
            }else{//Index mode
                positions.reserve(rawVertexs.size()*3);
                for(decltype(rawVertexs.size()) i = 0,sz = rawVertexs.size();i!=sz;++i){
                    godot::Vector3 pos = rawVertexs[i];
                    positions.push_back(pos.x);
                    positions.push_back(pos.y);
                    positions.push_back(pos.z);
                }
                vertexs.reserve(rawIndexs.size());
                for(decltype(rawIndexs.size()) i = 0,sz = rawIndexs.size();i!=sz;++i){
                    vertexs.push_back(rawIndexs[i]);
                }
                size_t vertexSize = vertexs.size();
                std::vector<godot::Color> collectColors;
                std::vector<godot::Vector3> collectNormals;
                std::vector<godot::Vector2> collectUVs;
                std::vector<godot::Vector2> collectUV2s;
                size_t rawColorSize = rawColors.size();
                if(rawColorSize != 0){
                    collectColors.reserve(vertexSize);
                    for(auto a : vertexs){
                        if(a < rawColorSize)
                            collectColors.emplace_back(rawColors[a]);
                        else 
                            collectColors.emplace_back(godot::Color());
                    }
                }
                size_t rawNormalSize = rawNormals.size();
                if(rawNormalSize != 0){
                    collectNormals.reserve(vertexSize);
                    for(auto a : vertexs){
                        if(a < rawNormalSize)
                            collectNormals.emplace_back(rawNormals[a]);
                        else 
                            collectNormals.emplace_back(godot::Vector3());
                    }
                }
                size_t rawUVSize = rawUVs.size();
                if(rawUVSize != 0){
                    collectUVs.reserve(vertexSize);
                    for(auto a : vertexs){
                        if(a < rawUVSize)
                            collectUVs.emplace_back(rawUVs[a]);
                        else 
                            collectUVs.emplace_back(godot::Vector2());
                    }
                }
                size_t rawUV2Size = rawUV2s.size();
                if(rawUV2Size != 0){
                    collectUV2s.reserve(vertexSize);
                    for(auto a : vertexs){
                        if(a < rawUV2Size)
                            collectUV2s.emplace_back(rawUV2s[a]);
                        else 
                            collectUV2s.emplace_back(godot::Vector2());
                    }
                }
                colors.reserve(collectColors.size()*3);
                for(decltype(collectColors.size()) i = 0,sz = collectColors.size();i!=sz;++i){
                    godot::Color col = collectColors[i];
                    colors.push_back(col.r);
                    colors.push_back(col.g);
                    colors.push_back(col.b);
                }
                normals.reserve(collectNormals.size()*3);
                for(decltype(collectNormals.size()) i = 0,sz = collectNormals.size();i!=sz;++i){
                    godot::Vector3 vec = collectNormals[i];
                    normals.push_back(vec.x);
                    normals.push_back(vec.y);
                    normals.push_back(vec.z);
                }
                uvs.reserve(collectUVs.size()*3);
                for(decltype(collectUVs.size()) i = 0,sz = collectUVs.size();i!=sz;++i){
                    godot::Vector2 vec = collectUVs[i];
                    uvs.push_back(vec.x);
                    uvs.push_back(vec.y);
                    uvs.push_back(0);
                }
                uv2s.reserve(collectUV2s.size()*3);
                for(decltype(collectUV2s.size()) i = 0,sz = collectUV2s.size();i!=sz;++i){
                    godot::Vector2 vec = collectUV2s[i];
                    uv2s.push_back(vec.x);
                    uv2s.push_back(vec.y);
                    uv2s.push_back(0);
                }
            }
            const char* matPath = keep_alive_string(rawMatPath.utf8().get_data());

            size_t vertexSize = allVertexs.size();
            if(!colors.empty()){
                if(allColors.size()/3 < vertexSize){
                    allColors.resize(vertexSize*3);
                }
                allColors.insert(allColors.end(),colors.begin(),colors.end());
            }
            if(!normals.empty()){
                if(allNormals.size()/3 < vertexSize){
                    allNormals.resize(vertexSize*3);
                }
                allNormals.insert(allNormals.end(),normals.begin(),normals.end());
            }
            if(!uvs.empty()){
                if(allUVs.size()/3 < vertexSize){
                    allUVs.resize(vertexSize*3);
                }
                allUVs.insert(allUVs.end(),uvs.begin(),uvs.end());
            }
            if(!uv2s.empty()){
                if(allUV2s.size()/3 < vertexSize){
                    allUV2s.resize(vertexSize*3);
                }
                allUV2s.insert(allUV2s.end(),uv2s.begin(),uv2s.end());
            }
            allPositions.insert(allPositions.end(),positions.begin(),positions.end());
            allVertexs.insert(allVertexs.end(),vertexs.begin(),vertexs.end());
            allMatPaths.insert(allMatPaths.end(),vertexs.size()/3,matPath);
        }
        allFaces = std::vector<int>(allVertexs.size()/3,3);

        HAPI_PartInfo node_part = HoudiniApi::PartInfo_Create();
        node_part.type = HAPI_PARTTYPE_MESH;
        node_part.faceCount = allFaces.size();
        node_part.vertexCount = allVertexs.size();
        node_part.pointCount = allPositions.size()/3;
        if(HoudiniApi::SetPartInfo(get_session(),id,0,&node_part) != HAPI_RESULT_SUCCESS){
            printError("Error create input node: ",HoudiniEngineUtility::getLastError().c_str());
            return -1;
        }

        HAPI_AttributeInfo pointInfo = HoudiniApi::AttributeInfo_Create();
        pointInfo.count = allPositions.size()/3;
        pointInfo.tupleSize = 3;
        pointInfo.exists = true;
        pointInfo.storage = HAPI_STORAGETYPE_FLOAT;
        pointInfo.owner = HAPI_ATTROWNER_POINT;
        if(HoudiniApi::AddAttribute(get_session(),id,0,"P",&pointInfo) != HAPI_RESULT_SUCCESS){
            printError(HoudiniEngineUtility::getLastError().c_str());
            return -1;
        }
        if(HoudiniApi::SetAttributeFloatData(get_session(),id,0,"P",&pointInfo,allPositions.data(),0,pointInfo.count) != HAPI_RESULT_SUCCESS){
            printError(HoudiniEngineUtility::getLastError().c_str());
            return -1;
        }
        if(HoudiniApi::SetVertexList(get_session(),id,0,allVertexs.data(),0,allVertexs.size()) != HAPI_RESULT_SUCCESS){
            printError(HoudiniEngineUtility::getLastError().c_str());
            return -1;
        }
        if(HoudiniApi::SetFaceCounts(get_session(),id,0,allFaces.data(),0,allFaces.size()) != HAPI_RESULT_SUCCESS){
            printError(HoudiniEngineUtility::getLastError().c_str());
            return -1;
        }
        if(!allColors.empty()){
            HAPI_AttributeInfo colorInfo = HoudiniApi::AttributeInfo_Create();
            colorInfo.count = allColors.size()/3;
            colorInfo.tupleSize = 3;
            colorInfo.exists = true;
            colorInfo.storage = HAPI_STORAGETYPE_FLOAT;
            colorInfo.owner = (HAPI_AttributeOwner)AttribOwner::Vertex;
            if(HoudiniApi::AddAttribute(get_session(),id,0,"Cd",&colorInfo) != HAPI_RESULT_SUCCESS){
                printError(HoudiniEngineUtility::getLastError().c_str());
                return -1;
            }
            std::cerr << "count: " << colorInfo.count << "\tsize: " << allColors.size()/3 << std::endl;
            if(HoudiniApi::SetAttributeFloatData(get_session(),id,0,"Cd",&colorInfo,allColors.data(),0,colorInfo.count) != HAPI_RESULT_SUCCESS){
                printError(HoudiniEngineUtility::getLastError().c_str());
                return -1;
            }
        }
        if(!allNormals.empty()){
            HAPI_AttributeInfo normalInfo = HoudiniApi::AttributeInfo_Create();
            normalInfo.count = allNormals.size()/3;
            normalInfo.tupleSize = 3;
            normalInfo.exists = true;
            normalInfo.storage = HAPI_STORAGETYPE_FLOAT;
            normalInfo.owner = (HAPI_AttributeOwner)AttribOwner::Vertex;
            if(HoudiniApi::AddAttribute(get_session(),id,0,"N",&normalInfo) != HAPI_RESULT_SUCCESS){
                printError(HoudiniEngineUtility::getLastError().c_str());
                return -1;
            }
            if(HoudiniApi::SetAttributeFloatData(get_session(),id,0,"N",&normalInfo,allNormals.data(),0,normalInfo.count) != HAPI_RESULT_SUCCESS){
                printError(HoudiniEngineUtility::getLastError().c_str());
                return -1;
            }
        }
        if(!allUVs.empty()){
            HAPI_AttributeInfo uvInfo = HoudiniApi::AttributeInfo_Create();
            uvInfo.count = allUVs.size()/3;
            uvInfo.tupleSize = 3;
            uvInfo.exists = true;
            uvInfo.storage = HAPI_STORAGETYPE_FLOAT;
            uvInfo.owner = (HAPI_AttributeOwner)AttribOwner::Vertex;
            if(HoudiniApi::AddAttribute(get_session(),id,0,"uv",&uvInfo) != HAPI_RESULT_SUCCESS){
                printError(HoudiniEngineUtility::getLastError().c_str());
                return -1;
            }
            if(HoudiniApi::SetAttributeFloatData(get_session(),id,0,"uv",&uvInfo,allUVs.data(),0,uvInfo.count) != HAPI_RESULT_SUCCESS){
                printError(HoudiniEngineUtility::getLastError().c_str());
                return -1;
            }
        }
        if(!allUV2s.empty()){
            HAPI_AttributeInfo uv2Info = HoudiniApi::AttributeInfo_Create();
            uv2Info.count = allUV2s.size()/3;
            uv2Info.tupleSize = 3;
            uv2Info.exists = true;
            uv2Info.storage = HAPI_STORAGETYPE_FLOAT;
            uv2Info.owner = (HAPI_AttributeOwner)AttribOwner::Vertex;
            if(HoudiniApi::AddAttribute(get_session(),id,0,"uv2",&uv2Info) != HAPI_RESULT_SUCCESS){
                printError(HoudiniEngineUtility::getLastError().c_str());
                return -1;
            }
            if(HoudiniApi::SetAttributeFloatData(get_session(),id,0,"uv2",&uv2Info,allUV2s.data(),0,uv2Info.count) != HAPI_RESULT_SUCCESS){
                printError(HoudiniEngineUtility::getLastError().c_str());
                return -1;
            }
        }
        if(!allMatPaths.empty()){
            HAPI_AttributeInfo matInfo = HoudiniApi::AttributeInfo_Create();
            matInfo.count = allMatPaths.size();
            matInfo.tupleSize = 1;
            matInfo.exists = true;
            matInfo.storage = HAPI_STORAGETYPE_STRING;
            matInfo.owner = HAPI_ATTROWNER_PRIM;
            if(HoudiniApi::AddAttribute(get_session(),id,0,"gd_mat_path",&matInfo) != HAPI_RESULT_SUCCESS){
                printError(HoudiniEngineUtility::getLastError().c_str());
                return -1;
            }
            if(HoudiniApi::SetAttributeStringData(get_session(),id,0,"gd_mat_path",&matInfo,allMatPaths.data(),0,matInfo.count) != HAPI_RESULT_SUCCESS){
                printError(HoudiniEngineUtility::getLastError().c_str());
                return -1;
            }
        }
        if(HoudiniApi::CommitGeo(get_session(),id) != HAPI_RESULT_SUCCESS){
            printError(HoudiniEngineUtility::getLastError().c_str());
            return -1;
        }
        return id;
    }
};


#endif //!HOUDINI_BIND_GODOT
