#ifndef HOUDINI_ENGINE_BIND
#define HOUDINI_ENGINE_BIND
#include "DebugSymbol.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/object.hpp>
#include <HoudiniApi.h>
#include <HoudiniEngineUtility.h>
#include <HoudiniEnginePlatform.h>
#include "Utility.h"
#include "HEClass.h"
#include "HEBindEnum.h"



class Bool: public godot::RefCounted{
GDCLASS(Bool, godot::RefCounted)
    void set_value(bool value){
        this->value = value;
    }
    bool get_value(){
        return value;
    }
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("set_value","value"),&Bool::set_value);
        godot::ClassDB::bind_method(godot::D_METHOD("get_value"),&Bool::get_value);
        godot::ClassDB::add_property("Bool",godot::PropertyInfo(godot::Variant::BOOL,"value"),"set_value","get_value");
    }
public:
    bool value;
    Bool(){}
    Bool(bool value){
        this->value = value;
    }
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
class Float: public godot::RefCounted{
GDCLASS(Float, godot::RefCounted)
    void set_value(double value){
        this->value = value;
    }
    double get_value(){
        return value;
    }
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("set_value","value"),&Float::set_value);
        godot::ClassDB::bind_method(godot::D_METHOD("get_value"),&Float::get_value);
        godot::ClassDB::add_property("Float",godot::PropertyInfo(godot::Variant::FLOAT,"value"),"set_value","get_value");
    }
public:
    float value;
    Float(){}
    template<typename T> requires std::floating_point<T>
    Float(T value){
        this->value = value;
    }
};
class RefArray: public godot::RefCounted{
    GDCLASS(RefArray,RefCounted)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("set_value","value"),&RefArray::set_value);
        godot::ClassDB::bind_method(godot::D_METHOD("get_value"),&RefArray::get_value);
        godot::ClassDB::add_property("RefArray",godot::PropertyInfo(godot::Variant::DICTIONARY,"value"),"set_value","get_value");
    }
    void set_value(godot::Array value){
        this->value = value;
    }
    godot::Array get_value(){
        return value;
    }
public:
    godot::Array value;
    template<typename ...T>
    RefArray(T&&... v){
        value = godot::Array(std::forward<decltype(v)>(v)...);
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
class HAPI: public godot::Object{
    GDCLASS(HAPI, godot::Object)
    static void _bind_methods(){

        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetObjectInfo","session","nodeId"),&HAPI::GetObjectInfo);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetAssetInfo","session","nodeId"),&HAPI::GetAssetInfo);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetNodeInfo","session","nodeId"),&HAPI::GetNodeInfo);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetGeoInfo","session","nodeId"),&HAPI::GetGeoInfo);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetMaterialInfo","session","nodeId"),&HAPI::GetMaterialInfo);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetPartInfo","session","nodeId","partId"),&HAPI::GetPartInfo);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetAttrInfo","session","nodeId","partId","name","owner"),&HAPI::GetAttrInfo);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetParmInfo","session","nodeId","parmId","parmInfo"),&HAPI::GetParmInfo);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("AddAttribute","session","nodeId","partId","name","attrInfo"),&HAPI::AddAttribute);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("Cleanup","session"),&HAPI::Cleanup);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("ClearConnectionError"),&HAPI::ClearConnectionError);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("CloseSession","session"),&HAPI::CloseSession);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("CommitGeo","session","nodeId"),&HAPI::CommitGeo);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("ConnectNodeInput","session","nodeId","inputIndex","nodeId_toConnect","outputIndex"),&HAPI::ConnectNodeInput);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("CookNode","session","nodeId","cookOptions"),&HAPI::CookNode);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("CookOptions_Create"),&HAPI::CookOptions_Create);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("CreateInProcessSession","session","sessionInfo"),&HAPI::CreateInProcessSession);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("CreateInputNode","session","parentId","nodeId","name"),&HAPI::CreateInputNode);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("CreateNode","session","parentId","operatorName","nodeLabel","cookOnCreation","nodeId"),&HAPI::CreateNode);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("CreateThriftNamedPipeSession","session","pipeName","sessionInfo"),&HAPI::CreateThriftNamedPipeSession);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("CreateThriftSharedMemorySession","session","sharedMemName","sessionInfo"),&HAPI::CreateThriftSharedMemorySession);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("CreateThriftSocketSession","session","hostName","port","sessionInfo"),&HAPI::CreateThriftSocketSession);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("DeleteAttribute","session","nodeId","partId","name","attrInfo"),&HAPI::DeleteAttribute);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("DeleteNode","session","nodeId"),&HAPI::DeleteNode);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("DisconnectNodeInput","session","nodeId","inputIndex"),&HAPI::DisconnectNodeInput);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GeoInfo_Create"),&HAPI::GeoInfo_Create);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetAttributeFloatData","session","nodeId","partId","name","owner"),&HAPI::GetAttributeFloatData);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetAttributeIntData","session","nodeId","partId","name","owner"),&HAPI::GetAttributeIntData);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetAttributeStringData","session","nodeId","partId","name","owner"),&HAPI::GetAttributeStringData);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetConnectionError"),&HAPI::GetConnectionError);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetCookingCurrentCount","session"),&HAPI::GetCookingCurrentCount);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetCookingTotalCount","session"),&HAPI::GetCookingTotalCount);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetDisplayGeoInfo","session","nodeId"),&HAPI::GetDisplayGeoInfo);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetInstanceTransformsOnPart","session","nodeId","partId","rst_order","transforms","start","length"),&HAPI::GetInstanceTransformsOnPart);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetInstancedObjectIds","session","nodeId","objectIds","start","length"),&HAPI::GetInstancedObjectIds);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetInstancedPartIds","session","nodeId","partId","partIds","start","length"),&HAPI::GetInstancedPartIds);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetInstancerPartTransforms", "session", "nodeId", "partId","rst_order","transforms","start","length"),&HAPI::GetInstancerPartTransforms);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetLastCookError","session"),&HAPI::GetLastCookError);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetLastError","session"),&HAPI::GetLastError);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetNodeCookResult","session","stringValue", "length"),&HAPI::GetNodeCookResult);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetNodeCookResultLength","session","nodeId","verbosity","length"),&HAPI::GetNodeCookResultLength);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetNodeFromPath","session","parentId","path","nodeId"),&HAPI::GetNodeFromPath);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetNodeInputName","session","nodeId","inputIndex","nameHandle"),&HAPI::GetNodeInputName);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetNodeOutputName","session","nodeId","outputIndex","nameHandle"),&HAPI::GetNodeOutputName);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetNodePath","session","nodeId","relativeToNodeId","pathHandle"),&HAPI::GetNodePath);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetObjectTransform","session","nodeId","relativeToNodeId","rst_order","transform"),&HAPI::GetObjectTransform);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetOutputGeoCount","session","nodeId","count"),&HAPI::GetOutputGeoCount);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetOutputGeoInfos","session","nodeId","geoInfos","count"),&HAPI::GetOutputGeoInfos);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetOutputNodeId","session","nodeId","output","outputNodeId"),&HAPI::GetOutputNodeId);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetParameters","session","nodeId","parameters","start","length"),&HAPI::GetParameters);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetParmFloatValue","session","nodeId","parmName","index","value"),&HAPI::GetParmFloatValue);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetParmIdFromName","session","nodeId","parmName","parmId"),&HAPI::GetParmIdFromName);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetParmIntValue","session","nodeId","parmName","index","value"),&HAPI::GetParmIntValue);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetParmNodeValue","session","nodeId","parmName","value"),&HAPI::GetParmNodeValue);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetParmStringValue","session","nodeId","parmName","index","evaluate","value"),&HAPI::GetParmStringValue);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetStatus","session","statusType","status"),&HAPI::GetStatus);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetStatusString","session","statusType"),&HAPI::GetStatusString);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("GetString","session","stringHandle"),&HAPI::GetString);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("Initialize","session","cookOptions","useCookingThread","cookingThreadStackSize","houdiniEnvironmentFiles","otlSearchPath","dsoSearchPath","imageDsoSearchPath","audioDsoSearchPath"),&HAPI::Initialize);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("Interrupt","session"),&HAPI::Interrupt);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("IsInitialized","session"),&HAPI::IsInitialized);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("IsNodeValid","session","nodeId","uniqueNodeId","answer"),&HAPI::IsNodeValid);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("LoadAssetLibraryFromFile","session","filePath","allowOverwrite","assetId"),&HAPI::LoadAssetLibraryFromFile);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("LoadAssetLibraryFromMemory","session","buffer","allowOverwrite", "assetId"),&HAPI::LoadAssetLibraryFromMemory);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("LoadGeoFromFile","session","nodeId","filePath"),&HAPI::LoadGeoFromFile);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("LoadGeoFromMemory","session","nodeId","format","buffer"),&HAPI::LoadGeoFromMemory);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("LoadHIPFile","session","filePath","cookOnLoad"),&HAPI::LoadHIPFile);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("LoadNodeFromFile","session","fileName","parentId","nodeLabel","cookOnLoad","newNodeId"),&HAPI::LoadNodeFromFile);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("MaterialInfo_Create"),&HAPI::MaterialInfo_Create);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("NodeInfo_Create"),&HAPI::NodeInfo_Create);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("ObjectInfo_Create"),&HAPI::ObjectInfo_Create);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("ParmInfo_Create"),&HAPI::ParmInfo_Create);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("PartInfo_Create"),&HAPI::PartInfo_Create);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("SessionInfo_Create"),&HAPI::SessionInfo_Create);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("RemoveCustomString","session","stringHandle"),&HAPI::RemoveCustomString);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("RenameNode","session","nodeId","newName"),&HAPI::RenameNode);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("RevertGeo","session","nodeId"),&HAPI::RevertGeo);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("RevertParmToDefault","session","nodeId","parmName","index"),&HAPI::RevertParmToDefault);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("RevertParmToDefaults","session","nodeId","parmName"),&HAPI::RevertParmToDefaults);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("SaveGeoToFile","session","nodeId","filePath"),&HAPI::SaveGeoToFile);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("SaveToHip","session","filename"),&HAPI::SaveToHip);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("SetCustomString","session","string_value","handle_value"),&HAPI::SetCustomString);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("SetAttributeFloatData","session","nodeId","partId","name","attrInfo","dataArray","start","length"),&HAPI::SetAttributeFloatData);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("SetAttributeIntData","session","nodeId","partId","name","attrInfo","dataArray","start","length"),&HAPI::SetAttributeIntData);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("SetAttributeStringData","session","nodeId","partId","name","attrInfo","dataArray","start","length"),&HAPI::SetAttributeStringData);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("SetCurveCounts","session","nodeId","partId","curveCounts","start","length"),&HAPI::SetCurveCounts);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("SetCurveInfo","session","nodeId","partId","curveInfo"),&HAPI::SetCurveInfo);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("SetCurveKnots","session","nodeId","partId","knots","start","length"),&HAPI::SetCurveKnots);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("SetCurveOrders","session","nodeId","partId","orders","start","length"),&HAPI::SetCurveOrders);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("SetFaceCounts","session","nodeId","partId","faceCounts","start","length"),&HAPI::SetFaceCounts);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("SetInputCurveInfo","session","nodeId","partId","curveInfo"),&HAPI::SetInputCurveInfo);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("SetInputCurvePositions","session","nodeId","partId","positions","start","length"),&HAPI::SetInputCurvePositions);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("SetNodeDisplay","session","nodeId","onOff"),&HAPI::SetNodeDisplay);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("SetObjectTransform","session","nodeId","transform"),&HAPI::SetObjectTransform);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("SetServerEnvInt","session","envName","value"),&HAPI::SetServerEnvInt);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("SetServerEnvString","session","envName","value"),&HAPI::SetServerEnvString);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("SetSessionSync","session","enable"),&HAPI::SetSessionSync);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("SetSessionSyncInfo","session","syncInfo"),&HAPI::SetSessionSyncInfo);
        godot::ClassDB::bind_static_method("HAPI",godot::D_METHOD("SaveNodeToFile","session","nodeId","fileName"),&HAPI::SaveNodeToFile);
    }
public:
    GDE_EXPORT static HAPI_SessionInfo  DictToSessionInfo(godot::Dictionary objectInfo);
    GDE_EXPORT static godot::Dictionary SessionInfoToDict(HAPI_SessionInfo info);
    GDE_EXPORT static HAPI_ObjectInfo  DictToObjectInfo(godot::Dictionary objectInfo);
    GDE_EXPORT static godot::Dictionary ObjectInfoToDict(HAPI_ObjectInfo info);
    GDE_EXPORT static HAPI_AssetInfo DictToAssetInfo(godot::Dictionary assetInfo);
    GDE_EXPORT static godot::Dictionary AssetInfoToDict(HAPI_AssetInfo info);
    GDE_EXPORT static HAPI_NodeInfo DictToNodeInfo(godot::Dictionary nodeInfo);
    GDE_EXPORT static godot::Dictionary NodeInfoToDict(HAPI_NodeInfo info);
    GDE_EXPORT static HAPI_GeoInfo DictToGeoInfo(godot::Dictionary geoInfo);
    GDE_EXPORT static godot::Dictionary GeoInfoToDict(HAPI_GeoInfo info);
    GDE_EXPORT static HAPI_MaterialInfo DictToMaterialInfo(godot::Dictionary matInfo);
    GDE_EXPORT static godot::Dictionary MaterialInfoToDict(HAPI_MaterialInfo info);
    GDE_EXPORT static HAPI_PartInfo DictToPartInfo(godot::Dictionary partInfo);
    GDE_EXPORT static godot::Dictionary PartInfoToDict(HAPI_PartInfo info);
    GDE_EXPORT static HAPI_AttributeInfo DictToAttrInfo(godot::Dictionary attrInfo);
    GDE_EXPORT static godot::Dictionary AttrInfoToDict(HAPI_AttributeInfo info);
    GDE_EXPORT static HAPI_ParmInfo DictToParmInfo(godot::Dictionary parmInfo);
    GDE_EXPORT static godot::Dictionary ParmInfoToDict(HAPI_ParmInfo info);
    GDE_EXPORT static HAPI_CookOptions DictToCookOptions(godot::Dictionary cookOptions);
    GDE_EXPORT static godot::Dictionary CookOptionsToDict(HAPI_CookOptions options);
    GDE_EXPORT static HAPI_Transform DictToTransform(godot::Dictionary transform);
    GDE_EXPORT static godot::Dictionary TransformToDict(HAPI_Transform transform);
    GDE_EXPORT static HAPI_TransformEuler DictToTransformEuler(godot::Dictionary transform);
    GDE_EXPORT static godot::Dictionary TransformEulerToDict(HAPI_TransformEuler transform);
    GDE_EXPORT static HAPI_CurveInfo DictToCurveInfo(godot::Dictionary inputCurveInfo);
    GDE_EXPORT static godot::Dictionary CurveInfoToDict(HAPI_CurveInfo info);
    GDE_EXPORT static HAPI_InputCurveInfo DictToInputCurveInfo(godot::Dictionary inputCurveInfo);
    GDE_EXPORT static godot::Dictionary InputCurveInfoToDict(HAPI_InputCurveInfo info);
    GDE_EXPORT static HAPI_SessionSyncInfo DictToSessionSyncInfo(godot::Dictionary sessionSyncInfo);
    GDE_EXPORT static godot::Dictionary SessionSyncInfoToDict(HAPI_SessionSyncInfo info);


    GDE_EXPORT static godot::Dictionary GetObjectInfo(godot::Ref<HESession> session,int nodeId);
    GDE_EXPORT static godot::Dictionary GetAssetInfo(godot::Ref<HESession> session, int nodeId);
    GDE_EXPORT static godot::Dictionary GetNodeInfo(godot::Ref<HESession> session, int nodeId);
    GDE_EXPORT static godot::Dictionary GetGeoInfo(godot::Ref<HESession> session, int nodeId);
    GDE_EXPORT static godot::Dictionary GetMaterialInfo(godot::Ref<HESession> session, int nodeId);
    GDE_EXPORT static godot::Dictionary GetPartInfo(godot::Ref<HESession> session, int nodeId, int partId);
    GDE_EXPORT static godot::Dictionary GetAttrInfo(godot::Ref<HESession> session,int nodeId,int partId,godot::String name,HAPI_AttributeOwner owner);
    GDE_EXPORT static godot::Dictionary GetParmInfo(godot::Ref<HESession> session,int nodeId,int parmId, godot::Ref<RefDictionary> parmInfo);
    GDE_EXPORT static HAPI_Result AddAttribute(godot::Ref<HESession> session,int nodeId,int partId,godot::String name,godot::Ref<RefDictionary> attrInfo);
    GDE_EXPORT static HAPI_Result Cleanup(godot::Ref<HESession> session);
    GDE_EXPORT static HAPI_Result ClearConnectionError();
    GDE_EXPORT static HAPI_Result CloseSession(godot::Ref<HESession> session);
    GDE_EXPORT static HAPI_Result CommitGeo(godot::Ref<HESession> session,int nodeId);
    GDE_EXPORT static HAPI_Result ConnectNodeInput(godot::Ref<HESession> session, int nodeId, int inputIndex, int nodeId_toConnect, int outputIndex);
    GDE_EXPORT static HAPI_Result CookNode(godot::Ref<HESession> session, int nodeId, godot::Ref<RefDictionary> cookOptions);
    GDE_EXPORT static godot::Dictionary CookOptions_Create();
    GDE_EXPORT static HAPI_Result CreateInProcessSession(godot::Ref<HESession> session,godot::Ref<RefDictionary> sessionInfo);
    GDE_EXPORT static HAPI_Result CreateInputNode(godot::Ref<HESession> session, int parentId, godot::Ref<Int> nodeId,godot::String name);
    GDE_EXPORT static HAPI_Result CreateNode(godot::Ref<HESession> session, int parentId, godot::String operatorName, godot::String nodeLabel, bool cookOnCreation, godot::Ref<Int> nodeId);
    GDE_EXPORT static HAPI_Result CreateThriftNamedPipeSession(godot::Ref<HESession> session, godot::String pipeName, godot::Ref<RefDictionary> sessionInfo);
    GDE_EXPORT static HAPI_Result CreateThriftSharedMemorySession(godot::Ref<HESession> session, godot::String sharedMemName, godot::Ref<RefDictionary> sessionInfo);
    GDE_EXPORT static HAPI_Result CreateThriftSocketSession(godot::Ref<HESession> session, godot::String hostName, int port, godot::Ref<RefDictionary> sessionInfo);
    GDE_EXPORT static HAPI_Result DeleteAttribute(godot::Ref<HESession> session, int nodeId, int partId, godot::String name, godot::Ref<RefDictionary> attrInfo);
    GDE_EXPORT static HAPI_Result DeleteNode(godot::Ref<HESession> session, int nodeId);
    GDE_EXPORT static HAPI_Result DisconnectNodeInput(godot::Ref<HESession> session, int nodeId, int inputIndex);
    GDE_EXPORT static godot::Dictionary GeoInfo_Create();
    GDE_EXPORT static HAPI_Result GetAttributeFloatData(godot::Ref<HESession> session, int nodeId, int partId, godot::String name, godot::Ref<RefDictionary> attrInfo, int stride, godot::Ref<RefArray> dataArray, int start, int length);
    GDE_EXPORT static HAPI_Result GetAttributeIntData(godot::Ref<HESession> session, int nodeId, int partId, godot::String name, godot::Ref<RefDictionary> attrInfo, int stride, godot::Ref<RefArray> dataArray, int start, int length);
    GDE_EXPORT static HAPI_Result GetAttributeStringData(godot::Ref<HESession> session, int nodeId, int partId, godot::String name, godot::Ref<RefDictionary> attrInfo, godot::Ref<RefArray> dataArray, int start, int length);
    GDE_EXPORT static godot::String GetConnectionError();
    GDE_EXPORT static int GetCookingCurrentCount(godot::Ref<HESession> session);
    GDE_EXPORT static int GetCookingTotalCount(godot::Ref<HESession> session);
    GDE_EXPORT static godot::Dictionary GetDisplayGeoInfo(godot::Ref<HESession> session, int nodeId);
    GDE_EXPORT static HAPI_Result GetInstanceTransformsOnPart(godot::Ref<HESession> session, int nodeId, int partId, HAPI_RSTOrder rst_order, godot::Ref<RefArray> transforms, int start, int length);
    GDE_EXPORT static HAPI_Result GetInstancedObjectIds(godot::Ref<HESession> session, int nodeId, godot::Ref<RefArray> objectIds, int start, int length);
    GDE_EXPORT static HAPI_Result GetInstancedPartIds(godot::Ref<HESession> session, int nodeId, int partId, godot::Ref<RefArray> partIds, int start, int length);
    GDE_EXPORT static HAPI_Result GetInstancerPartTransforms(godot::Ref<HESession> session, int nodeId, int partId, HAPI_RSTOrder rst_order, godot::Ref<RefArray> transforms, int start, int length);
    GDE_EXPORT static godot::String GetLastCookError(godot::Ref<HESession> session = nullptr);
    GDE_EXPORT static godot::String GetLastError(godot::Ref<HESession> session = nullptr);
    GDE_EXPORT static HAPI_Result GetNodeCookResult(godot::Ref<HESession> session, godot::String stringValue, int length);
    GDE_EXPORT static HAPI_Result GetNodeCookResultLength(godot::Ref<HESession> session, int nodeId, HAPI_StatusVerbosity verbosity, godot::Ref<Int> length);
    GDE_EXPORT static HAPI_Result GetNodeFromPath(godot::Ref<HESession> session, int parentId, godot::String path, godot::Ref<Int> nodeId);
    GDE_EXPORT static HAPI_Result GetNodeInputName(godot::Ref<HESession> session, int nodeId, int inputIndex, godot::Ref<Int> nameHandle);
    GDE_EXPORT static HAPI_Result GetNodeOutputName(godot::Ref<HESession> session, int nodeId, int outputIndex, godot::Ref<Int> nameHandle);
    GDE_EXPORT static HAPI_Result GetNodePath(godot::Ref<HESession> session, int nodeId, int relativeToNodeId, godot::Ref<Int> pathHandle);
    GDE_EXPORT static HAPI_Result GetObjectTransform(godot::Ref<HESession> session, int nodeId, int relativeToNodeId, HAPI_RSTOrder rst_order, godot::Ref<RefDictionary> transform);
    GDE_EXPORT static HAPI_Result GetOutputGeoCount(godot::Ref<HESession> session, int nodeId, godot::Ref<Int> count);
    GDE_EXPORT static HAPI_Result GetOutputGeoInfos(godot::Ref<HESession> session, int nodeId, godot::Ref<RefArray> geoInfos, int count);
    GDE_EXPORT static HAPI_Result GetOutputNodeId(godot::Ref<HESession> session, int nodeId, int output, godot::Ref<Int> outputNodeId);
    GDE_EXPORT static HAPI_Result GetParameters(godot::Ref<HESession> session, int nodeId, godot::Ref<RefArray> parameters, int start, int length);
    GDE_EXPORT static HAPI_Result GetParmFloatValue(godot::Ref<HESession> session, int nodeId, godot::String parmName, int index, godot::Ref<Float> value);
    GDE_EXPORT static HAPI_Result GetParmIdFromName(godot::Ref<HESession> session, int nodeId, godot::String parmName, godot::Ref<Int> parmId);
    GDE_EXPORT static HAPI_Result GetParmIntValue(godot::Ref<HESession> session, int nodeId, godot::String parmName, int index, godot::Ref<Int> value);
    GDE_EXPORT static HAPI_Result GetParmNodeValue(godot::Ref<HESession> session, int nodeId, godot::String parmName, godot::Ref<Int> value);
    GDE_EXPORT static HAPI_Result GetParmStringValue(godot::Ref<HESession> session, int nodeId, godot::String parmName, int index, bool evaluate, godot::Ref<Int> value);
    GDE_EXPORT static HAPI_Result GetStatus(godot::Ref<HESession> session, HAPI_StatusType statusType, godot::Ref<Int> status);
    GDE_EXPORT static godot::String GetStatusString(godot::Ref<HESession> session, HAPI_StatusType status_type);
    GDE_EXPORT static godot::String GetString(godot::Ref<HESession> session, int stringHandle);
    GDE_EXPORT static HAPI_Result Initialize(godot::Ref<HESession> session, godot::Dictionary cookOptions, bool useCookingThread, int cookingThreadStackSize, godot::String houdiniEnvironmentFiles, godot::String otlSearchPath, godot::String dsoSearchPath, godot::String imageDsoSearchPath, godot::String audioDsoSearchPath);
    GDE_EXPORT static HAPI_Result Interrupt(godot::Ref<HESession> session);
    GDE_EXPORT static HAPI_Result IsInitialized(godot::Ref<HESession> session);
    GDE_EXPORT static HAPI_Result IsSessionValid(godot::Ref<HESession> session);
    GDE_EXPORT static HAPI_Result IsNodeValid(godot::Ref<HESession> session, int nodeId, int uniqueNodeId, godot::Ref<Bool> answer);
    GDE_EXPORT static HAPI_Result LoadAssetLibraryFromFile(godot::Ref<HESession> session, godot::String filePath, bool allowOverwrite, godot::Ref<Int> assetId);
    GDE_EXPORT static HAPI_Result LoadAssetLibraryFromMemory(godot::Ref<HESession> session, godot::String buffer, bool allowOverwrite, godot::Ref<Int> assetId);
    GDE_EXPORT static HAPI_Result LoadGeoFromFile(godot::Ref<HESession> session, int nodeId, godot::String filePath);
    GDE_EXPORT static HAPI_Result LoadGeoFromMemory(godot::Ref<HESession> session, int nodeId, godot::String format, godot::String buffer);
    GDE_EXPORT static HAPI_Result LoadHIPFile(godot::Ref<HESession> session, godot::String fileName, bool cookOnLoad);
    GDE_EXPORT static HAPI_Result LoadNodeFromFile(godot::Ref<HESession> session, godot::String fileName, int parentId, godot::String nodeLabel, bool cookOnLoad, godot::Ref<Int> newNodeId);
    GDE_EXPORT static godot::Dictionary MaterialInfo_Create();
    GDE_EXPORT static godot::Dictionary NodeInfo_Create();
    GDE_EXPORT static godot::Dictionary ObjectInfo_Create();
    GDE_EXPORT static godot::Dictionary ParmInfo_Create();
    GDE_EXPORT static godot::Dictionary PartInfo_Create();
    GDE_EXPORT static godot::Dictionary SessionInfo_Create();
    GDE_EXPORT static HAPI_Result RemoveCustomString(godot::Ref<HESession> session, int stringHandle);
    GDE_EXPORT static HAPI_Result RenameNode(godot::Ref<HESession> session, int nodeId, godot::String newName);
    GDE_EXPORT static HAPI_Result RevertGeo(godot::Ref<HESession> session, int nodeId);
    GDE_EXPORT static HAPI_Result RevertParmToDefault(godot::Ref<HESession> session, int nodeId, godot::String parmName, int index);
    GDE_EXPORT static HAPI_Result RevertParmToDefaults(godot::Ref<HESession> session, int nodeId, godot::String parmName);
    GDE_EXPORT static HAPI_Result SaveGeoToFile(godot::Ref<HESession> session, int nodeId, godot::String filePath);
    GDE_EXPORT static bool SaveToHip(godot::Ref<HESession> session, godot::String filename);
    GDE_EXPORT static HAPI_Result SaveNodeToFile(godot::Ref<HESession> session, int nodeId, godot::String fileName);
    GDE_EXPORT static HAPI_Result SetAttributeFloatData(godot::Ref<HESession> session, int nodeId, int partId, godot::String name, godot::Dictionary attrInfo, godot::Ref<RefArray> dataArray, int start, int length);
    GDE_EXPORT static HAPI_Result SetAttributeIntData(godot::Ref<HESession> session, int nodeId, int partId, godot::String name, godot::Dictionary attrInfo, godot::Ref<RefArray> dataArray, int start, int length);
    GDE_EXPORT static HAPI_Result SetAttributeStringData(godot::Ref<HESession> session, int nodeId, int partId, godot::String name, godot::Dictionary attrInfo, godot::Ref<RefArray> dataArray, int start, int length);
    GDE_EXPORT static HAPI_Result SetCurveCounts(godot::Ref<HESession> session, int nodeId, int partId, godot::Array curveCounts, int start, int length);
    GDE_EXPORT static HAPI_Result SetCurveInfo(godot::Ref<HESession> session, int nodeId, int partId, godot::Dictionary curveInfo);
    GDE_EXPORT static HAPI_Result SetCurveKnots(godot::Ref<HESession> session, int nodeId, int partId, godot::Array knots, int start, int length);
    GDE_EXPORT static HAPI_Result SetCurveOrders(godot::Ref<HESession> session, int nodeId, int partId, godot::Array orders, int start, int length);
    GDE_EXPORT static HAPI_Result SetCustomString(godot::Ref<HESession> session, godot::String string_value, godot::Ref<Int> handle_value);
    GDE_EXPORT static HAPI_Result SetFaceCounts(godot::Ref<HESession> session, int nodeId, int partId, godot::Array faceCounts, int start, int length);
    GDE_EXPORT static HAPI_Result SetInputCurveInfo(godot::Ref<HESession> session, int nodeId, int partId, godot::Dictionary curveInfo);
    GDE_EXPORT static HAPI_Result SetInputCurvePositions(godot::Ref<HESession> session, int nodeId, int partId, godot::Array positions, int start, int length);
    GDE_EXPORT static HAPI_Result SetNodeDisplay(godot::Ref<HESession> session, int nodeId, int onOff);
    GDE_EXPORT static HAPI_Result SetObjectTransform(godot::Ref<HESession> session, int nodeId, godot::Dictionary transform);
    GDE_EXPORT static HAPI_Result SetServerEnvInt(godot::Ref<HESession> session, godot::String envName, int value);
    GDE_EXPORT static HAPI_Result SetServerEnvString(godot::Ref<HESession> session, godot::String envName, godot::String value);
    GDE_EXPORT static HAPI_Result SetSessionSync(godot::Ref<HESession> session, bool enable);
    GDE_EXPORT static HAPI_Result SetSessionSyncInfo(godot::Ref<HESession> session, godot::Dictionary syncInfo);
};
#endif 