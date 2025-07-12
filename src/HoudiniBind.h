#ifndef HOUDINI_BIND_GODOT
#define HOUDINI_BIND_GODOT

#include <cstddef>
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
#include <godot_cpp/classes/display_server.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/material.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/surface_tool.hpp>
#include <godot_cpp/classes/sub_viewport.hpp>
#include <godot_cpp/classes/multi_mesh.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/multi_mesh_instance3d.hpp>
#include <godot_cpp/classes/editor_interface.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/transform3d.hpp>
#include <godot_cpp/core/math.hpp>
#include <HoudiniApi.h>
#include <HoudiniEngineUtility.h>
#include <HoudiniEnginePlatform.h>
#include "Utility.h"
#include "Contact.h"
#include "HDAImporter.h"
typedef std::size_t size_t;

VARIANT_ENUM_CAST(HAPI_AttributeTypeInfo)
VARIANT_ENUM_CAST(HAPI_AttributeOwner)
VARIANT_ENUM_CAST(HAPI_NodeType)
VARIANT_ENUM_CAST(HAPI_GeoType)
VARIANT_ENUM_CAST(HAPI_PartType)
VARIANT_ENUM_CAST(HAPI_ParmType)
VARIANT_ENUM_CAST(HAPI_InputType)
VARIANT_ENUM_CAST(HAPI_CurveType)
VARIANT_ENUM_CAST(HAPI_Result)
VARIANT_ENUM_CAST(HAPI_NodeFlags)
VARIANT_ENUM_CAST(HAPI_StorageType)
VARIANT_ENUM_CAST(HAPI_PrmScriptType)
VARIANT_ENUM_CAST(HAPI_StatusVerbosity)
VARIANT_ENUM_CAST(HAPI_StatusType)
VARIANT_ENUM_CAST(HAPI_RampType)
VARIANT_ENUM_CAST(HAPI_RSTOrder)
VARIANT_ENUM_CAST(HAPI_XYZOrder)
VARIANT_ENUM_CAST(HAPI_TCP_PortType)
VARIANT_ENUM_CAST(HAPI_ThriftSharedMemoryBufferType)
VARIANT_ENUM_CAST(HAPI_Permissions)
VARIANT_ENUM_CAST(HAPI_ChoiceListType)
VARIANT_ENUM_CAST(HAPI_PackedPrimInstancingMode)
VARIANT_ENUM_CAST(HAPI_InputCurveMethod)
VARIANT_ENUM_CAST(HAPI_InputCurveParameterization)
enum SessionType
{
    None = 0,
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

        BIND_ENUM_CONSTANT(HAPI_STATUSVERBOSITY_0);
        BIND_ENUM_CONSTANT(HAPI_STATUSVERBOSITY_1);
        BIND_ENUM_CONSTANT(HAPI_STATUSVERBOSITY_2);
        BIND_ENUM_CONSTANT(HAPI_STATUSVERBOSITY_ALL);
        BIND_ENUM_CONSTANT(HAPI_STATUSVERBOSITY_ERRORS);
        BIND_ENUM_CONSTANT(HAPI_STATUSVERBOSITY_WARNINGS);
        BIND_ENUM_CONSTANT(HAPI_STATUSVERBOSITY_MESSAGES);

        BIND_ENUM_CONSTANT(HAPI_TRS);
        BIND_ENUM_CONSTANT(HAPI_TSR);
        BIND_ENUM_CONSTANT(HAPI_RTS);
        BIND_ENUM_CONSTANT(HAPI_RST);
        BIND_ENUM_CONSTANT(HAPI_STR);
        BIND_ENUM_CONSTANT(HAPI_SRT);
        BIND_ENUM_CONSTANT(HAPI_RSTORDER_DEFAULT);

        BIND_ENUM_CONSTANT(HAPI_XYZ);
        BIND_ENUM_CONSTANT(HAPI_XZY);
        BIND_ENUM_CONSTANT(HAPI_YXZ);
        BIND_ENUM_CONSTANT(HAPI_YZX);
        BIND_ENUM_CONSTANT(HAPI_ZXY);
        BIND_ENUM_CONSTANT(HAPI_ZYX);
        BIND_ENUM_CONSTANT(HAPI_XYZORDER_DEFAULT);

        BIND_ENUM_CONSTANT(HAPI_STATUS_CALL_RESULT);
        BIND_ENUM_CONSTANT(HAPI_STATUS_COOK_RESULT);
        BIND_ENUM_CONSTANT(HAPI_STATUS_COOK_STATE);
        BIND_ENUM_CONSTANT(HAPI_STATUS_MAX);

        BIND_ENUM_CONSTANT(HAPI_TCP_PORT_ANY);
        BIND_ENUM_CONSTANT(HAPI_TCP_PORT_RANGE);
        BIND_ENUM_CONSTANT(HAPI_TCP_PORT_LIST);

        BIND_ENUM_CONSTANT(HAPI_THRIFT_SHARED_MEMORY_FIXED_LENGTH_BUFFER);
        BIND_ENUM_CONSTANT(HAPI_THRIFT_SHARED_MEMORY_RING_BUFFER);

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
        
        BIND_ENUM_CONSTANT(HAPI_PARTTYPE_INVALID);
        BIND_ENUM_CONSTANT(HAPI_PARTTYPE_MESH);
        BIND_ENUM_CONSTANT(HAPI_PARTTYPE_CURVE);
        BIND_ENUM_CONSTANT(HAPI_PARTTYPE_VOLUME);
        BIND_ENUM_CONSTANT(HAPI_PARTTYPE_INSTANCER);
        BIND_ENUM_CONSTANT(HAPI_PARTTYPE_BOX);
        BIND_ENUM_CONSTANT(HAPI_PARTTYPE_SPHERE);
        BIND_ENUM_CONSTANT(HAPI_PARTTYPE_MAX);

        BIND_ENUM_CONSTANT(HAPI_INPUT_INVALID);
        BIND_ENUM_CONSTANT(HAPI_INPUT_TRANSFORM);
        BIND_ENUM_CONSTANT(HAPI_INPUT_GEOMETRY);
        BIND_ENUM_CONSTANT(HAPI_INPUT_MAX);

        BIND_ENUM_CONSTANT(HAPI_CURVETYPE_INVALID);
        BIND_ENUM_CONSTANT(HAPI_CURVETYPE_LINEAR);
        BIND_ENUM_CONSTANT(HAPI_CURVETYPE_NURBS);
        BIND_ENUM_CONSTANT(HAPI_CURVETYPE_BEZIER);
        BIND_ENUM_CONSTANT(HAPI_CURVETYPE_MAX);

        BIND_ENUM_CONSTANT(HAPI_PARMTYPE_INT);
        BIND_ENUM_CONSTANT(HAPI_PARMTYPE_MULTIPARMLIST);
        BIND_ENUM_CONSTANT(HAPI_PARMTYPE_TOGGLE);
        BIND_ENUM_CONSTANT(HAPI_PARMTYPE_BUTTON);
        BIND_ENUM_CONSTANT(HAPI_PARMTYPE_FLOAT);
        BIND_ENUM_CONSTANT(HAPI_PARMTYPE_COLOR);
        BIND_ENUM_CONSTANT(HAPI_PARMTYPE_STRING);
        BIND_ENUM_CONSTANT(HAPI_PARMTYPE_PATH_FILE);
        BIND_ENUM_CONSTANT(HAPI_PARMTYPE_PATH_FILE_GEO);
        BIND_ENUM_CONSTANT(HAPI_PARMTYPE_PATH_FILE_IMAGE);
        BIND_ENUM_CONSTANT(HAPI_PARMTYPE_NODE);
        BIND_ENUM_CONSTANT(HAPI_PARMTYPE_FOLDERLIST);
        BIND_ENUM_CONSTANT(HAPI_PARMTYPE_FOLDERLIST_RADIO);
        BIND_ENUM_CONSTANT(HAPI_PARMTYPE_FOLDER);
        BIND_ENUM_CONSTANT(HAPI_PARMTYPE_LABEL);
        BIND_ENUM_CONSTANT(HAPI_PARMTYPE_SEPARATOR);
        BIND_ENUM_CONSTANT(HAPI_PARMTYPE_PATH_FILE_DIR);
        BIND_ENUM_CONSTANT(HAPI_PARMTYPE_MAX);
        BIND_ENUM_CONSTANT(HAPI_PARMTYPE_INT_START);
        BIND_ENUM_CONSTANT(HAPI_PARMTYPE_INT_END);
        BIND_ENUM_CONSTANT(HAPI_PARMTYPE_FLOAT_START);
        BIND_ENUM_CONSTANT(HAPI_PARMTYPE_FLOAT_END);
        BIND_ENUM_CONSTANT(HAPI_PARMTYPE_STRING_START);
        BIND_ENUM_CONSTANT(HAPI_PARMTYPE_STRING_END);
        BIND_ENUM_CONSTANT(HAPI_PARMTYPE_PATH_START);
        BIND_ENUM_CONSTANT(HAPI_PARMTYPE_PATH_END);
        BIND_ENUM_CONSTANT(HAPI_PARMTYPE_NODE_START);
        BIND_ENUM_CONSTANT(HAPI_PARMTYPE_NODE_END);
        BIND_ENUM_CONSTANT(HAPI_PARMTYPE_CONTAINER_START);
        BIND_ENUM_CONSTANT(HAPI_PARMTYPE_CONTAINER_END);
        BIND_ENUM_CONSTANT(HAPI_PARMTYPE_NONVALUE_START);
        BIND_ENUM_CONSTANT(HAPI_PARMTYPE_NONVALUE_END);

        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_INT);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_FLOAT);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_ANGLE);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_STRING);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_FILE);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_DIRECTORY);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_IMAGE);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_GEOMETRY);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_TOGGLE);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_BUTTON);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_VECTOR2);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_VECTOR3);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_VECTOR4);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_INTVECTOR2);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_INTVECTOR3);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_INTVECTOR4);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_UV);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_UVW);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_DIR);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_COLOR);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_COLOR4);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_HUECIRCLE);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_OPPATH);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_OPLIST);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_OBJECT);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_OBJECTLIST);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_RENDER);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_SEPARATOR);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_GEOMETRY_DATA);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_KEY_VALUE_DICT);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_LABEL);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_RGBAMASK);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_ORDINAL);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_RAMP_FLT);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_RAMP_RGB);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_FLOAT_LOG);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_INT_LOG);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_DATA);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_FLOAT_MINMAX);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_INT_MINMAX);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_INT_STARTEND);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_BUTTONSTRIP);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_ICONSTRIP);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_GROUPRADIO);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_GROUPCOLLAPSIBLE);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_GROUPSIMPLE);
        BIND_ENUM_CONSTANT(HAPI_PRM_SCRIPT_TYPE_GROUP);

        BIND_ENUM_CONSTANT(HAPI_PERMISSIONS_NON_APPLICABLE);
        BIND_ENUM_CONSTANT(HAPI_PERMISSIONS_READ_WRITE);
        BIND_ENUM_CONSTANT(HAPI_PERMISSIONS_READ_ONLY);
        BIND_ENUM_CONSTANT(HAPI_PERMISSIONS_WRITE_ONLY);
        BIND_ENUM_CONSTANT(HAPI_PERMISSIONS_MAX);

        BIND_ENUM_CONSTANT(HAPI_CHOICELISTTYPE_NONE);
        BIND_ENUM_CONSTANT(HAPI_CHOICELISTTYPE_NORMAL);
        BIND_ENUM_CONSTANT(HAPI_CHOICELISTTYPE_MINI);
        BIND_ENUM_CONSTANT(HAPI_CHOICELISTTYPE_REPLACE);
        BIND_ENUM_CONSTANT(HAPI_CHOICELISTTYPE_TOGGLE);

        BIND_ENUM_CONSTANT(HAPI_NODEFLAGS_ANY);
        BIND_ENUM_CONSTANT(HAPI_NODEFLAGS_NONE);
        BIND_ENUM_CONSTANT(HAPI_NODEFLAGS_DISPLAY);
        BIND_ENUM_CONSTANT(HAPI_NODEFLAGS_RENDER);
        BIND_ENUM_CONSTANT(HAPI_NODEFLAGS_TEMPLATED);
        BIND_ENUM_CONSTANT(HAPI_NODEFLAGS_LOCKED);
        BIND_ENUM_CONSTANT(HAPI_NODEFLAGS_EDITABLE);
        BIND_ENUM_CONSTANT(HAPI_NODEFLAGS_BYPASS);
        BIND_ENUM_CONSTANT(HAPI_NODEFLAGS_NETWORK);
        BIND_ENUM_CONSTANT(HAPI_NODEFLAGS_OBJ_GEOMETRY);
        BIND_ENUM_CONSTANT(HAPI_NODEFLAGS_OBJ_CAMERA);
        BIND_ENUM_CONSTANT(HAPI_NODEFLAGS_OBJ_LIGHT);
        BIND_ENUM_CONSTANT(HAPI_NODEFLAGS_OBJ_SUBNET);
        BIND_ENUM_CONSTANT(HAPI_NODEFLAGS_SOP_CURVE);
        BIND_ENUM_CONSTANT(HAPI_NODEFLAGS_SOP_GUIDE);
        BIND_ENUM_CONSTANT(HAPI_NODEFLAGS_TOP_NONSCHEDULER);
        BIND_ENUM_CONSTANT(HAPI_NODEFLAGS_NON_BYPASS);

        BIND_ENUM_CONSTANT(HAPI_RAMPTYPE_INVALID);
        BIND_ENUM_CONSTANT(HAPI_RAMPTYPE_FLOAT);
        BIND_ENUM_CONSTANT(HAPI_RAMPTYPE_COLOR);
        BIND_ENUM_CONSTANT(HAPI_RAMPTYPE_MAX);

        BIND_ENUM_CONSTANT(HAPI_CURVEMETHOD_INVALID);
        BIND_ENUM_CONSTANT(HAPI_CURVEMETHOD_CVS);
        BIND_ENUM_CONSTANT(HAPI_CURVEMETHOD_BREAKPOINTS);
        BIND_ENUM_CONSTANT(HAPI_CURVEMETHOD_MAX);

        BIND_ENUM_CONSTANT(HAPI_CURVEPARAMETERIZATION_INVALID);
        BIND_ENUM_CONSTANT(HAPI_CURVEPARAMETERIZATION_UNIFORM);
        BIND_ENUM_CONSTANT(HAPI_CURVEPARAMETERIZATION_CHORD);
        BIND_ENUM_CONSTANT(HAPI_CURVEPARAMETERIZATION_CENTRIPETAL);
        BIND_ENUM_CONSTANT(HAPI_CURVEPARAMETERIZATION_MAX);

        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetObjectInfo","session","nodeId"),&HoudiniEngine::GetObjectInfo);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetAssetInfo","session","nodeId"),&HoudiniEngine::GetAssetInfo);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetNodeInfo","session","nodeId"),&HoudiniEngine::GetNodeInfo);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetGeoInfo","session","nodeId"),&HoudiniEngine::GetGeoInfo);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetMaterialInfo","session","nodeId"),&HoudiniEngine::GetMaterialInfo);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetPartInfo","session","nodeId","partId"),&HoudiniEngine::GetPartInfo);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetAttrInfo","session","nodeId","partId","name","owner"),&HoudiniEngine::GetAttrInfo);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetParmInfo","session","nodeId","parmId","parmInfo"),&HoudiniEngine::GetParmInfo);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("AddAttribute","session","nodeId","partId","name","attrInfo"),&HoudiniEngine::AddAttribute);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("Cleanup","session"),&HoudiniEngine::Cleanup);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("ClearConnectionError"),&HoudiniEngine::ClearConnectionError);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("CloseSession","session"),&HoudiniEngine::CloseSession);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("CommitGeo","session","nodeId"),&HoudiniEngine::CommitGeo);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("ConnectNodeInput","session","nodeId","inputIndex","nodeId_toConnect","outputIndex"),&HoudiniEngine::ConnectNodeInput);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("CookNode","session","nodeId","cookOptions"),&HoudiniEngine::CookNode);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("CookOptions_Create"),&HoudiniEngine::CookOptions_Create);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("CreateInProcessSession","session","sessionInfo"),&HoudiniEngine::CreateInProcessSession);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("CreateInputNode","session","parentId","nodeId","name"),&HoudiniEngine::CreateInputNode);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("CreateNode","session","parentId","operatorName","nodeLabel","cookOnCreation","nodeId"),&HoudiniEngine::CreateNode);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("CreateThriftNamedPipeSession","session","pipeName","sessionInfo"),&HoudiniEngine::CreateThriftNamedPipeSession);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("CreateThriftSharedMemorySession","session","sharedMemName","sessionInfo"),&HoudiniEngine::CreateThriftSharedMemorySession);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("CreateThriftSocketSession","session","hostName","port","sessionInfo"),&HoudiniEngine::CreateThriftSocketSession);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("DeleteAttribute","session","nodeId","partId","name","attrInfo"),&HoudiniEngine::DeleteAttribute);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("DeleteNode","session","nodeId"),&HoudiniEngine::DeleteNode);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("DisconnectNodeInput","session","nodeId","inputIndex"),&HoudiniEngine::DisconnectNodeInput);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GeoInfo_Create"),&HoudiniEngine::GeoInfo_Create);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetAttributeFloatData","session","nodeId","partId","name","owner"),&HoudiniEngine::GetAttributeFloatData);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetAttributeIntData","session","nodeId","partId","name","owner"),&HoudiniEngine::GetAttributeIntData);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetAttributeStringData","session","nodeId","partId","name","owner"),&HoudiniEngine::GetAttributeStringData);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetConnectionError"),&HoudiniEngine::GetConnectionError);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetCookingCurrentCount","session"),&HoudiniEngine::GetCookingCurrentCount);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetCookingTotalCount","session"),&HoudiniEngine::GetCookingTotalCount);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetDisplayGeoInfo","session","nodeId"),&HoudiniEngine::GetDisplayGeoInfo);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetInstanceTransformsOnPart","session","nodeId","partId","rst_order","transforms","start","length"),&HoudiniEngine::GetInstanceTransformsOnPart);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetInstancedObjectIds","session","nodeId","objectIds","start","length"),&HoudiniEngine::GetInstancedObjectIds);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetInstancedPartIds","session","nodeId","partId","partIds","start","length"),&HoudiniEngine::GetInstancedPartIds);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetInstancerPartTransforms", "session", "nodeId", "partId","rst_order","transforms","start","length"),&HoudiniEngine::GetInstancerPartTransforms);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetLastCookError","session"),&HoudiniEngine::GetLastCookError);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetLastError","session"),&HoudiniEngine::GetLastError);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetNodeCookResult","session","stringValue", "length"),&HoudiniEngine::GetNodeCookResult);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetNodeCookResultLength","session","nodeId","verbosity","length"),&HoudiniEngine::GetNodeCookResultLength);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetNodeFromPath","session","parentId","path","nodeId"),&HoudiniEngine::GetNodeFromPath);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetNodeInputName","session","nodeId","inputIndex","nameHandle"),&HoudiniEngine::GetNodeInputName);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetNodeOutputName","session","nodeId","outputIndex","nameHandle"),&HoudiniEngine::GetNodeOutputName);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetNodePath","session","nodeId","relativeToNodeId","pathHandle"),&HoudiniEngine::GetNodePath);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetObjectTransform","session","nodeId","relativeToNodeId","rst_order","transform"),&HoudiniEngine::GetObjectTransform);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetOutputGeoCount","session","nodeId","count"),&HoudiniEngine::GetOutputGeoCount);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetOutputGeoInfos","session","nodeId","geoInfos","count"),&HoudiniEngine::GetOutputGeoInfos);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetOutputNodeId","session","nodeId","output","outputNodeId"),&HoudiniEngine::GetOutputNodeId);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetParameters","session","nodeId","parameters","start","length"),&HoudiniEngine::GetParameters);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetParmFloatValue","session","nodeId","parmName","index","value"),&HoudiniEngine::GetParmFloatValue);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetParmIdFromName","session","nodeId","parmName","parmId"),&HoudiniEngine::GetParmIdFromName);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetParmIntValue","session","nodeId","parmName","index","value"),&HoudiniEngine::GetParmIntValue);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetParmNodeValue","session","nodeId","parmName","value"),&HoudiniEngine::GetParmNodeValue);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetParmStringValue","session","nodeId","parmName","index","evaluate","value"),&HoudiniEngine::GetParmStringValue);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetStatus","session","statusType","status"),&HoudiniEngine::GetStatus);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetStatusString","session","statusType"),&HoudiniEngine::GetStatusString);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("GetString","session","stringHandle"),&HoudiniEngine::GetString);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("Initialize","session","cookOptions","useCookingThread","cookingThreadStackSize","houdiniEnvironmentFiles","otlSearchPath","dsoSearchPath","imageDsoSearchPath","audioDsoSearchPath"),&HoudiniEngine::Initialize);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("Interrupt","session"),&HoudiniEngine::Interrupt);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("IsInitialized","session"),&HoudiniEngine::IsInitialized);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("IsNodeValid","session","nodeId","uniqueNodeId","answer"),&HoudiniEngine::IsNodeValid);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("LoadAssetLibraryFromFile","session","filePath","allowOverwrite","assetId"),&HoudiniEngine::LoadAssetLibraryFromFile);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("LoadAssetLibraryFromMemory","session","buffer","allowOverwrite", "assetId"),&HoudiniEngine::LoadAssetLibraryFromMemory);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("LoadGeoFromFile","session","nodeId","filePath"),&HoudiniEngine::LoadGeoFromFile);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("LoadGeoFromMemory","session","nodeId","format","buffer"),&HoudiniEngine::LoadGeoFromMemory);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("LoadHIPFile","session","filePath","cookOnLoad"),&HoudiniEngine::LoadHIPFile);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("LoadNodeFromFile","session","fileName","parentId","nodeLabel","cookOnLoad","newNodeId"),&HoudiniEngine::LoadNodeFromFile);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("MaterialInfo_Create"),&HoudiniEngine::MaterialInfo_Create);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("NodeInfo_Create"),&HoudiniEngine::NodeInfo_Create);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("ObjectInfo_Create"),&HoudiniEngine::ObjectInfo_Create);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("ParmInfo_Create"),&HoudiniEngine::ParmInfo_Create);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("PartInfo_Create"),&HoudiniEngine::PartInfo_Create);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("SessionInfo_Create"),&HoudiniEngine::SessionInfo_Create);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("RemoveCustomString","session","stringHandle"),&HoudiniEngine::RemoveCustomString);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("RenameNode","session","nodeId","newName"),&HoudiniEngine::RenameNode);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("RevertGeo","session","nodeId"),&HoudiniEngine::RevertGeo);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("RevertParmToDefault","session","nodeId","parmName","index"),&HoudiniEngine::RevertParmToDefault);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("RevertParmToDefaults","session","nodeId","parmName"),&HoudiniEngine::RevertParmToDefaults);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("SaveGeoToFile","session","nodeId","filePath"),&HoudiniEngine::SaveGeoToFile);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("SaveToHip","session","filename"),&HoudiniEngine::SaveToHip);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("SetCustomString","session","string_value","handle_value"),&HoudiniEngine::SetCustomString);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("SetAttributeFloatData","session","nodeId","partId","name","attrInfo","dataArray","start","length"),&HoudiniEngine::SetAttributeFloatData);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("SetAttributeIntData","session","nodeId","partId","name","attrInfo","dataArray","start","length"),&HoudiniEngine::SetAttributeIntData);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("SetAttributeStringData","session","nodeId","partId","name","attrInfo","dataArray","start","length"),&HoudiniEngine::SetAttributeStringData);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("SetCurveCounts","session","nodeId","partId","curveCounts","start","length"),&HoudiniEngine::SetCurveCounts);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("SetCurveInfo","session","nodeId","partId","curveInfo"),&HoudiniEngine::SetCurveInfo);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("SetCurveKnots","session","nodeId","partId","knots","start","length"),&HoudiniEngine::SetCurveKnots);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("SetCurveOrders","session","nodeId","partId","orders","start","length"),&HoudiniEngine::SetCurveOrders);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("SetFaceCounts","session","nodeId","partId","faceCounts","start","length"),&HoudiniEngine::SetFaceCounts);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("SetInputCurveInfo","session","nodeId","partId","curveInfo"),&HoudiniEngine::SetInputCurveInfo);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("SetInputCurvePositions","session","nodeId","partId","positions","start","length"),&HoudiniEngine::SetInputCurvePositions);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("SetNodeDisplay","session","nodeId","onOff"),&HoudiniEngine::SetNodeDisplay);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("SetObjectTransform","session","nodeId","transform"),&HoudiniEngine::SetObjectTransform);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("SetServerEnvInt","session","envName","value"),&HoudiniEngine::SetServerEnvInt);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("SetServerEnvString","session","envName","value"),&HoudiniEngine::SetServerEnvString);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("SetSessionSync","session","enable"),&HoudiniEngine::SetSessionSync);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("SetSessionSyncInfo","session","syncInfo"),&HoudiniEngine::SetSessionSyncInfo);
        godot::ClassDB::bind_static_method("HoudiniEngine",godot::D_METHOD("SaveNodeToFile","session","nodeId","fileName"),&HoudiniEngine::SaveNodeToFile);
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


    GDE_EXPORT static godot::Dictionary GetObjectInfo(HoudiniEngineManager* session,int nodeId);
    GDE_EXPORT static godot::Dictionary GetAssetInfo(HoudiniEngineManager* session, int nodeId);
    GDE_EXPORT static godot::Dictionary GetNodeInfo(HoudiniEngineManager* session, int nodeId);
    GDE_EXPORT static godot::Dictionary GetGeoInfo(HoudiniEngineManager* session, int nodeId);
    GDE_EXPORT static godot::Dictionary GetMaterialInfo(HoudiniEngineManager* session, int nodeId);
    GDE_EXPORT static godot::Dictionary GetPartInfo(HoudiniEngineManager* session, int nodeId, int partId);
    GDE_EXPORT static godot::Dictionary GetAttrInfo(HoudiniEngineManager* session,int nodeId,int partId,godot::String name,HAPI_AttributeOwner owner);
    GDE_EXPORT static godot::Dictionary GetParmInfo(HoudiniEngineManager* session,int nodeId,int parmId, godot::Ref<RefDictionary> parmInfo);
    GDE_EXPORT static HAPI_Result AddAttribute(HoudiniEngineManager* session,int nodeId,int partId,godot::String name,godot::Ref<RefDictionary> attrInfo);
    GDE_EXPORT static HAPI_Result Cleanup(HoudiniEngineManager* session);
    GDE_EXPORT static HAPI_Result ClearConnectionError();
    GDE_EXPORT static HAPI_Result CloseSession(HoudiniEngineManager* session);
    GDE_EXPORT static HAPI_Result CommitGeo(HoudiniEngineManager* session,int nodeId);
    GDE_EXPORT static HAPI_Result ConnectNodeInput(HoudiniEngineManager* session, int nodeId, int inputIndex, int nodeId_toConnect, int outputIndex);
    GDE_EXPORT static HAPI_Result CookNode(HoudiniEngineManager* session, int nodeId, godot::Ref<RefDictionary> cookOptions);
    GDE_EXPORT static godot::Dictionary CookOptions_Create();
    GDE_EXPORT static HAPI_Result CreateInProcessSession(HoudiniEngineManager* session,godot::Ref<RefDictionary> sessionInfo);
    GDE_EXPORT static HAPI_Result CreateInputNode(HoudiniEngineManager* session, int parentId, godot::Ref<Int> nodeId,godot::String name);
    GDE_EXPORT static HAPI_Result CreateNode(HoudiniEngineManager* session, int parentId, godot::String operatorName, godot::String nodeLabel, bool cookOnCreation, godot::Ref<Int> nodeId);
    GDE_EXPORT static HAPI_Result CreateThriftNamedPipeSession(HoudiniEngineManager* session, godot::String pipeName, godot::Ref<RefDictionary> sessionInfo);
    GDE_EXPORT static HAPI_Result CreateThriftSharedMemorySession(HoudiniEngineManager* session, godot::String sharedMemName, godot::Ref<RefDictionary> sessionInfo);
    GDE_EXPORT static HAPI_Result CreateThriftSocketSession(HoudiniEngineManager* session, godot::String hostName, int port, godot::Ref<RefDictionary> sessionInfo);
    GDE_EXPORT static HAPI_Result DeleteAttribute(HoudiniEngineManager* session, int nodeId, int partId, godot::String name, godot::Ref<RefDictionary> attrInfo);
    GDE_EXPORT static HAPI_Result DeleteNode(HoudiniEngineManager* session, int nodeId);
    GDE_EXPORT static HAPI_Result DisconnectNodeInput(HoudiniEngineManager* session, int nodeId, int inputIndex);
    GDE_EXPORT static godot::Dictionary GeoInfo_Create();
    GDE_EXPORT static HAPI_Result GetAttributeFloatData(HoudiniEngineManager* session, int nodeId, int partId, godot::String name, godot::Ref<RefDictionary> attrInfo, int stride, godot::Ref<RefArray> dataArray, int start, int length);
    GDE_EXPORT static HAPI_Result GetAttributeIntData(HoudiniEngineManager* session, int nodeId, int partId, godot::String name, godot::Ref<RefDictionary> attrInfo, int stride, godot::Ref<RefArray> dataArray, int start, int length);
    GDE_EXPORT static HAPI_Result GetAttributeStringData(HoudiniEngineManager* session, int nodeId, int partId, godot::String name, godot::Ref<RefDictionary> attrInfo, godot::Ref<RefArray> dataArray, int start, int length);
    GDE_EXPORT static godot::String GetConnectionError();
    GDE_EXPORT static int GetCookingCurrentCount(HoudiniEngineManager* session);
    GDE_EXPORT static int GetCookingTotalCount(HoudiniEngineManager* session);
    GDE_EXPORT static godot::Dictionary GetDisplayGeoInfo(HoudiniEngineManager* session, int nodeId);
    GDE_EXPORT static HAPI_Result GetInstanceTransformsOnPart(HoudiniEngineManager* session, int nodeId, int partId, HAPI_RSTOrder rst_order, godot::Ref<RefArray> transforms, int start, int length);
    GDE_EXPORT static HAPI_Result GetInstancedObjectIds(HoudiniEngineManager* session, int nodeId, godot::Ref<RefArray> objectIds, int start, int length);
    GDE_EXPORT static HAPI_Result GetInstancedPartIds(HoudiniEngineManager* session, int nodeId, int partId, godot::Ref<RefArray> partIds, int start, int length);
    GDE_EXPORT static HAPI_Result GetInstancerPartTransforms(HoudiniEngineManager* session, int nodeId, int partId, HAPI_RSTOrder rst_order, godot::Ref<RefArray> transforms, int start, int length);
    GDE_EXPORT static godot::String GetLastCookError(HoudiniEngineManager* session = nullptr);
    GDE_EXPORT static godot::String GetLastError(HoudiniEngineManager* session = nullptr);
    GDE_EXPORT static HAPI_Result GetNodeCookResult(HoudiniEngineManager* session, godot::String stringValue, int length);
    GDE_EXPORT static HAPI_Result GetNodeCookResultLength(HoudiniEngineManager* session, int nodeId, HAPI_StatusVerbosity verbosity, godot::Ref<Int> length);
    GDE_EXPORT static HAPI_Result GetNodeFromPath(HoudiniEngineManager* session, int parentId, godot::String path, godot::Ref<Int> nodeId);
    GDE_EXPORT static HAPI_Result GetNodeInputName(HoudiniEngineManager* session, int nodeId, int inputIndex, godot::Ref<Int> nameHandle);
    GDE_EXPORT static HAPI_Result GetNodeOutputName(HoudiniEngineManager* session, int nodeId, int outputIndex, godot::Ref<Int> nameHandle);
    GDE_EXPORT static HAPI_Result GetNodePath(HoudiniEngineManager* session, int nodeId, int relativeToNodeId, godot::Ref<Int> pathHandle);
    GDE_EXPORT static HAPI_Result GetObjectTransform(HoudiniEngineManager* session, int nodeId, int relativeToNodeId, HAPI_RSTOrder rst_order, godot::Ref<RefDictionary> transform);
    GDE_EXPORT static HAPI_Result GetOutputGeoCount(HoudiniEngineManager* session, int nodeId, godot::Ref<Int> count);
    GDE_EXPORT static HAPI_Result GetOutputGeoInfos(HoudiniEngineManager* session, int nodeId, godot::Ref<RefArray> geoInfos, int count);
    GDE_EXPORT static HAPI_Result GetOutputNodeId(HoudiniEngineManager* session, int nodeId, int output, godot::Ref<Int> outputNodeId);
    GDE_EXPORT static HAPI_Result GetParameters(HoudiniEngineManager* session, int nodeId, godot::Ref<RefArray> parameters, int start, int length);
    GDE_EXPORT static HAPI_Result GetParmFloatValue(HoudiniEngineManager* session, int nodeId, godot::String parmName, int index, godot::Ref<Float> value);
    GDE_EXPORT static HAPI_Result GetParmIdFromName(HoudiniEngineManager* session, int nodeId, godot::String parmName, godot::Ref<Int> parmId);
    GDE_EXPORT static HAPI_Result GetParmIntValue(HoudiniEngineManager* session, int nodeId, godot::String parmName, int index, godot::Ref<Int> value);
    GDE_EXPORT static HAPI_Result GetParmNodeValue(HoudiniEngineManager* session, int nodeId, godot::String parmName, godot::Ref<Int> value);
    GDE_EXPORT static HAPI_Result GetParmStringValue(HoudiniEngineManager* session, int nodeId, godot::String parmName, int index, bool evaluate, godot::Ref<Int> value);
    GDE_EXPORT static HAPI_Result GetStatus(HoudiniEngineManager* session, HAPI_StatusType statusType, godot::Ref<Int> status);
    GDE_EXPORT static godot::String GetStatusString(HoudiniEngineManager* session, HAPI_StatusType status_type);
    GDE_EXPORT static godot::String GetString(HoudiniEngineManager* session, int stringHandle);
    GDE_EXPORT static HAPI_Result Initialize(HoudiniEngineManager* session, godot::Dictionary cookOptions, bool useCookingThread, int cookingThreadStackSize, godot::String houdiniEnvironmentFiles, godot::String otlSearchPath, godot::String dsoSearchPath, godot::String imageDsoSearchPath, godot::String audioDsoSearchPath);
    GDE_EXPORT static HAPI_Result Interrupt(HoudiniEngineManager* session);
    GDE_EXPORT static HAPI_Result IsInitialized(HoudiniEngineManager* session);
    GDE_EXPORT static HAPI_Result IsSessionValid(HoudiniEngineManager* session);
    GDE_EXPORT static HAPI_Result IsNodeValid(HoudiniEngineManager* session, int nodeId, int uniqueNodeId, godot::Ref<Bool> answer);
    GDE_EXPORT static HAPI_Result LoadAssetLibraryFromFile(HoudiniEngineManager* session, godot::String filePath, bool allowOverwrite, godot::Ref<Int> assetId);
    GDE_EXPORT static HAPI_Result LoadAssetLibraryFromMemory(HoudiniEngineManager* session, godot::String buffer, bool allowOverwrite, godot::Ref<Int> assetId);
    GDE_EXPORT static HAPI_Result LoadGeoFromFile(HoudiniEngineManager* session, int nodeId, godot::String filePath);
    GDE_EXPORT static HAPI_Result LoadGeoFromMemory(HoudiniEngineManager* session, int nodeId, godot::String format, godot::String buffer);
    GDE_EXPORT static HAPI_Result LoadHIPFile(HoudiniEngineManager* session, godot::String fileName, bool cookOnLoad);
    GDE_EXPORT static HAPI_Result LoadNodeFromFile(HoudiniEngineManager* session, godot::String fileName, int parentId, godot::String nodeLabel, bool cookOnLoad, godot::Ref<Int> newNodeId);
    GDE_EXPORT static godot::Dictionary MaterialInfo_Create();
    GDE_EXPORT static godot::Dictionary NodeInfo_Create();
    GDE_EXPORT static godot::Dictionary ObjectInfo_Create();
    GDE_EXPORT static godot::Dictionary ParmInfo_Create();
    GDE_EXPORT static godot::Dictionary PartInfo_Create();
    GDE_EXPORT static godot::Dictionary SessionInfo_Create();
    GDE_EXPORT static HAPI_Result RemoveCustomString(HoudiniEngineManager* session, int stringHandle);
    GDE_EXPORT static HAPI_Result RenameNode(HoudiniEngineManager* session, int nodeId, godot::String newName);
    GDE_EXPORT static HAPI_Result RevertGeo(HoudiniEngineManager* session, int nodeId);
    GDE_EXPORT static HAPI_Result RevertParmToDefault(HoudiniEngineManager* session, int nodeId, godot::String parmName, int index);
    GDE_EXPORT static HAPI_Result RevertParmToDefaults(HoudiniEngineManager* session, int nodeId, godot::String parmName);
    GDE_EXPORT static HAPI_Result SaveGeoToFile(HoudiniEngineManager* session, int nodeId, godot::String filePath);
    GDE_EXPORT static bool SaveToHip(HoudiniEngineManager* session, godot::String filename);
    GDE_EXPORT static HAPI_Result SaveNodeToFile(HoudiniEngineManager* session, int nodeId, godot::String fileName);
    GDE_EXPORT static HAPI_Result SetAttributeFloatData(HoudiniEngineManager* session, int nodeId, int partId, godot::String name, godot::Dictionary attrInfo, godot::Ref<RefArray> dataArray, int start, int length);
    GDE_EXPORT static HAPI_Result SetAttributeIntData(HoudiniEngineManager* session, int nodeId, int partId, godot::String name, godot::Dictionary attrInfo, godot::Ref<RefArray> dataArray, int start, int length);
    GDE_EXPORT static HAPI_Result SetAttributeStringData(HoudiniEngineManager* session, int nodeId, int partId, godot::String name, godot::Dictionary attrInfo, godot::Ref<RefArray> dataArray, int start, int length);
    GDE_EXPORT static HAPI_Result SetCurveCounts(HoudiniEngineManager* session, int nodeId, int partId, godot::Array curveCounts, int start, int length);
    GDE_EXPORT static HAPI_Result SetCurveInfo(HoudiniEngineManager* session, int nodeId, int partId, godot::Dictionary curveInfo);
    GDE_EXPORT static HAPI_Result SetCurveKnots(HoudiniEngineManager* session, int nodeId, int partId, godot::Array knots, int start, int length);
    GDE_EXPORT static HAPI_Result SetCurveOrders(HoudiniEngineManager* session, int nodeId, int partId, godot::Array orders, int start, int length);
    GDE_EXPORT static HAPI_Result SetCustomString(HoudiniEngineManager* session, godot::String string_value, godot::Ref<Int> handle_value);
    GDE_EXPORT static HAPI_Result SetFaceCounts(HoudiniEngineManager* session, int nodeId, int partId, godot::Array faceCounts, int start, int length);
    GDE_EXPORT static HAPI_Result SetInputCurveInfo(HoudiniEngineManager* session, int nodeId, int partId, godot::Dictionary curveInfo);
    GDE_EXPORT static HAPI_Result SetInputCurvePositions(HoudiniEngineManager* session, int nodeId, int partId, godot::Array positions, int start, int length);
    GDE_EXPORT static HAPI_Result SetNodeDisplay(HoudiniEngineManager* session, int nodeId, int onOff);
    GDE_EXPORT static HAPI_Result SetObjectTransform(HoudiniEngineManager* session, int nodeId, godot::Dictionary transform);
    GDE_EXPORT static HAPI_Result SetServerEnvInt(HoudiniEngineManager* session, godot::String envName, int value);
    GDE_EXPORT static HAPI_Result SetServerEnvString(HoudiniEngineManager* session, godot::String envName, godot::String value);
    GDE_EXPORT static HAPI_Result SetSessionSync(HoudiniEngineManager* session, bool enable);
    GDE_EXPORT static HAPI_Result SetSessionSyncInfo(HoudiniEngineManager* session, godot::Dictionary syncInfo);
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
            //nodeInfo = HoudiniEngine::GetNodeInfo(session,nodeId);
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
constexpr int DefaultNewSessionTimeoutSec = 120;
class HoudiniSettings: public godot::Object{
    GDCLASS(HoudiniSettings,godot::Object)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("_settings_changed"),&HoudiniSettings::_settings_changed);
        godot::ClassDB::bind_method(godot::D_METHOD("set_logFilePath","path"),&HoudiniSettings::set_logFilePath);
    }
    friend class HoudiniEngineManager;
    GDE_EXPORT 
    void _notification(int what){
        switch(what){
        case NOTIFICATION_POSTINITIALIZE:{
            _init_settings();
            godot::ProjectSettings::get_singleton()->connect("settings_changed",godot::Callable(this,"_settings_changed"));
            _settings_changed();
        }break;
        case NOTIFICATION_PREDELETE:{
            godot::ProjectSettings::get_singleton()->disconnect("settings_changed",godot::Callable(this,"_settings_changed"));
        }break;
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
        
        addSetting("houdini/config/hapiLib",HAPILibType::HAPIL,godot::Variant::INT,godot::PROPERTY_HINT_ENUM,"HAPIL:0,HAPI:1");

        settings->set_restart_if_changed("houdini/config/hapiLib",true);

        addSetting("houdini/config/houdiniRootPath","",godot::Variant::STRING,godot::PROPERTY_HINT_GLOBAL_DIR);

        settings->set_restart_if_changed("houdini/config/houdiniRootPath",true);

        addSetting("houdini/config/houdiniLibPath","",godot::Variant::STRING,godot::PROPERTY_HINT_GLOBAL_DIR);
        
        settings->set_restart_if_changed("houdini/config/houdiniLibPath",true);

        addSetting("houdini/config/logFilePath","",godot::Variant::STRING,godot::PROPERTY_HINT_SAVE_FILE);

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

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/hapiLib");
        if(libType != (HAPILibType)(int)value){
            libType = (HAPILibType)(int)value;
        }

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/houdiniRootPath");
        tempStr = string_cast((godot::String)value);
        if(tempStr != houdiniRootPath){
            set_houdiniRootPath((godot::String)value);
        }

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/houdiniLibPath");
        tempStr = string_cast((godot::String)value);
        if(tempStr != houdiniRootPath){
            set_houdiniLibPath((godot::String)value);
        }

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/logFilePath");
        tempStr = string_cast((godot::String)value);
        if(tempStr != logFilePath){
            set_logFilePath((godot::String)value);
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
            logFile.open(string_cast(path),std::ios::app|std::ios::out);
            logFilePath = string_cast(path);
        }else if(path.is_empty()){
            logFile.close();
            logFile.clear();
            logFilePath.clear();
        }
    }

    bool useEnvLibPath = true;
    std::string houdiniRootPath = "";
    void set_houdiniRootPath(godot::String path){
        path = godot::ProjectSettings::get_singleton()->globalize_path(path);
        if(path == ""){
            houdiniRootPath.clear();
            return;
        }
        std::string houdiniPath = string_cast(path);
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
        std::string cmd = "\"" + hconfigPath + "\"";
        std::string output = exec_output(cmd.c_str());
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
            if(envKey.empty()||envValue.empty())
                continue;
            if(envValue[0] == '\'')
                envValue.erase(0,1);
            if(envValue[envValue.size()-1] == '\'')
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
        std::string houdiniPath = string_cast(path);
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
            hapiLib = HoudiniEnginePlatform::LoadLibHAPIL(libType == HAPI);
        else 
            hapiLib = HoudiniEnginePlatform::LoadLibHAPIL(libType == HAPI, houdiniLibPath);
        if(hapiLib != nullptr){
            HoudiniApi::InitializeHAPI(hapiLib);
        }else{
            printError("Failed to initialize hapi");
        }
        if(!HoudiniApi::IsHAPIInitialized()){
            if(libType == HAPIL){
                printError("Failed to load and initialize the "
                        "Houdini Engine API from libHAPIL.\n");
            }else{
                printError("Failed to load and initialize the "
                        "Houdini Engine API from libHAPI.\n");  
            }
        }
    }
    void* hapiLib = nullptr;
public:
    enum HAPILibType{
        HAPIL = 0,
        HAPI = 1
    } libType = HAPIL;
    static HoudiniSettings* get_singleton(){
        static HoudiniSettings* singleton = nullptr;
        if(!singleton)
            singleton = memnew(HoudiniSettings());
        return singleton;
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

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/hapiLib");
        if(libType != (HAPILibType)(int)value){
            godot::ProjectSettings::get_singleton()->set_setting("houdini/config/hapiLib",libType);
        }

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/houdiniRootPath");
        tempStr = string_cast(houdiniRootPath);
        if(tempStr != value){
            godot::ProjectSettings::get_singleton()->set_setting("houdini/config/houdiniRootPath",tempStr);
        }

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/houdiniLibPath");
        tempStr = string_cast(houdiniLibPath);
        if(tempStr != value){
            godot::ProjectSettings::get_singleton()->set_setting("houdini/config/houdiniLibPath",tempStr);
        }

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/logFilePath");
        tempStr = string_cast(logFilePath);
        if(tempStr != value){
            set_logFilePath((godot::String)value);
            godot::ProjectSettings::get_singleton()->set_setting("houdini/config/logFilePath",tempStr);
        }
    }
};
class HoudiniEngineManager: public godot::Node3D{
    GDCLASS(HoudiniEngineManager,godot::Node3D)
    friend class HoudiniEngine;
    static void _bind_methods(){

        godot::ClassDB::bind_method(godot::D_METHOD("get_assets"),&HoudiniEngineManager::get_assets);
        godot::ClassDB::bind_method(godot::D_METHOD("get_nodes"),&HoudiniEngineManager::get_nodes);
        godot::ClassDB::bind_method(godot::D_METHOD("cookSession"),&HoudiniEngineManager::cookSession);
        godot::ClassDB::bind_method(godot::D_METHOD("cookAsset"),&HoudiniEngineManager::cookAsset);
        godot::ClassDB::bind_method(godot::D_METHOD("set_sessionAction","action"),&HoudiniEngineManager::set_sessionAction);
        godot::ClassDB::bind_method(godot::D_METHOD("set_assetAction","action"),&HoudiniEngineManager::set_assetAction);
        godot::ClassDB::bind_method(godot::D_METHOD("set_nodeAction","action"),&HoudiniEngineManager::set_nodeAction);
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

        // godot::ClassDB::add_signal("HoudiniEngineManager",godot::MethodInfo("materialChanged",godot::PropertyInfo(godot::Variant::INT,"nodeId")));
        // godot::ClassDB::add_signal("HoudiniEngineManager",godot::MethodInfo("geometryChanged",godot::PropertyInfo(godot::Variant::INT,"nodeId")));
    }


    void _get_property_list(godot::List<godot::PropertyInfo>* list){
        list->clear();


        list->push_back(godot::PropertyInfo(godot::Variant::DICTIONARY,"cookOptions"));

        list->push_back(godot::PropertyInfo(godot::Variant::NIL,"Session Settings",godot::PROPERTY_HINT_NONE,"SessionSettings_",godot::PROPERTY_USAGE_GROUP));
        list->push_back(godot::PropertyInfo(godot::Variant::OBJECT,"SessionSettings_sessionAction",godot::PROPERTY_HINT_RESOURCE_TYPE,"SessionAction"));
        list->push_back(godot::PropertyInfo(godot::Variant::INT,"SessionSettings_sessionType",godot::PROPERTY_HINT_ENUM,"InProcess:1,NewNamedPipe:2,NewTCPSocket:3,ExistingNamedPipe:4,ExistingTCPSocket:5,ExistingSharedMemory:6"));
        list->push_back(godot::PropertyInfo(godot::Variant::BOOL,"SessionSettings_useCookingThread"));
        list->push_back(godot::PropertyInfo(godot::Variant::DICTIONARY,"SessionSettings_sessionConfig"));

        list->push_back(godot::PropertyInfo(godot::Variant::NIL,"Asset Settings",godot::PROPERTY_HINT_NONE,"AssetSettings_",godot::PROPERTY_USAGE_GROUP));
        list->push_back(godot::PropertyInfo(godot::Variant::OBJECT,"AssetSettings_assetAction",godot::PROPERTY_HINT_RESOURCE_TYPE,"AssetAction"));
        list->push_back(godot::PropertyInfo(godot::Variant::OBJECT,"AssetSettings_nowAsset",godot::PROPERTY_HINT_RESOURCE_TYPE,"HDAResource"));
        list->push_back(godot::PropertyInfo(godot::Variant::ARRAY,"AssetSettings_assets"));


        list->push_back(godot::PropertyInfo(godot::Variant::NIL,"Node Settings",godot::PROPERTY_HINT_NONE,"NodeSettings_",godot::PROPERTY_USAGE_GROUP));
        list->push_back(godot::PropertyInfo(godot::Variant::BOOL,"NodeSettings_showModel"));
        list->push_back(godot::PropertyInfo(godot::Variant::BOOL,"NodeSettings_syncViewport"));
        list->push_back(godot::PropertyInfo(godot::Variant::OBJECT,"NodeSettings_nodeAction",godot::PROPERTY_HINT_RESOURCE_TYPE,"NodeAction"));
        list->push_back(godot::PropertyInfo(godot::Variant::OBJECT,"NodeSettings_nowNode",godot::PROPERTY_HINT_RESOURCE_TYPE,"HDANode"));
        list->push_back(godot::PropertyInfo(godot::Variant::OBJECT,"NodeSettings_inputMesh",godot::PROPERTY_HINT_RESOURCE_TYPE,"Mesh"));
        list->push_back(godot::PropertyInfo(godot::Variant::OBJECT,"NodeSettings_inputMeshTreeRoot",godot::PROPERTY_HINT_NODE_TYPE));
        list->push_back(godot::PropertyInfo(godot::Variant::ARRAY,"NodeSettings_nodes"));
        

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

        std::string propertyName = string_cast(property.c_escape());
        if(propertyName == "cookOptions"){
            ret = get_cookOptions();
            return true;
        }else if(propertyName == "SessionSettings_sessionAction"){
            ret = sessionAction;
            return true;
        }else if(propertyName == "SessionSettings_sessionType"){
            if(HoudiniSettings::get_singleton()->libType == HoudiniSettings::HAPI)
                sessionType = SessionType::InProcess;
            ret = sessionType;
            return true;
        }else if(propertyName == "SessionSettings_useCookingThread"){
            ret = useCookingThread;
            return true;
        }else if(propertyName == "SessionSettings_sessionConfig"){
            ret = get_sessionConfig();
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
        }else if(propertyName == "NodeSettings_showModel"){
            ret = showModel;
            return true;
        }else if(propertyName == "NodeSettings_syncViewport"){
            ret = syncViewport;
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
                        ret = string_cast(std::get<std::string>(res[0]));
                    }
                }else{
                    godot::Array arr;
                    for(auto& a : res){
                        if(std::holds_alternative<int64_t>(res[0])){
                            arr.push_back(std::get<int64_t>(res[0]));
                        }else if(std::holds_alternative<double>(res[0])){
                            arr.push_back(std::get<double>(res[0]));
                        }else if(std::holds_alternative<std::string>(res[0])){
                            arr.push_back(string_cast(std::get<std::string>(res[0])));
                        }
                    }
                    ret = arr;
                }
            }
        }
        return true;
    }
    bool _set(const godot::StringName& property, const godot::Variant& value){

        std::string propertyName = string_cast(property.c_escape());
        
        if(propertyName == "cookOptions"){
            set_cookOptions((godot::Dictionary)value);
            return true;
        }else if(propertyName == "SessionSettings_sessionAction"){
            set_sessionAction((godot::Ref<SessionAction>)(value));
            return true;
        }else if(propertyName == "SessionSettings_sessionType"){
            sessionType = (SessionType)(int)value;
            if(HoudiniSettings::get_singleton()->libType == HoudiniSettings::HAPI)
                sessionType = SessionType::InProcess;
            return true;
        }else if(propertyName == "SessionSettings_useCookingThread"){
            useCookingThread = (bool)value;
            return true;
        }else if(propertyName == "SessionSettings_sessionConfig"){
            set_sessionConfig((godot::Dictionary)value);
            return true;
        }else if(propertyName == "AssetSettings_assetAction"){
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
                nodeCookSync(nowNode->nodeId);
                updateInternalModel();
            }
            for(auto a : internalModels)
                a.second->set_visible(showModel);
            return true;
        }else if(propertyName == "NodeSettings_syncViewport"){
            HAPI_SessionSyncInfo info;
            HoudiniApi::GetSessionSyncInfo(get_session(),&info);
            info.syncViewport = true;
            HoudiniApi::SetSessionSyncInfo(get_session(),&info);
            syncViewport = (bool)value;
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
            if(nowNode->nodeId == id){
                if(autoCook&&nowNode.is_valid()){
                    cookNode(id);
                }
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
                    res[0] = string_cast((godot::String)value);
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
                        res.push_back(string_cast((godot::String)((godot::Array)value)[i]));
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
            return true;
        }
        return false;
    }
    GDE_EXPORT 
    void _init_hserver(){
        if(!findproc("hserver")){
            printError("Can't find hserver. Try to restart it by hkey.");
            printError("Please restart the hserver to manually.");
            printWarning("Run ",(HoudiniSettings::get_singleton()->houdiniRootPath+"/bin/hkey").c_str()," to restart hserver");
            if(HoudiniSettings::get_singleton()->houdiniRootPath.empty()){
                printWarning("Run Houdini License Administrator (hkey) to restart hserver");
            }else{
                printWarning("Run ",(HoudiniSettings::get_singleton()->houdiniRootPath+"/bin/hkey").c_str()," to restart hserver");
            }
        }
    }
    GDE_EXPORT 
    void _notification(int what){
        static HoudiniSettings* settings = nullptr;
        switch(what){
        case NOTIFICATION_ENTER_TREE:{
            init();
            set_process(1);
            settings = HoudiniSettings::get_singleton();
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
    bool focused = false;
    std::set<godot::Node*> createdGDNodes;
    std::map<godot::Node*, std::shared_ptr<std::jthread>> freeGDNodeTasks;
    std::chrono::milliseconds freeTimeout = defaultFreeTimeout;
    constexpr static std::chrono::milliseconds defaultFreeTimeout = std::chrono::milliseconds(100);
    GDE_EXPORT
    void init(){
        godot::OS::get_singleton()->set_low_processor_usage_mode(true);
        freeTimeout = defaultFreeTimeout;
        godot::Ref<godot::StandardMaterial3D> defaultMaterial;
        defaultMaterial.instantiate();
        defaultMaterial->set_flag(godot::BaseMaterial3D::Flags::FLAG_ALBEDO_FROM_VERTEX_COLOR,true);
        materialRes[""] = defaultMaterial;

        default_cookOptions();
        
        get_tree()->connect("node_removed",godot::Callable(this,"freeGDNode"));
        get_tree()->connect("node_added",godot::Callable(this,"stopFreeGDNode"));

        sessionAction.unref();
        assetAction.unref();
        nodeAction.unref();
        nowAsset.unref();
        nowNode.unref();
        inputMesh.unref();
        inputMeshTreeRoot = nullptr;
    }
    GDE_EXPORT
    void process(){
        // if(!sessionOpened)
        //     for(auto a : nodeIds){
        //         // if(checkMaterialChange(a.first)){
        //         //     emit_signal("materialChanged",a.first);
        //         // }
        //         if(checkGeometryChange(a.first)){
        //             emit_signal("geometryChanged",a.first);
        //         }
        //     }
        if(sessionOpened){
            if(focused != godot::DisplayServer::get_singleton()->window_is_focused()){
                focused = !focused;
                if(focused)
                    sessionCookSync();
            }

            syncCamera();
        }
    }
    GDE_EXPORT
    void term(){
        get_tree()->disconnect("node_removed",godot::Callable(this,"freeGDNode"));
        get_tree()->disconnect("node_added",godot::Callable(this,"stopFreeGDNode"));
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
        std::this_thread::sleep_for(defaultFreeTimeout<1s?defaultFreeTimeout:1000ms);
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
        const auto type = action->get_class();
        //const auto& type = typeid(*(action.ptr()));
        if(type == StartSessionAction::get_class_static()){
            this->sessionAction = action;
            std::jthread([this]{
                startSession(sessionType,useCookingThread);
                this->sessionAction.unref();
                Contact::add_call([this]{
                    notify_property_list_changed();
                });
            }).detach();
        }else if(type == StopSessionAction::get_class_static()){
            this->sessionAction = action;
            std::jthread([this]{
                stopSession();
                this->sessionAction.unref();
                Contact::add_call([this]{
                    notify_property_list_changed();
                });
            }).detach();
        }else if(type == CookSessionAction::get_class_static()){
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
        const auto& type = action->get_class();
        if(type == CookAssetAction::get_class_static()){
            this->assetAction = action;
            std::jthread([this]{
                
                cookAsset();
                this->assetAction.unref();
                Contact::add_call([this]{
                    notify_property_list_changed();
                });
            }).detach();
        }else if(type == LoadAssetAction::get_class_static()){
            printLog("");
            this->assetAction = action;
            std::jthread([this]{
            printLog("");
                if(nowAsset.is_null())
                    return;
                loadAssets(nowAsset,Void{});
                this->assetAction.unref();
                Contact::add_call([this]{
            printLog("");
                    notify_property_list_changed();
                });
            }).detach();
            printLog("");
        }else{
            return;
        }
    }
    godot::Ref<NodeAction> nodeAction;
    GDE_EXPORT
    void set_nodeAction(godot::Ref<NodeAction> action){
        if(action.is_null())
            return;
        const auto& type = action->get_class();
        if(type == LoadInputNodeAction::get_class_static()){
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
                            createInputNode(string_cast(a->get_name().c_escape()),id,-1,mesh);
                        }else if(a->get_class() == godot::MultiMeshInstance3D::get_class_static()){
                            auto mesh = static_cast<godot::MultiMeshInstance3D*>(a)->get_multimesh()->get_mesh();
                            createInputNode(string_cast(a->get_name().c_escape()),id,-1,mesh);
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
        else if(type == CookNodeAction::get_class_static()){
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
        else if(type == DeleteNodeAction::get_class_static()){
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
        else if(type == CreateMeshAction::get_class_static()){
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
    bool syncViewport = 0;
    godot::Ref<godot::Mesh> inputMesh;
    godot::Node* inputMeshTreeRoot = nullptr;

    void syncCamera(){
        if(!syncViewport)
            return;
        godot::Camera3D* camera = godot::EditorInterface::get_singleton()->get_editor_viewport_3d()->get_camera_3d();
        auto trans = camera->get_transform();
        auto pos = trans.get_origin();
        auto rot = trans.get_basis().get_rotation_quaternion();
        HAPI_Viewport viewport;
        HoudiniApi::GetViewport(get_session(),&viewport);
        bool equal = true;
        if(viewport.position[0] != pos.x){
            viewport.position[0] = pos.x;
            equal = false;
        }
        if(viewport.position[1] != pos.y){
            viewport.position[1] = pos.y;
            equal = false;
        }
        if(viewport.position[2] != pos.z){
            viewport.position[2] = pos.z;
            equal = false;
        }
        if(viewport.rotationQuaternion[0] != rot.x){
            viewport.rotationQuaternion[0] = rot.x;
            equal = false;
        }
        if(viewport.rotationQuaternion[1] != rot.y){
            viewport.rotationQuaternion[1] = rot.y;
            equal = false;
        }
        if(viewport.rotationQuaternion[2] != rot.z){
            viewport.rotationQuaternion[2] = rot.z;
            equal = false;
        }
        if(viewport.rotationQuaternion[3] != rot.w){
            viewport.rotationQuaternion[3] = rot.w;
            equal = false;
        }
        if(!equal)
            HoudiniApi::SetViewport(get_session(),&viewport);
    }

    HAPI_Session session;
    SessionType sessionType = InProcess;
    SessionType nowSessionType = None;
    bool useCookingThread = true;

    struct {
        std::string namedPipe = DefaultNamedPipe;
        std::string hostName = DefaultHostName;
        std::string sharedMemoryName = DefaultSharedMemoryName;
        int tcpPort = DefaultTcpPort;
        int newSessionTimeoutSec = DefaultNewSessionTimeoutSec;
    } sessionConfig;
    GDE_EXPORT
    void set_sessionConfig(godot::Dictionary config){
        if(config.has("namedPipe"))
            sessionConfig.namedPipe = string_cast(static_cast<godot::String>(config["namedPipe"]));
        if(config.has("hostName"))
            sessionConfig.hostName = string_cast(static_cast<godot::String>(config["hostName"]));
        if(config.has("sharedMemoryName"))
            sessionConfig.sharedMemoryName = string_cast(static_cast<godot::String>(config["sharedMemoryName"]));
        if(config.has("tcpPort"))
            sessionConfig.tcpPort = (int)config["tcpPort"];
        if(config.has("newSessionTimeoutSec"))
            sessionConfig.newSessionTimeoutSec = (int)config["newSessionTimeoutSec"];
    }
    GDE_EXPORT
    godot::Dictionary get_sessionConfig(){
        godot::Dictionary dic;
        dic["namedPipe"] = string_cast(sessionConfig.namedPipe);
        dic["hostName"] = string_cast(sessionConfig.hostName);
        dic["sharedMemoryName"] = string_cast(sessionConfig.sharedMemoryName);
        dic["tcpPort"] = sessionConfig.tcpPort;
        dic["newSessionTimeoutSec"] = sessionConfig.newSessionTimeoutSec;
        return dic;
    }


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
    //      nodeId  cookCount
    std::map<int,int> cookCounts;
    // made by user
    //      godot path          Material instance
    std::map<std::string,godot::Ref<godot::Material>> materialRes;


public:
    HAPI_Session* get_session(){
        return &session;
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
        int times(0);
        const int timeout(sessionConfig.newSessionTimeoutSec);
        sessionType = type;
        switch (type)
        {
        case SessionType::InProcess:{
            HAPI_SessionInfo sessionInfo = HoudiniApi::SessionInfo_Create();
            SessionResult = HoudiniApi::CreateInProcessSession(
                get_session(),&sessionInfo
            );
            if(SessionResult == HAPI_RESULT_SUCCESS){
                nowSessionType = type;
                printLog("Successful create a HAPI in-process session\n");
            }else{
                printError("Error create session: ",SessionResult);
            }
        }break;
        case SessionType::NewNamedPipe:{
            auto houdini = std::filesystem::canonical(HoudiniSettings::get_singleton()->houdiniRootPath+"/bin/houdini");
            if(!execute(houdini.string()+" -hess=pipe:"+sessionConfig.namedPipe)){
                printError("Can't open houdini.");
                break;
            }
            using namespace std::chrono_literals;
            HAPI_SessionInfo sessionInfo = HoudiniApi::SessionInfo_Create();
            printLog("Wait for opening houdini");
            while(SessionResult != HAPI_RESULT_SUCCESS && times < timeout){
                ++times;
                SessionResult = HoudiniApi::CreateThriftNamedPipeSession(
                    get_session(),sessionConfig.namedPipe.c_str(),&sessionInfo
                );
                std::this_thread::sleep_for(1s); 
            }
            if(SessionResult == HAPI_RESULT_SUCCESS){
                nowSessionType = type;
                printLog("Successful create a HAPI named-pipe session\n");
            }else{
                printError("Error create session: ",SessionResult);
            }
        }break;
        case SessionType::NewTCPSocket:{
            auto houdini = std::filesystem::canonical(HoudiniSettings::get_singleton()->houdiniRootPath+"/bin/houdini");
            if(!execute(houdini.string()+" -hess=port:"+std::to_string(sessionConfig.tcpPort))){
                printError("Can't open houdini.");
                break;
            }
            using namespace std::chrono_literals;
            HAPI_SessionInfo sessionInfo = HoudiniApi::SessionInfo_Create();
            printLog("Wait for opening houdini");
            while(SessionResult != HAPI_RESULT_SUCCESS && times < timeout){
                ++times;
                SessionResult = HoudiniApi::CreateThriftSocketSession(
                    get_session(),sessionConfig.hostName.c_str(), sessionConfig.tcpPort, &sessionInfo
                );
                std::this_thread::sleep_for(1s);
            }
            if(SessionResult == HAPI_RESULT_SUCCESS){
                nowSessionType = type;
                printLog("Successful create a HAPI TCP socket session\n");
            }else{
                printError("Error create session: ",SessionResult);
            }
        }break;
        case SessionType::ExistingNamedPipe:{
            HAPI_SessionInfo sessionInfo = HoudiniApi::SessionInfo_Create();
            SessionResult = HoudiniApi::CreateThriftNamedPipeSession(
                get_session(),sessionConfig.namedPipe.c_str(),&sessionInfo
            );
            if(SessionResult == HAPI_RESULT_SUCCESS){
                nowSessionType = type;
                printLog("Successful connect to an existint HAPI named-pipe session\n");
            }else{
                printError("Error create session: ",SessionResult);
            }
        }break;
        case SessionType::ExistingTCPSocket:{
            HAPI_SessionInfo sessionInfo = HoudiniApi::SessionInfo_Create();
            SessionResult = HoudiniApi::CreateThriftSocketSession(
                get_session(),sessionConfig.hostName.c_str(), sessionConfig.tcpPort, &sessionInfo
            );
            if(SessionResult == HAPI_RESULT_SUCCESS){
                nowSessionType = type;
                printLog("Successful connect to an existint HAPI TCP socket session\n");
            }else{
                printError("Error create session: ",SessionResult);
            }
        }break;
        case SessionType::ExistingSharedMemory:{
            HAPI_SessionInfo sessionInfo = HoudiniApi::SessionInfo_Create();
            SessionResult = HoudiniApi::CreateThriftSharedMemorySession(
                get_session(),sessionConfig.sharedMemoryName.c_str(), &sessionInfo
            );
            if(SessionResult == HAPI_RESULT_SUCCESS){
                nowSessionType = type;
                printLog("Successful connect to an existint HAPI shared memory session\n");
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
            HoudiniSettings::get_singleton()->_update_settings();
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
        if(!sessionOpened){
            printError("Error load Asset with invalid session");
            return {};
        }

        std::cerr << __LINE__ << std::endl;
        int assetId = -1;
        try{
        if(auto a = HoudiniApi::LoadAssetLibraryFromFile(get_session(),hdaRes->path.c_str(),true,&assetId);a != HAPI_RESULT_SUCCESS){
            printError("Error load Asset from file: ", a);
            return {};
        }
        }catch(std::exception& e){
            printError(e.what());
        }
        std::cerr << __LINE__ << std::endl;
        int asset_count = 0;
        if(auto a = HoudiniApi::GetAvailableAssetCount(get_session(),assetId,&asset_count); a != HAPI_RESULT_SUCCESS){
            printError("Error get available asset count: ",a);
            return {};
        }
        std::cerr << __LINE__ << std::endl;
        std::vector<HAPI_StringHandle> assetSH;
        assetSH.resize(asset_count);
        if(auto a = HoudiniApi::GetAvailableAssets(get_session(),assetId,assetSH.data(),asset_count);a != HAPI_RESULT_SUCCESS){
            printError("Error get available assets: ",a);
            return {};
        }
        std::cerr << __LINE__ << std::endl;
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
        std::cerr << __LINE__ << std::endl;
        hdaRes->assetId = assetId;
        assetIds.insert({assetId,hdaRes});
        std::cerr << __LINE__ << std::endl;
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
        return createNode(string_cast(nodeLabel),string_cast(operatorName),*id->id.get(),(int)**parentId,assetId);
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
        if(waitForCook())
            cookCounts[id]++;
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
    void nodeCookSync(int id){
        if(!showModel)
            return;
        if(auto a = cookCounts.find(id);a != cookCounts.end()){
            int count = 0;
            if(HoudiniApi::GetTotalCookCount(get_session(),id
                ,(HAPI_NodeTypeBits)(HAPI_NODETYPE_OBJ||HAPI_NODETYPE_SOP)
                ,(HAPI_NodeFlagsBits)(HAPI_NODEFLAGS_DISPLAY||HAPI_NODEFLAGS_RENDER||HAPI_NODEFLAGS_OBJ_GEOMETRY)
                ,true,&count) != HAPI_RESULT_SUCCESS)
                return;
            if(count != a->second){
                a->second = count;
                cookNode(id);
            }
        }
    }
    GDE_EXPORT
    void sessionCookSync(){
        for(auto id : nodeIds){
            nodeCookSync(id.first);
        }
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
                    arr.push_back(string_cast(std::get<std::string>(b)));
                }
            }
            dict[string_cast(a.first)] = arr;
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
            point_attr["Name"] = string_cast(attr_name);
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
            vertex_attr["Name"] = string_cast(attr_name);
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
            prim_attr["Name"] = string_cast(attr_name);
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
            detail_attr["Name"] = string_cast(attr_name);
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
                    materialRes[path] = godot::ResourceLoader::get_singleton()->load(string_cast(path));
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
        godot::MultiMesh mp;
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
            const char* matPath = keep_alive_string(string_cast(rawMatPath));

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
