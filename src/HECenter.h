#ifndef HOUDINI_ENGINE_CERTER
#define HOUDINI_ENGINE_CERTER
#include "DebugSymbol.h"

#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <variant>
#include <filesystem>
#include <concepts>
#include <thread>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/classes/multi_mesh.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/box_mesh.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/editor_interface.hpp>
#include <godot_cpp/classes/display_server.hpp>
#include <godot_cpp/classes/sub_viewport.hpp>
#include <godot_cpp/classes/surface_tool.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <HoudiniApi.h>
#include <HoudiniEngineUtility.h>
#include <HoudiniEnginePlatform.h>

#include "Utility.h"
#include "Contact.h"
#include "HEImporter.h"
#include "HEClass.h"
#include "HESettings.h"
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
struct MeshPartData {
    std::vector<int> faceCounts;
    std::vector<float> positions;
    std::vector<int> vertexList;
    std::pair<AttribOwner, std::vector<float>> colors;
    std::pair<AttribOwner, std::vector<float>> normals;
    std::pair<AttribOwner, std::vector<float>> uvs;
    std::pair<AttribOwner, std::vector<float>> uv2s;
};
struct CurvePartData {
    HAPI_CurveInfo info;
    std::vector<int> curveCounts;
    std::vector<float> controlPoints;
    std::vector<float> weights;
    std::vector<float> knots;
};
struct InstancerPartData {
    std::vector<int> instancedPartIds;
    std::vector<HAPI_Transform> transforms;
};
struct AttributesData {
    std::vector<HAPI_AttributeInfo> pointAttribs;
    std::vector<HAPI_AttributeInfo> vertexAttribs;
    std::vector<HAPI_AttributeInfo> primAttribs;
    std::vector<HAPI_AttributeInfo> detailAttribs;
};
class HECenter: public godot::Node{
    GDCLASS(HECenter, godot::Node)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("freeGDNode","node"),static_cast<bool(HECenter::*)(godot::Node*)>(&HECenter::freeGDNode));
        godot::ClassDB::bind_method(godot::D_METHOD("stopFreeGDNode","node"),static_cast<bool(HECenter::*)(godot::Node*)>(&HECenter::stopFreeGDNode));
        godot::ClassDB::add_signal("HECenter", godot::MethodInfo("SessionStarted"));
    }
    void _notification(int what){
        try{
            switch(what){
            case NOTIFICATION_ENTER_TREE:{
                init();
                set_process(1);
                HESettings::get_singleton();
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
        }catch(const std::exception& e){
            printError(e.what());
        }
    }
    void init(){
        godot::OS::get_singleton()->set_low_processor_usage_mode(true);
        freeTimeout = HESettings::defaultFreeTimeout;
        godot::Ref<godot::StandardMaterial3D> defaultMaterial;
        defaultMaterial.instantiate();
        defaultMaterial->set_flag(godot::BaseMaterial3D::Flags::FLAG_ALBEDO_FROM_VERTEX_COLOR,true);
        materialRes[""] = defaultMaterial;

        get_tree()->connect("node_removed",godot::Callable(this,"freeGDNode"));
        get_tree()->connect("node_added",godot::Callable(this,"stopFreeGDNode"));
    }
    void process(){
        if(ensureSession()){
            if(focused != godot::DisplayServer::get_singleton()->window_is_focused()){
                focused = !focused;
                if(focused)
                    sessionCookSync();
            }
            syncCamera();
        }
    }
    void term(){
        get_tree()->disconnect("node_removed",godot::Callable((godot::Object*)this,"freeGDNode"));
        get_tree()->disconnect("node_added",godot::Callable((godot::Object*)this,"stopFreeGDNode"));
    } 
    void predel(){
        if(getHESession()->valid()){
            stopSession();
        }
        using namespace std::chrono_literals;
        freeTimeout = 0ms;
        for(auto& [node, threadPtr] : freeGDNodeTasks){
            threadPtr->request_stop();
        }
        for(auto& [node, threadPtr] : freeGDNodeTasks){
            if(threadPtr->joinable())
                threadPtr->join();
        }
        freeGDNodeTasks.clear();
        std::filesystem::remove_all(get_temp_dir());
    }
//Class

//Configs:

    bool focused = false;
    std::chrono::milliseconds freeTimeout = HESettings::defaultFreeTimeout;
    SessionType sessionType = SessionType::InProcess;
    //TODO:
    bool syncViewport = 0;
    int maxSessionRetries = 3;
    int sessionRetryCooldownSec = 5;
    int _retryCount = 0;
    std::chrono::steady_clock::time_point _lastRetryTime;

//Datas:

    godot::Ref<HESession> session;

    std::unordered_set<int> nodeIds;
    //      NodeId          NodeRef
    std::unordered_map<int,godot::Ref<HENode>> nodeRefs;
    //      AssetId         AssetRef
    std::unordered_map<int,godot::Ref<HEAsset>> assetRefs;
    //      NodeId, CookState (0=None,1=Cooking,2=Success,3=Failed,4=NodeInvalid)
    std::unordered_map<int, int> cookStatus;
    //      NodeId          ParamName            ParamValues
    std::unordered_map<int,std::unordered_map<std::string,std::vector<std::variant<int64_t,double,std::string>>>> parameters;
    //      nodeId      partId  type
    std::unordered_map<int,std::unordered_map<int,PartType>> partType;
    //      nodeId    meshPartId                    faces            P            vertexs                          Cd                                      N                                           uv                                  uv2
    std::unordered_map<int,std::unordered_map<int,MeshPartData>> meshGeometries;
    //      nodeId    curvePartId          info             curveCounts       controlPoints       weights         knots
    std::unordered_map<int,std::unordered_map<int,CurvePartData>> curveGeometries;
    //      nodeId  instancerPartId       instancedPartIds          transform
    std::unordered_map<int,std::unordered_map<int,InstancerPartData>> packedPrimData;
    //Must fill from data directly rather than from other mesh.
    //      nodeId  instancerPartId   instancedPartId     multiMesh
    std::unordered_map<int,std::unordered_map<int,std::unordered_map<int,godot::Ref<godot::MultiMesh>>>> packedPrimMesh;
    //      nodeId      partId      sharedMesh
    std::unordered_map<int,std::unordered_map<int,godot::Ref<godot::ArrayMesh>>> meshRefs;
    //      nodeId      partId      curveMesh
    std::unordered_map<int,std::unordered_map<int,std::vector<godot::Ref<godot::Curve3D>>>> curveRefs;
    //      nodeId       partId                 Point-Attrib                Vertex-Attrib               Prim-Attrib                     Detail-Attrib    
    std::unordered_map<int,std::unordered_map<int,AttributesData>> attributes;
    //      nodeId      partId      materialResPath
    std::unordered_map<int,std::unordered_map<int,std::vector<std::string>>> materials;
    //      nodeId      partId       if-allSame     material-nodeIds
    std::unordered_map<int,std::unordered_map<int,std::pair<bool,std::vector<int>>>> materialIds;
    //      nodeId      info                imageData
    std::unordered_map<int,std::pair<HAPI_ImageInfo,godot::PackedByteArray>> images;
    //      nodeId      image
    std::unordered_map<int, godot::Ref<godot::Image>> imageRefs;
    //      nodeId  cookCount
    std::unordered_map<int,int> cookCounts;
    // made by user
    //      godot path          Material instance
    std::unordered_map<std::string,godot::Ref<godot::Material>> materialRes;
    // for `freeGDNode` and `stopFreeGDNode`
    std::unordered_set<godot::Node*> createdGDNodes;
    std::unordered_map<godot::Node*, std::shared_ptr<std::jthread>> freeGDNodeTasks;

    void cleanup(){
        nodeIds.clear();
        nodeRefs.clear();
        assetRefs.clear();
        cookStatus.clear();
        cookCounts.clear();
        parameters.clear();
        partType.clear();
        meshGeometries.clear();
        meshRefs.clear();
        images.clear();
        imageRefs.clear();
        materials.clear();
        materialIds.clear();
        attributes.clear();
        curveGeometries.clear();
        curveRefs.clear();
        packedPrimData.clear();
        packedPrimMesh.clear();
        createdGDNodes.clear();
    }
    void _delete_data(int nodeId){
        nodeIds.erase(nodeId);
        nodeRefs.erase(nodeId);
        cookStatus.erase(nodeId);
        cookCounts.erase(nodeId);
        parameters.erase(nodeId);
        partType.erase(nodeId);
        meshGeometries.erase(nodeId);
        meshRefs.erase(nodeId);
        images.erase(nodeId);
        imageRefs.erase(nodeId);
        materials.erase(nodeId);
        materialIds.erase(nodeId);
        attributes.erase(nodeId);
        curveGeometries.erase(nodeId);
        curveRefs.erase(nodeId);
        packedPrimData.erase(nodeId);
        packedPrimMesh.erase(nodeId);
    }
//Functions:
    void _init_hserver(){
        if(!findproc("hserver")){
            printError("Can't find hserver. Try to restart it by hkey.");
            printError("Please restart the hserver to manually.");
            printWarning("Run ",(HESettings::get_singleton()->houdiniRootPath+"/bin/hkey").c_str()," to restart hserver");
            if(HESettings::get_singleton()->houdiniRootPath.empty()){
                printWarning("Run Houdini License Administrator (hkey) to restart hserver");
            }else{
                printWarning("Run Houdini License Administrator (",(HESettings::get_singleton()->houdiniRootPath+"/bin/hkey").c_str(),") to restart hserver");
            }
        }
    }
    bool freeGDNode(godot::Node* node){
        if(createdGDNodes.find(node) != createdGDNodes.end()){
            auto threadPtr = std::make_shared<std::jthread>([this,node](std::stop_token st){
                std::this_thread::sleep_for(freeTimeout);
                {
                    auto it = freeGDNodeTasks.find(node);
                    if(it != freeGDNodeTasks.end())
                        freeGDNodeTasks.erase(node);
                }
                if(st.stop_requested())
                    return;
                Contact::add_call([=]{
                    node->queue_free();
                });
            });
            freeGDNodeTasks[node] = threadPtr;
            return true;
        }
        return false;
    }
    bool stopFreeGDNode(godot::Node* node){
        if(freeGDNodeTasks.find(node)!=freeGDNodeTasks.end()){
            freeGDNodeTasks[node]->request_stop();
            return true;
        }
        return false;
    }
    //TODO:
    void syncCamera(){
        if(!syncViewport)
            return;
        auto* viewport3d = godot::EditorInterface::get_singleton()->get_editor_viewport_3d();
        if(!viewport3d)
            return;
        godot::Camera3D* camera = viewport3d->get_camera_3d();
        if(!camera)
            return;
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
    void sessionCookSync(){
        for(auto id : nodeIds){
            nodeCookSync(id);
        }
    }
    void nodeCookSync(int id){
        if(auto a = cookCounts.find(id);a != cookCounts.end()){
            int count = 0;
            if(HoudiniApi::GetTotalCookCount(get_session(),id
                ,(HAPI_NodeTypeBits)(HAPI_NODETYPE_OBJ|HAPI_NODETYPE_SOP)
                ,(HAPI_NodeFlagsBits)(HAPI_NODEFLAGS_DISPLAY|HAPI_NODEFLAGS_RENDER|HAPI_NODEFLAGS_OBJ_GEOMETRY)
                ,true,&count) != HAPI_RESULT_SUCCESS)
                return;
            if(count != a->second){
                a->second = count;
                cookNode(id);
            }
        }
    }
public:

    bool ensureSession(){
        if(getHESession()->valid())
            return true;

        if(getHESession()->type == SessionType::None)
            return false;

        auto now = std::chrono::steady_clock::now();
        if(now - _lastRetryTime < std::chrono::seconds(sessionRetryCooldownSec))
            return false;

        _lastRetryTime = now;

        if(_retryCount >= maxSessionRetries){
            printError("Session recovery failed after ", maxSessionRetries, " attempts");
            return false;
        }

        auto type = sessionType;
        if(type == SessionType::None)
            type = SessionType::InProcess;

        printWarning("Session lost, attempting recovery (", _retryCount+1, "/", maxSessionRetries, ")...");
        stopSession();
        if(!startSession(type)){
            _retryCount++;
            return false;
        }

        _retryCount = 0;
        return true;
    }

//Datas:

//Functions:

    static HECenter* get_singleton(){
        static HECenter* singleton = nullptr;
        if(singleton == nullptr)
            singleton = memnew(HECenter());
        return singleton;
    }
    //Return relative path if in res / user dir tree.
    static godot::String getPluginPath(){
        return godot::ProjectSettings::get_singleton()->localize_path(string_cast(get_current_dylib_path()));
    }
    godot::Ref<HESession> getHESession(){
        if(session.is_null())
            session.instantiate();
        return session;
    }
    HAPI_NodeInfo getNodeInfo(int id){
        HAPI_NodeInfo info;
        if(HoudiniApi::GetNodeInfo(get_session(),id,&info) != HAPI_RESULT_SUCCESS){
            return {};
        }
        return info;
    }
    HAPI_GeoInfo getGeoInfo(int sopId){
        HAPI_GeoInfo info;
        if(HoudiniApi::GetGeoInfo(get_session(),sopId,&info) != HAPI_RESULT_SUCCESS){
            return {};
        }
        return info;
    }
    std::vector<int> getNodeIds(){
        std::vector<int> ids;
        ids.reserve(nodeIds.size());
        for(auto a : nodeIds){
            ids.push_back(a);
        }
        return ids;
    }
    std::vector<int> getAssetIds(){
        std::vector<int> ids;
        ids.reserve(assetRefs.size());
        for(auto& a : assetRefs){
            ids.push_back(a.first);
        }
        return ids;
    }
    int getCookStatus(int nodeId){
        auto it = cookStatus.find(nodeId);
        if(it == cookStatus.end())
            return 0;
        return it->second;
    }
    std::vector<std::variant<int64_t,double,std::string>> getParameter(int nodeId, std::string name){
        return parameters[nodeId][name];
    }
    void setParameter(int nodeId, std::string name, std::vector<std::variant<int64_t,double,std::string>> value){
        auto& res = parameters[nodeId][name];

        if(res.size() == 1){
            if(std::holds_alternative<int64_t>(res[0])){
                HoudiniApi::SetParmIntValue(get_session(),nodeId,name.c_str(),0,std::get<int64_t>(value[0]));
            }else if(std::holds_alternative<double>(res[0])){
                HoudiniApi::SetParmFloatValue(get_session(),nodeId,name.c_str(),0,std::get<double>(value[0]));
            }else if(std::holds_alternative<std::string>(res[0])){
                HAPI_ParmId parmId;
                HoudiniApi::GetParmIdFromName(get_session(),nodeId,name.c_str(),&parmId);
                HoudiniApi::SetParmStringValue(get_session(),nodeId,std::get<std::string>(value[0]).c_str(),parmId,0);
            }
        }else{
            int size = value.size();
            
            for(int i = 0;i!=size;++i){
                if(std::holds_alternative<int64_t>(value[i])){
                    HoudiniApi::SetParmIntValue(get_session(),nodeId,name.c_str(),i,std::get<int64_t>(value[i]));
                }else if(std::holds_alternative<double>(value[i])){
                    HoudiniApi::SetParmFloatValue(get_session(),nodeId,name.c_str(),i,std::get<double>(value[i]));
                }else if(std::holds_alternative<std::string>(value[i])){
                    HAPI_ParmId parmId;
                    HoudiniApi::GetParmIdFromName(get_session(),nodeId,name.c_str(),&parmId);
                    HoudiniApi::SetParmStringValue(get_session(),nodeId,std::get<std::string>(value[i]).c_str(),parmId,i);
                }
            }
        }
        parameters[nodeId][name] = std::move(value);
        if(HESettings::get_singleton()->autoCook)
            cookNode(nodeId);
    }
    std::vector<std::string> getParameterList(int nodeId){
        std::vector<std::string> res;
        res.reserve(parameters[nodeId].size());
        for(auto& a : parameters[nodeId]){
            res.push_back(a.first);
        }
        return res;
    }
    godot::Node* get_scene_root(){
        godot::SceneTree *tree = Object::cast_to<godot::SceneTree>(godot::Engine::get_singleton()->get_main_loop());
        if(godot::Engine::get_singleton()->is_editor_hint()){
            return tree->get_edited_scene_root();
        }else{
            return tree->get_current_scene();
        }
    }
    HAPI_Session* get_session(){
        if(session.is_null())
            session.instantiate();
        return session->get_session();
    }
    template<typename T> requires std::derived_from<T,godot::Node>
    T* makeTreeNode(std::string name, godot::Node* father, godot::Node::InternalMode p_internal = godot::Node::INTERNAL_MODE_DISABLED){
        godot::Node* root = get_scene_root();
        godot::Node* node = memnew(T());
        node->set_name(name.c_str());
        father->add_child(node,true,p_internal);
        node->set_owner(root);
        createdGDNodes.insert(node);
        return (T*)node;
    }
    void removeTreeNode(godot::Node* treeNode){
        treeNode->get_parent()->remove_child(treeNode);
    }
    int getPartCount(int nodeId){
        return partType[nodeId].size();
    }
    PartType getPartType(int nodeId, int partId){
        if(partType[nodeId].find(partId) == partType[nodeId].end())
            return PartType::Invalid;
        return partType[nodeId][partId];
    }
    godot::Ref<HENode> findNodeRef(int nodeId){
        if(nodeRefs.find(nodeId) == nodeRefs.end())
            return {};
        return nodeRefs[nodeId];
    }
    godot::Ref<HEAsset> findAssetRef(int assetId){
        if(assetRefs.find(assetId) == assetRefs.end())
            return {};
        return assetRefs[assetId];
    }
    std::string getString(HAPI_StringHandle sh){
        if(sh == -1) return "";
        int bufLength;
        if(HoudiniApi::GetStringBufLength(get_session(),sh,&bufLength) != HAPI_RESULT_SUCCESS)
            return "";
        std::string buffer(bufLength+1,'\0');
        if(HoudiniApi::GetString(get_session(),sh,buffer.data(),bufLength) != HAPI_RESULT_SUCCESS)
            return "";
        return buffer;
    }
    HAPI_StringHandle addString(std::string value){
        HAPI_StringHandle sh;
        if(HoudiniApi::SetCustomString(get_session(),keep_alive_string(value),&sh) != HAPI_RESULT_SUCCESS){
            return -1;
        }
        return sh;
    }
    bool startSession(SessionType type,bool use_cooking_thread = true){
        if(getHESession()->valid()){
            printFile("Now session is valid.");
            return true;
        }

        _init_hserver();

        HoudiniApi::ClearConnectionError();
        // HAPI_ThriftServerOptions server_options = HoudiniApi::ThriftServerOptions_Create();
        // server_options.autoClose = true;
        // server_options.timeoutMs = 3000.f;

        HAPI_Result SessionResult = HAPI_RESULT_FAILURE;
        int times(0);
        const int timeout(HESettings::get_singleton()->sessionConfig.newSessionTimeoutSec);
        sessionType = type;
        switch (type)
        {
        case SessionType::InProcess:{
            HAPI_SessionInfo sessionInfo = HoudiniApi::SessionInfo_Create();
            SessionResult = HoudiniApi::CreateInProcessSession(
                get_session(),&sessionInfo
            );
            if(SessionResult == HAPI_RESULT_SUCCESS){
                printLog("Successful create a HAPI in-process session");
            }else{
                printError("Error create session: ",SessionResult);
            }
        }break;
        case SessionType::NewNamedPipe:{
            auto houdini = std::filesystem::canonical(HESettings::get_singleton()->houdiniRootPath+"/bin/houdini");
            if(!execute(houdini.string()+" -hess=pipe:"+HESettings::get_singleton()->sessionConfig.namedPipe)){
                printError("Can't open houdini.");
                break;
            }
            using namespace std::chrono_literals;
            HAPI_SessionInfo sessionInfo = HoudiniApi::SessionInfo_Create();
            printLog("Wait for opening houdini");
            while(SessionResult != HAPI_RESULT_SUCCESS && times < timeout){
                ++times;
                SessionResult = HoudiniApi::CreateThriftNamedPipeSession(
                    get_session(),HESettings::get_singleton()->sessionConfig.namedPipe.c_str(),&sessionInfo
                );
                std::this_thread::sleep_for(1s); 
            }
            if(SessionResult == HAPI_RESULT_SUCCESS){
                printLog("Successful create a HAPI named-pipe session");
            }else{
                printError("Error create session: ",SessionResult);
            }
        }break;
        case SessionType::NewTCPSocket:{
            auto houdini = std::filesystem::canonical(HESettings::get_singleton()->houdiniRootPath+"/bin/houdini");
            if(!execute(houdini.string()+" -hess=port:"+std::to_string(HESettings::get_singleton()->sessionConfig.tcpPort))){
                printError("Can't open houdini.");
                break;
            }
            using namespace std::chrono_literals;
            HAPI_SessionInfo sessionInfo = HoudiniApi::SessionInfo_Create();
            printLog("Wait for opening houdini");
            while(SessionResult != HAPI_RESULT_SUCCESS && times < timeout){
                ++times;
                SessionResult = HoudiniApi::CreateThriftSocketSession(
                    get_session(),HESettings::get_singleton()->sessionConfig.hostName.c_str(), HESettings::get_singleton()->sessionConfig.tcpPort, &sessionInfo
                );
                std::this_thread::sleep_for(1s);
            }
            if(SessionResult == HAPI_RESULT_SUCCESS){
                printLog("Successful create a HAPI TCP socket session");
            }else{
                printError("Error create session: ",SessionResult);
            }
        }break;
        case SessionType::ExistingNamedPipe:{
            HAPI_SessionInfo sessionInfo = HoudiniApi::SessionInfo_Create();
            SessionResult = HoudiniApi::CreateThriftNamedPipeSession(
                get_session(),HESettings::get_singleton()->sessionConfig.namedPipe.c_str(),&sessionInfo
            );
            if(SessionResult == HAPI_RESULT_SUCCESS){
                printLog("Successful connect to an existint HAPI named-pipe session");
            }else{
                printError("Error create session: ",SessionResult);
            }
        }break;
        case SessionType::ExistingTCPSocket:{
            HAPI_SessionInfo sessionInfo = HoudiniApi::SessionInfo_Create();
            SessionResult = HoudiniApi::CreateThriftSocketSession(
                get_session(),HESettings::get_singleton()->sessionConfig.hostName.c_str(), HESettings::get_singleton()->sessionConfig.tcpPort, &sessionInfo
            );
            if(SessionResult == HAPI_RESULT_SUCCESS){
                printLog("Successful connect to an existint HAPI TCP socket session");
            }else{
                printError("Error create session: ",SessionResult);
            }
        }break;
        case SessionType::ExistingSharedMemory:{
            HAPI_SessionInfo sessionInfo = HoudiniApi::SessionInfo_Create();
            SessionResult = HoudiniApi::CreateThriftSharedMemorySession(
                get_session(),HESettings::get_singleton()->sessionConfig.sharedMemoryName.c_str(), &sessionInfo
            );
            if(SessionResult == HAPI_RESULT_SUCCESS){
                printLog("Successful connect to an existint HAPI shared memory session");
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
        if(auto a = HoudiniApi::IsInitialized(get_session());a == HAPI_RESULT_NOT_INITIALIZED){
            HAPI_Result Result = HoudiniApi::Initialize(
                get_session(),&HESettings::get_singleton()->cookOptions,use_cooking_thread,-1,"",nullptr,nullptr,nullptr,nullptr
            );
            HESettings::get_singleton()->_update_settings();
            if(Result == HAPI_RESULT_SUCCESS){
                printFile("Successfully initialized Houdini Engine.");
            }else if(Result == HAPI_RESULT_ALREADY_INITIALIZED){
                printFile("Successfully initialized Houdini Engine - HAPI was already initialized.");
            }else{
                printError("Houdini Engine API initialization failed: ",Result);
            }
        }
        emit_signal("SessionStarted");
        getHESession()->active = true;
        getHESession()->type = type;
        _retryCount = 0;
        return true;
    }
    bool cookSession(){
        bool suc = true;
        for(auto a : nodeIds){
            if(!cookNode(a))
                suc = false;
        }
        return suc;
    }
    bool stopSession(){
        if(getHESession()->valid()){
            if(HoudiniApi::Cleanup(get_session()) != HAPI_RESULT_SUCCESS){
                printError("Failed to stop the Houdini Engine session - Clean up failed.");
                return false;
            }
            if(HoudiniApi::CloseSession(get_session()) != HAPI_RESULT_SUCCESS){
                printError("Failed to stop the Houdini Engine session - Close session failed.");
                return false;
            }
        }
        
        cleanup();
        getHESession()->active = false;
        getHESession()->type = SessionType::None;
        return true;
    }
    bool restartSession(){
        auto type = getHESession()->type;
        if(type == SessionType::None)
            type = SessionType::InProcess;
        stopSession();
        return startSession(type);
    }
    std::vector<int> loadHIP(std::string path, bool append = false, bool cook = true){
        std::vector<int> newNodeIds;
        if(append){
            HAPI_HIPFileId fileId;
            if(HoudiniApi::MergeHIPFile(get_session(),path.c_str(),cook,&fileId) != HAPI_RESULT_SUCCESS){
                printError("Failed to load hip file");
                return {};
            }
            int count = 0;
            if(HoudiniApi::GetHIPFileNodeCount(get_session(),fileId,&count) != HAPI_RESULT_SUCCESS){
                printError("Can't get append node'id count");
                return {};
            }
            std::vector<int> nodeIds(count);
            if(HoudiniApi::GetHIPFileNodeIds(get_session(),fileId,nodeIds.data(),count) !=HAPI_RESULT_SUCCESS){
                printError("Can't get append node'ids");
                return {};
            }
            newNodeIds = std::move(nodeIds);
        }else{
            restartSession();
            if(HoudiniApi::LoadHIPFile(get_session(),path.c_str(),cook) != HAPI_RESULT_SUCCESS){
                printError("Failed to load hip file\n\t"
                    "WIP! Last session is clear because append mode is false");
                return {};
            }
            newNodeIds.push_back(0);
            newNodeIds = getChildNodes(0, {HAPI_NODETYPE_ANY}, {HAPI_NODEFLAGS_ANY}, true);
        }
        for(auto a : newNodeIds){
            if(findNodeRef(a).is_null()){
                _registe_node(a);
            }
        }
        if(cook){
            for(auto id : newNodeIds){
                std::jthread td([this,id]{
                    cookThread(id);
                });
                if(HESettings::get_singleton()->useCookingThread)
                    td.detach();
                else
                    td.join();
            }
        }
        return newNodeIds;
    }
    bool saveHIP(std::string path, bool lock = true){
        if(!ensureSession()){
            printError("Error load Asset with invalid session");
            return false;
        }
        if(HoudiniApi::SaveHIPFile(get_session(),path.c_str(),lock) != HAPI_RESULT_SUCCESS){
            printError("Failed to save session to hip file");
            return false;
        }
        return true;
    }
    bool loadAsset(std::string path, int& assetId){
        if(!ensureSession()){
            printError("Error load Asset with invalid session");
            return false;
        }
        assetId = -1;
        try{
        if(auto a = HoudiniApi::LoadAssetLibraryFromFile(get_session(),path.c_str(),true,&assetId);a != HAPI_RESULT_SUCCESS){
            printError("Error load Asset from file: ", a);
            return false;
        }
        }catch(std::exception& e){
            printError("Failed to load asset, do you have license?");
            printError(e.what());
            return false;
        }catch(...){
            printError("Failed to load asset, do you have license?");
            return false;
        }
        int asset_count = 0;
        if(auto a = HoudiniApi::GetAvailableAssetCount(get_session(),assetId,&asset_count); a != HAPI_RESULT_SUCCESS){
            printError("Error get available asset count: ",a);
            return false;
        }
        std::vector<HAPI_StringHandle> assetSH;
        assetSH.resize(asset_count);
        if(auto a = HoudiniApi::GetAvailableAssets(get_session(),assetId,assetSH.data(),asset_count);a != HAPI_RESULT_SUCCESS){
            printError("Error get available assets: ",a);
            return false;
        }
        std::string temp;
        godot::Ref<HEAsset> assetLib;
        assetLib.instantiate();
        for(int i = 0;i!=asset_count;++i){
            temp = HoudiniEngineUtility::getString(get_session(),assetSH[i]);
            assetLib->ownedNodeOperators.push_back(string_cast(temp));
        }
        assetRefs.insert({assetId,assetLib});
        return true;
    }
    godot::Ref<HENode> makeTypeNode(HAPI_NodeType type){
        auto genRes = [] <typename T> () -> godot::Ref<HENode> requires std::derived_from<T,HENode>||std::same_as<T,HENode> {
            godot::Ref<T> temp;
            temp.instantiate();
            return temp;
        };
        switch(type){
            case HAPI_NODETYPE_OBJ:
                return genRes.operator()<HEObjNode>();
            case HAPI_NODETYPE_SOP:
                return genRes.operator()<HESopNode>();
            case HAPI_NODETYPE_CHOP:
                return genRes.operator()<HEChopNode>();
            case HAPI_NODETYPE_ROP:
                return genRes.operator()<HERopNode>();
            case HAPI_NODETYPE_SHOP:
                return genRes.operator()<HEShopNode>();
            case HAPI_NODETYPE_COP:
                return genRes.operator()<HECopNode>();
            case HAPI_NODETYPE_VOP:
                return genRes.operator()<HEVopNode>();
            case HAPI_NODETYPE_DOP:
                return genRes.operator()<HEDopNode>();
            case HAPI_NODETYPE_TOP:
                return genRes.operator()<HETopNode>();
            default:
                return genRes.operator()<HENode>();
        }
    }
    void _registe_node(int id){
        auto info = getNodeInfo(id);
        nodeIds.insert(id);
        godot::Ref<HENode> nodeRef = makeTypeNode(info.type);
        nodeRef->id = id;
        nodeRefs.insert({id,nodeRef});
    }
    bool createNode(std::string nodeLabel, std::string operatorName, int& id, int parentId){
        if(!ensureSession()){
            printError("Failed to create node: The session is invalid.");
            return false;
        }
        if(HoudiniApi::CreateNode(get_session(),parentId,operatorName.c_str(),nodeLabel.c_str(),false,&id) != HAPI_RESULT_SUCCESS){
            printError("Error create node: ",HoudiniEngineUtility::getLastError().c_str());
            return false;
        }else{
            printFile("Success create node, ID: ",id);
        }
        std::list<int> queue;
        queue.push_back(id);
        while(!queue.empty()){
            int _id = queue.front();
            queue.pop_front();
            auto info = getNodeInfo(_id);
            if(info.childNodeCount > 0)
                for(int id2 : getChildNodes(_id,{HAPI_NODETYPE_ANY},{HAPI_NODEFLAGS_ANY})){
                    queue.push_back(id2);
                }
            _registe_node(_id);
        }
        return true;
    }
    bool connectNode(int resNode, int inputIndex,int srcNode,int outputIndex){
        if(!ensureSession()){
            printError("Failed to connect node: The session is invalid.");
            return false;
        }
        if(HoudiniApi::ConnectNodeInput(get_session(),resNode,inputIndex,srcNode,outputIndex) != HAPI_RESULT_SUCCESS){
            printError("Error connect node: ",HoudiniEngineUtility::getLastError().c_str());
            return false;
        }else{
            printFile("Success connect node ",resNode," with ",srcNode);
        }

        if(HESettings::get_singleton()->autoCook)
            cookNode(resNode);
        return true;
    }
    bool disconnectNode(int nodeId, int inputIndex){
        if(!ensureSession()){
            printError("Failed to disconnect node: The session is invalid.");
            return false;
        }
        if(HoudiniApi::DisconnectNodeInput(get_session(),nodeId,inputIndex) != HAPI_RESULT_SUCCESS){
            printError("Error disconnect node: ",HoudiniEngineUtility::getLastError().c_str());
            return false;
        }else{
            printFile("Success dicconnect node ",nodeId,"'s port ",inputIndex);
        }

        if(HESettings::get_singleton()->autoCook)
            cookNode(nodeId);
        return true;
    }
    // Return input node id
    int queryConnectedNode(int nodeId, int inputIndex){
        if(!ensureSession()){
            printError("Failed to query node input: The session is invalid.");
            return -1;
        }
        int id2;
        if(HoudiniApi::QueryNodeInput(get_session(),nodeId,inputIndex,&id2) != HAPI_RESULT_SUCCESS){
            printError("Error query node input: ",HoudiniEngineUtility::getLastError().c_str());
            return -1;
        }else{
            printFile("Success query node ",nodeId,"'s port ",inputIndex);
        }
        return id2;
    }
    bool cookNode(int id,std::function<void(bool)> cookedCallBack = [](bool){}){
        if(!ensureSession()){
            printError("Failed to cook node: The session is invalid.");
            Contact::add_call([cookedCallBack]{
                cookedCallBack(false);
            });
            return false;
        }
        if(id == -1){
            Contact::add_call([cookedCallBack]{
                cookedCallBack(false);
            });
            return false;
        }
        if(nodeIds.find(id) == nodeIds.end()){
            Contact::add_call([cookedCallBack]{
                cookedCallBack(false);
            });
            return false;
        }
        if(auto a = HoudiniApi::CookNode(get_session(),id,&HESettings::get_singleton()->cookOptions);a != HAPI_RESULT_SUCCESS){
            printError("Failed to cook node: ",HoudiniEngineUtility::getLastCookError().c_str());
            if(a == HAPI_RESULT_NODE_INVALID){
                printError("Failed to cook node: The node is invalid.");
                cookStatus[id] = 4;
                _delete_data(id);
            }
            Contact::add_call([cookedCallBack]{
                cookedCallBack(false);
            });
            return false;
        }
        std::jthread td([this,id,cookedCallBack]{
            cookThread(id,cookedCallBack);
        });
        if(HESettings::get_singleton()->useCookingThread)
            td.detach();
        else
            td.join();
        return true;
    }
    void cookThread(int id, std::function<void(bool)> cookedCallBack = [](bool){}){
        cookStatus[id] = 1;
        int status;
        HAPI_Result result;
        do{
            result = HoudiniApi::GetStatus(get_session(),HAPI_STATUS_COOK_STATE,&status);
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }while(status > HAPI_STATE_MAX_READY_STATE && result == HAPI_RESULT_SUCCESS);
        if(status != HAPI_STATE_READY || result != HAPI_RESULT_SUCCESS){
            printError("Cook failed: ",HoudiniEngineUtility::getLastCookError().c_str());
            if(result == HAPI_RESULT_NODE_INVALID){
                cookStatus[id] = 4;
                _delete_data(id);
                Contact::add_call([cookedCallBack]{ cookedCallBack(false); });
                return;
            }
            cookStatus[id] = 3;
            Contact::add_call([cookedCallBack]{ cookedCallBack(false); });
            return;
        }
        cookCounts[id]++;
        HAPI_NodeInfo info = getNodeInfo(id);
        switch(info.type){
            case HAPI_NODETYPE_SOP:
                updateSopData(id);
                break;
            case HAPI_NODETYPE_COP:
                updateCopData(id);
                break;
            default:
                getParameters(id);
                break;
        }
        cookStatus[id] = 2;
        Contact::add_call([cookedCallBack]{
            cookedCallBack(true);
        });
    }
    bool deleteNode(int id){
        if(!ensureSession()){
            printError("Failed to delete node: The session is invalid.");
            return false;
        }
        std::list<int> queue,temp_queue;
        temp_queue.push_back(id);
        while(!temp_queue.empty()){
            int _id = temp_queue.front();
            temp_queue.pop_front();
            auto info = getNodeInfo(_id);
            if(info.childNodeCount > 0)
                for(int id2 : getChildNodes(_id,{HAPI_NODETYPE_ANY},{HAPI_NODEFLAGS_ANY})){
                    temp_queue.push_back(id2);
                }
            queue.push_back(_id);
        }
        if(auto a = HoudiniApi::DeleteNode(get_session(),id);a != HAPI_RESULT_SUCCESS){
            if(a == HAPI_RESULT_NODE_INVALID){
                if(nodeIds.find(id) != nodeIds.end()){
                    _delete_data(id);
                    return true;
                }else{
                    printWarning("No such node to delete.");
                    return true;
                }
            }
            printError("Failed to delete node: ",a," - ",HoudiniEngineUtility::getLastError().c_str());
            return false;
        }
        for(int _id : queue)
            _delete_data(_id);
        return true;
    }
    std::vector<int> getChildNodes(int nodeId, std::vector<HAPI_NodeType> typeFilter, std::vector<HAPI_NodeFlags> flagFilter,bool recursive = false){
        if(!ensureSession()){
            printError("Failed to rename node: The session is invalid.");
            return {};
        }
        HAPI_NodeTypeBits types = 0;
        for(auto a : typeFilter){
            types |= a;
        }
        HAPI_NodeFlagsBits flags = 0;
        for(auto a : flagFilter){
            flags |= a;
        }
        int count = 0;
        if(auto a = HoudiniApi::ComposeChildNodeList(get_session(),nodeId,types,flags,recursive,&count);a != HAPI_RESULT_SUCCESS){
            if(a == HAPI_RESULT_INVALID_ARGUMENT)
                return {};
            printError("Failed to get child nodes: ",HoudiniEngineUtility::getLastError().c_str());
            if(a == HAPI_RESULT_NODE_INVALID){
                _delete_data(nodeId);
            }
            return {};
        }
        std::vector<int> res;
        res.resize(count);
        if(auto a = HoudiniApi::GetComposedChildNodeList(get_session(),nodeId,res.data(),count);a != HAPI_RESULT_SUCCESS){
            if(a == HAPI_RESULT_INVALID_ARGUMENT)
                return {};
            printError("Failed to get child nodes: ",HoudiniEngineUtility::getLastCookError().c_str());
            if(a == HAPI_RESULT_NODE_INVALID){
                _delete_data(nodeId);
            }
            return {};
        }

        return res;
    }
    bool renameNode(int id, std::string name){
        if(!ensureSession()){
            printError("Failed to rename node: The session is invalid.");
            return false;
        }
        if(id == -1)
            return false;
        if(nodeIds.find(id) == nodeIds.end())
            return false;
        if(!getNodeInfo(id).createdPostAssetLoad)
            return false;
        if(auto a = HoudiniApi::RenameNode(get_session(),id,name.c_str());a != HAPI_RESULT_SUCCESS){
            printError("Failed to rename node: ",HoudiniEngineUtility::getLastCookError().c_str());
            if(a == HAPI_RESULT_NODE_INVALID){
                _delete_data(id);
            }
            return false;
        }
        return true;
    }
    void getParameters(int id){
        if(!ensureSession()){
            printError("Failed to get parameters: The session is invalid.");
            return;
        }
        HAPI_NodeInfo info = getNodeInfo(id);
        std::vector<HAPI_ParmInfo> parm_infos;
        parm_infos.resize(info.parmCount);
        if(auto a = HoudiniApi::GetParameters(get_session(),id,parm_infos.data(),0,info.parmCount);a != HAPI_RESULT_SUCCESS){
            printFile(HoudiniEngineUtility::getLastError().c_str(), " No parm got.");
            if(a == HAPI_RESULT_NODE_INVALID){
                _delete_data(id);
            }
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
                parmSH_values.resize(parm_string_count);
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
    template<typename T>
    bool getAttributeData(int nodeId, int partId, std::string name, HAPI_AttributeOwner owner, std::vector<T>& data){
        HAPI_AttributeInfo info;
        if(auto a = HoudiniApi::GetAttributeInfo(get_session(),nodeId,partId,name.c_str(),owner,&info);a != HAPI_RESULT_SUCCESS){
            if(a == HAPI_RESULT_NODE_INVALID){
                _delete_data(nodeId);
            }
            return false;
        }
        if(!info.exists) return false;
        std::size_t size = info.count * info.tupleSize;
        data.resize(size);
        if constexpr (std::is_same_v<T,int>){
            if(HoudiniApi::GetAttributeIntData(get_session(),nodeId,partId,name.c_str(),&info,-1,data.data(),0,info.count) != HAPI_RESULT_SUCCESS){
                return false;
            }
        }else if constexpr (std::is_same_v<T,float>){
            if(HoudiniApi::GetAttributeFloatData(get_session(),nodeId,partId,name.c_str(),&info,-1,data.data(),0,info.count) != HAPI_RESULT_SUCCESS){
                return false;
            }
        }else if constexpr (std::is_same_v<T, std::string>){
            if(HoudiniApi::GetAttributeStringData(get_session(),nodeId,partId,name.c_str(),&info, data.data(),0,info.count) != HAPI_RESULT_SUCCESS){
                return false;
            }
        }else{
            return false;
        }
        return true;
    };

    void updateSopData(int nodeId){
        HAPI_GeoInfo info = getGeoInfo(nodeId);
        getParameters(nodeId);
        if(!info.isDisplayGeo)
            return;
        getGeometry(nodeId);
        getMaterial(nodeId);
        for(auto& a : partType[nodeId]){
            switch(a.second){
                case PartType::Mesh:
                    createMeshRes(nodeId,a.first);
                    break;
                case PartType::Instancer:
                    createMultiMeshRes(nodeId,a.first);
                    break;
                case PartType::Curve:
                    createCurveRes(nodeId,a.first);
                    break;
                default:
                    printWarning("Unsupported part type: ",(int)a.second);
                    break;
            }
        }
    }
    void updateCopData(int nodeId){
        getParameters(nodeId);
        getImageData(nodeId);
        createImageRes(nodeId);
    }
    //Only for sop node
    bool checkGeometryChange(int nodeId){
        HAPI_NodeInfo nodeInfo = getNodeInfo(nodeId);
        if(nodeInfo.type != HAPI_NODETYPE_SOP)
            return false;
        HAPI_GeoInfo geoInfo = getGeoInfo(nodeId);
        return geoInfo.hasGeoChanged;
    }
    void getGeometry(int id){

        HAPI_GeoInfo mesh_geo_info;
        if(HoudiniApi::GetDisplayGeoInfo(get_session(), id, &mesh_geo_info) != HAPI_RESULT_SUCCESS){
            printError(HoudiniEngineUtility::getLastError().c_str());
            return;
        }
        for(int partId = 0;partId!=mesh_geo_info.partCount;++partId){
            HAPI_PartInfo partInfo;
            if(HoudiniApi::GetPartInfo(get_session(), mesh_geo_info.nodeId, partId, &partInfo) != HAPI_RESULT_SUCCESS){
                printError(HoudiniEngineUtility::getLastError().c_str());
                return;
            }
            if(!partInfo.hasChanged)
                continue;
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
                std::vector<float> mesh_p_attrib_info;
                getAttributeData(id, partId, HAPI_ATTRIB_POSITION, HAPI_ATTROWNER_POINT, mesh_p_attrib_info);
                std::cout << "get: mesh_p_attrib_info.size()" << mesh_p_attrib_info.size() << std::endl;

                std::vector<float> mesh_cd_attrib_data;
                AttribOwner mesh_cd_attrib_owner = AttribOwner::Vertex;
                if(!getAttributeData(id, partId, HAPI_ATTRIB_COLOR, HAPI_ATTROWNER_VERTEX, mesh_cd_attrib_data)){
                    getAttributeData(id, partId, HAPI_ATTRIB_COLOR, HAPI_ATTROWNER_POINT, mesh_cd_attrib_data);
                    mesh_cd_attrib_owner = AttribOwner::Point;
                }
                std::cout << "get: mesh_cd_attrib_data.size()" << mesh_cd_attrib_data.size() << std::endl;

                std::vector<float> mesh_N_attrib_data;
                AttribOwner mesh_N_attrib_owner = AttribOwner::Vertex;
                if(!getAttributeData(id, partId, HAPI_ATTRIB_NORMAL, HAPI_ATTROWNER_VERTEX, mesh_N_attrib_data)){
                    getAttributeData(id, partId, HAPI_ATTRIB_NORMAL, HAPI_ATTROWNER_POINT, mesh_N_attrib_data);
                    mesh_N_attrib_owner = AttribOwner::Point;
                }
                std::cout << "get: mesh_N_attrib_data.size()" << mesh_N_attrib_data.size() << std::endl;
                
                std::vector<float> mesh_uv_attrib_data;
                AttribOwner mesh_uv_attrib_owner = AttribOwner::Vertex;
                if(!getAttributeData(id, partId, HAPI_ATTRIB_UV, HAPI_ATTROWNER_VERTEX, mesh_uv_attrib_data)){
                    getAttributeData(id, partId, HAPI_ATTRIB_UV, HAPI_ATTROWNER_POINT, mesh_uv_attrib_data);
                    mesh_uv_attrib_owner = AttribOwner::Point;
                }
                std::cout << "get: mesh_uv_attrib_data.size()" << mesh_uv_attrib_data.size() << std::endl;
                
                std::vector<float> mesh_uv2_attrib_data;
                AttribOwner mesh_uv2_attrib_owner = AttribOwner::Vertex;
                if(!getAttributeData(id, partId, HAPI_ATTRIB_UV2, HAPI_ATTROWNER_VERTEX, mesh_uv2_attrib_data)){
                    getAttributeData(id, partId, HAPI_ATTRIB_UV2, HAPI_ATTROWNER_POINT, mesh_uv2_attrib_data);
                    mesh_uv2_attrib_owner = AttribOwner::Point;
                }
                std::cout << "get: mesh_uv2_attrib_data.size()" << mesh_uv2_attrib_data.size() << std::endl;
                
                meshGeometries[id][partId] = {std::move(mesh_face_counts),std::move(mesh_p_attrib_info),std::move(mesh_vertex_list),{mesh_cd_attrib_owner,std::move(mesh_cd_attrib_data)},{mesh_N_attrib_owner,std::move(mesh_N_attrib_data)},{mesh_uv_attrib_owner,std::move(mesh_uv_attrib_data)},{mesh_uv2_attrib_owner,std::move(mesh_uv2_attrib_data)}};
            }break;
            case PartType::Instancer:{
                std::vector<HAPI_Transform> instancer_transforms(partInfo.instanceCount);
                auto result = HoudiniApi::GetInstancerPartTransforms(get_session(),id,partId,HAPI_SRT,instancer_transforms.data(),0,partInfo.instanceCount);
                if(result != HAPI_RESULT_SUCCESS){
                    printFile(HoudiniEngineUtility::getLastError().c_str());
                    continue;
                }
                std::vector<int> instancePartIds(partInfo.instanceCount);
                result = HoudiniApi::GetInstancedPartIds(get_session(),id,partId,instancePartIds.data(),0,partInfo.instanceCount);
                if(result != HAPI_RESULT_SUCCESS){
                    printFile(HoudiniEngineUtility::getLastError().c_str());
                    continue;
                }
                packedPrimData[id][partId] = {std::move(instancePartIds),std::move(instancer_transforms)};
            }break;
            case PartType::Curve:{
                HAPI_CurveInfo info;
                if(HoudiniApi::GetCurveInfo(get_session(),id,partId,&info) != HAPI_RESULT_SUCCESS){
                    printError(HoudiniEngineUtility::getLastError().c_str());
                    return;
                }
                const char* attr_name = info.isRational ? "Pw" : HAPI_ATTRIB_POSITION;
                [[maybe_unused]]int components = info.isRational ? 4 : 3;
                std::vector<float> rawControlPoints;
                //Using for debug
                HAPI_AttributeInfo attr_info;
                HoudiniApi::GetAttributeInfo(get_session(),id,partId,attr_name,HAPI_ATTROWNER_POINT,&attr_info);
                assert(attr_info.count == info.vertexCount);
                assert(attr_info.tupleSize == components);

                getAttributeData(id,partId,attr_name,HAPI_ATTROWNER_POINT,rawControlPoints);
                std::vector<float> controlPoints;
                std::vector<float> weights;
                if(info.isRational){
                    controlPoints.resize(info.vertexCount*3);
                    weights.resize(info.vertexCount);
                    for(int i = 0;i!=info.vertexCount;++i){
                        controlPoints[i*3] = rawControlPoints[i*4];
                        controlPoints[i*3+1] = rawControlPoints[i*4+1];
                        controlPoints[i*3+2] = rawControlPoints[i*4+2];
                        weights[i] = rawControlPoints[i*4+3];
                    }
                }else{
                    controlPoints = std::move(rawControlPoints);
                    weights = std::vector<float>(info.vertexCount,1.f);
                }
                std::vector<int> curveCounts(info.curveCount);
                if(HoudiniApi::GetCurveCounts(get_session(),id,partId,curveCounts.data(),0,info.curveCount)!=HAPI_RESULT_SUCCESS){
                    printError(HoudiniEngineUtility::getLastError().c_str());
                    return;
                }
                std::vector<float> knots;
                if(info.hasKnots){
                    knots = std::vector<float>(info.knotCount);
                    HoudiniApi::GetCurveKnots(get_session(),id,partId,knots.data(),0,info.knotCount);
                }
                curveGeometries[id][partId] = {info,std::move(curveCounts),std::move(controlPoints),std::move(weights),std::move(knots)};
            }break;
            case PartType::Box:{
                HAPI_BoxInfo info;
                if(HoudiniApi::GetBoxInfo(get_session(),id,partId,&info) != HAPI_RESULT_SUCCESS){
                    printError(HoudiniEngineUtility::getLastError().c_str());
                    return;
                }
                //TODO: Other part type
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
            else
                for(auto& b : a.second.second){
                    HAPI_MaterialInfo materialInfo;
                    if(HoudiniApi::GetMaterialInfo(get_session(),b,&materialInfo)!=HAPI_RESULT_SUCCESS){
                        printError(HoudiniEngineUtility::getLastError().c_str());
                    }else{
                        if(materialInfo.hasChanged)
                            return true;
                    }
                }
        }
        return false;
    }
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
    bool isInstanceNode(int nodeId){
        return packedPrimData.find(nodeId) != packedPrimData.end();
    }
    godot::Ref<godot::Mesh> createMeshRes(int nodeId, int partId){
        if(meshGeometries.find(nodeId) == meshGeometries.end()){
            return {};
        }
        auto& inner = meshGeometries[nodeId];
        if(inner.find(partId) == inner.end()){
            return {};
        }
        auto& part = inner[partId];
        [[maybe_unused]]auto& faces = part.faceCounts;
        auto& positions = part.positions;
        auto& vertexs = part.vertexList;
        auto& color_Attrib = part.colors;
        auto& normal_Attrib = part.normals;
        auto& uv_Attrib = part.uvs;
        auto& uv2_Attrib = part.uv2s;
        std::vector<float>& colors = color_Attrib.second;
        std::vector<float>& normals = normal_Attrib.second;
        std::vector<float>& uvs = uv_Attrib.second;
        std::vector<float>& uv2s = uv2_Attrib.second;
        std::vector<std::string> voidMaterialPaths;
        std::vector<std::string>* materialPaths = &voidMaterialPaths;
        if(materials.find(nodeId)!=materials.end()&&materials[nodeId].find(partId)!=materials[nodeId].end()){
            materialPaths = &materials[nodeId][partId];
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
        int mat_num = materialPaths->size();
        std::string lastMatPath;
        if(!materialPaths->empty())
            lastMatPath = (*materialPaths)[0];
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
                std::string newPath = (*materialPaths)[i/3];
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
        auto& ref = meshRefs[nodeId][partId];
        if(ref.is_null())
            ref.instantiate();
        ref->clear_surfaces();
        
        for(auto i = 0,count = arr_mesh->get_surface_count();i!=count;++i){
            ref->add_surface_from_arrays(godot::Mesh::PRIMITIVE_TRIANGLES,arr_mesh->surface_get_arrays(i));
            ref->surface_set_material(i,arr_mesh->surface_get_material(i));
        }
        return ref;
    }
    godot::Ref<godot::Mesh> getMeshRef(int nodeId, int partId){
        return meshRefs[nodeId][partId];
    }
    std::unordered_map<int,godot::Ref<godot::MultiMesh>> createMultiMeshRes(int nodeId, int partId){
        if(packedPrimData.find(nodeId) == packedPrimData.end()){
            return {};
        }
        auto& inner = packedPrimData[nodeId];
        if(inner.find(partId) == inner.end()){
            return {};
        }
        auto& instancedParts = inner[partId];
        auto& instancePartIds = instancedParts.instancedPartIds;
        auto& instancePartTrans = instancedParts.transforms;

        std::vector<godot::Transform3D> transforms;
        godot::Node3D* transformer = memnew(godot::Node3D());
        for(auto& b : instancePartTrans){
            transformer->set_position(godot::Vector3(b.position[0],b.position[1],b.position[2]));
            transformer->set_rotation(godot::Vector3(b.rotationQuaternion[0],b.rotationQuaternion[1],b.rotationQuaternion[2]));
            transformer->set_scale(godot::Vector3(b.scale[0],b.scale[1],b.scale[2]));
            transforms.push_back(transformer->get_transform());
        }
        godot::memdelete(transformer);
        packedPrimMesh[nodeId][partId].clear();
        for(int i = 0,size = instancePartIds.size();i!=size;++i){
            auto& multiMesh = packedPrimMesh[nodeId][partId][instancePartIds[i]];
            switch(partType[nodeId][instancePartIds[i]]){
                case PartType::Mesh:{
                    if(multiMesh.is_null()){
                        multiMesh.instantiate();
                        multiMesh->set_use_colors(true);
                        multiMesh->set_transform_format(godot::MultiMesh::TRANSFORM_3D);
                        multiMesh->set_mesh(meshRefs[nodeId][instancePartIds[i]]);
                    }
                    int count = multiMesh->get_instance_count();
                    multiMesh->set_instance_count(count+1);
                    multiMesh->set_instance_transform(count,transforms[i]);
                }break;
                // case PartType::Instancer:{
                //     if(multiMesh.is_null()){
                //         multiMesh.instantiate();
                //         multiMesh->set_use_colors(true);
                //         multiMesh->set_transform_format(godot::MultiMesh::TRANSFORM_3D);
                //         //multiMesh->set_mesh(meshRefs[nodeId][instancePartIds[i]]);

                //     }
                //     //TODO:
                // }break;
                default:
                    printError("Unsupported type of part.");
                break;
            }
        }
        return packedPrimMesh[nodeId][partId];
    }
    std::unordered_map<int,godot::Ref<godot::MultiMesh>> getMultiMeshRef(int nodeId, int partId){
        return packedPrimMesh[nodeId][partId];
    }
    std::vector<godot::Ref<godot::Curve3D>> createCurveRes(int nodeId, int partId){
        if (curveGeometries.find(nodeId) == curveGeometries.end() || curveGeometries.at(nodeId).find(partId) == curveGeometries.at(nodeId).end()){
            return {};
        }

        auto& curveData = curveGeometries.at(nodeId).at(partId);
        auto& info = curveData.info;
        auto& curveCounts = curveData.curveCounts;
        auto& controlPoints = curveData.controlPoints;

        auto& vec = curveRefs[nodeId][partId];
        vec.clear();
        vec.resize(info.curveCount);
        int point_offset = 0;
        for (int i = 0; i < info.curveCount; ++i){
            auto& ref = vec[i];
            if (ref.is_null())
                ref.instantiate();
            
            ref->clear_points();
            int num_points_in_curve = curveCounts[i];
            switch (info.curveType)
            {
                case HAPI_CURVETYPE_LINEAR:
                case HAPI_CURVETYPE_NURBS:
                {
                    for (int j = 0; j < num_points_in_curve; ++j)
                    {
                        int current_point_idx = point_offset + j;
                        godot::Vector3 pos(
                            controlPoints[current_point_idx * 3],
                            controlPoints[current_point_idx * 3 + 1],
                            controlPoints[current_point_idx * 3 + 2]
                        );
                        ref->add_point(pos, godot::Vector3(), godot::Vector3());
                    }
                    break;
                }
                case HAPI_CURVETYPE_BEZIER:
                {
                    if (num_points_in_curve < 4 || (num_points_in_curve - 1) % 3 != 0)
                    {
                        printWarning("Unsupported Bezier curve vertex count. Treating as linear.");
                        for (int j = 0; j < num_points_in_curve; ++j)
                        {
                            int current_point_idx = point_offset + j;
                            godot::Vector3 pos(controlPoints[current_point_idx * 3], controlPoints[current_point_idx * 3 + 1], controlPoints[current_point_idx * 3 + 2]);
                            ref->add_point(pos);
                        }
                        break;
                    }
                    godot::Vector3 p1(controlPoints[point_offset * 3], controlPoints[point_offset * 3 + 1], controlPoints[point_offset * 3 + 2]);
                    godot::Vector3 c1(controlPoints[(point_offset + 1) * 3], controlPoints[(point_offset + 1) * 3 + 1], controlPoints[(point_offset + 1) * 3 + 2]);
                    ref->add_point(p1, godot::Vector3(), c1 - p1);
                    for (int j = 1; j < num_points_in_curve; j += 3)
                    {
                        int base_idx = point_offset + j;
                        godot::Vector3 c2(controlPoints[base_idx * 3], controlPoints[base_idx * 3 + 1], controlPoints[base_idx * 3 + 2]);
                        godot::Vector3 p2(controlPoints[(base_idx + 1) * 3], controlPoints[(base_idx + 1) * 3 + 1], controlPoints[(base_idx + 1) * 3 + 2]);
                        godot::Vector3 c3 = (base_idx + 2 < point_offset + num_points_in_curve) ?
                            godot::Vector3(controlPoints[(base_idx + 2) * 3], controlPoints[(base_idx + 2) * 3 + 1], controlPoints[(base_idx + 2) * 3 + 2]) : p2;

                        ref->add_point(p2, c2 - p2, c3 - p2);
                    }
                    break;
                }
                default:
                    printError("Unsupported curve type.");
                    break;
            }
            point_offset += num_points_in_curve;
        }
        
        return vec;
    }
    std::vector<godot::Ref<godot::Curve3D>> getCurveRef(int nodeId, int partId){
        return curveRefs[nodeId][partId];
    }
    void getImageData(int nodeId){
        if (HoudiniApi::RenderCOPToImage(get_session(), nodeId) != HAPI_RESULT_SUCCESS){
            printError("Failed to render COP node: ", HoudiniEngineUtility::getLastError().c_str());
            return;
        }
        HAPI_ImageInfo imageInfo;
        if (HoudiniApi::GetImageInfo(get_session(), nodeId, &imageInfo) != HAPI_RESULT_SUCCESS){
            printError("Failed to get image info from COP node: ", HoudiniEngineUtility::getLastError().c_str());
            return;
        }
        int bufSize = 0;
        if (HoudiniApi::ExtractImageToMemory(get_session(),nodeId,"png","C A", &bufSize) != HAPI_RESULT_SUCCESS){
            printError("Failed to extract image from COP node: ", HoudiniEngineUtility::getLastError().c_str());
            return;
        }
        godot::PackedByteArray buf;
        buf.resize(bufSize);
        if(HoudiniApi::GetImageMemoryBuffer(get_session(),nodeId, (char*)buf.ptrw(), bufSize) != HAPI_RESULT_SUCCESS){
            printError("Failed to get image data from memory: ", HoudiniEngineUtility::getLastError().c_str());
            return;
        }
        images[nodeId] = {std::move(imageInfo), std::move(buf)};
    }
    godot::Ref<godot::Image> createImageRes(int nodeId){
        if (images.find(nodeId) == images.end()){
            getImageData(nodeId);
            if (images.find(nodeId) == images.end())
                return {};
        }
        auto& buffer = images[nodeId].second;
        if (buffer.is_empty())
            return {};

        godot::Ref<godot::Image> image;
        image.instantiate();

        if (image->load_png_from_buffer(buffer) != godot::Error::OK){
            printError("Failed to load image from buffer for node: ", nodeId);
            return {};
        }
        imageRefs[nodeId] = image;
        return image;
    }
    godot::Ref<godot::Image> getImageRef(int nodeId){
        return imageRefs[nodeId];
    }
    
    bool createInputMeshNode(std::string nodeLabel, int& id, int parentId, godot::Ref<godot::Mesh> mesh){
        if(!ensureSession()){
            printError("Failed to create input node: The session is invalid.");
            return false;
        }
        if(HoudiniApi::CreateInputNode(get_session(),parentId,&id,nodeLabel.c_str()) != HAPI_RESULT_SUCCESS){
            printError("Error create input node: ",HoudiniEngineUtility::getLastError().c_str());
            return false;
        }else{
            printFile("Success create input node, ID: ",id);
        }
        if(initInputMeshNode(id,mesh) != -1){
            _registe_node(id);
            if(HESettings::get_singleton()->autoCook)
                cookNode(id);
        }else{
            deleteNode(id);
            printError("Failed to init input node.");
        }
        return true;
    }
    int initInputMeshNode(int id,godot::Ref<godot::Mesh> mesh){
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
                        if((size_t)a < rawColorSize)
                            collectColors.emplace_back(rawColors[a]);
                        else 
                            collectColors.emplace_back(godot::Color());
                    }
                }
                size_t rawNormalSize = rawNormals.size();
                if(rawNormalSize != 0){
                    collectNormals.reserve(vertexSize);
                    for(auto a : vertexs){
                        if((size_t)a < rawNormalSize)
                            collectNormals.emplace_back(rawNormals[a]);
                        else 
                            collectNormals.emplace_back(godot::Vector3());
                    }
                }
                size_t rawUVSize = rawUVs.size();
                if(rawUVSize != 0){
                    collectUVs.reserve(vertexSize);
                    for(auto a : vertexs){
                        if((size_t)a < rawUVSize)
                            collectUVs.emplace_back(rawUVs[a]);
                        else 
                            collectUVs.emplace_back(godot::Vector2());
                    }
                }
                size_t rawUV2Size = rawUV2s.size();
                if(rawUV2Size != 0){
                    collectUV2s.reserve(vertexSize);
                    for(auto a : vertexs){
                        if((size_t)a < rawUV2Size)
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
        if(HoudiniApi::AddAttribute(get_session(),id,0,HAPI_ATTRIB_POSITION,&pointInfo) != HAPI_RESULT_SUCCESS){
            printError(HoudiniEngineUtility::getLastError().c_str());
            return -1;
        }
        if(HoudiniApi::SetAttributeFloatData(get_session(),id,0,HAPI_ATTRIB_POSITION,&pointInfo,allPositions.data(),0,pointInfo.count) != HAPI_RESULT_SUCCESS){
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
            if(HoudiniApi::AddAttribute(get_session(),id,0,HAPI_ATTRIB_COLOR,&colorInfo) != HAPI_RESULT_SUCCESS){
                printError(HoudiniEngineUtility::getLastError().c_str());
                return -1;
            }
            std::cerr << "count: " << colorInfo.count << "\tsize: " << allColors.size()/3 << std::endl;
            if(HoudiniApi::SetAttributeFloatData(get_session(),id,0,HAPI_ATTRIB_COLOR,&colorInfo,allColors.data(),0,colorInfo.count) != HAPI_RESULT_SUCCESS){
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
            if(HoudiniApi::AddAttribute(get_session(),id,0,HAPI_ATTRIB_NORMAL,&normalInfo) != HAPI_RESULT_SUCCESS){
                printError(HoudiniEngineUtility::getLastError().c_str());
                return -1;
            }
            if(HoudiniApi::SetAttributeFloatData(get_session(),id,0,HAPI_ATTRIB_NORMAL,&normalInfo,allNormals.data(),0,normalInfo.count) != HAPI_RESULT_SUCCESS){
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
            if(HoudiniApi::AddAttribute(get_session(),id,0,HAPI_ATTRIB_UV,&uvInfo) != HAPI_RESULT_SUCCESS){
                printError(HoudiniEngineUtility::getLastError().c_str());
                return -1;
            }
            if(HoudiniApi::SetAttributeFloatData(get_session(),id,0,HAPI_ATTRIB_UV,&uvInfo,allUVs.data(),0,uvInfo.count) != HAPI_RESULT_SUCCESS){
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
            if(HoudiniApi::AddAttribute(get_session(),id,0,HAPI_ATTRIB_UV2,&uv2Info) != HAPI_RESULT_SUCCESS){
                printError(HoudiniEngineUtility::getLastError().c_str());
                return -1;
            }
            if(HoudiniApi::SetAttributeFloatData(get_session(),id,0,HAPI_ATTRIB_UV2,&uv2Info,allUV2s.data(),0,uv2Info.count) != HAPI_RESULT_SUCCESS){
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
    bool createInputCurveNode(std::string nodeLabel, int& id, int parentId, godot::Ref<godot::Curve3D> curve){
        if(!ensureSession()){
            printError("Failed to create input node: The session is invalid.");
            return false;
        }
        if (curve.is_null() || curve->get_point_count() < 2){
            printError("Input curve is invalid or has less than 2 points.");
            return -1;
        }
        if(HoudiniApi::CreateInputCurveNode(get_session(),parentId,&id, nodeLabel.c_str()) != HAPI_RESULT_SUCCESS){
            printError("Error create input node: ",HoudiniEngineUtility::getLastError().c_str());
            return false;
        }else{
            printFile("Success create input node, ID: ",id);
        }
        if(initInputCurveNode(id,curve) != -1){
            _registe_node(id);
            if(HESettings::get_singleton()->autoCook)
                cookNode(id);
        }else{
            deleteNode(id);
            printError("Failed to init input node.");
        }
        return true;
    }
    int initInputCurveNode(int nodeId, godot::Ref<godot::Curve3D> curve){
        int point_count = curve->get_point_count();
        int segment_count = point_count - 1;
        int total_vertices = segment_count * 3 + 1;

        std::vector<float> allPositions;
        allPositions.reserve(total_vertices * 3);

        godot::Vector3 pos = curve->get_point_position(0);
        allPositions.push_back(pos.x);
        allPositions.push_back(pos.y);
        allPositions.push_back(pos.z);

        for (int i = 0; i < segment_count; ++i){
            godot::Vector3 out_handle = curve->get_point_position(i) + curve->get_point_out(i);
            allPositions.push_back(out_handle.x);
            allPositions.push_back(out_handle.y);
            allPositions.push_back(out_handle.z);

            godot::Vector3 in_handle = curve->get_point_position(i + 1) + curve->get_point_in(i + 1);
            allPositions.push_back(in_handle.x);
            allPositions.push_back(in_handle.y);
            allPositions.push_back(in_handle.z);

            godot::Vector3 next_pos = curve->get_point_position(i + 1);
            allPositions.push_back(next_pos.x);
            allPositions.push_back(next_pos.y);
            allPositions.push_back(next_pos.z);
        }

        HAPI_PartInfo part_info = HoudiniApi::PartInfo_Create();
        part_info.id = 0;
        part_info.nameSH = 0;
        part_info.type = HAPI_PARTTYPE_CURVE;
        part_info.faceCount = 0;
        part_info.vertexCount = 0;
        part_info.pointCount = total_vertices;
        part_info.hasChanged = true;
        if (HoudiniApi::SetPartInfo(get_session(), nodeId, 0, &part_info) != HAPI_RESULT_SUCCESS){
            printError("Failed to set part info for curve node: ", HoudiniEngineUtility::getLastError().c_str());
            return -1;
        }

        HAPI_CurveInfo curve_info = HoudiniApi::CurveInfo_Create();
        curve_info.curveCount = 1;
        curve_info.vertexCount = total_vertices;
        curve_info.knotCount = 0;
        curve_info.isPeriodic = false;
        curve_info.isRational = false;
        curve_info.order = 4;
        curve_info.hasKnots = false;
        curve_info.curveType = HAPI_CURVETYPE_BEZIER;

        if (HoudiniApi::SetCurveInfo(get_session(), nodeId, 0, &curve_info) != HAPI_RESULT_SUCCESS){
            printError("Failed to set curve info: ", HoudiniEngineUtility::getLastError().c_str());
            return -1;
        }

        int curve_count_val = total_vertices;
        if (HoudiniApi::SetCurveCounts(get_session(), nodeId, 0, &curve_count_val, 0, 1) != HAPI_RESULT_SUCCESS){
            printError("Failed to set curve counts: ", HoudiniEngineUtility::getLastError().c_str());
            return -1;
        }
        HAPI_AttributeInfo attr_info_p = HoudiniApi::AttributeInfo_Create();
        attr_info_p.count = total_vertices;
        attr_info_p.tupleSize = 3;
        attr_info_p.exists = true;
        attr_info_p.storage = HAPI_STORAGETYPE_FLOAT;
        attr_info_p.owner = HAPI_ATTROWNER_POINT;

        if (HoudiniApi::AddAttribute(get_session(), nodeId, 0, HAPI_ATTRIB_POSITION, &attr_info_p) != HAPI_RESULT_SUCCESS){
            printError("Failed to add P attribute: ", HoudiniEngineUtility::getLastError().c_str());
            return -1;
        }

        if (HoudiniApi::SetAttributeFloatData(get_session(), nodeId, 0, HAPI_ATTRIB_POSITION, &attr_info_p, allPositions.data(), 0, attr_info_p.count) != HAPI_RESULT_SUCCESS){
            printError("Failed to set P attribute data: ", HoudiniEngineUtility::getLastError().c_str());
            return -1;
        }

        if (HoudiniApi::CommitGeo(get_session(), nodeId) != HAPI_RESULT_SUCCESS){
            printError("Failed to commit curve geo: ", HoudiniEngineUtility::getLastError().c_str());
            return -1;
        }
        return nodeId;
    }
    bool createInputCopNode(std::string nodeLabel, int& id, int parentId, godot::Ref<godot::Image> img){
        std::string filepath = get_temp_dir()+'/'+generate_random_string(8);
        img->save_png(string_cast(filepath));
        return false;
    }
};

#endif