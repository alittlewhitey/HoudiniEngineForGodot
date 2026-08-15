#ifndef HOUDINI_ENGINE_CLASS
#define HOUDINI_ENGINE_CLASS
#include "DebugSymbol.h"

#include <cstdint>
#include <vector>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/classes/material.hpp>
#include <godot_cpp/classes/multi_mesh.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/multi_mesh_instance3d.hpp>
#include <godot_cpp/classes/curve3d.hpp>
#include <godot_cpp/classes/path3d.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/variant/typed_dictionary.hpp>
#include <HoudiniApi.h>
#include <HoudiniEngineUtility.h>
#include <HoudiniEnginePlatform.h>
#include "HEGImporter.h"
#include "HEGEnum.h"

enum HEGSessionType
{
    None = 0,
    InProcess = 1,
    NewNamedPipe = 2,
    NewTCPSocket = 3,
    ExistingNamedPipe = 4,
    ExistingTCPSocket = 5,
    ExistingSharedMemory = 6
};
HE_ENUM_CAST(HEGSessionType)
enum HEGCookState
{
    COOK_STATE_NONE,
    COOK_STATE_COOKING,
    COOK_STATE_SUCCESS,
    COOK_STATE_FAILED,
    COOK_STATE_NODE_INVALID
};
HE_ENUM_CAST(HEGCookState)
class HEGSession;
class HEGAsset;
class HEGUtil: public godot::RefCounted{
    GDCLASS(HEGUtil,godot::RefCounted)
    static void _bind_methods(){
        godot::ClassDB::bind_static_method("HEGUtil", godot::D_METHOD("getLastError"), &HEGUtil::getLastError);
    }
    static godot::String getLastError(){
        return string_cast(_get_last_error());
    }
};
class HEGNode: public godot::RefCounted{
    GDCLASS(HEGNode,godot::RefCounted)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("getId"),&HEGNode::getId);
        godot::ClassDB::bind_method(godot::D_METHOD("isValid"),&HEGNode::isValid);
        godot::ClassDB::add_property("HEGNode",godot::PropertyInfo(godot::Variant::Type::INT,"id",godot::PROPERTY_HINT_NONE,"",godot::PROPERTY_USAGE_READ_ONLY),godot::StringName(),"getId");
        godot::ClassDB::bind_method(godot::D_METHOD("getName"),&HEGNode::getName);
        godot::ClassDB::bind_method(godot::D_METHOD("setName","name"),&HEGNode::setName);
        godot::ClassDB::add_property("HEGNode",godot::PropertyInfo(godot::Variant::Type::STRING,"name"),"setName","getName");
        godot::ClassDB::bind_method(godot::D_METHOD("getType"),&HEGNode::getType);
        godot::ClassDB::bind_method(godot::D_METHOD("getNodeInfo"),&HEGNode::getNodeInfo);
        godot::ClassDB::bind_method(godot::D_METHOD("getParent"),&HEGNode::getParent);
        godot::ClassDB::bind_method(godot::D_METHOD("getParentId"),&HEGNode::getParentId);
        godot::ClassDB::bind_method(godot::D_METHOD("getPath"),&HEGNode::getPath);
        godot::ClassDB::bind_method(godot::D_METHOD("getInputCount"),&HEGNode::getInputCount);
        godot::ClassDB::bind_method(godot::D_METHOD("getOutputCount"),&HEGNode::getOutputCount);
        godot::ClassDB::bind_method(godot::D_METHOD("getInputName","inputIndex"),&HEGNode::getInputName);
        godot::ClassDB::bind_method(godot::D_METHOD("getOutputName","outputIndex"),&HEGNode::getOutputName);
        godot::ClassDB::bind_method(godot::D_METHOD("isNodeValid"),&HEGNode::isNodeValid);
        godot::ClassDB::bind_method(godot::D_METHOD("setDisplay","display"),&HEGNode::setDisplay);
        godot::ClassDB::bind_method(godot::D_METHOD("isTimeDependent"),&HEGNode::isTimeDependent);
        godot::ClassDB::bind_method(godot::D_METHOD("getAsset"),&HEGNode::getAsset);
        godot::ClassDB::bind_method(godot::D_METHOD("cook"),&HEGNode::cook);
        godot::ClassDB::bind_method(godot::D_METHOD("connectNodeTo","resNode","resInputIdx","outputIdx"),&HEGNode::connectNodeTo,0);
        godot::ClassDB::bind_method(godot::D_METHOD("disconnectNode","inputIdx"),&HEGNode::disconnectNode);
        godot::ClassDB::bind_method(godot::D_METHOD("queryConnect","inputIdx"),&HEGNode::queryConnect);
        godot::ClassDB::bind_method(godot::D_METHOD("deleteNode"),&HEGNode::deleteNode);
        godot::ClassDB::bind_method(godot::D_METHOD("isCookFinished"),&HEGNode::isCookFinished);
        godot::ClassDB::bind_method(godot::D_METHOD("getCookState"),&HEGNode::getCookState);
        godot::ClassDB::bind_method(godot::D_METHOD("getLastCookError"),&HEGNode::getLastCookError);
        BIND_ENUM_CONSTANT(COOK_STATE_NONE);
        BIND_ENUM_CONSTANT(COOK_STATE_COOKING);
        BIND_ENUM_CONSTANT(COOK_STATE_SUCCESS);
        BIND_ENUM_CONSTANT(COOK_STATE_FAILED);
        BIND_ENUM_CONSTANT(COOK_STATE_NODE_INVALID);
        godot::ClassDB::bind_method(godot::D_METHOD("getParameter","name"),&HEGNode::getParameter);
        godot::ClassDB::bind_method(godot::D_METHOD("setParameter","name","value","autoCook"),&HEGNode::setParameter,true);
        godot::ClassDB::bind_method(godot::D_METHOD("getParameterList"),&HEGNode::getParameterList);
        godot::ClassDB::bind_method(godot::D_METHOD("refreshParameters"),&HEGNode::refreshParameters);
        godot::ClassDB::bind_method(godot::D_METHOD("getParameterInfo","name"),&HEGNode::getParameterInfo);
        godot::ClassDB::bind_method(godot::D_METHOD("getParameterInfos"),&HEGNode::getParameterInfos);
        godot::ClassDB::bind_method(godot::D_METHOD("getParameterChoices","name"),&HEGNode::getParameterChoices);
        godot::ClassDB::bind_method(godot::D_METHOD("insertMultiparmInstance","name","instancePosition"),&HEGNode::insertMultiparmInstance);
        godot::ClassDB::bind_method(godot::D_METHOD("removeMultiparmInstance","name","instancePosition"),&HEGNode::removeMultiparmInstance);
        godot::ClassDB::bind_method(godot::D_METHOD("setParameterNodeValue","name","nodeValue"),&HEGNode::setParameterNodeValue);
        godot::ClassDB::bind_method(godot::D_METHOD("getParameterNodeValue","name"),&HEGNode::getParameterNodeValue);
        godot::ClassDB::bind_method(godot::D_METHOD("revertParameter","name"),&HEGNode::revertParameter);
        godot::ClassDB::bind_method(godot::D_METHOD("getAssetId"),&HEGNode::getAssetId);
        godot::ClassDB::bind_method(godot::D_METHOD("isAssetRoot"),&HEGNode::isAssetRoot);
        godot::ClassDB::bind_method(godot::D_METHOD("duplicate"),&HEGNode::duplicate);
        godot::ClassDB::bind_method(godot::D_METHOD("rebuild"),&HEGNode::rebuild);
        godot::ClassDB::bind_method(godot::D_METHOD("resetParameters"),&HEGNode::resetParameters);
        godot::ClassDB::bind_method(godot::D_METHOD("getChildList","types","flags","recursive"),&HEGNode::getChildList,godot::PackedInt32Array{HAPI_NODETYPE_ANY},godot::PackedInt32Array{HAPI_NODEFLAGS_ANY},false);
        godot::ClassDB::bind_static_method(get_class_static(),godot::D_METHOD("createNode","label","operatorName","parentNode"),&HEGNode::createNode,godot::Ref<HEGNode>());
        godot::ClassDB::add_signal("HEGNode",godot::MethodInfo("cookFinished",godot::PropertyInfo(godot::Variant::Type::BOOL,"success")));
    }
    virtual godot::String _to_string(){
        return string_cast(std::format("<{}#{}>",string_cast(get_class()),id));
    }
    friend class HEGCenter;
protected:
    HAPI_NodeId id = -1;
    uint64_t sessionEpoch = 0;
    bool valid = false;
public:
    bool isValid() const;
    int getId(){
        return id;
    }
    godot::String getName();
    void setName(godot::String name);
    HAPI_NodeType getType();
    godot::Dictionary getNodeInfo();
    godot::Ref<HEGNode> getParent();
    int getParentId();
    godot::String getPath();
    int getInputCount();
    int getOutputCount();
    godot::String getInputName(int inputIndex);
    godot::String getOutputName(int outputIndex);
    bool isNodeValid();
    bool setDisplay(bool display);
    bool isTimeDependent();
    godot::Ref<HEGAsset> getAsset();
    bool cook();
    bool connectNodeTo(godot::Ref<HEGNode> resNode, int resInputIdx, int outputIdx = 0);
    bool disconnectNode(int inputIdx);
    godot::Ref<HEGNode> queryConnect(int inputIdx);
    void deleteNode();
    bool isCookFinished();
    int getCookState();
    godot::String getLastCookError();
    godot::Variant getParameter(godot::String name);
    void setParameter(godot::String name, godot::Variant value, bool autoCook = true);
    godot::PackedStringArray getParameterList();
    bool refreshParameters();
    godot::Dictionary getParameterInfo(godot::String name);
    godot::Array getParameterInfos();
    godot::Array getParameterChoices(godot::String name);
    bool insertMultiparmInstance(godot::String name, int instancePosition);
    bool removeMultiparmInstance(godot::String name, int instancePosition);
    bool setParameterNodeValue(godot::String name, int nodeValue);
    int getParameterNodeValue(godot::String name);
    bool revertParameter(godot::String name);
    int getAssetId();
    bool isAssetRoot();
    godot::Ref<HEGNode> duplicate();
    godot::Ref<HEGNode> rebuild();
    bool resetParameters();
    godot::Array getChildList(godot::PackedInt32Array types = {HAPI_NODETYPE_ANY}, godot::PackedInt32Array = {HAPI_NODEFLAGS_ANY}, bool recursive = false);
    static godot::Ref<HEGNode> createNode(godot::String label, godot::String operatorName, godot::Ref<HEGNode> parentNode = {});
};
class HEGAsset: public godot::RefCounted{
    GDCLASS(HEGAsset,godot::RefCounted)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("get_ownedNodeOperators"),&HEGAsset::get_ownedNodeOperators);
        godot::ClassDB::add_property("HEGAsset",godot::PropertyInfo(godot::Variant::Type::PACKED_STRING_ARRAY,"ownedNodeOperators",godot::PROPERTY_HINT_NONE,"",godot::PROPERTY_USAGE_READ_ONLY),godot::StringName(),"get_ownedNodeOperators");
        godot::ClassDB::bind_method(godot::D_METHOD("getId"),&HEGAsset::getId);
        godot::ClassDB::bind_method(godot::D_METHOD("isValid"),&HEGAsset::isValid);
        godot::ClassDB::bind_method(godot::D_METHOD("getPath"),&HEGAsset::getPath);
        godot::ClassDB::add_property("HEGAsset",godot::PropertyInfo(godot::Variant::Type::INT,"id",godot::PROPERTY_HINT_NONE,"",godot::PROPERTY_USAGE_READ_ONLY),godot::StringName(),"getId");
        godot::ClassDB::bind_method(godot::D_METHOD("getInfo"),&HEGAsset::getInfo);
        godot::ClassDB::add_property("HEGAsset",godot::PropertyInfo(godot::Variant::Type::DICTIONARY,"info",godot::PROPERTY_HINT_NONE,"",godot::PROPERTY_USAGE_READ_ONLY),godot::StringName(),"getInfo");
        godot::ClassDB::bind_method(godot::D_METHOD("getInstances"),&HEGAsset::getInstances);
        godot::ClassDB::bind_method(godot::D_METHOD("getOutputNodes"),&HEGAsset::getOutputNodes);
        godot::ClassDB::bind_method(godot::D_METHOD("createOutputNode"),&HEGAsset::createOutputNode);
        godot::ClassDB::bind_method(godot::D_METHOD("setMaterial","materialPath","material"),&HEGAsset::setMaterial);
        godot::ClassDB::bind_method(godot::D_METHOD("getMaterial","materialPath"),&HEGAsset::getMaterial);
        godot::ClassDB::bind_method(godot::D_METHOD("instantiate","operatorName","parentNode"),&HEGAsset::instantiate,godot::Ref<HEGNode>());
        godot::ClassDB::bind_method(godot::D_METHOD("cook"),&HEGAsset::cook);
        godot::ClassDB::bind_method(godot::D_METHOD("unload"),&HEGAsset::unload);
        godot::ClassDB::bind_method(godot::D_METHOD("reload"),&HEGAsset::reload);
    }
    virtual godot::String _to_string(){
        return string_cast(std::format("<{}#{}>",string_cast(get_class()),id));
    }
    friend class HEGCenter;
    friend class HEGSession;
protected:
    HAPI_AssetLibraryId id = -1;
    uint64_t sessionEpoch = 0;
    bool valid = false;
public:
    bool isValid() const;
    int getId(){
        return id;
    }
    godot::String getPath();
    godot::Ref<HEGHdaResource> res;
    std::vector<godot::String> ownedNodeOperators;
    godot::PackedStringArray get_ownedNodeOperators(){
        godot::PackedStringArray arr;
        for(auto a : ownedNodeOperators){
            arr.push_back(a);
        }
        return arr;
    }
    godot::Dictionary getInfo();
    godot::Array getInstances();
    godot::Array getOutputNodes();
    godot::Node3D* createOutputNode();
    void setMaterial(godot::String materialPath, godot::Ref<godot::Material> material);
    godot::Ref<godot::Material> getMaterial(godot::String materialPath);
    godot::Ref<HEGNode> instantiate(godot::String operatorName, godot::Ref<HEGNode> parentNode = {});
    bool cook();
    bool unload();
    bool reload();
};
class HEGSession: public godot::RefCounted{
    GDCLASS(HEGSession,godot::RefCounted)
    static void _bind_methods(){
        BIND_ENUM_CONSTANT(None);
        BIND_ENUM_CONSTANT(InProcess);
        BIND_ENUM_CONSTANT(NewNamedPipe);
        BIND_ENUM_CONSTANT(NewTCPSocket);
        BIND_ENUM_CONSTANT(ExistingNamedPipe);
        BIND_ENUM_CONSTANT(ExistingTCPSocket);
        BIND_ENUM_CONSTANT(ExistingSharedMemory);

        godot::ClassDB::bind_static_method("HEGSession", godot::D_METHOD("getSession"),&HEGSession::getSession);
        godot::ClassDB::bind_static_method("HEGSession", godot::D_METHOD("switchSession","type"),&HEGSession::switchSession);
        godot::ClassDB::bind_method(godot::D_METHOD("valid"),&HEGSession::valid);
        godot::ClassDB::bind_method(godot::D_METHOD("getSessionType"),&HEGSession::getSessionType);
        godot::ClassDB::bind_method(godot::D_METHOD("stop"),&HEGSession::stop);
        godot::ClassDB::bind_method(godot::D_METHOD("restart"),&HEGSession::restart);
        godot::ClassDB::bind_method(godot::D_METHOD("cookSession"),&HEGSession::cookSession);
        godot::ClassDB::bind_method(godot::D_METHOD("getAssets"),&HEGSession::getAssets);
        godot::ClassDB::bind_method(godot::D_METHOD("getNodes"),&HEGSession::getNodes);
        godot::ClassDB::bind_method(godot::D_METHOD("getAsset","assetId"),&HEGSession::getAsset);
        godot::ClassDB::bind_method(godot::D_METHOD("getNode","nodeId"),&HEGSession::getNode);
        godot::ClassDB::bind_method(godot::D_METHOD("loadHDA","hda"),&HEGSession::loadHDA);
        godot::ClassDB::bind_method(godot::D_METHOD("loadHDAExternal","hdaPath"),&HEGSession::loadHDAExternal);
        godot::ClassDB::bind_method(godot::D_METHOD("saveHIP","hipPath","lock"),&HEGSession::saveHIP,true);
        godot::ClassDB::bind_method(godot::D_METHOD("loadHIP","hip","append","cook"),&HEGSession::loadHIP,false,true);
        godot::ClassDB::bind_method(godot::D_METHOD("loadHIPExternal","hipPath","append","cook"),&HEGSession::loadHIPExternal,false,true);

    }
    virtual godot::String _to_string(){
        return string_cast(std::format("<{}#0>",string_cast(get_class())));
    }
    friend class HEGCenter;
    HAPI_Session session;
    HEGSessionType type = HEGSessionType::None;
    bool active = false;

    // Godot script function
    static godot::Ref<HEGSession> switchSession(HEGSessionType type);
public:
    static godot::Ref<HEGSession> getSession();
    HAPI_Session* get_session(){
        return &session;
    }
    bool valid(){
        if(!active)
            return false;
        return HoudiniApi::IsSessionValid(get_session()) == HAPI_RESULT_SUCCESS;
    }
    std::string getString(HAPI_StringHandle sh);
    int getSessionType() const;
    bool stop();
    bool restart();
    bool cookSession();
    godot::Array getAssets();
    godot::Array getNodes();
    godot::Ref<HEGAsset> getAsset(int assetId);
    godot::Ref<HEGNode> getNode(int nodeId);
    godot::Ref<HEGAsset> loadHDA(godot::Ref<HEGHdaResource> hda);
    godot::Ref<HEGAsset> loadHDAExternal(godot::String hdaPath);
    bool saveHIP(godot::String hipPath, bool lock = true);
    godot::Array loadHIP(godot::Ref<HEGHipResource> hip, bool append = false, bool cook = true);
    godot::Array loadHIPExternal(godot::String hipPath, bool append = false, bool cook = true);
};
class HEGGeometry;
class HEGObjNode: public HEGNode{
    GDCLASS(HEGObjNode, HEGNode)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("getType"),&HEGObjNode::getType);
    }
    virtual godot::String _to_string()override{
        return string_cast(std::format("<{}#{}>",string_cast(get_class()),id));
    }
    friend class HEGCenter;
public:
    HAPI_NodeType getType(){
        return HAPI_NODETYPE_OBJ;
    }

};
class HEGSopNode: public HEGNode{
    GDCLASS(HEGSopNode, HEGNode)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("getType"),&HEGSopNode::getType);
        godot::ClassDB::bind_method(godot::D_METHOD("getGeometryCount"),&HEGSopNode::getGeometryCount);
        godot::ClassDB::bind_method(godot::D_METHOD("getGeometry","partId"),&HEGSopNode::getGeometry);
        godot::ClassDB::bind_method(godot::D_METHOD("getGeometries"),&HEGSopNode::getGeometries);
        godot::ClassDB::bind_method(godot::D_METHOD("createOutputNode"),&HEGSopNode::createOutputNode);
        godot::ClassDB::bind_static_method(get_class_static(),godot::D_METHOD("inputMeshNode","label","mesh","parentId"),&HEGSopNode::inputMeshNode,godot::Ref<HEGNode>());
        godot::ClassDB::bind_static_method(get_class_static(),godot::D_METHOD("inputCurveNode","label","curve","parentId"),&HEGSopNode::inputCurveNode,godot::Ref<HEGNode>());
    }
    virtual godot::String _to_string()override{
        return string_cast(std::format("<{}#{}>",string_cast(get_class()),id));
    }
    friend class HEGCenter;
public:
    HAPI_NodeType getType(){
        return HAPI_NODETYPE_SOP;
    }
    int getGeometryCount();
    godot::Ref<HEGGeometry> getGeometry(int partId);
    godot::Array getGeometries();
    godot::Node3D* createOutputNode();
    static godot::Ref<HEGSopNode> inputMeshNode(godot::String label, godot::Ref<godot::Mesh> mesh, godot::Ref<HEGNode> parentId = godot::Ref<HEGNode>());
    static godot::Ref<HEGSopNode> inputCurveNode(godot::String label, godot::Ref<godot::Curve3D> curve, godot::Ref<HEGNode> parentId = godot::Ref<HEGNode>());
};
class HEGImage;
class HEGCopNode: public HEGNode{
    GDCLASS(HEGCopNode, HEGNode)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("getType"),&HEGCopNode::getType);
        godot::ClassDB::bind_method(godot::D_METHOD("getPNGImage"),&HEGCopNode::getPNGImage);
    }
    virtual godot::String _to_string()override{
        return string_cast(std::format("<{}#{}>",string_cast(get_class()),id));
    }
    friend class HEGCenter;
public:
    HAPI_NodeType getType(){
        return HAPI_NODETYPE_COP;
    }
    godot::Ref<HEGImage> getPNGImage();
};
class HEGShopNode: public HEGNode{
    GDCLASS(HEGShopNode, HEGNode)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("getType"),&HEGShopNode::getType);
    }
    virtual godot::String _to_string()override{
        return string_cast(std::format("<{}#{}>",string_cast(get_class()),id));
    }
    friend class HEGCenter;
public:
    HAPI_NodeType getType(){
        return HAPI_NODETYPE_SHOP;
    }
};
class HEGChopNode: public HEGNode{
    GDCLASS(HEGChopNode, HEGNode)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("getType"),&HEGChopNode::getType);
    }
    virtual godot::String _to_string()override{
        return string_cast(std::format("<{}#{}>",string_cast(get_class()),id));
    }
    friend class HEGCenter;
public:
    HAPI_NodeType getType(){
        return HAPI_NODETYPE_CHOP;
    }
};
class HEGRopNode: public HEGNode{
    GDCLASS(HEGRopNode, HEGNode)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("getType"),&HEGRopNode::getType);
    }
    virtual godot::String _to_string()override{
        return string_cast(std::format("<{}#{}>",string_cast(get_class()),id));
    }
    friend class HEGCenter;
public:
    HAPI_NodeType getType(){
        return HAPI_NODETYPE_ROP;
    }
};
class HEGVopNode: public HEGNode{
    GDCLASS(HEGVopNode, HEGNode)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("getType"),&HEGVopNode::getType);
    }
    virtual godot::String _to_string()override{
        return string_cast(std::format("<{}#{}>",string_cast(get_class()),id));
    }
    friend class HEGCenter;
public:
    HAPI_NodeType getType(){
        return HAPI_NODETYPE_VOP;
    }
};
class HEGDopNode: public HEGNode{
    GDCLASS(HEGDopNode, HEGNode)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("getType"),&HEGDopNode::getType);
    }
    virtual godot::String _to_string()override{
        return string_cast(std::format("<{}#{}>",string_cast(get_class()),id));
    }
    friend class HEGCenter;
public:
    HAPI_NodeType getType(){
        return HAPI_NODETYPE_DOP;
    }
};
class HEGTopNode: public HEGNode{
    GDCLASS(HEGTopNode, HEGNode)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("getType"),&HEGTopNode::getType);
    }
    virtual godot::String _to_string()override{
        return string_cast(std::format("<{}#{}>",string_cast(get_class()),id));
    }
    friend class HEGCenter;
public:
    HAPI_NodeType getType(){
        return HAPI_NODETYPE_TOP;
    }
};
class HEGGeometry: public godot::RefCounted{
    GDCLASS(HEGGeometry, godot::RefCounted)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("wrap"),&HEGGeometry::wrap);
    }
    virtual godot::String _to_string(){
        return string_cast(std::format("<{}#0>",string_cast(get_class())));
    }
    friend class HEGCenter;
public:
    //This return node should not be set owner for not saving.
    virtual godot::Node3D* wrap(){
        godot::Node3D* node = memnew(godot::Node3D);
        return node;
    }
};
class HEGMesh: public HEGGeometry{
    GDCLASS(HEGMesh, HEGGeometry)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("bakeAsMesh"),&HEGMesh::bakeAsMesh);
        godot::ClassDB::bind_method(godot::D_METHOD("setMaterial","surfaceIndex","material"),&HEGMesh::setMaterial);
        godot::ClassDB::bind_method(godot::D_METHOD("getMaterial","surfaceIndex"),&HEGMesh::getMaterial);
        godot::ClassDB::bind_method(godot::D_METHOD("wrap"),&HEGMesh::wrap);
    }
    virtual godot::String _to_string()override{
        return string_cast(std::format("<{}#({},{})>",string_cast(get_class()),nodeId,partId));
    }
    friend class HEGCenter;
    int nodeId;
    int partId;
public:
    static godot::Ref<HEGMesh> make_mesh(int nodeId, int partId){
        godot::Ref<HEGMesh> mesh;
        mesh.instantiate();
        mesh->nodeId = nodeId;
        mesh->partId = partId;
        return mesh;
    }
    godot::Node3D* wrap() override;
    godot::Ref<godot::Mesh> bakeAsMesh();
    bool setMaterial(int surfaceIndex, godot::Ref<godot::Material> material);
    godot::Ref<godot::Material> getMaterial(int surfaceIndex);
};

class HEGMultiMesh: public HEGGeometry{
    GDCLASS(HEGMultiMesh, HEGGeometry)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("bakeAsMultiMesh"),&HEGMultiMesh::bakeAsMultiMesh);
        godot::ClassDB::bind_method(godot::D_METHOD("wrap"),&HEGMultiMesh::wrap);
    }
    virtual godot::String _to_string()override{
        return string_cast(std::format("<{}#({},{})>",string_cast(get_class()),nodeId,partId));
    }
    friend class HEGCenter;
    int nodeId;
    int partId;
public:
    static godot::Ref<HEGMultiMesh> make_multimesh(int nodeId, int partId){
        godot::Ref<HEGMultiMesh> multimesh;
        multimesh.instantiate();
        multimesh->nodeId = nodeId;
        multimesh->partId = partId;
        return multimesh;
    }
    godot::Node3D* wrap() override;
    godot::Dictionary bakeAsMultiMesh();
};

class HEGCurve: public HEGGeometry{
    GDCLASS(HEGCurve, HEGGeometry)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("bakeAsCurve"),&HEGCurve::bakeAsCurve);
        godot::ClassDB::bind_method(godot::D_METHOD("wrap"),&HEGCurve::wrap);
    }
    virtual godot::String _to_string()override{
        return string_cast(std::format("<{}#({},{})>",string_cast(get_class()),nodeId,partId));
    }
    friend class HEGCenter;
    int nodeId;
    int partId;
public:
    static godot::Ref<HEGCurve> make_curve(int nodeId, int partId){
        godot::Ref<HEGCurve> curve;
        curve.instantiate();
        curve->nodeId = nodeId;
        curve->partId = partId;
        return curve;
    }
    godot::Node3D* wrap() override;
    godot::Array bakeAsCurve();
};
class HEGImage: public godot::RefCounted{
    GDCLASS(HEGImage, godot::RefCounted)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("bakeAsImage"),&HEGImage::bakeAsImage);
    }
    virtual godot::String _to_string(){
        return string_cast(std::format("<{}#{}>",string_cast(get_class()),nodeId));
    }
    friend class HEGCenter;
    int nodeId;
public:
    static godot::Ref<HEGImage> make_image(int nodeId){
        godot::Ref<HEGImage> mesh;
        mesh.instantiate();
        mesh->nodeId = nodeId;
        return mesh;
    }
    godot::Ref<godot::Image> bakeAsImage();
};
#endif