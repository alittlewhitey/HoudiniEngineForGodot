#ifndef HOUDINI_ENGINE_CERTER
#define HOUDINI_ENGINE_CERTER
#include "DebugSymbol.h"

#include <map>
#include <chrono>
#include <variant>
#include <filesystem>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/classes/multi_mesh.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/box_mesh.hpp>
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
#include "HDAImporter.h"
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
class HECenter: public godot::Node{
    GDCLASS(HECenter, godot::Node)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("freeGDNode","node"),static_cast<bool(HECenter::*)(godot::Node*)>(&HECenter::freeGDNode));
        godot::ClassDB::bind_method(godot::D_METHOD("stopFreeGDNode","node"),static_cast<bool(HECenter::*)(godot::Node*)>(&HECenter::stopFreeGDNode));
    }
    void _notification(int what){
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

        if(sessionOpened){
            if(focused != godot::DisplayServer::get_singleton()->window_is_focused()){
                focused = !focused;
                if(focused)
                    sessionCookSync();
            }

            syncCamera();
        }
    }
    void process(){

    }
    void term(){
        get_tree()->disconnect("node_removed",godot::Callable(this,"freeGDNode"));
        get_tree()->disconnect("node_added",godot::Callable(this,"stopFreeGDNode"));
    }
    void predel(){
        if(sessionOpened){
            stopSession();
        }
        using namespace std::chrono_literals;
        freeTimeout = 0ms;
        std::this_thread::sleep_for(HESettings::defaultFreeTimeout<1s?HESettings::defaultFreeTimeout:1000ms);
    }
//Class

//Configs:

    bool sessionOpened = false;
    bool focused = false;
    std::chrono::milliseconds freeTimeout = HESettings::defaultFreeTimeout;
    SessionType sessionType = SessionType::InProcess;
    //TODO:
    bool syncViewport = 0;

//Datas:

    godot::Ref<HESession> session;

    //      NodeId,AssetId
    std::map<int,int> nodeIds;
    //      NodeId          NodeRef
    std::map<int,godot::Ref<HENode>> nodeRefs;
    //      AssetId         AssetRef
    std::map<int,godot::Ref<HEAsset>> assetRefs;
    //      NodeId          ParamName            ParamValues
    std::map<int,std::map<std::string,std::vector<std::variant<int64_t,double,std::string>>>> parameters;
    //      nodeId      partId  type
    std::map<int,std::map<int,PartType>> partType;
    //      nodeId    meshPartId                    faces            P            vertexs                          Cd                                      N                                           uv                                  uv2
    std::map<int,std::map<int,std::tuple<std::vector<int>,std::vector<float>,std::vector<int>,std::pair<AttribOwner,std::vector<float>>,std::pair<AttribOwner,std::vector<float>>,std::pair<AttribOwner,std::vector<float>>,std::pair<AttribOwner,std::vector<float>>>>> meshGeometries;
    //      nodeId    curvePartId          type  isRational,isPeriodic,order   curveCounts       controlPoints       weights         knots
    std::map<int,std::map<int,std::tuple<HAPI_CurveType,bool,bool,int,std::vector<int>,std::vector<float>,std::vector<float>,std::vector<float>>>> curveGeometries;
    //      nodeId  instancerPartId       instancedPartIds          transform
    std::map<int,std::map<int,std::tuple<std::vector<int>,std::vector<HAPI_Transform>>>> packedPrimData;
    //Must fill from data directly rather than from other mesh.
    //      nodeId  instancerPartId   instancedPartId     multiMesh
    std::map<int,std::map<int,std::map<int,godot::Ref<godot::MultiMesh>>>> packedPrimMesh;
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
    // for `freeGDNode` and `stopFreeGDNode`
    std::set<godot::Node*> createdGDNodes;
    std::map<godot::Node*, std::shared_ptr<std::jthread>> freeGDNodeTasks;

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
    void sessionCookSync(){
        for(auto id : nodeIds){
            nodeCookSync(id.first);
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
            ids.push_back(a.first);
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
        if(godot::Engine::get_singleton()->is_editor_hint()){
            return get_tree()->get_edited_scene_root();
        }else{
            return get_tree()->get_current_scene();
        }
    }
    HAPI_Session* get_session(){
        return session->get_session();
    }
    int getPartCount(int nodeId){
        return partType[nodeId].size();
    }
    PartType getPartType(int nodeId, int partId){
        return partType[nodeId][partId];
    }
    godot::Ref<HENode> findNodeRef(int nodeId){
        return nodeRefs[nodeId];
    }
    godot::Ref<HEAsset> findAssetRef(int assetId){
        return assetRefs[assetId];
    }
    std::string getString(HAPI_StringHandle sh){
        if(sh == -1) return "";
        char buffer[1024];
        if(HoudiniApi::GetString(get_session(),sh,buffer,1024) != HAPI_RESULT_SUCCESS)
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
    bool startSession(SessionType type,bool use_cooking_thread = HESettings::get_singleton()->useCookingThread){
        using namespace _houdini_engine_log;
        if(sessionOpened){
            printFile("Now session is valid.\n");
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
                printLog("Successful create a HAPI in-process session\n");
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
                printLog("Successful create a HAPI named-pipe session\n");
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
                printLog("Successful create a HAPI TCP socket session\n");
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
                printLog("Successful connect to an existint HAPI named-pipe session\n");
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
                printLog("Successful connect to an existint HAPI TCP socket session\n");
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
        if(HoudiniApi::IsInitialized(get_session()) == HAPI_RESULT_NOT_INITIALIZED){

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
        return true;
    }
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
        nodeIds.clear();
        assetRefs.clear();
        parameters.clear();
        partType.clear();
        meshGeometries.clear();
        meshRef.clear();
        materials.clear();
        materialIds.clear();
        attributes.clear();
        return true;
    }
    bool loadAsset(std::string path, int& assetId){
        if(!sessionOpened){
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
            printError(e.what());
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
        int rootId = -1;
        godot::Ref<HEAsset> assetLib;
        assetLib.instantiate();
        for(int i = 0;i!=asset_count;++i){
            temp = HoudiniEngineUtility::getString(get_session(),assetSH[i]);
            int id = -1;
            createNode(temp,temp,id,rootId);
            assetLib->ownedNodeIds.push_back(id);
        }
        assetRefs.insert({assetId,assetLib});
        return true;
    }
    bool createNode(std::string nodeLabel, std::string operatorName, int& id, int parentId){
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
        godot::Ref<HENode> nodeRef;
        nodeRef.instantiate();
        nodeRefs.insert({id,nodeRef});
        return true;
    }
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
    // Return input node id
    int queryConnectedNode(int nodeId, int inputIndex){
        if(!sessionOpened){
            printError("Failed to query node input: The session is invalid.");
            return false;
        }
        int id2;
        if(HoudiniApi::QueryNodeInput(get_session(),nodeId,inputIndex,&id2) != HAPI_RESULT_SUCCESS){
            printError("Error query node input: ",HoudiniEngineUtility::getLastError().c_str());
            return false;
        }else{
            printFile("Success query node ",nodeId,"'s port ",inputIndex);
        }
        return id2;
    }
    bool cookNode(int id){
        if(!sessionOpened){
            printError("Failed to cook node: The session is invalid.");
            return false;
        }
        if(id == -1)
            return false;
        if(nodeIds.find(id) == nodeIds.end())
            return false;
        if(HoudiniApi::CookNode(get_session(),id,&HESettings::get_singleton()->cookOptions) != HAPI_RESULT_SUCCESS){
            printError("Failed to cook node",HoudiniEngineUtility::getLastCookError().c_str());
            return false;
        }
        std::jthread td([this,id](){
            int status;
            HAPI_Result result;
            do{
                result = HoudiniApi::GetStatus(get_session(),HAPI_STATUS_COOK_STATE,&status);
                std::this_thread::sleep_for(std::chrono::microseconds(1));
            }while(status > HAPI_STATE_MAX_READY_STATE && result == HAPI_RESULT_SUCCESS);
            if(status != HAPI_STATE_READY || result != HAPI_RESULT_SUCCESS){
                printError("Cook failed: ",HoudiniEngineUtility::getLastCookError().c_str());
            }else{
                cookCounts[id]++;
                getParameters(id);
            }
        });
        td.detach();
        return true;
    }
    bool deleteNode(int id){
        if(!sessionOpened){
            printError("Failed to cook node: The session is invalid.");
            return false;
        }
        if(auto a = HoudiniApi::DeleteNode(get_session(),id);a != HAPI_RESULT_SUCCESS){
            printError("Failed to delete node: ",a," - ",HoudiniEngineUtility::getLastError().c_str());
            return false;
        }
        nodeIds.erase(id);
        parameters.erase(id);
        partType.erase(id);
        meshGeometries.erase(id);
        packedPrimData.erase(id);
        meshRef.erase(id);
        materials.erase(id);
        materialIds.erase(id);
        attributes.erase(id);

        return true;
    }
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
    template<typename T>
    bool getAttributeData(int nodeId, int partId, std::string name, HAPI_AttributeOwner owner, std::vector<T>& data){
        HAPI_AttributeInfo info;
        if(HoudiniApi::GetAttributeInfo(get_session(),nodeId,partId,name.c_str(),owner,&info) != HAPI_RESULT_SUCCESS){
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
        }
        return true;
    };
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
                getAttributeData(id, partId, "P", HAPI_ATTROWNER_POINT, mesh_p_attrib_info);
                std::cout << "get: mesh_p_attrib_info.size()" << mesh_p_attrib_info.size() << std::endl;

                std::vector<float> mesh_cd_attrib_data;
                AttribOwner mesh_cd_attrib_owner = AttribOwner::Vertex;
                if(!getAttributeData(id, partId, "Cd", HAPI_ATTROWNER_VERTEX, mesh_cd_attrib_data)){
                    getAttributeData(id, partId, "Cd", HAPI_ATTROWNER_POINT, mesh_cd_attrib_data);
                    mesh_cd_attrib_owner = AttribOwner::Point;
                }
                std::cout << "get: mesh_cd_attrib_data.size()" << mesh_cd_attrib_data.size() << std::endl;

                std::vector<float> mesh_N_attrib_data;
                AttribOwner mesh_N_attrib_owner = AttribOwner::Vertex;
                if(!getAttributeData(id, partId, "N", HAPI_ATTROWNER_VERTEX, mesh_N_attrib_data)){
                    getAttributeData(id, partId, "N", HAPI_ATTROWNER_POINT, mesh_N_attrib_data);
                    mesh_N_attrib_owner = AttribOwner::Point;
                }
                std::cout << "get: mesh_N_attrib_data.size()" << mesh_N_attrib_data.size() << std::endl;
                
                std::vector<float> mesh_uv_attrib_data;
                AttribOwner mesh_uv_attrib_owner = AttribOwner::Vertex;
                if(!getAttributeData(id, partId, "uv", HAPI_ATTROWNER_VERTEX, mesh_uv_attrib_data)){
                    getAttributeData(id, partId, "uv", HAPI_ATTROWNER_POINT, mesh_uv_attrib_data);
                    mesh_uv_attrib_owner = AttribOwner::Point;
                }
                std::cout << "get: mesh_uv_attrib_data.size()" << mesh_uv_attrib_data.size() << std::endl;
                
                std::vector<float> mesh_uv2_attrib_data;
                AttribOwner mesh_uv2_attrib_owner = AttribOwner::Vertex;
                if(!getAttributeData(id, partId, "uv2", HAPI_ATTROWNER_VERTEX, mesh_uv2_attrib_data)){
                    getAttributeData(id, partId, "uv2", HAPI_ATTROWNER_POINT, mesh_uv2_attrib_data);
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
                if(HAPI_GetCurveInfo(get_session(),id,partId,&info) != HAPI_RESULT_SUCCESS){
                    printError(HoudiniEngineUtility::getLastError().c_str());
                    return;
                }
                const char* attr_name = info.isRational ? "Pw" : "P";
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
                if(HAPI_GetCurveCounts(get_session(),id,partId,curveCounts.data(),0,info.curveCount)!=HAPI_RESULT_SUCCESS){
                    printError(HoudiniEngineUtility::getLastError().c_str());
                    return;
                }
                std::vector<float> knots;
                if(info.hasKnots){
                    knots = std::vector<float>(info.knotCount);
                    HAPI_GetCurveKnots(get_session(),id,partId,knots.data(),0,info.knotCount);
                }
                curveGeometries[id][partId] = {info.curveType,info.isRational,info.isClosed||info.isPeriodic,info.order,std::move(curveCounts),std::move(controlPoints),std::move(weights),std::move(knots)};
            }break;
            case PartType::Box:{
                HAPI_BoxInfo info;
                if(HAPI_GetBoxInfo(get_session(),id,partId,&info) != HAPI_RESULT_SUCCESS){
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
        getParameters(nodeId);
        getGeometry(nodeId);
        getMaterial(nodeId);
        if(meshGeometries.find(nodeId) == meshGeometries.end()||meshGeometries[nodeId].empty()){
            return {};
        }
        auto& part = meshGeometries[nodeId][partId];
        [[maybe_unused]]std::vector<int>& faces = std::get<0>(part);
        std::vector<float>& positions = std::get<1>(part);
        std::vector<int>& vertexs = std::get<2>(part);
        std::pair<AttribOwner,std::vector<float>>& color_Attrib = std::get<3>(part);
        std::pair<AttribOwner,std::vector<float>>& normal_Attrib = std::get<4>(part);
        std::pair<AttribOwner,std::vector<float>>& uv_Attrib = std::get<5>(part);
        std::pair<AttribOwner,std::vector<float>>& uv2_Attrib = std::get<6>(part);
        std::vector<float>& colors = color_Attrib.second;
        std::vector<float>& normals = normal_Attrib.second;
        std::vector<float>& uvs = uv_Attrib.second;
        std::vector<float>& uv2s = uv2_Attrib.second;
        static std::vector<std::string> voidMaterialPaths;
        std::vector<std::string>& materialPaths = voidMaterialPaths;
        if(materials.find(nodeId)!=materials.end()&&materials[nodeId].find(partId)!=materials[nodeId].end()){
            materialPaths = materials[nodeId][partId];
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
        auto& ref = meshRef[nodeId][partId];
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
        return meshRef[nodeId][partId];
    }
    std::map<int,godot::Ref<godot::MultiMesh>> createMultiMeshRes(int nodeId, int partId){
        getParameters(nodeId);
        getGeometry(nodeId);
        getMaterial(nodeId);
        if(packedPrimData.find(nodeId) == packedPrimData.end()||packedPrimData[nodeId].empty()){
            return {};
        }
        auto& instancedParts = packedPrimData[nodeId][partId];
        auto& instancePartIds = std::get<0>(instancedParts);
        auto& instancePartTrans = std::get<1>(instancedParts);

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
                        multiMesh->set_mesh(meshRef[nodeId][instancePartIds[i]]);
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
                //         //multiMesh->set_mesh(meshRef[nodeId][instancePartIds[i]]);

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
    std::map<int,godot::Ref<godot::MultiMesh>> getMultiMeshRef(int nodeId, int partId){
        return packedPrimMesh[nodeId][partId];
    }
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

#endif