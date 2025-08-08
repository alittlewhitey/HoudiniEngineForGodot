#ifndef HOUDINI_ENGINE_CLASS
#define HOUDINI_ENGINE_CLASS
#include "DebugSymbol.h"

#include <vector>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/classes/multi_mesh.hpp>
#include <godot_cpp/classes/curve3d.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/variant/typed_dictionary.hpp>
#include <HoudiniApi.h>
#include <HoudiniEngineUtility.h>
#include <HoudiniEnginePlatform.h>
#include "HDAImporter.h"
#include "HEBindEnum.h"

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
HE_ENUM_CAST(SessionType)
class HESession;
class HEUtil: public godot::RefCounted{
    GDCLASS(HEUtil,godot::RefCounted)
    static void _bind_methods(){
        godot::ClassDB::bind_static_method("HEUtil", godot::D_METHOD("getLastError"), &HEUtil::getLastError);
    }
    static godot::String getLastError(){
        return string_cast(_get_last_error());
    }
};
class HENode: public godot::RefCounted{
    GDCLASS(HENode,godot::RefCounted)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("getId"),&HENode::getId);
        godot::ClassDB::bind_method(godot::D_METHOD("setId","__"),&HENode::setId);
        godot::ClassDB::add_property("HENode",godot::PropertyInfo(godot::Variant::Type::INT,"id"),"setId","getId");
        godot::ClassDB::bind_method(godot::D_METHOD("getName"),&HENode::getName);
        godot::ClassDB::bind_method(godot::D_METHOD("setName","name"),&HENode::setName);
        godot::ClassDB::add_property("HENode",godot::PropertyInfo(godot::Variant::Type::STRING,"name"),"setName","getName");
        godot::ClassDB::bind_method(godot::D_METHOD("getType"),&HENode::getType);
        godot::ClassDB::bind_method(godot::D_METHOD("cook"),&HENode::cook);
        godot::ClassDB::bind_method(godot::D_METHOD("isCookFinished"),&HENode::isCookFinished);
        godot::ClassDB::bind_method(godot::D_METHOD("getParameter","name"),&HENode::getParameter);
        godot::ClassDB::bind_method(godot::D_METHOD("setParameter","name","value"),&HENode::setParameter);
        godot::ClassDB::bind_method(godot::D_METHOD("getParameterList"),&HENode::getParameterList);
        godot::ClassDB::bind_method(godot::D_METHOD("getChildList","types","flags","recursive"),&HENode::getChildList,godot::PackedInt32Array{HAPI_NODETYPE_ANY},godot::PackedInt32Array{HAPI_NODEFLAGS_ANY},false);
        godot::ClassDB::add_signal("HENode",godot::MethodInfo("cookFinished"));
    }
    virtual godot::String _to_string(){
        return string_cast(std::format("<{}#{}>",string_cast(get_class()),id));
    }
    friend class HECenter;
    void setId(int){}
    void setType(int){}
protected:
    HAPI_NodeId id;
public:
    int getId(){
        return id;
    }
    godot::String getName();
    void setName(godot::String name);
    HAPI_NodeType getType();
    bool cook();
    bool isCookFinished();
    godot::Variant getParameter(godot::String name);
    void setParameter(godot::String name, godot::Variant value);
    godot::PackedStringArray getParameterList();
    godot::Array getChildList(godot::PackedInt32Array types = {HAPI_NODETYPE_ANY}, godot::PackedInt32Array = {HAPI_NODEFLAGS_ANY}, bool recursive = false);
};
class HEAsset: public godot::RefCounted{
    GDCLASS(HEAsset,godot::RefCounted)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("get_ownedNodeOperators"),&HEAsset::get_ownedNodeOperators);
        godot::ClassDB::bind_method(godot::D_METHOD("set_ownedNodeOperators","__"),&HEAsset::set_ownedNodeOperators);
        godot::ClassDB::add_property("HEAsset",godot::PropertyInfo(godot::Variant::Type::PACKED_STRING_ARRAY,"ownedNodeOperators"),"set_ownedNodeOperators","get_ownedNodeOperators");
    }
    virtual godot::String _to_string(){
        return string_cast(std::format("<{}#{}>",string_cast(get_class()),id));
    }
    friend class HECenter;
    friend class HESession;

    HAPI_AssetLibraryId id;
    godot::Ref<HDAResource> res;
    std::vector<godot::String> ownedNodeOperators;
    godot::PackedStringArray get_ownedNodeOperators(){
        godot::PackedStringArray arr;
        for(auto a : ownedNodeOperators){
            arr.push_back(a);
        }
        return arr;
    }
    void set_ownedNodeOperators(godot::Array){}

};
class HESession: public godot::RefCounted{
    GDCLASS(HESession,godot::RefCounted)
    static void _bind_methods(){
        BIND_ENUM_CONSTANT(None);
        BIND_ENUM_CONSTANT(InProcess);
        BIND_ENUM_CONSTANT(NewNamedPipe);
        BIND_ENUM_CONSTANT(NewTCPSocket);
        BIND_ENUM_CONSTANT(ExistingNamedPipe);
        BIND_ENUM_CONSTANT(ExistingTCPSocket);
        BIND_ENUM_CONSTANT(ExistingSharedMemory);

        godot::ClassDB::bind_static_method("HESession", godot::D_METHOD("getSession"),&HESession::getSession);
        godot::ClassDB::bind_static_method("HESession", godot::D_METHOD("switchSession","type"),&HESession::switchSession);
        godot::ClassDB::bind_method(godot::D_METHOD("valid"),&HESession::valid);
        godot::ClassDB::bind_method(godot::D_METHOD("loadHDA","hda"),&HESession::loadHDA);
        godot::ClassDB::bind_method(godot::D_METHOD("loadHDAExternal","hdaPath"),&HESession::loadHDAExternal);
        godot::ClassDB::bind_method(godot::D_METHOD("createNode","label","operatorName","parentNode"),&HESession::createNode,godot::Ref<HENode>());
        godot::ClassDB::bind_method(godot::D_METHOD("connectNode","connectingNode","inputIdx","connectedNode","connectedOutputIdx"),&HESession::connectNode);
        godot::ClassDB::bind_method(godot::D_METHOD("deleteNode","node"),&HESession::deleteNode);
    }
    virtual godot::String _to_string(){
        return string_cast(std::format("<{}#0>",string_cast(get_class())));
    }
    friend class HECenter;
    HAPI_Session session;
    SessionType type = SessionType::None;
    bool active = false;

    // Godot script function
    static godot::Ref<HESession> switchSession(SessionType type);
public:
    static godot::Ref<HESession> getSession();
    HAPI_Session* get_session(){
        return &session;
    }
    bool valid(){
        if(!active)
            return false;
        return HoudiniApi::IsSessionValid(get_session()) == HAPI_RESULT_SUCCESS;
    }
    std::string getString(HAPI_StringHandle sh);
    godot::Ref<HEAsset> loadHDA(godot::Ref<HDAResource> hda);
    godot::Ref<HEAsset> loadHDAExternal(godot::String hdaPath);
    godot::Ref<HENode> createNode(godot::String label, godot::String operatorName, godot::Ref<HENode> parentNode = {});
    godot::Ref<HENode> inputMeshNode(godot::String nodeLabel, godot::Ref<godot::Mesh> mesh, godot::Ref<HENode> parentId = godot::Ref<HENode>());
    bool connectNode(godot::Ref<HENode> connectingNode, int inputIdx, godot::Ref<HENode> connectedNode, int connectedOutputIdx);
    void deleteNode(godot::Ref<HENode> node);
};
class HEGeometry;
class HEObjNode: public HENode{
    GDCLASS(HEObjNode, HENode)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("getType"),&HEObjNode::getType);
    }
    virtual godot::String _to_string()override{
        return string_cast(std::format("<{}#{}>",string_cast(get_class()),id));
    }
    friend class HECenter;
public:
    HAPI_NodeType getType(){
        return HAPI_NODETYPE_OBJ;
    }

};
class HESopNode: public HENode{
    GDCLASS(HESopNode, HENode)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("getType"),&HESopNode::getType);
        godot::ClassDB::bind_method(godot::D_METHOD("getGeometryCount"),&HESopNode::getGeometryCount);
        godot::ClassDB::bind_method(godot::D_METHOD("getGeometry","partId"),&HESopNode::getGeometry);
    }
    virtual godot::String _to_string()override{
        return string_cast(std::format("<{}#{}>",string_cast(get_class()),id));
    }
    friend class HECenter;
public:
    HAPI_NodeType getType(){
        return HAPI_NODETYPE_SOP;
    }
    int getGeometryCount();
    godot::Ref<HEGeometry> getGeometry(int partId);
};
class HEImage;
class HECopNode: public HENode{
    GDCLASS(HECopNode, HENode)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("getType"),&HECopNode::getType);
        godot::ClassDB::bind_method(godot::D_METHOD("getPNGImage"),&HECopNode::getPNGImage);
    }
    virtual godot::String _to_string()override{
        return string_cast(std::format("<{}#{}>",string_cast(get_class()),id));
    }
    friend class HECenter;
public:
    HAPI_NodeType getType(){
        return HAPI_NODETYPE_COP;
    }
    godot::Ref<HEImage> getPNGImage();
};
class HEShopNode: public HENode{
    GDCLASS(HEShopNode, HENode)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("getType"),&HEShopNode::getType);
    }
    virtual godot::String _to_string()override{
        return string_cast(std::format("<{}#{}>",string_cast(get_class()),id));
    }
    friend class HECenter;
public:
    HAPI_NodeType getType(){
        return HAPI_NODETYPE_SHOP;
    }
};
class HEChopNode: public HENode{
    GDCLASS(HEChopNode, HENode)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("getType"),&HEChopNode::getType);
    }
    virtual godot::String _to_string()override{
        return string_cast(std::format("<{}#{}>",string_cast(get_class()),id));
    }
    friend class HECenter;
public:
    HAPI_NodeType getType(){
        return HAPI_NODETYPE_CHOP;
    }
};
class HERopNode: public HENode{
    GDCLASS(HERopNode, HENode)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("getType"),&HERopNode::getType);
    }
    virtual godot::String _to_string()override{
        return string_cast(std::format("<{}#{}>",string_cast(get_class()),id));
    }
    friend class HECenter;
public:
    HAPI_NodeType getType(){
        return HAPI_NODETYPE_ROP;
    }
};
class HEVopNode: public HENode{
    GDCLASS(HEVopNode, HENode)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("getType"),&HEVopNode::getType);
    }
    virtual godot::String _to_string()override{
        return string_cast(std::format("<{}#{}>",string_cast(get_class()),id));
    }
    friend class HECenter;
public:
    HAPI_NodeType getType(){
        return HAPI_NODETYPE_VOP;
    }
};
class HEDopNode: public HENode{
    GDCLASS(HEDopNode, HENode)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("getType"),&HEDopNode::getType);
    }
    virtual godot::String _to_string()override{
        return string_cast(std::format("<{}#{}>",string_cast(get_class()),id));
    }
    friend class HECenter;
public:
    HAPI_NodeType getType(){
        return HAPI_NODETYPE_DOP;
    }
};
class HETopNode: public HENode{
    GDCLASS(HETopNode, HENode)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("getType"),&HETopNode::getType);
    }
    virtual godot::String _to_string()override{
        return string_cast(std::format("<{}#{}>",string_cast(get_class()),id));
    }
    friend class HECenter;
public:
    HAPI_NodeType getType(){
        return HAPI_NODETYPE_TOP;
    }
};
class HEGeometry: public godot::RefCounted{
    GDCLASS(HEGeometry, godot::RefCounted)
    static void _bind_methods(){}
    virtual godot::String _to_string(){
        return string_cast(std::format("<{}#0>",string_cast(get_class())));
    }
    friend class HECenter;
};
class HEMesh: public HEGeometry{
    GDCLASS(HEMesh, HEGeometry)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("bakeAsMesh"),&HEMesh::bakeAsMesh);
    }
    virtual godot::String _to_string()override{
        return string_cast(std::format("<{}#({},{})>",string_cast(get_class()),nodeId,partId));
    }
    friend class HECenter;
    int nodeId;
    int partId;
public:
    static godot::Ref<HEMesh> make_mesh(int nodeId, int partId){
        godot::Ref<HEMesh> mesh;
        mesh.instantiate();
        mesh->nodeId = nodeId;
        mesh->partId = partId;
        return mesh;
    }
    godot::Ref<godot::Mesh> bakeAsMesh();
};

class HEMultiMesh: public HEGeometry{
    GDCLASS(HEMultiMesh, HEGeometry)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("bakeAsMultiMesh"),&HEMultiMesh::bakeAsMultiMesh);
    }
    virtual godot::String _to_string()override{
        return string_cast(std::format("<{}#({},{})>",string_cast(get_class()),nodeId,partId));
    }
    friend class HECenter;
    int nodeId;
    int partId;
public:
    static godot::Ref<HEMultiMesh> make_multimesh(int nodeId, int partId){
        godot::Ref<HEMultiMesh> multimesh;
        multimesh.instantiate();
        multimesh->nodeId = nodeId;
        multimesh->partId = partId;
        return multimesh;
    }
    godot::Dictionary bakeAsMultiMesh();
};

class HECurve: public HEGeometry{
    GDCLASS(HECurve, HEGeometry)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("bakeAsCurve"),&HECurve::bakeAsCurve);
    }
    virtual godot::String _to_string()override{
        return string_cast(std::format("<{}#({},{})>",string_cast(get_class()),nodeId,partId));
    }
    friend class HECenter;
    int nodeId;
    int partId;
public:
    static godot::Ref<HECurve> make_curve(int nodeId, int partId){
        godot::Ref<HECurve> curve;
        curve.instantiate();
        curve->nodeId = nodeId;
        curve->partId = partId;
        return curve;
    }
    godot::Array bakeAsCurve();
};
class HEImage: public godot::RefCounted{
    GDCLASS(HEImage, godot::RefCounted)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("bakeAsImage"),&HEImage::bakeAsImage);
    }
    virtual godot::String _to_string(){
        return string_cast(std::format("<{}#{}>",string_cast(get_class()),nodeId));
    }
    friend class HECenter;
    int nodeId;
public:
    static godot::Ref<HEImage> make_image(int nodeId){
        godot::Ref<HEImage> mesh;
        mesh.instantiate();
        mesh->nodeId = nodeId;
        return mesh;
    }
    godot::Ref<godot::Image> bakeAsImage();
};
#endif