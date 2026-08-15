#ifndef HOUDINI_ENGINE_BIND
#define HOUDINI_ENGINE_BIND
#include "DebugSymbol.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/object.hpp>
#include <HoudiniApi.h>
#include <HoudiniEngineUtility.h>
#include <HoudiniEnginePlatform.h>
#include "HEGUtility.h"
#include "HEGClasses.h"
#include "HEGEnum.h"



class HEGBool: public godot::RefCounted{
GDCLASS(HEGBool, godot::RefCounted)
    void set_value(bool value){
        this->value = value;
    }
    bool get_value(){
        return value;
    }
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("set_value","value"),&HEGBool::set_value);
        godot::ClassDB::bind_method(godot::D_METHOD("get_value"),&HEGBool::get_value);
        godot::ClassDB::add_property("HEGBool",godot::PropertyInfo(godot::Variant::BOOL,"value"),"set_value","get_value");
    }
public:
    bool value;
    HEGBool(){}
    HEGBool(bool value){
        this->value = value;
    }
};
class HEGInt: public godot::RefCounted{
GDCLASS(HEGInt, godot::RefCounted)
    void set_value(int64_t value){
        this->value = value;
    }
    int64_t get_value(){
        return value;
    }
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("set_value","value"),&HEGInt::set_value);
        godot::ClassDB::bind_method(godot::D_METHOD("get_value"),&HEGInt::get_value);
        godot::ClassDB::add_property("HEGInt",godot::PropertyInfo(godot::Variant::INT,"value"),"set_value","get_value");
    }
public:
    intmax_t value;
    HEGInt(){}
    template<typename T> requires std::integral<T>
    HEGInt(T value){
        this->value = value;
    }
};
class HEGFloat: public godot::RefCounted{
GDCLASS(HEGFloat, godot::RefCounted)
    void set_value(double value){
        this->value = value;
    }
    double get_value(){
        return value;
    }
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("set_value","value"),&HEGFloat::set_value);
        godot::ClassDB::bind_method(godot::D_METHOD("get_value"),&HEGFloat::get_value);
        godot::ClassDB::add_property("HEGFloat",godot::PropertyInfo(godot::Variant::FLOAT,"value"),"set_value","get_value");
    }
public:
    float value;
    HEGFloat(){}
    template<typename T> requires std::floating_point<T>
    HEGFloat(T value){
        this->value = value;
    }
};
class HEGRefArray: public godot::RefCounted{
    GDCLASS(HEGRefArray,RefCounted)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("set_value","value"),&HEGRefArray::set_value);
        godot::ClassDB::bind_method(godot::D_METHOD("get_value"),&HEGRefArray::get_value);
        godot::ClassDB::add_property("HEGRefArray",godot::PropertyInfo(godot::Variant::DICTIONARY,"value"),"set_value","get_value");
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
    HEGRefArray(T&&... v){
        value = godot::Array(std::forward<decltype(v)>(v)...);
    }
};
class HEGRefDictionary: public godot::RefCounted{
    GDCLASS(HEGRefDictionary,RefCounted)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("set_value","value"),&HEGRefDictionary::set_value);
        godot::ClassDB::bind_method(godot::D_METHOD("get_value"),&HEGRefDictionary::get_value);
        godot::ClassDB::add_property("HEGRefDictionary",godot::PropertyInfo(godot::Variant::DICTIONARY,"value"),"set_value","get_value");
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
    HEGRefDictionary(T&&... v){
        value = godot::Dictionary(std::forward<decltype(v)>(v)...);
    }
};
class HEGHapi: public godot::Object{
    GDCLASS(HEGHapi, godot::Object)
    static void _bind_methods(){
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetObjectInfo","session","nodeId"),&HEGHapi::GetObjectInfo);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetAssetInfo","session","nodeId"),&HEGHapi::GetAssetInfo);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetNodeInfo","session","nodeId"),&HEGHapi::GetNodeInfo);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetGeoInfo","session","nodeId"),&HEGHapi::GetGeoInfo);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetMaterialInfo","session","nodeId"),&HEGHapi::GetMaterialInfo);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetPartInfo","session","nodeId","partId"),&HEGHapi::GetPartInfo);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetAttrInfo","session","nodeId","partId","name","owner"),&HEGHapi::GetAttrInfo);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetParmInfo","session","nodeId","parmId","parmInfo"),&HEGHapi::GetParmInfo);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("AddAttribute","session","nodeId","partId","name","attrInfo"),&HEGHapi::AddAttribute);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("Cleanup","session"),&HEGHapi::Cleanup);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("ClearConnectionError"),&HEGHapi::ClearConnectionError);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("CloseSession","session"),&HEGHapi::CloseSession);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("CommitGeo","session","nodeId"),&HEGHapi::CommitGeo);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("ConnectNodeInput","session","nodeId","inputIndex","nodeId_toConnect","outputIndex"),&HEGHapi::ConnectNodeInput);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("CookNode","session","nodeId","cookOptions"),&HEGHapi::CookNode);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("CookOptions_Create"),&HEGHapi::CookOptions_Create);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("CreateInProcessSession","session","sessionInfo"),&HEGHapi::CreateInProcessSession);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("CreateInputNode","session","parentId","nodeId","name"),&HEGHapi::CreateInputNode);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("CreateNode","session","parentId","operatorName","nodeLabel","cookOnCreation","nodeId"),&HEGHapi::CreateNode);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("CreateThriftNamedPipeSession","session","pipeName","sessionInfo"),&HEGHapi::CreateThriftNamedPipeSession);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("CreateThriftSharedMemorySession","session","sharedMemName","sessionInfo"),&HEGHapi::CreateThriftSharedMemorySession);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("CreateThriftSocketSession","session","hostName","port","sessionInfo"),&HEGHapi::CreateThriftSocketSession);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("DeleteAttribute","session","nodeId","partId","name","attrInfo"),&HEGHapi::DeleteAttribute);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("DeleteNode","session","nodeId"),&HEGHapi::DeleteNode);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("DisconnectNodeInput","session","nodeId","inputIndex"),&HEGHapi::DisconnectNodeInput);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GeoInfo_Create"),&HEGHapi::GeoInfo_Create);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetAttributeFloatData","session","nodeId","partId","name","owner"),&HEGHapi::GetAttributeFloatData);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetAttributeIntData","session","nodeId","partId","name","owner"),&HEGHapi::GetAttributeIntData);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetAttributeStringData","session","nodeId","partId","name","owner"),&HEGHapi::GetAttributeStringData);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetConnectionError"),&HEGHapi::GetConnectionError);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetCookingCurrentCount","session"),&HEGHapi::GetCookingCurrentCount);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetCookingTotalCount","session"),&HEGHapi::GetCookingTotalCount);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetDisplayGeoInfo","session","nodeId"),&HEGHapi::GetDisplayGeoInfo);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetInstanceTransformsOnPart","session","nodeId","partId","rst_order","transforms","start","length"),&HEGHapi::GetInstanceTransformsOnPart);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetInstancedObjectIds","session","nodeId","objectIds","start","length"),&HEGHapi::GetInstancedObjectIds);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetInstancedPartIds","session","nodeId","partId","partIds","start","length"),&HEGHapi::GetInstancedPartIds);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetInstancerPartTransforms", "session", "nodeId", "partId","rst_order","transforms","start","length"),&HEGHapi::GetInstancerPartTransforms);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetLastCookError","session"),&HEGHapi::GetLastCookError);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetLastError","session"),&HEGHapi::GetLastError);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetNodeCookResult","session","nodeId","verbosity"),&HEGHapi::GetNodeCookResult);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetNodeCookResultLength","session","nodeId","verbosity","length"),&HEGHapi::GetNodeCookResultLength);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetNodeFromPath","session","parentId","path","nodeId"),&HEGHapi::GetNodeFromPath);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetNodeInputName","session","nodeId","inputIndex","nameHandle"),&HEGHapi::GetNodeInputName);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetNodeOutputName","session","nodeId","outputIndex","nameHandle"),&HEGHapi::GetNodeOutputName);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetNodePath","session","nodeId","relativeToNodeId","pathHandle"),&HEGHapi::GetNodePath);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetObjectTransform","session","nodeId","relativeToNodeId","rst_order","transform"),&HEGHapi::GetObjectTransform);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetOutputGeoCount","session","nodeId","count"),&HEGHapi::GetOutputGeoCount);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetOutputGeoInfos","session","nodeId","geoInfos","count"),&HEGHapi::GetOutputGeoInfos);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetOutputNodeId","session","nodeId","output","outputNodeId"),&HEGHapi::GetOutputNodeId);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetParameters","session","nodeId","parameters","start","length"),&HEGHapi::GetParameters);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetParmFloatValue","session","nodeId","parmName","index","value"),&HEGHapi::GetParmFloatValue);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetParmIdFromName","session","nodeId","parmName","parmId"),&HEGHapi::GetParmIdFromName);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetParmIntValue","session","nodeId","parmName","index","value"),&HEGHapi::GetParmIntValue);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetParmNodeValue","session","nodeId","parmName","value"),&HEGHapi::GetParmNodeValue);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetParmStringValue","session","nodeId","parmName","index","evaluate","value"),&HEGHapi::GetParmStringValue);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetStatus","session","statusType","status"),&HEGHapi::GetStatus);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetStatusString","session","statusType"),&HEGHapi::GetStatusString);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("GetString","session","stringHandle"),&HEGHapi::GetString);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("Initialize","session","cookOptions","useCookingThread","cookingThreadStackSize","houdiniEnvironmentFiles","otlSearchPath","dsoSearchPath","imageDsoSearchPath","audioDsoSearchPath"),&HEGHapi::Initialize);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("Interrupt","session"),&HEGHapi::Interrupt);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("IsInitialized","session"),&HEGHapi::IsInitialized);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("IsNodeValid","session","nodeId","uniqueNodeId","answer"),&HEGHapi::IsNodeValid);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("LoadAssetLibraryFromFile","session","filePath","allowOverwrite","assetId"),&HEGHapi::LoadAssetLibraryFromFile);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("LoadAssetLibraryFromMemory","session","buffer","allowOverwrite", "assetId"),&HEGHapi::LoadAssetLibraryFromMemory);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("LoadGeoFromFile","session","nodeId","filePath"),&HEGHapi::LoadGeoFromFile);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("LoadGeoFromMemory","session","nodeId","format","buffer"),&HEGHapi::LoadGeoFromMemory);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("LoadHIPFile","session","filePath","cookOnLoad"),&HEGHapi::LoadHIPFile);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("LoadNodeFromFile","session","fileName","parentId","nodeLabel","cookOnLoad","newNodeId"),&HEGHapi::LoadNodeFromFile);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("MaterialInfo_Create"),&HEGHapi::MaterialInfo_Create);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("NodeInfo_Create"),&HEGHapi::NodeInfo_Create);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("ObjectInfo_Create"),&HEGHapi::ObjectInfo_Create);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("ParmInfo_Create"),&HEGHapi::ParmInfo_Create);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("PartInfo_Create"),&HEGHapi::PartInfo_Create);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("SessionInfo_Create"),&HEGHapi::SessionInfo_Create);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("RemoveCustomString","session","stringHandle"),&HEGHapi::RemoveCustomString);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("RenameNode","session","nodeId","newName"),&HEGHapi::RenameNode);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("RevertGeo","session","nodeId"),&HEGHapi::RevertGeo);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("RevertParmToDefault","session","nodeId","parmName","index"),&HEGHapi::RevertParmToDefault);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("RevertParmToDefaults","session","nodeId","parmName"),&HEGHapi::RevertParmToDefaults);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("SaveGeoToFile","session","nodeId","filePath"),&HEGHapi::SaveGeoToFile);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("SaveToHip","session","filename"),&HEGHapi::SaveToHip);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("SetCustomString","session","string_value","handle_value"),&HEGHapi::SetCustomString);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("SetAttributeFloatData","session","nodeId","partId","name","attrInfo","dataArray","start","length"),&HEGHapi::SetAttributeFloatData);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("SetAttributeIntData","session","nodeId","partId","name","attrInfo","dataArray","start","length"),&HEGHapi::SetAttributeIntData);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("SetAttributeStringData","session","nodeId","partId","name","attrInfo","dataArray","start","length"),&HEGHapi::SetAttributeStringData);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("SetCurveCounts","session","nodeId","partId","curveCounts","start","length"),&HEGHapi::SetCurveCounts);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("SetCurveInfo","session","nodeId","partId","curveInfo"),&HEGHapi::SetCurveInfo);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("SetCurveKnots","session","nodeId","partId","knots","start","length"),&HEGHapi::SetCurveKnots);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("SetCurveOrders","session","nodeId","partId","orders","start","length"),&HEGHapi::SetCurveOrders);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("SetFaceCounts","session","nodeId","partId","faceCounts","start","length"),&HEGHapi::SetFaceCounts);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("SetInputCurveInfo","session","nodeId","partId","curveInfo"),&HEGHapi::SetInputCurveInfo);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("SetInputCurvePositions","session","nodeId","partId","positions","start","length"),&HEGHapi::SetInputCurvePositions);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("SetNodeDisplay","session","nodeId","onOff"),&HEGHapi::SetNodeDisplay);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("SetObjectTransform","session","nodeId","transform"),&HEGHapi::SetObjectTransform);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("SetServerEnvInt","session","envName","value"),&HEGHapi::SetServerEnvInt);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("SetServerEnvString","session","envName","value"),&HEGHapi::SetServerEnvString);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("SetSessionSync","session","enable"),&HEGHapi::SetSessionSync);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("SetSessionSyncInfo","session","syncInfo"),&HEGHapi::SetSessionSyncInfo);
        godot::ClassDB::bind_static_method("HEGHapi",godot::D_METHOD("SaveNodeToFile","session","nodeId","fileName"),&HEGHapi::SaveNodeToFile);
    }
public:
    static HAPI_SessionInfo  DictToSessionInfo(godot::Dictionary objectInfo);
    static godot::Dictionary SessionInfoToDict(HAPI_SessionInfo info);
    static HAPI_ObjectInfo  DictToObjectInfo(godot::Dictionary objectInfo);
    static godot::Dictionary ObjectInfoToDict(HAPI_ObjectInfo info);
    static HAPI_AssetInfo DictToAssetInfo(godot::Dictionary assetInfo);
    static godot::Dictionary AssetInfoToDict(HAPI_AssetInfo info);
    static HAPI_NodeInfo DictToNodeInfo(godot::Dictionary nodeInfo);
    static godot::Dictionary NodeInfoToDict(HAPI_NodeInfo info);
    static HAPI_GeoInfo DictToGeoInfo(godot::Dictionary geoInfo);
    static godot::Dictionary GeoInfoToDict(HAPI_GeoInfo info);
    static HAPI_MaterialInfo DictToMaterialInfo(godot::Dictionary matInfo);
    static godot::Dictionary MaterialInfoToDict(HAPI_MaterialInfo info);
    static HAPI_PartInfo DictToPartInfo(godot::Dictionary partInfo);
    static godot::Dictionary PartInfoToDict(HAPI_PartInfo info);
    static HAPI_AttributeInfo DictToAttrInfo(godot::Dictionary attrInfo);
    static godot::Dictionary AttrInfoToDict(HAPI_AttributeInfo info);
    static HAPI_ParmInfo DictToParmInfo(godot::Dictionary parmInfo);
    static godot::Dictionary ParmInfoToDict(HAPI_ParmInfo info);
    static HAPI_CookOptions DictToCookOptions(godot::Dictionary cookOptions);
    static godot::Dictionary CookOptionsToDict(HAPI_CookOptions options);
    static HAPI_Transform DictToTransform(godot::Dictionary transform);
    static godot::Dictionary TransformToDict(HAPI_Transform transform);
    static HAPI_TransformEuler DictToTransformEuler(godot::Dictionary transform);
    static godot::Dictionary TransformEulerToDict(HAPI_TransformEuler transform);
    static HAPI_CurveInfo DictToCurveInfo(godot::Dictionary inputCurveInfo);
    static godot::Dictionary CurveInfoToDict(HAPI_CurveInfo info);
    static HAPI_InputCurveInfo DictToInputCurveInfo(godot::Dictionary inputCurveInfo);
    static godot::Dictionary InputCurveInfoToDict(HAPI_InputCurveInfo info);
    static HAPI_SessionSyncInfo DictToSessionSyncInfo(godot::Dictionary sessionSyncInfo);
    static godot::Dictionary SessionSyncInfoToDict(HAPI_SessionSyncInfo info);


    static godot::Dictionary GetObjectInfo(godot::Ref<HEGSession> session,int nodeId);
    static godot::Dictionary GetAssetInfo(godot::Ref<HEGSession> session, int nodeId);
    static godot::Dictionary GetNodeInfo(godot::Ref<HEGSession> session, int nodeId);
    static godot::Dictionary GetGeoInfo(godot::Ref<HEGSession> session, int nodeId);
    static godot::Dictionary GetMaterialInfo(godot::Ref<HEGSession> session, int nodeId);
    static godot::Dictionary GetPartInfo(godot::Ref<HEGSession> session, int nodeId, int partId);
    static godot::Dictionary GetAttrInfo(godot::Ref<HEGSession> session,int nodeId,int partId,godot::String name,HAPI_AttributeOwner owner);
    static godot::Dictionary GetParmInfo(godot::Ref<HEGSession> session,int nodeId,int parmId, godot::Ref<HEGRefDictionary> parmInfo);
    static HAPI_Result AddAttribute(godot::Ref<HEGSession> session,int nodeId,int partId,godot::String name,godot::Ref<HEGRefDictionary> attrInfo);
    static HAPI_Result Cleanup(godot::Ref<HEGSession> session);
    static HAPI_Result ClearConnectionError();
    static HAPI_Result CloseSession(godot::Ref<HEGSession> session);
    static HAPI_Result CommitGeo(godot::Ref<HEGSession> session,int nodeId);
    static HAPI_Result ConnectNodeInput(godot::Ref<HEGSession> session, int nodeId, int inputIndex, int nodeId_toConnect, int outputIndex);
    static HAPI_Result CookNode(godot::Ref<HEGSession> session, int nodeId, godot::Ref<HEGRefDictionary> cookOptions);
    static godot::Dictionary CookOptions_Create();
    static HAPI_Result CreateInProcessSession(godot::Ref<HEGSession> session,godot::Ref<HEGRefDictionary> sessionInfo);
    static HAPI_Result CreateInputNode(godot::Ref<HEGSession> session, int parentId, godot::Ref<HEGInt> nodeId,godot::String name);
    static HAPI_Result CreateNode(godot::Ref<HEGSession> session, int parentId, godot::String operatorName, godot::String nodeLabel, bool cookOnCreation, godot::Ref<HEGInt> nodeId);
    static HAPI_Result CreateThriftNamedPipeSession(godot::Ref<HEGSession> session, godot::String pipeName, godot::Ref<HEGRefDictionary> sessionInfo);
    static HAPI_Result CreateThriftSharedMemorySession(godot::Ref<HEGSession> session, godot::String sharedMemName, godot::Ref<HEGRefDictionary> sessionInfo);
    static HAPI_Result CreateThriftSocketSession(godot::Ref<HEGSession> session, godot::String hostName, int port, godot::Ref<HEGRefDictionary> sessionInfo);
    static HAPI_Result DeleteAttribute(godot::Ref<HEGSession> session, int nodeId, int partId, godot::String name, godot::Ref<HEGRefDictionary> attrInfo);
    static HAPI_Result DeleteNode(godot::Ref<HEGSession> session, int nodeId);
    static HAPI_Result DisconnectNodeInput(godot::Ref<HEGSession> session, int nodeId, int inputIndex);
    static godot::Dictionary GeoInfo_Create();
    static HAPI_Result GetAttributeFloatData(godot::Ref<HEGSession> session, int nodeId, int partId, godot::String name, godot::Ref<HEGRefDictionary> attrInfo, int stride, godot::Ref<HEGRefArray> dataArray, int start, int length);
    static HAPI_Result GetAttributeIntData(godot::Ref<HEGSession> session, int nodeId, int partId, godot::String name, godot::Ref<HEGRefDictionary> attrInfo, int stride, godot::Ref<HEGRefArray> dataArray, int start, int length);
    static HAPI_Result GetAttributeStringData(godot::Ref<HEGSession> session, int nodeId, int partId, godot::String name, godot::Ref<HEGRefDictionary> attrInfo, godot::Ref<HEGRefArray> dataArray, int start, int length);
    static godot::String GetConnectionError();
    static int GetCookingCurrentCount(godot::Ref<HEGSession> session);
    static int GetCookingTotalCount(godot::Ref<HEGSession> session);
    static godot::Dictionary GetDisplayGeoInfo(godot::Ref<HEGSession> session, int nodeId);
    static HAPI_Result GetInstanceTransformsOnPart(godot::Ref<HEGSession> session, int nodeId, int partId, HAPI_RSTOrder rst_order, godot::Ref<HEGRefArray> transforms, int start, int length);
    static HAPI_Result GetInstancedObjectIds(godot::Ref<HEGSession> session, int nodeId, godot::Ref<HEGRefArray> objectIds, int start, int length);
    static HAPI_Result GetInstancedPartIds(godot::Ref<HEGSession> session, int nodeId, int partId, godot::Ref<HEGRefArray> partIds, int start, int length);
    static HAPI_Result GetInstancerPartTransforms(godot::Ref<HEGSession> session, int nodeId, int partId, HAPI_RSTOrder rst_order, godot::Ref<HEGRefArray> transforms, int start, int length);
    static godot::String GetLastCookError(godot::Ref<HEGSession> session = nullptr);
    static godot::String GetLastError(godot::Ref<HEGSession> session = nullptr);
    static godot::String GetNodeCookResult(godot::Ref<HEGSession> session, int nodeId, HAPI_StatusVerbosity verbosity);
    static HAPI_Result GetNodeCookResultLength(godot::Ref<HEGSession> session, int nodeId, HAPI_StatusVerbosity verbosity, godot::Ref<HEGInt> length);
    static HAPI_Result GetNodeFromPath(godot::Ref<HEGSession> session, int parentId, godot::String path, godot::Ref<HEGInt> nodeId);
    static HAPI_Result GetNodeInputName(godot::Ref<HEGSession> session, int nodeId, int inputIndex, godot::Ref<HEGInt> nameHandle);
    static HAPI_Result GetNodeOutputName(godot::Ref<HEGSession> session, int nodeId, int outputIndex, godot::Ref<HEGInt> nameHandle);
    static HAPI_Result GetNodePath(godot::Ref<HEGSession> session, int nodeId, int relativeToNodeId, godot::Ref<HEGInt> pathHandle);
    static HAPI_Result GetObjectTransform(godot::Ref<HEGSession> session, int nodeId, int relativeToNodeId, HAPI_RSTOrder rst_order, godot::Ref<HEGRefDictionary> transform);
    static HAPI_Result GetOutputGeoCount(godot::Ref<HEGSession> session, int nodeId, godot::Ref<HEGInt> count);
    static HAPI_Result GetOutputGeoInfos(godot::Ref<HEGSession> session, int nodeId, godot::Ref<HEGRefArray> geoInfos, int count);
    static HAPI_Result GetOutputNodeId(godot::Ref<HEGSession> session, int nodeId, int output, godot::Ref<HEGInt> outputNodeId);
    static HAPI_Result GetParameters(godot::Ref<HEGSession> session, int nodeId, godot::Ref<HEGRefArray> parameters, int start, int length);
    static HAPI_Result GetParmFloatValue(godot::Ref<HEGSession> session, int nodeId, godot::String parmName, int index, godot::Ref<HEGFloat> value);
    static HAPI_Result GetParmIdFromName(godot::Ref<HEGSession> session, int nodeId, godot::String parmName, godot::Ref<HEGInt> parmId);
    static HAPI_Result GetParmIntValue(godot::Ref<HEGSession> session, int nodeId, godot::String parmName, int index, godot::Ref<HEGInt> value);
    static HAPI_Result GetParmNodeValue(godot::Ref<HEGSession> session, int nodeId, godot::String parmName, godot::Ref<HEGInt> value);
    static HAPI_Result GetParmStringValue(godot::Ref<HEGSession> session, int nodeId, godot::String parmName, int index, bool evaluate, godot::Ref<HEGInt> value);
    static HAPI_Result GetStatus(godot::Ref<HEGSession> session, HAPI_StatusType statusType, godot::Ref<HEGInt> status);
    static godot::String GetStatusString(godot::Ref<HEGSession> session, HAPI_StatusType status_type);
    static godot::String GetString(godot::Ref<HEGSession> session, int stringHandle);
    static HAPI_Result Initialize(godot::Ref<HEGSession> session, godot::Dictionary cookOptions, bool useCookingThread, int cookingThreadStackSize, godot::String houdiniEnvironmentFiles, godot::String otlSearchPath, godot::String dsoSearchPath, godot::String imageDsoSearchPath, godot::String audioDsoSearchPath);
    static HAPI_Result Interrupt(godot::Ref<HEGSession> session);
    static HAPI_Result IsInitialized(godot::Ref<HEGSession> session);
    static HAPI_Result IsSessionValid(godot::Ref<HEGSession> session);
    static HAPI_Result IsNodeValid(godot::Ref<HEGSession> session, int nodeId, int uniqueNodeId, godot::Ref<HEGBool> answer);
    static HAPI_Result LoadAssetLibraryFromFile(godot::Ref<HEGSession> session, godot::String filePath, bool allowOverwrite, godot::Ref<HEGInt> assetId);
    static HAPI_Result LoadAssetLibraryFromMemory(godot::Ref<HEGSession> session, godot::String buffer, bool allowOverwrite, godot::Ref<HEGInt> assetId);
    static HAPI_Result LoadGeoFromFile(godot::Ref<HEGSession> session, int nodeId, godot::String filePath);
    static HAPI_Result LoadGeoFromMemory(godot::Ref<HEGSession> session, int nodeId, godot::String format, godot::String buffer);
    static HAPI_Result LoadHIPFile(godot::Ref<HEGSession> session, godot::String fileName, bool cookOnLoad);
    static HAPI_Result LoadNodeFromFile(godot::Ref<HEGSession> session, godot::String fileName, int parentId, godot::String nodeLabel, bool cookOnLoad, godot::Ref<HEGInt> newNodeId);
    static godot::Dictionary MaterialInfo_Create();
    static godot::Dictionary NodeInfo_Create();
    static godot::Dictionary ObjectInfo_Create();
    static godot::Dictionary ParmInfo_Create();
    static godot::Dictionary PartInfo_Create();
    static godot::Dictionary SessionInfo_Create();
    static HAPI_Result RemoveCustomString(godot::Ref<HEGSession> session, int stringHandle);
    static HAPI_Result RenameNode(godot::Ref<HEGSession> session, int nodeId, godot::String newName);
    static HAPI_Result RevertGeo(godot::Ref<HEGSession> session, int nodeId);
    static HAPI_Result RevertParmToDefault(godot::Ref<HEGSession> session, int nodeId, godot::String parmName, int index);
    static HAPI_Result RevertParmToDefaults(godot::Ref<HEGSession> session, int nodeId, godot::String parmName);
    static HAPI_Result SaveGeoToFile(godot::Ref<HEGSession> session, int nodeId, godot::String filePath);
    static bool SaveToHip(godot::Ref<HEGSession> session, godot::String filename);
    static HAPI_Result SaveNodeToFile(godot::Ref<HEGSession> session, int nodeId, godot::String fileName);
    static HAPI_Result SetAttributeFloatData(godot::Ref<HEGSession> session, int nodeId, int partId, godot::String name, godot::Dictionary attrInfo, godot::Ref<HEGRefArray> dataArray, int start, int length);
    static HAPI_Result SetAttributeIntData(godot::Ref<HEGSession> session, int nodeId, int partId, godot::String name, godot::Dictionary attrInfo, godot::Ref<HEGRefArray> dataArray, int start, int length);
    static HAPI_Result SetAttributeStringData(godot::Ref<HEGSession> session, int nodeId, int partId, godot::String name, godot::Dictionary attrInfo, godot::Ref<HEGRefArray> dataArray, int start, int length);
    static HAPI_Result SetCurveCounts(godot::Ref<HEGSession> session, int nodeId, int partId, godot::Array curveCounts, int start, int length);
    static HAPI_Result SetCurveInfo(godot::Ref<HEGSession> session, int nodeId, int partId, godot::Dictionary curveInfo);
    static HAPI_Result SetCurveKnots(godot::Ref<HEGSession> session, int nodeId, int partId, godot::Array knots, int start, int length);
    static HAPI_Result SetCurveOrders(godot::Ref<HEGSession> session, int nodeId, int partId, godot::Array orders, int start, int length);
    static HAPI_Result SetCustomString(godot::Ref<HEGSession> session, godot::String string_value, godot::Ref<HEGInt> handle_value);
    static HAPI_Result SetFaceCounts(godot::Ref<HEGSession> session, int nodeId, int partId, godot::Array faceCounts, int start, int length);
    static HAPI_Result SetInputCurveInfo(godot::Ref<HEGSession> session, int nodeId, int partId, godot::Dictionary curveInfo);
    static HAPI_Result SetInputCurvePositions(godot::Ref<HEGSession> session, int nodeId, int partId, godot::Array positions, int start, int length);
    static HAPI_Result SetNodeDisplay(godot::Ref<HEGSession> session, int nodeId, int onOff);
    static HAPI_Result SetObjectTransform(godot::Ref<HEGSession> session, int nodeId, godot::Dictionary transform);
    static HAPI_Result SetServerEnvInt(godot::Ref<HEGSession> session, godot::String envName, int value);
    static HAPI_Result SetServerEnvString(godot::Ref<HEGSession> session, godot::String envName, godot::String value);
    static HAPI_Result SetSessionSync(godot::Ref<HEGSession> session, bool enable);
    static HAPI_Result SetSessionSyncInfo(godot::Ref<HEGSession> session, godot::Dictionary syncInfo);
};
#endif 