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
#include <atomic>
#include <mutex>
#include <cstring>
#include <algorithm>
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

#include "HEGUtility.h"
#include "HEGContact.h"
#include "HEGImporter.h"
#include "HEGClasses.h"
#include "HEGSettings.h"
#include "HECore.h"
#include "HEPluginLibrary.h"

class HEGCenter: public godot::Node{
    GDCLASS(HEGCenter, godot::Node)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("freeGDNode","node"),static_cast<bool(HEGCenter::*)(godot::Node*)>(&HEGCenter::freeGDNode));
        godot::ClassDB::bind_method(godot::D_METHOD("stopFreeGDNode","node"),static_cast<bool(HEGCenter::*)(godot::Node*)>(&HEGCenter::stopFreeGDNode));
        godot::ClassDB::add_signal("HEGCenter", godot::MethodInfo("SessionStarted"));
    }
    void _notification(int what){
        try{
            switch(what){
            case NOTIFICATION_ENTER_TREE:{
                init();
                set_process(1);
                HEGSettings::get_singleton();
            }break;
            case NOTIFICATION_PROCESS:{
                HEGContact::process_call();
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
        if(core.findPlugin("cook_diagnostics") == nullptr
           || core.findPlugin("asset_lifecycle") == nullptr
           || core.findPlugin("parameter_audit") == nullptr)
            HEPluginLibrary::registerBuiltinPlugins(core);

        godot::OS::get_singleton()->set_low_processor_usage_mode(true);
        freeTimeout = HEGSettings::defaultFreeTimeout;
        godot::Ref<godot::StandardMaterial3D> defaultMaterial;
        defaultMaterial.instantiate();
        defaultMaterial->set_flag(godot::BaseMaterial3D::Flags::FLAG_ALBEDO_FROM_VERTEX_COLOR,true);
        materialRes[""] = defaultMaterial;

        get_tree()->connect("node_removed",godot::Callable(this,"freeGDNode"));
        get_tree()->connect("node_added",godot::Callable(this,"stopFreeGDNode"));
    }
    void process(){
        if(ensureSession()){
            const bool windowFocused = godot::DisplayServer::get_singleton()->window_is_focused();
            if(focused != windowFocused){
                focused = windowFocused;
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
    std::chrono::milliseconds freeTimeout = HEGSettings::defaultFreeTimeout;
    HEGSessionType sessionType = HEGSessionType::InProcess;
    //TODO:
    bool syncViewport = 0;
    int maxSessionRetries = 3;
    int sessionRetryCooldownSec = 5;
    int _retryCount = 0;
    std::chrono::steady_clock::time_point _lastRetryTime;

//Datas:

    godot::Ref<HEGSession> session;

    //      NodeId          NodeRef
    std::unordered_map<int,godot::Ref<HEGNode>> nodeRefs;
    //      AssetId         AssetRef
    std::unordered_map<int,godot::Ref<HEGAsset>> assetRefs;
    //      AssetId         HAPI_AssetInfo
    std::unordered_map<int,HAPI_AssetInfo> assetMeta;
    //      NodeId          ParamName            ParmInfo
    std::unordered_map<int,std::unordered_map<std::string,HAPI_ParmInfo>> parmInfos;
    //Must fill from data directly rather than from other mesh.
    //      nodeId  instancerPartId   instancedPartId     multiMesh
    std::unordered_map<int,std::unordered_map<int,std::unordered_map<int,godot::Ref<godot::MultiMesh>>>> packedPrimMesh;
    //      nodeId      partId      sharedMesh
    std::unordered_map<int,std::unordered_map<int,godot::Ref<godot::ArrayMesh>>> meshRefs;
    //      nodeId      partId      curveMesh
    std::unordered_map<int,std::unordered_map<int,std::vector<godot::Ref<godot::Curve3D>>>> curveRefs;
    //      nodeId      partId      materialResPath
    std::unordered_map<int,std::unordered_map<int,std::vector<std::string>>> materials;
    //      nodeId      partId       if-allSame     material-nodeIds
    std::unordered_map<int,std::unordered_map<int,std::pair<bool,std::vector<int>>>> materialIds;
    //      nodeId      image
    std::unordered_map<int, godot::Ref<godot::Image>> imageRefs;
    // made by user
    //      godot path          Material instance
    std::unordered_map<std::string,godot::Ref<godot::Material>> materialRes;
    // for `freeGDNode` and `stopFreeGDNode`
    std::unordered_set<godot::Node*> createdGDNodes;
    std::unordered_map<godot::Node*, std::shared_ptr<std::jthread>> freeGDNodeTasks;

    // Host-independent core. Owns cook tasks, session epoch, and plugins.
    HECore core;

    // Serializes HAPI CookNode + status polling + geometry fetching. HAPI
    // cook state is session-global, so concurrent cooks would observe each
    // other\'s status and corrupt the shared geometry caches.
    std::mutex cookHapiMutex_;

//Alias:
    using ParamValues = std::vector<std::variant<int64_t,double,std::string>>;
    using NodeParams = std::unordered_map<std::string,ParamValues>;
    using TreeParams = std::unordered_map<std::string,NodeParams>;
    //      assetId          operatorName
    using InstanceData = std::pair<int,std::string>;


    void invalidateAllRefs(){
        for(auto& [nodeId, ref] : nodeRefs){
            if(ref.is_valid()){
                ref->valid = false;
                ref->id = -1;
            }
        }
        for(auto& [assetId, ref] : assetRefs){
            if(ref.is_valid()){
                ref->valid = false;
                ref->id = -1;
            }
        }
    }

    void cleanup(){
        core.cookService().cancelAllAndWait();
        invalidateAllRefs();
        core.clearRuntimeState();

        nodeRefs.clear();
        assetRefs.clear();
        assetMeta.clear();
        parmInfos.clear();
        meshRefs.clear();
        imageRefs.clear();
        materials.clear();
        materialIds.clear();
        curveRefs.clear();
        packedPrimMesh.clear();
        createdGDNodes.clear();
    }
    void _delete_houdini_core_data(int nodeId){
        core.nodeService().clearNode(nodeId);
        core.parameterService().clearNode(nodeId);
        core.geometryService().clearNode(nodeId);
        core.assetService().unregisterNode(nodeId);
    }

    void _delete_godot_wrappers(int nodeId, bool wasTracked, bool wasInstanceRoot){
        auto refIt = nodeRefs.find(nodeId);
        if(refIt != nodeRefs.end() && refIt->second.is_valid()){
            refIt->second->valid = false;
            refIt->second->id = -1;
        }

        nodeRefs.erase(nodeId);
        parmInfos.erase(nodeId);
        meshRefs.erase(nodeId);
        imageRefs.erase(nodeId);
        materials.erase(nodeId);
        materialIds.erase(nodeId);
        curveRefs.erase(nodeId);
        packedPrimMesh.erase(nodeId);

        if(wasInstanceRoot)
            core.notifyInstanceDeleted(nodeId);
        if(wasTracked)
            core.notifyNodeDeleted(nodeId);
    }

    // Main-thread safe full cleanup.
    void _delete_data(int nodeId){
        const bool wasTracked = core.nodeService().nodeIds.find(nodeId) != core.nodeService().nodeIds.end();
        const bool wasInstanceRoot = core.assetService().assetInstances.find(nodeId) != core.assetService().assetInstances.end();
        _delete_houdini_core_data(nodeId);
        _delete_godot_wrappers(nodeId, wasTracked, wasInstanceRoot);
    }

    // Worker-thread path: Houdini/core data can be discarded immediately, only
    // the Godot wrapper objects must be invalidated on the main thread.
    void _delete_data_from_worker(int nodeId){
        const bool wasTracked = core.nodeService().nodeIds.find(nodeId) != core.nodeService().nodeIds.end();
        const bool wasInstanceRoot = core.assetService().assetInstances.find(nodeId) != core.assetService().assetInstances.end();
        _delete_houdini_core_data(nodeId);
        HEGContact::add_call([this,nodeId,wasTracked,wasInstanceRoot]{
            _delete_godot_wrappers(nodeId, wasTracked, wasInstanceRoot);
        });
    }
//Functions:
    void _init_hserver(){
        if(!findproc("hserver")){
            printError("Can't find hserver. Try to restart it by hkey.");
            printError("Please restart the hserver to manually.");
            printWarning("Run ",(HEGSettings::get_singleton()->houdiniRootPath+"/bin/hkey").c_str()," to restart hserver");
            if(HEGSettings::get_singleton()->houdiniRootPath.empty()){
                printWarning("Run Houdini License Administrator (hkey) to restart hserver");
            }else{
                printWarning("Run Houdini License Administrator (",(HEGSettings::get_singleton()->houdiniRootPath+"/bin/hkey").c_str(),") to restart hserver");
            }
        }
    }
    bool freeGDNode(godot::Node* node){
        if(createdGDNodes.find(node) == createdGDNodes.end()){
            return false;
        }
        uint64_t oid = node->get_instance_id();
        auto threadPtr = std::make_shared<std::jthread>([this,node,oid](std::stop_token st){
            std::this_thread::sleep_for(freeTimeout);
            if(st.stop_requested())
                return;
            HEGContact::add_call([this,node,oid]{
                freeGDNodeTasks.erase(node);
                godot::Node* n = godot::Object::cast_to<godot::Node>(godot::ObjectDB::get_instance(oid));
                if(n)
                    n->queue_free();
            });
        });
        freeGDNodeTasks[node] = threadPtr;
        return true;
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
        // Don't query HAPI from the main thread while a cook worker is using
        // the session.
        if(core.cookService().activeCount() > 0)
            return;
        for(auto id : core.nodeService().nodeIds){
            nodeCookSync(id);
        }
    }
    int getTotalCookCount(int id){
        // Prefer HAPI_NodeInfo.totalCookCount. GetTotalCookCount() with
        // combined DISPLAY|RENDER|OBJ_GEOMETRY flags can count one cook
        // multiple times and made sessionCookSync() re-cook the same node
        // on every window focus change.
        HAPI_NodeInfo info = HoudiniApi::NodeInfo_Create();
        if(HoudiniApi::GetNodeInfo(get_session(),id,&info) != HAPI_RESULT_SUCCESS)
            return -1;
        return info.totalCookCount;
    }
    void nodeCookSync(int id){
        auto it = core.nodeService().cookCounts.find(id);
        if(it == core.nodeService().cookCounts.end())
            return;

        const int count = getTotalCookCount(id);
        if(count < 0)
            return;

        const int localCount = it->second.load(std::memory_order_relaxed);
        if(count != localCount){
            it->second.store(count, std::memory_order_relaxed);
            cookNode(id);
        }
    }
    std::string getNodePath(int nodeId){
        HAPI_StringHandle pathSH = -1;
        if(HoudiniApi::GetNodePath(get_session(),nodeId,-1,&pathSH) != HAPI_RESULT_SUCCESS)
            return {};
        return getString(pathSH);
    }
    TreeParams snapshotParams(int rootNodeId){
        TreeParams out;
        std::list<int> queue;
        queue.push_back(rootNodeId);
        while(!queue.empty()){
            int id = queue.front();
            queue.pop_front();
            auto pit = core.parameterService().parameters.find(id);
            if(pit != core.parameterService().parameters.end())
                out[getNodePath(id)] = pit->second;
            for(int c : getChildNodes(id,{HAPI_NODETYPE_ANY},{HAPI_NODEFLAGS_ANY}))
                queue.push_back(c);
        }
        return out;
    }
    void restoreParams(int rootNodeId, const TreeParams& snap){
        struct AutoCookGuard{
            bool oldAutoCook;
            AutoCookGuard(){
                oldAutoCook = HEGSettings::get_singleton()->autoCook;
                HEGSettings::get_singleton()->autoCook = false;
            }
            ~AutoCookGuard(){
                HEGSettings::get_singleton()->autoCook = oldAutoCook;
            }
        } autoCookGuard;

        std::list<int> queue;
        queue.push_back(rootNodeId);
        while(!queue.empty()){
            int id = queue.front();
            queue.pop_front();
            auto sit = snap.find(getNodePath(id));
            if(sit != snap.end()){
                for(auto& [parmName, values] : sit->second)
                    setParameter(id,parmName,values,false);
            }
            for(int c : getChildNodes(id,{HAPI_NODETYPE_ANY},{HAPI_NODEFLAGS_ANY}))
                queue.push_back(c);
        }
    }
    godot::Ref<HEGNode> makeTypeNode(HAPI_NodeType type){
        auto genRes = [] <typename T> () -> godot::Ref<HEGNode> requires std::derived_from<T,HEGNode>||std::same_as<T,HEGNode> {
            godot::Ref<T> temp;
            temp.instantiate();
            return temp;
        };
        switch(type){
            case HAPI_NODETYPE_OBJ:
                return genRes.operator()<HEGObjNode>();
            case HAPI_NODETYPE_SOP:
                return genRes.operator()<HEGSopNode>();
            case HAPI_NODETYPE_CHOP:
                return genRes.operator()<HEGChopNode>();
            case HAPI_NODETYPE_ROP:
                return genRes.operator()<HEGRopNode>();
            case HAPI_NODETYPE_SHOP:
                return genRes.operator()<HEGShopNode>();
            case HAPI_NODETYPE_COP:
                return genRes.operator()<HEGCopNode>();
            case HAPI_NODETYPE_VOP:
                return genRes.operator()<HEGVopNode>();
            case HAPI_NODETYPE_DOP:
                return genRes.operator()<HEGDopNode>();
            case HAPI_NODETYPE_TOP:
                return genRes.operator()<HEGTopNode>();
            default:
                return genRes.operator()<HEGNode>();
        }
    }
    void _registe_node(int id, int assetId = -1){
        auto oldIt = nodeRefs.find(id);
        if(oldIt != nodeRefs.end() && oldIt->second.is_valid()){
            oldIt->second->valid = false;
            oldIt->second->id = -1;
        }

        auto info = getNodeInfo(id);
        core.nodeService().nodeIds.insert(id);
        godot::Ref<HEGNode> nodeRef = makeTypeNode(info.type);
        nodeRef->id = id;
        nodeRef->valid = true;
        nodeRef->sessionEpoch = core.sessionEpoch();
        nodeRefs.insert_or_assign(id,nodeRef);
        if(assetId != -1){
            core.assetService().nodeAsset[id] = assetId;
            core.assetService().assetNodes[assetId].insert(id);
        }
        core.notifyNodeCreated(id);
    }
    void getParameters(int id){
        if(!ensureSession()){
            printError("Failed to get parameters: The session is invalid.");
            return;
        }
        HAPI_NodeInfo info = getNodeInfo(id);

        // HAPI_GetParameters requires length >= 1. Nodes without parameters
        // (common for OBJ roots, subnets, etc.) must clear stale caches and exit.
        if(info.parmCount <= 0){
            core.parameterService().parameters[id].clear();
            parmInfos[id].clear();
            core.parameterService().parameterOrder[id].clear();
            return;
        }

        core.parameterService().parameters[id].clear();
        parmInfos[id].clear();

        std::vector<HAPI_ParmInfo> parm_infos;
        parm_infos.resize(info.parmCount);
        if(auto a = HoudiniApi::GetParameters(get_session(),id,parm_infos.data(),0,info.parmCount);a != HAPI_RESULT_SUCCESS){
            printFile(HoudiniEngineUtility::getLastError().c_str(), " No parm got.");
            core.parameterService().parameterOrder[id].clear();
            if(a == HAPI_RESULT_NODE_INVALID){
                _delete_data_from_worker(id);
            }
            return;
        }

        std::vector<std::string> orderedNames;
        orderedNames.reserve(info.parmCount);

        for(int i = 0;i != info.parmCount;++i){
            auto name = HoudiniEngineUtility::getString(get_session(), parm_infos[i].nameSH);
            std::vector<std::variant<int64_t,double,std::string>> arr;
            if(HoudiniApi::ParmInfo_IsInt(&parm_infos[i])){
                int parm_int_count = HoudiniApi::ParmInfo_GetIntValueCount(&parm_infos[i]);
                std::vector<int> parm_int_values;
                if(parm_int_count > 0){
                    parm_int_values.resize(parm_int_count);
                    if(auto a = HoudiniApi::GetParmIntValues(get_session(),id,parm_int_values.data(),parm_infos[i].intValuesIndex,parm_int_count);a != HAPI_RESULT_SUCCESS){
                        printFile(HoudiniEngineUtility::getLastError().c_str()," ",a);
                        continue;
                    }
                }
                for(int v = 0;v != parm_int_count;++v){
                    arr.push_back(parm_int_values[v]);
                }
            }else if(HoudiniApi::ParmInfo_IsFloat(&parm_infos[i])){
                int parm_float_count = HoudiniApi::ParmInfo_GetFloatValueCount(&parm_infos[i]);
                std::vector<float> parm_float_values;
                if(parm_float_count > 0){
                    parm_float_values.resize(parm_float_count);
                    if(HoudiniApi::GetParmFloatValues(get_session(),id,parm_float_values.data(),parm_infos[i].floatValuesIndex,parm_float_count) != HAPI_RESULT_SUCCESS){
                        printFile(HoudiniEngineUtility::getLastError().c_str());
                        continue;
                    }
                }
                for(int v = 0;v != parm_float_count;++v){
                    arr.push_back(parm_float_values[v]);
                }
            }else if(HoudiniApi::ParmInfo_IsString(&parm_infos[i])){
                int parm_string_count = HoudiniApi::ParmInfo_GetStringValueCount(&parm_infos[i]);
                std::vector<HAPI_StringHandle> parmSH_values;
                if(parm_string_count > 0){
                    parmSH_values.resize(parm_string_count);
                    if(HoudiniApi::GetParmStringValues(get_session(),id,true,parmSH_values.data(),parm_infos[i].stringValuesIndex,parm_string_count)!=HAPI_RESULT_SUCCESS){
                        printFile(HoudiniEngineUtility::getLastError().c_str());
                        continue;
                    }
                }
                for(int v = 0;v != parm_string_count;++v){
                    arr.push_back(HoudiniEngineUtility::getString(get_session(),parmSH_values[v]));
                }
            }
            parmInfos[id][name] = parm_infos[i];
            core.parameterService().parameters[id][name] = arr;
            orderedNames.push_back(name);
        }

        core.parameterService().parameterOrder[id] = std::move(orderedNames);
    }
    bool fetchAvailableAssets(int assetId, std::vector<godot::String>& operators){
        int asset_count = 0;
        if(HoudiniApi::GetAvailableAssetCount(get_session(),assetId,&asset_count) != HAPI_RESULT_SUCCESS){
            printError("Error get available asset count");
            return false;
        }
        std::vector<HAPI_StringHandle> assetSH;
        assetSH.resize(asset_count);
        if(HoudiniApi::GetAvailableAssets(get_session(),assetId,assetSH.data(),asset_count) != HAPI_RESULT_SUCCESS){
            printError("Error get available assets");
            return false;
        }
        operators.clear();
        operators.reserve(asset_count);
        for(int i = 0;i != asset_count;++i){
            operators.push_back(string_cast(
                HoudiniEngineUtility::getString(get_session(),assetSH[i])));
        }
        return true;
    }
    bool _populateAssetLibrary(int assetId, const std::string& path){
        std::vector<godot::String> operators;
        if(!fetchAvailableAssets(assetId,operators))
            return false;

        auto oldIt = assetRefs.find(assetId);
        if(oldIt != assetRefs.end() && oldIt->second.is_valid()){
            oldIt->second->valid = false;
            oldIt->second->id = -1;
        }

        HEAssetRecord record;
        record.handle.id = assetId;
        record.handle.epoch = core.sessionEpoch();
        record.path = path;
        record.operators.reserve(operators.size());
        for(const godot::String& op : operators)
            record.operators.push_back(string_cast(op));

        godot::Ref<HEGAsset> assetLib;
        assetLib.instantiate();
        assetLib->id = assetId;
        assetLib->valid = true;
        assetLib->sessionEpoch = core.sessionEpoch();
        assetLib->ownedNodeOperators = std::move(operators);
        assetRefs[assetId] = assetLib;
        core.assetService().assetPaths[assetId] = path;
        core.notifyAssetLoaded(record);
        return true;
    }
    bool loadAssetLibraryFile(const std::string& path, int& assetId){
        assetId = -1;
        try{
            if(HoudiniApi::LoadAssetLibraryFromFile(
                   get_session(),path.c_str(),true,&assetId) != HAPI_RESULT_SUCCESS){
                printError("Error load Asset from file: ",
                           HoudiniEngineUtility::getLastError().c_str());
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
        return true;
    }
public:

    bool ensureSession(){
        if(getHESession()->valid())
            return true;

        if(getHESession()->type == HEGSessionType::None)
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
        if(type == HEGSessionType::None)
            type = HEGSessionType::InProcess;

        printWarning("Session lost, attempting recovery (", _retryCount+1, "/", maxSessionRetries, ")...");
        stopSession();
        if(!startSession(type)){
            _retryCount++;
            return false;
        }

        _retryCount = 0;
        return true;
    }

    // Public parameter-cache refresh used by the editor GDScript UI.
    // HEGNode creation is intentionally lazy (it does not fetch parameters),
    // so the UI calls this after instantiate/reload to make getParameterList(),
    // getParameterInfo() and getParameter() usable before the first cook.
    bool refreshNodeParameters(int nodeId){
        if(!ensureSession()){
            printWarning("Failed to refresh parameters: The session is invalid.");
            return false;
        }
        if(core.nodeService().nodeIds.find(nodeId) == core.nodeService().nodeIds.end()){
            printWarning("Failed to refresh parameters: Unknown node id ",nodeId);
            return false;
        }
        getParameters(nodeId);
        return true;
    }

//Datas:

//Functions:

    static HEGCenter* get_singleton(){
        static HEGCenter* singleton = nullptr;
        if(singleton == nullptr)
            singleton = memnew(HEGCenter());
        return singleton;
    }
    uint64_t getSessionEpoch() const{
        return core.sessionEpoch();
    }

    HECore& getCore(){
        return core;
    }

    void registerPlugin(std::unique_ptr<HEPlugin> plugin){
        core.registerPlugin(std::move(plugin));
    }
    //Return relative path if in res / user dir tree.
    godot::Ref<HEGSession> getHESession(){
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
        HAPI_GeoInfo info = HoudiniApi::GeoInfo_Create();
        if(HoudiniApi::GetGeoInfo(get_session(),sopId,&info) != HAPI_RESULT_SUCCESS){
            return {};
        }
        return info;
    }
    int getNodeParentId(int nodeId){
        return getNodeInfo(nodeId).parentId;
    }
    int getSafeParentId(int nodeId){
        const int parentId = getNodeInfo(nodeId).parentId;
        return parentId > 0 ? parentId : -1;
    }
    std::string getNodePathString(int nodeId){
        return getNodePath(nodeId);
    }
    std::string getNodeInputName(int nodeId, int inputIndex){
        HAPI_StringHandle handle = -1;
        if(HoudiniApi::GetNodeInputName(
               get_session(),nodeId,inputIndex,&handle) != HAPI_RESULT_SUCCESS)
            return {};
        return getString(handle);
    }
    std::string getNodeOutputName(int nodeId, int outputIndex){
        HAPI_StringHandle handle = -1;
        if(HoudiniApi::GetNodeOutputName(
               get_session(),nodeId,outputIndex,&handle) != HAPI_RESULT_SUCCESS)
            return {};
        return getString(handle);
    }
    bool setNodeDisplay(int nodeId, bool display){
        return HoudiniApi::SetNodeDisplay(
                   get_session(),nodeId,display ? 1 : 0) == HAPI_RESULT_SUCCESS;
    }
    bool isNodeValidLive(int nodeId){
        HAPI_NodeInfo info = getNodeInfo(nodeId);
        HAPI_Bool answer = false;
        return HoudiniApi::IsNodeValid(
                   get_session(),nodeId,info.uniqueHoudiniNodeId,&answer)
                   == HAPI_RESULT_SUCCESS
               && answer;
    }
    std::vector<int> getNodeIds(){
        std::vector<int> ids;
        ids.reserve(core.nodeService().nodeIds.size());
        for(auto a : core.nodeService().nodeIds){
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
    HAPI_AssetInfo getAssetInfo(int assetId){
        auto it = assetMeta.find(assetId);
        if(it != assetMeta.end())
            return it->second;
        return {};
    }
    std::string getAssetPath(int assetId){
        auto it = core.assetService().assetPaths.find(assetId);
        if(it != core.assetService().assetPaths.end())
            return it->second;
        return {};
    }
    std::vector<int> getAssetInstances(int assetId){
        std::vector<int> roots;
        for(auto& [root, rec] : core.assetService().assetInstances){
            if(rec.first == assetId)
                roots.push_back(root);
        }
        return roots;
    }
    int getNodeAssetId(int nodeId){
        auto it = core.assetService().nodeAsset.find(nodeId);
        if(it != core.assetService().nodeAsset.end())
            return it->second;
        return -1;
    }
    bool isAssetInstanceRoot(int nodeId){
        return core.assetService().assetInstances.find(nodeId) != core.assetService().assetInstances.end();
    }
    int getCookStatus(int nodeId){
        auto it = core.nodeService().cookStatus.find(nodeId);
        if(it == core.nodeService().cookStatus.end())
            return 0;
        return it->second;
    }
    std::string getLastCookError(int nodeId){
        return core.nodeService().cookError(nodeId);
    }
    ParamValues getParameter(int nodeId, const std::string& name){
        auto nit = core.parameterService().parameters.find(nodeId);
        if(nit == core.parameterService().parameters.end())
            return {};
        auto pit = nit->second.find(name);
        if(pit == nit->second.end())
            return {};
        return pit->second;
    }
    void setParameter(int nodeId, const std::string& name, ParamValues value, bool autoCook = true){
        if(!ensureSession()){
            printWarning("Failed to set parameter: The session is invalid.");
            return;
        }
        if(value.empty()){
            printWarning("Refusing to set parameter with an empty value: ",name.c_str());
            return;
        }

        const bool allInt = std::all_of(value.begin(),value.end(),[](const auto& v){
            return std::holds_alternative<int64_t>(v);
        });
        const bool allFloat = std::all_of(value.begin(),value.end(),[](const auto& v){
            return std::holds_alternative<double>(v);
        });
        const bool allString = std::all_of(value.begin(),value.end(),[](const auto& v){
            return std::holds_alternative<std::string>(v);
        });

        if((int)allInt + (int)allFloat + (int)allString != 1){
            printWarning("Refusing to set mixed-type parameter values: ",name.c_str());
            return;
        }

        // Validate against cached HAPI_ParmInfo when available.
        auto infoIt = parmInfos.find(nodeId);
        if(infoIt != parmInfos.end()){
            auto pit = infoIt->second.find(name);
            if(pit != infoIt->second.end()){
                const HAPI_ParmInfo& info = pit->second;
                if(HoudiniApi::ParmInfo_IsInt(&info) && !allInt){
                    printWarning("Parameter expects int values: ",name.c_str());
                    return;
                }
                if(HoudiniApi::ParmInfo_IsFloat(&info) && !allFloat){
                    printWarning("Parameter expects float values: ",name.c_str());
                    return;
                }
                if(HoudiniApi::ParmInfo_IsString(&info) && !allString){
                    printWarning("Parameter expects string values: ",name.c_str());
                    return;
                }
            }
        }

        HAPI_Result result = HAPI_RESULT_SUCCESS;
        HAPI_ParmId stringParmId = -1;
        bool haveStringParmId = false;

        for(int i = 0;i != (int)value.size();++i){
            if(std::holds_alternative<int64_t>(value[i])){
                result = HoudiniApi::SetParmIntValue(
                    get_session(),nodeId,name.c_str(),i,std::get<int64_t>(value[i]));
            }else if(std::holds_alternative<double>(value[i])){
                result = HoudiniApi::SetParmFloatValue(
                    get_session(),nodeId,name.c_str(),i,std::get<double>(value[i]));
            }else{
                if(!haveStringParmId){
                    result = HoudiniApi::GetParmIdFromName(
                        get_session(),nodeId,name.c_str(),&stringParmId);
                    if(result != HAPI_RESULT_SUCCESS)
                        break;
                    haveStringParmId = true;
                }
                result = HoudiniApi::SetParmStringValue(
                    get_session(),nodeId,std::get<std::string>(value[i]).c_str(),
                    stringParmId,i);
            }

            if(result != HAPI_RESULT_SUCCESS)
                break;
        }

        if(result != HAPI_RESULT_SUCCESS){
            printWarning("Failed to set parameter: ",name.c_str(),
                         " - ",HoudiniEngineUtility::getLastError().c_str());
            return;
        }

        core.parameterService().parameters[nodeId][name] = value;
        core.notifyParameterChanged(nodeId,name,value);

        if(autoCook && HEGSettings::get_singleton()->autoCook)
            cookNodeAndEmit(nodeId);
    }
    std::vector<std::string> getParameterList(int nodeId){
        auto it = core.parameterService().parameterOrder.find(nodeId);
        if(it == core.parameterService().parameterOrder.end())
            return {};
        return it->second;
    }
    HAPI_ParmInfo getParmInfo(int nodeId, const std::string& name){
        auto it = parmInfos.find(nodeId);
        if(it != parmInfos.end()){
            auto pit = it->second.find(name);
            if(pit != it->second.end())
                return pit->second;
        }
        return {};
    }
    std::vector<std::pair<std::string,std::string>> getParmChoices(int nodeId, const std::string& name){
        auto infoIt = parmInfos.find(nodeId);
        if(infoIt == parmInfos.end())
            return {};
        auto pit = infoIt->second.find(name);
        if(pit == infoIt->second.end() || pit->second.choiceCount == 0)
            return {};
        HAPI_NodeInfo nodeInfo = getNodeInfo(nodeId);
        std::vector<HAPI_ParmChoiceInfo> choices;
        choices.resize(nodeInfo.parmChoiceCount);
        if(HoudiniApi::GetParmChoiceLists(get_session(),nodeId,choices.data(),0,nodeInfo.parmChoiceCount) != HAPI_RESULT_SUCCESS){
            printError("Failed to get parm choice lists");
            return {};
        }
        std::vector<std::pair<std::string,std::string>> res;
        int start = pit->second.choiceIndex;
        int count = pit->second.choiceCount;
        for(int i = start;i != start + count;++i){
            res.emplace_back(getString(choices[i].labelSH),getString(choices[i].valueSH));
        }
        return res;
    }
    bool insertMultiparmInstance(int nodeId, const std::string& name, int instancePosition){
        HAPI_ParmId parmId;
        if(HoudiniApi::GetParmIdFromName(get_session(),nodeId,name.c_str(),&parmId) != HAPI_RESULT_SUCCESS){
            printError("Failed to get parm id: ",name.c_str());
            return false;
        }
        if(HoudiniApi::InsertMultiparmInstance(get_session(),nodeId,parmId,instancePosition) != HAPI_RESULT_SUCCESS){
            printError("Failed to insert multiparm instance: ",name.c_str());
            return false;
        }
        getParameters(nodeId);
        if(HEGSettings::get_singleton()->autoCook)
            cookNodeAndEmit(nodeId);
        return true;
    }
    bool removeMultiparmInstance(int nodeId, const std::string& name, int instancePosition){
        HAPI_ParmId parmId;
        if(HoudiniApi::GetParmIdFromName(get_session(),nodeId,name.c_str(),&parmId) != HAPI_RESULT_SUCCESS){
            printError("Failed to get parm id: ",name.c_str());
            return false;
        }
        if(HoudiniApi::RemoveMultiparmInstance(get_session(),nodeId,parmId,instancePosition) != HAPI_RESULT_SUCCESS){
            printError("Failed to remove multiparm instance: ",name.c_str());
            return false;
        }
        getParameters(nodeId);
        if(HEGSettings::get_singleton()->autoCook)
            cookNodeAndEmit(nodeId);
        return true;
    }
    bool setParmNodeValue(int nodeId, const std::string& name, int nodeValue){
        if(HoudiniApi::SetParmNodeValue(get_session(),nodeId,name.c_str(),nodeValue) != HAPI_RESULT_SUCCESS){
            printError("Failed to set parm node value: ",name.c_str());
            return false;
        }
        getParameters(nodeId);
        if(HEGSettings::get_singleton()->autoCook)
            cookNodeAndEmit(nodeId);
        return true;
    }
    int getParmNodeValue(int nodeId, const std::string& name){
        HAPI_NodeId value;
        if(HoudiniApi::GetParmNodeValue(get_session(),nodeId,name.c_str(),&value) != HAPI_RESULT_SUCCESS)
            return -1;
        return value;
    }
    bool revertParm(int nodeId, const std::string& name){
        if(HoudiniApi::RevertParmToDefaults(get_session(),nodeId,name.c_str()) != HAPI_RESULT_SUCCESS){
            printError("Failed to revert parameter: ",name.c_str());
            return false;
        }
        getParameters(nodeId);
        if(HEGSettings::get_singleton()->autoCook)
            cookNodeAndEmit(nodeId);
        return true;
    }
    HAPI_Session* get_session(){
        if(session.is_null())
            session.instantiate();
        return session->get_session();
    }
    int getPartCount(int nodeId){
        auto it = core.geometryService().partType.find(nodeId);
        return it == core.geometryService().partType.end() ? 0 : (int)it->second.size();
    }
    HEPartType getPartType(int nodeId, int partId){
        auto nodeIt = core.geometryService().partType.find(nodeId);
        if(nodeIt == core.geometryService().partType.end())
            return HEPartType::Invalid;

        auto partIt = nodeIt->second.find(partId);
        return partIt == nodeIt->second.end() ? HEPartType::Invalid : partIt->second;
    }
    godot::Ref<HEGNode> findNodeRef(int nodeId){
        if(nodeRefs.find(nodeId) == nodeRefs.end())
            return {};
        return nodeRefs[nodeId];
    }
    godot::Ref<HEGNode> findOrRegisterNode(int nodeId, int assetId = -1){
        auto ref = findNodeRef(nodeId);
        if(ref.is_null())
            _registe_node(nodeId, assetId);
        return findNodeRef(nodeId);
    }
    godot::Ref<HEGNode> findNodeRefOrAncestor(int nodeId){
        int cursor = nodeId;
        while(cursor >= 0){
            auto ref = findNodeRef(cursor);
            if(ref.is_valid())
                return ref;
            cursor = getNodeInfo(cursor).parentId;
        }
        return {};
    }
    godot::Ref<HEGAsset> findAssetRef(int assetId){
        if(assetRefs.find(assetId) == assetRefs.end())
            return {};
        return assetRefs[assetId];
    }
    std::string getString(HAPI_StringHandle sh){
        if(sh <= 0) return "";
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
    bool startSession(HEGSessionType type,bool use_cooking_thread = true){
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
        const int timeout(HEGSettings::get_singleton()->sessionConfig.newSessionTimeoutSec);
        sessionType = type;
        switch (type)
        {
        case HEGSessionType::InProcess:{
            HAPI_SessionInfo sessionInfo = HoudiniApi::SessionInfo_Create();
            SessionResult = HoudiniApi::CreateInProcessSession(
                get_session(),&sessionInfo
            );
            if(SessionResult == HAPI_RESULT_SUCCESS){
                printLog("Successful create a HEGHapi in-process session");
            }else{
                printError("Error create session: ",SessionResult);
            }
        }break;
        case HEGSessionType::NewNamedPipe:{
            auto houdini = std::filesystem::canonical(HEGSettings::get_singleton()->houdiniRootPath+"/bin/houdini");
            if(!execute(houdini.string()+" -hess=pipe:"+HEGSettings::get_singleton()->sessionConfig.namedPipe)){
                printError("Can't open houdini.");
                break;
            }
            using namespace std::chrono_literals;
            HAPI_SessionInfo sessionInfo = HoudiniApi::SessionInfo_Create();
            printLog("Wait for opening houdini");
            while(SessionResult != HAPI_RESULT_SUCCESS && times < timeout){
                ++times;
                SessionResult = HoudiniApi::CreateThriftNamedPipeSession(
                    get_session(),HEGSettings::get_singleton()->sessionConfig.namedPipe.c_str(),&sessionInfo
                );
                std::this_thread::sleep_for(1s); 
            }
            if(SessionResult == HAPI_RESULT_SUCCESS){
                printLog("Successful create a HEGHapi named-pipe session");
            }else{
                printError("Error create session: ",SessionResult);
            }
        }break;
        case HEGSessionType::NewTCPSocket:{
            auto houdini = std::filesystem::canonical(HEGSettings::get_singleton()->houdiniRootPath+"/bin/houdini");
            if(!execute(houdini.string()+" -hess=port:"+std::to_string(HEGSettings::get_singleton()->sessionConfig.tcpPort))){
                printError("Can't open houdini.");
                break;
            }
            using namespace std::chrono_literals;
            HAPI_SessionInfo sessionInfo = HoudiniApi::SessionInfo_Create();
            printLog("Wait for opening houdini");
            while(SessionResult != HAPI_RESULT_SUCCESS && times < timeout){
                ++times;
                SessionResult = HoudiniApi::CreateThriftSocketSession(
                    get_session(),HEGSettings::get_singleton()->sessionConfig.hostName.c_str(), HEGSettings::get_singleton()->sessionConfig.tcpPort, &sessionInfo
                );
                std::this_thread::sleep_for(1s);
            }
            if(SessionResult == HAPI_RESULT_SUCCESS){
                printLog("Successful create a HEGHapi TCP socket session");
            }else{
                printError("Error create session: ",SessionResult);
            }
        }break;
        case HEGSessionType::ExistingNamedPipe:{
            HAPI_SessionInfo sessionInfo = HoudiniApi::SessionInfo_Create();
            SessionResult = HoudiniApi::CreateThriftNamedPipeSession(
                get_session(),HEGSettings::get_singleton()->sessionConfig.namedPipe.c_str(),&sessionInfo
            );
            if(SessionResult == HAPI_RESULT_SUCCESS){
                printLog("Successful connect to an existint HEGHapi named-pipe session");
            }else{
                printError("Error create session: ",SessionResult);
            }
        }break;
        case HEGSessionType::ExistingTCPSocket:{
            HAPI_SessionInfo sessionInfo = HoudiniApi::SessionInfo_Create();
            SessionResult = HoudiniApi::CreateThriftSocketSession(
                get_session(),HEGSettings::get_singleton()->sessionConfig.hostName.c_str(), HEGSettings::get_singleton()->sessionConfig.tcpPort, &sessionInfo
            );
            if(SessionResult == HAPI_RESULT_SUCCESS){
                printLog("Successful connect to an existint HEGHapi TCP socket session");
            }else{
                printError("Error create session: ",SessionResult);
            }
        }break;
        case HEGSessionType::ExistingSharedMemory:{
            HAPI_SessionInfo sessionInfo = HoudiniApi::SessionInfo_Create();
            SessionResult = HoudiniApi::CreateThriftSharedMemorySession(
                get_session(),HEGSettings::get_singleton()->sessionConfig.sharedMemoryName.c_str(), &sessionInfo
            );
            if(SessionResult == HAPI_RESULT_SUCCESS){
                printLog("Successful connect to an existint HEGHapi shared memory session");
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
            if (sessionType != HEGSessionType::InProcess)
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
                get_session(),&HEGSettings::get_singleton()->cookOptions,use_cooking_thread,-1,"",nullptr,nullptr,nullptr,nullptr
            );
            HEGSettings::get_singleton()->_update_settings();
            if(Result == HAPI_RESULT_SUCCESS){
                printFile("Successfully initialized Houdini Engine.");
            }else if(Result == HAPI_RESULT_ALREADY_INITIALIZED){
                printFile("Successfully initialized Houdini Engine - HEGHapi was already initialized.");
            }else{
                printError("Houdini Engine API initialization failed: ",Result);
                HoudiniApi::Cleanup(get_session());
                HoudiniApi::CloseSession(get_session());
                return false;
            }
        }
        core.notifySessionStarted((HESessionType)type);
        emit_signal("SessionStarted");
        getHESession()->active = true;
        getHESession()->type = type;
        _retryCount = 0;
        return true;
    }
    bool cookSession(){
        bool suc = true;
        for(auto a : core.nodeService().nodeIds){
            if(!cookNode(a))
                suc = false;
        }
        return suc;
    }
    bool stopSession(){
        bool hapiCleanupSucceeded = true;

        // Stop all cook workers before touching the HAPI session. Otherwise a
        // worker can call into HAPI after Cleanup/CloseSession and produce
        // "HAPI_Initialize() needs to be called" errors.
        core.cookService().cancelAllAndWait();

        if(getHESession()->valid()){
            if(HoudiniApi::Cleanup(get_session()) != HAPI_RESULT_SUCCESS){
                printError("Failed to stop the Houdini Engine session - Clean up failed.");
                hapiCleanupSucceeded = false;
            }
            if(HoudiniApi::CloseSession(get_session()) != HAPI_RESULT_SUCCESS){
                printError("Failed to stop the Houdini Engine session - Close session failed.");
                hapiCleanupSucceeded = false;
            }
        }

        // Local state must be cleared even when HEGHapi cleanup fails. Otherwise
        // a later session recovery starts with stale node/asset references.
        cleanup();
        getHESession()->active = false;
        getHESession()->type = HEGSessionType::None;
        core.notifySessionStopped();
        return hapiCleanupSucceeded;
    }
    bool restartSession(){
        auto type = getHESession()->type;
        if(type == HEGSessionType::None)
            type = HEGSessionType::InProcess;
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

            // After LoadHIPFile, node 0 is the root manager and cannot always
            // be passed to ComposeChildNodeList. Enumerate the OBJ manager, its
            // objects, and each object\'s SOP children instead.
            HAPI_NodeId objManager = -1;
            if(HoudiniApi::GetManagerNodeId(get_session(),HAPI_NODETYPE_OBJ,&objManager) == HAPI_RESULT_SUCCESS && objManager >= 0){
                int objectCount = 0;
                if(HoudiniApi::ComposeObjectList(get_session(),objManager,nullptr,&objectCount) == HAPI_RESULT_SUCCESS && objectCount > 0){
                    std::vector<HAPI_ObjectInfo> objects(objectCount);
                    if(HoudiniApi::GetComposedObjectList(get_session(),objManager,objects.data(),0,objectCount) == HAPI_RESULT_SUCCESS){
                        for(const HAPI_ObjectInfo& object : objects){
                            if(object.nodeId >= 0)
                                newNodeIds.push_back(object.nodeId);
                            int childCount = 0;
                            if(HoudiniApi::ComposeChildNodeList(get_session(),object.nodeId,HAPI_NODETYPE_ANY,HAPI_NODEFLAGS_ANY,true,&childCount) == HAPI_RESULT_SUCCESS && childCount > 0){
                                std::vector<HAPI_NodeId> children(childCount);
                                if(HoudiniApi::GetComposedChildNodeList(get_session(),object.nodeId,children.data(),childCount) == HAPI_RESULT_SUCCESS)
                                    newNodeIds.insert(newNodeIds.end(),children.begin(),children.end());
                            }
                        }
                    }
                }
            }
            if(newNodeIds.empty())
                newNodeIds.push_back(0);
        }
        for(auto a : newNodeIds){
            if(findNodeRef(a).is_null()){
                _registe_node(a);
            }
        }
        printFile("loadHIP discovered nodes: ",newNodeIds.size());
        if(cook){
            for(auto id : newNodeIds)
                cookNode(id);
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

        std::error_code ec;
        std::filesystem::path normalized = std::filesystem::weakly_canonical(path, ec);
        std::string key = ec ? std::filesystem::absolute(path).lexically_normal().string()
                             : normalized.string();
        if(!std::filesystem::exists(key)){
            printError("Asset file does not exist: ",path.c_str());
            return false;
        }

        for(auto& [aid, p] : core.assetService().assetPaths){
            if(p == key){
                assetId = aid;
                printFile("Asset already loaded: ",key.c_str()," as id ",aid);
                return true;
            }
        }

        if(!loadAssetLibraryFile(key, assetId))
            return false;

        return _populateAssetLibrary(assetId, key);
    }
    bool createNode(std::string nodeLabel, std::string operatorName, int& id, int parentId, int assetId = -1){
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
        // Register only the newly created node. Child nodes are registered
        // lazily when requested through getChildList(). Eager recursive
        // registration forces Houdini to instantiate the whole HDA network
        // during CreateNode, which can take tens of seconds.
        _registe_node(id, assetId);
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

        if(HEGSettings::get_singleton()->autoCook)
            cookNodeAndEmit(resNode);
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

        if(HEGSettings::get_singleton()->autoCook)
            cookNodeAndEmit(nodeId);
        return true;
    }
    // Return input node id
    int queryConnectedNode(int nodeId, int inputIndex){
        if(!ensureSession()){
            printError("Failed to query node input: The session is invalid.");
            return -1;
        }
        int id2 = -1;
        if(HoudiniApi::QueryNodeInput(get_session(),nodeId,inputIndex,&id2) != HAPI_RESULT_SUCCESS){
            printError("Error query node input: ",HoudiniEngineUtility::getLastError().c_str());
            return -1;
        }else{
            printFile("Success query node ",nodeId,"'s port ",inputIndex);
        }
        return id2;
    }
    void cookNodeAndEmit(int id){
        cookNode(id,[this,id](bool success){
            auto node = findNodeRef(id);
            if(node.is_valid())
                node->emit_signal("cookFinished",success);
        });
    }
    bool cookNode(int id,std::function<void(bool)> cookedCallBack = [](bool){}){
        if(!ensureSession()){
            printError("Failed to cook node: The session is invalid.");
            cookedCallBack(false);
            return false;
        }
        if(id == -1){
            cookedCallBack(false);
            return false;
        }
        if(core.nodeService().nodeIds.find(id) == core.nodeService().nodeIds.end()){
            cookedCallBack(false);
            return false;
        }
        if(core.cookService().isCooking(id) || core.nodeService().cookStatus[id] == 1){
            cookedCallBack(false);
            return false;
        }

        core.nodeService().cookStatus[id] = 1;

        // HAPI_CookNode is now issued on the worker under cookHapiMutex_. This
        // keeps the main thread responsive and serializes all HAPI cook work.
        const uint64_t epoch = core.sessionEpoch();
        auto taskId = core.cookService().start(
            id,
            epoch,
            [this,id,cookedCallBack,epoch](std::stop_token stopToken){
                cookThread(id,cookedCallBack,stopToken,epoch);
            });

        if(taskId == 0){
            printWarning("Failed to start cook task for node ",id);
            core.nodeService().cookStatus[id] = 3;
            cookedCallBack(false);
            return false;
        }

        core.notifyCookStarted(id);

        if(!HEGSettings::get_singleton()->useCookingThread)
            core.cookService().waitAll();

        return true;
    }
    void cookThread(int id, std::function<void(bool)> cookedCallBack, std::stop_token stopToken, uint64_t epoch){
        std::lock_guard<std::mutex> hapiLock(cookHapiMutex_);

        if(stopToken.stop_requested())
            return;

        if(auto a = HoudiniApi::CookNode(get_session(),id,&HEGSettings::get_singleton()->cookOptions);a != HAPI_RESULT_SUCCESS){
            const std::string error = HoudiniEngineUtility::getLastCookError();
            printError("Failed to cook node: ",error.c_str());
            HEGContact::add_call([this,id,cookedCallBack,a,error,epoch]{
                if(core.sessionEpoch() != epoch || core.nodeService().nodeIds.find(id) == core.nodeService().nodeIds.end())
                    return;
                core.nodeService().setCookError(id,error);
                if(a == HAPI_RESULT_NODE_INVALID){
                    core.nodeService().cookStatus[id] = 4;
                    _delete_data(id);
                }else{
                    core.nodeService().cookStatus[id] = 3;
                }
                core.notifyCookFinished(id,false);
                cookedCallBack(false);
            });
            return;
        }

        int status = HAPI_STATE_MAX_READY_STATE;
        HAPI_Result result = HAPI_RESULT_SUCCESS;
        const auto cookDeadline = std::chrono::steady_clock::now() + std::chrono::seconds(120);
        do{
            if(stopToken.stop_requested())
                return;

            result = HoudiniApi::GetStatus(get_session(),HAPI_STATUS_COOK_STATE,&status);
            if(std::chrono::steady_clock::now() >= cookDeadline){
                result = HAPI_RESULT_FAILURE;
                break;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }while(status > HAPI_STATE_MAX_READY_STATE && result == HAPI_RESULT_SUCCESS);

        if(stopToken.stop_requested())
            return;

        if(result != HAPI_RESULT_SUCCESS || status == HAPI_STATE_READY_WITH_FATAL_ERRORS){
            const std::string error = HoudiniEngineUtility::getLastCookError();
            if(error.empty())
                printError("Cook failed: status=",status,", result=",(int)result);
            else
                printError("Cook failed: ",error.c_str());
            core.nodeService().setCookError(id,error);
            HEGContact::add_call([this,id,cookedCallBack,result,epoch]{
                if(core.sessionEpoch() != epoch || core.nodeService().nodeIds.find(id) == core.nodeService().nodeIds.end())
                    return;
                if(result == HAPI_RESULT_NODE_INVALID){
                    core.nodeService().cookStatus[id] = 4;
                    _delete_data(id);
                }else{
                    core.nodeService().cookStatus[id] = 3;
                }
                core.notifyCookFinished(id,false);
                cookedCallBack(false);
            });
            return;
        }

        // HAPI_STATE_READY_WITH_COOK_ERRORS means the cook finished but some
        // nodes reported errors. The display geometry is often still valid,
        // so continue and try to read the result instead of failing the cook.
        if(status == HAPI_STATE_READY_WITH_COOK_ERRORS){
            const std::string error = HoudiniEngineUtility::getLastCookError();
            if(error.empty())
                printWarning("Cook finished with cook errors (status=2). Attempting to read geometry.");
            else
                printWarning("Cook finished with cook errors: ",error.c_str());
        }

        const int totalCookCount = getTotalCookCount(id);
        if(totalCookCount >= 0)
            core.nodeService().cookCounts[id].store(totalCookCount, std::memory_order_relaxed);

        HAPI_NodeInfo info = getNodeInfo(id);
        switch(info.type){
            case HAPI_NODETYPE_SOP:
                fetchSopData(id);
                break;
            case HAPI_NODETYPE_COP:
                fetchCopData(id);
                break;
            default:
                getParameters(id);
                break;
        }

        if(stopToken.stop_requested())
            return;

        HEGContact::add_call([this,id,info,cookedCallBack,epoch]{
            if(core.sessionEpoch() != epoch || core.nodeService().nodeIds.find(id) == core.nodeService().nodeIds.end())
                return;
            buildAndFinalize(id, info.type, cookedCallBack);
            core.notifyCookFinished(id,true);
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

        // Do not delete nodes while a cook worker is still touching them.
        for(int _id : queue)
            core.cookService().cancelAndWait(_id);

        if(auto a = HoudiniApi::DeleteNode(get_session(),id);a != HAPI_RESULT_SUCCESS){
            if(a == HAPI_RESULT_NODE_INVALID){
                for(int _id : queue)
                    _delete_data(_id);
                return true;
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
        bool anyFlagRequested = false;
        for(auto a : flagFilter){
            if(a == HAPI_NODEFLAGS_ANY)
                anyFlagRequested = true;
            else
                flags |= a;
        }
        if(anyFlagRequested)
            flags = HAPI_NODEFLAGS_NONE;
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
    int instantiateAsset(int assetId, const std::string& operatorName, int parentId = -1){
        if(!ensureSession()){
            printError("Failed to instantiate asset: The session is invalid.");
            return -1;
        }
        auto assetIt = assetRefs.find(assetId);
        if(assetIt == assetRefs.end() || !assetIt->second.is_valid()){
            printError("Failed to instantiate asset: Unknown asset id ",assetId);
            return -1;
        }

        const auto& operators = assetIt->second->ownedNodeOperators;
        if(std::find(operators.begin(),operators.end(),string_cast(operatorName)) == operators.end()){
            printError("Failed to instantiate asset: Unknown operator ",operatorName.c_str());
            return -1;
        }

        int id = -1;
        if(!createNode(operatorName,operatorName,id,parentId,assetId)){
            printError("Failed to instantiate asset operator: ",operatorName.c_str());
            return -1;
        }
        core.assetService().assetInstances[id] = {assetId,operatorName};
        core.notifyInstanceCreated(id,assetId);
        if(assetMeta.find(assetId) == assetMeta.end()){
            HAPI_AssetInfo info;
            if(HoudiniApi::GetAssetInfo(get_session(),id,&info) == HAPI_RESULT_SUCCESS)
                assetMeta[assetId] = info;
        }
        return id;
    }
    bool cookAsset(int assetId){
        auto roots = getAssetInstances(assetId);
        if(roots.empty()){
            printError("Failed to cook asset: No instances for asset ",assetId);
            return false;
        }

        bool suc = true;
        for(int root : roots){
            if(!cookAssetInstance(root))
                suc = false;
        }
        return suc;
    }
    bool cookAssetInstance(int rootNodeId){
        if(core.assetService().assetInstances.find(rootNodeId) == core.assetService().assetInstances.end()){
            printError("Failed to cook asset instance: Not an instance root ",rootNodeId);
            return false;
        }

        // Cooking the asset root is sufficient: HAPI/Houdini will evaluate
        // the internal dependency graph as needed. Explicitly cooking every
        // descendant is redundant and surfaces unrelated intermediate node
        // errors as if they were requested by the user.
        return cookNode(rootNodeId);
    }
    bool unloadAsset(int assetId){
        auto assetIt = assetRefs.find(assetId);
        if(assetIt == assetRefs.end() || !assetIt->second.is_valid()){
            printWarning("Failed to unload asset: Unknown asset id ",assetId);
            return false;
        }

        godot::Ref<HEGAsset> asset = assetIt->second;

        for(int root : getAssetInstances(assetId)){
            if(!deleteNode(root)){
                printError("Failed to unload asset: Could not delete instance root ",root);
                return false;
            }
        }

        // HAPI does not expose a per-library unload function in this wrapper.
        // "Unload" is therefore logical: instances are gone and the local
        // library record is removed. The library itself remains loaded until
        // the HAPI session is closed.
        if(asset->res.is_valid())
            asset->res->assetId = -1;
        asset->res.unref();
        asset->valid = false;
        asset->id = -1;

        core.assetService().assetNodes.erase(assetId);
        core.assetService().assetPaths.erase(assetId);
        assetRefs.erase(assetId);
        assetMeta.erase(assetId);
        core.notifyAssetUnloaded(assetId);
        return true;
    }
    bool reloadAsset(int assetId){
        if(!ensureSession()){
            printError("Failed to reload asset: The session is invalid.");
            return false;
        }

        auto pathIt = core.assetService().assetPaths.find(assetId);
        if(pathIt == core.assetService().assetPaths.end()){
            printError("Failed to reload asset: Unknown asset id ",assetId);
            return false;
        }

        auto assetIt = assetRefs.find(assetId);
        if(assetIt == assetRefs.end() || !assetIt->second.is_valid()){
            printError("Failed to reload asset: Invalid asset reference ",assetId);
            return false;
        }

        godot::Ref<HEGAsset> asset = assetIt->second;
        std::string path = pathIt->second;

        std::vector<int> roots = getAssetInstances(assetId);
        std::vector<int> snapParents;
        std::vector<std::string> snapOperators;
        std::vector<TreeParams> snapParams;
        for(int root : roots){
            auto it = core.assetService().assetInstances.find(root);
            if(it == core.assetService().assetInstances.end())
                continue;
            snapParents.push_back(-1);
            snapOperators.push_back(it->second.second);
            snapParams.push_back(snapshotParams(root));
        }

        // Load the new library first. If this fails, old instances are still
        // alive and untouched.
        int newAssetId = -1;
        if(!loadAssetLibraryFile(path,newAssetId))
            return false;

        std::vector<godot::String> newOperators;
        if(!fetchAvailableAssets(newAssetId,newOperators))
            return false;

        if(newAssetId != assetId){
            auto collision = assetRefs.find(newAssetId);
            if(collision != assetRefs.end() && collision->second != asset){
                printError("Failed to reload asset: Asset id collision for ",newAssetId);
                return false;
            }
        }

        // Stage the new asset definition so instantiateAsset() can find it,
        // while keeping the old HEGAsset object untouched for rollback.
        std::vector<godot::String> oldOperators;
        godot::Ref<HEGAsset> stagedAsset;
        if(newAssetId == assetId){
            oldOperators = asset->ownedNodeOperators;
            asset->ownedNodeOperators = newOperators;
            stagedAsset = asset;
        }else{
            stagedAsset.instantiate();
            stagedAsset->id = newAssetId;
            stagedAsset->valid = true;
            stagedAsset->sessionEpoch = core.sessionEpoch();
            stagedAsset->ownedNodeOperators = newOperators;
            assetRefs[newAssetId] = stagedAsset;
        }

        std::vector<int> newRoots;
        bool allCreated = true;
        for(size_t i = 0;i != snapParams.size();++i){
            int newRoot = instantiateAsset(newAssetId,snapOperators[i],snapParents[i]);
            if(newRoot == -1){
                allCreated = false;
                break;
            }
            newRoots.push_back(newRoot);
            restoreParams(newRoot,snapParams[i]);
            cookNodeAndEmit(newRoot);
        }

        if(!allCreated){
            for(int root : newRoots)
                deleteNode(root);
            if(newAssetId == assetId)
                asset->ownedNodeOperators = oldOperators;
            else
                assetRefs.erase(newAssetId);
            printError("Failed to reload asset: Could not recreate all instances.");
            return false;
        }

        // New instances are ready. Now delete the old instance tree(s).
        bool allDeleted = true;
        for(int root : roots){
            if(!deleteNode(root)){
                printError("Failed to reload asset: Could not delete old root ",root);
                allDeleted = false;
            }
        }

        // Commit the same HEGAsset object to the new library id so GDScript
        // references remain valid across reload.
        if(newAssetId != assetId){
            assetRefs.erase(assetId);
            assetRefs[newAssetId] = asset;
            asset->id = newAssetId;
        }
        asset->ownedNodeOperators = newOperators;
        asset->valid = true;
        asset->sessionEpoch = core.sessionEpoch();

        core.assetService().assetPaths.erase(assetId);
        core.assetService().assetPaths[newAssetId] = path;
        assetMeta.erase(assetId);

        if(!newRoots.empty()){
            HAPI_AssetInfo info;
            if(HoudiniApi::GetAssetInfo(get_session(),newRoots.front(),&info) == HAPI_RESULT_SUCCESS)
                assetMeta[newAssetId] = info;
        }

        if(asset->res.is_valid())
            asset->res->assetId = newAssetId;

        core.notifyAssetReloaded(assetId,newAssetId,path);

        if(!allDeleted)
            return false;
        return true;
    }
    int duplicateAsset(int rootNodeId){
        auto it = core.assetService().assetInstances.find(rootNodeId);
        if(it == core.assetService().assetInstances.end()){
            printError("Failed to duplicate asset: Not an instance root ",rootNodeId);
            return -1;
        }
        const InstanceData& rec = it->second;
        int assetId = rec.first;
        std::string operatorName = rec.second;
        int parentId = -1;
        TreeParams snap = snapshotParams(rootNodeId);
        int newId = instantiateAsset(assetId,operatorName,parentId);
        if(newId == -1)
            return -1;
        restoreParams(newId,snap);
        cookNodeAndEmit(newId);
        return newId;
    }
    int rebuildAsset(int rootNodeId){
        auto it = core.assetService().assetInstances.find(rootNodeId);
        if(it == core.assetService().assetInstances.end()){
            printError("Failed to rebuild asset: Not an instance root ",rootNodeId);
            return -1;
        }
        const InstanceData& rec = it->second;
        int assetId = rec.first;
        std::string operatorName = rec.second;
        int parentId = -1;
        TreeParams snap = snapshotParams(rootNodeId);
        if(!deleteNode(rootNodeId)){
            printError("Failed to rebuild asset: Delete old instance failed.");
            return -1;
        }
        int newId = instantiateAsset(assetId,operatorName,parentId);
        if(newId == -1)
            return -1;
        restoreParams(newId,snap);
        cookNodeAndEmit(newId);
        return newId;
    }
    bool resetAssetParams(int rootNodeId){
        if(!ensureSession()){
            printError("Failed to reset parameters: The session is invalid.");
            return false;
        }
        bool suc = true;
        std::list<int> queue;
        std::vector<int> nodeIds;
        queue.push_back(rootNodeId);
        while(!queue.empty()){
            int id = queue.front();
            queue.pop_front();
            nodeIds.push_back(id);
            auto info = getNodeInfo(id);
            if(info.childNodeCount > 0)
                for(int c : getChildNodes(id,{HAPI_NODETYPE_ANY},{HAPI_NODEFLAGS_ANY}))
                    queue.push_back(c);
        }

        for(int id : nodeIds){
            auto pit = core.parameterService().parameters.find(id);
            if(pit != core.parameterService().parameters.end()){
                for(auto& [parmName, values] : pit->second){
                    if(HoudiniApi::RevertParmToDefaults(get_session(),id,parmName.c_str()) != HAPI_RESULT_SUCCESS)
                        suc = false;
                }
            }
            core.parameterService().parameters.erase(id);
            parmInfos.erase(id);
            core.parameterService().parameterOrder.erase(id);
        }

        for(int id : nodeIds)
            getParameters(id);

        if(HEGSettings::get_singleton()->autoCook)
            cookNodeAndEmit(rootNodeId);

        return suc;
    }
    bool renameNode(int id, std::string name){
        if(!ensureSession()){
            printError("Failed to rename node: The session is invalid.");
            return false;
        }
        if(id == -1)
            return false;
        if(core.nodeService().nodeIds.find(id) == core.nodeService().nodeIds.end())
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
    template<typename T>
    bool getAttributeData(int nodeId, int partId, std::string name, HAPI_AttributeOwner owner, std::vector<T>& data){
        HAPI_AttributeInfo info = HoudiniApi::AttributeInfo_Create();
        if(auto a = HoudiniApi::GetAttributeInfo(get_session(),nodeId,partId,name.c_str(),owner,&info);a != HAPI_RESULT_SUCCESS){
            if(a == HAPI_RESULT_NODE_INVALID){
                _delete_data_from_worker(nodeId);
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

    // Background-thread phase: only HEGHapi calls + pure std-data cache writes.
    void fetchSopData(int nodeId){
        HAPI_GeoInfo info = getGeoInfo(nodeId);
        getParameters(nodeId);
        if(!info.isDisplayGeo)
            return;
        getGeometry(nodeId);
        fetchMaterial(nodeId);
    }
    // Main-thread phase: build Godot objects from the fetched cache.
    void buildSopData(int nodeId){
        buildMaterial(nodeId);
        for(auto& a : core.geometryService().partType[nodeId]){
            switch(a.second){
                case HEPartType::Mesh:
                    createMeshRes(nodeId,a.first);
                    break;
                case HEPartType::Instancer:
                    createMultiMeshRes(nodeId,a.first);
                    break;
                case HEPartType::Curve:
                    createCurveRes(nodeId,a.first);
                    break;
                case HEPartType::Invalid:
                    // Transient invalid parts can be observed while a cook is
                    // settling. There is no geometry to build for them.
                    break;
                default:
                    printWarning("Unsupported part type: ",(int)a.second);
                    break;
            }
        }
    }
    // Background-thread phase.
    void fetchCopData(int nodeId){
        getParameters(nodeId);
        getImageData(nodeId);
    }
    // Main-thread phase.
    void buildCopData(int nodeId){
        createImageRes(nodeId);
    }
    // Main-thread finalize: build Godot objects, then mark success + notify.
    void buildAndFinalize(int id, HAPI_NodeType type, std::function<void(bool)> cookedCallBack){
        switch(type){
            case HAPI_NODETYPE_SOP:
                buildSopData(id);
                break;
            case HAPI_NODETYPE_COP:
                buildCopData(id);
                break;
            default:
                break;
        }
        core.nodeService().cookStatus[id] = 2;
        core.nodeService().clearCookError(id);
        cookedCallBack(true);
    }
    //Only for sop node
    void getGeometry(int id){

        HAPI_GeoInfo mesh_geo_info = HoudiniApi::GeoInfo_Create();
        if(HoudiniApi::GetDisplayGeoInfo(get_session(), id, &mesh_geo_info) != HAPI_RESULT_SUCCESS){
            printError(HoudiniEngineUtility::getLastError().c_str());
            return;
        }
        auto& partTypes = core.geometryService().partType[id];
        for(int partId = 0;partId!=mesh_geo_info.partCount;++partId){
            HAPI_PartInfo partInfo = HoudiniApi::PartInfo_Create();
            if(HoudiniApi::GetPartInfo(get_session(), mesh_geo_info.nodeId, partId, &partInfo) != HAPI_RESULT_SUCCESS){
                printError(HoudiniEngineUtility::getLastError().c_str());
                continue;
            }

            const HEPartType type = (HEPartType)partInfo.type;
            auto partTypeIt = partTypes.find(partId);
            const bool hasKnownCachedType = partTypeIt != partTypes.end() && partTypeIt->second != HEPartType::Invalid;

            if(type == HEPartType::Invalid){
                // A transient invalid part must not overwrite a part type that
                // was already fetched successfully.
                if(!hasKnownCachedType)
                    partTypes[partId] = HEPartType::Invalid;
                continue;
            }

            partTypes[partId] = type;
            if(!partInfo.hasChanged)
                continue;

            switch(type){
            case HEPartType::Mesh:{
                std::vector<int> mesh_face_counts;
                if(partInfo.faceCount > 0){
                    mesh_face_counts.resize(partInfo.faceCount);
                    if(HoudiniApi::GetFaceCounts(get_session(),mesh_geo_info.nodeId,partInfo.id,mesh_face_counts.data(),0,partInfo.faceCount) != HAPI_RESULT_SUCCESS){
                        printError(HoudiniEngineUtility::getLastError().c_str());
                        continue;
                    }
                }
                std::vector<int> mesh_vertex_list;
                if(partInfo.vertexCount > 0){
                    mesh_vertex_list.resize(partInfo.vertexCount);
                    if(HoudiniApi::GetVertexList(get_session(),mesh_geo_info.nodeId,partInfo.id,mesh_vertex_list.data(),0, partInfo.vertexCount) != HAPI_RESULT_SUCCESS){
                        printError(HoudiniEngineUtility::getLastError().c_str());
                        continue;
                    }
                }
                // std::for_each(std::execution::par_unseq,mesh_face_counts.begin(),mesh_face_counts.end(),[](int& data){
                //     //TODO: triangulate
                // });
                std::vector<float> mesh_p_attrib_info;
                if(!getAttributeData(mesh_geo_info.nodeId, partInfo.id, HAPI_ATTRIB_POSITION, HAPI_ATTROWNER_POINT, mesh_p_attrib_info)){
                    printError("Mesh part ",partInfo.id," has no P attribute; skipping.");
                    continue;
                }

                std::vector<float> mesh_cd_attrib_data;
                HEAttribOwner mesh_cd_attrib_owner = HEAttribOwner::Vertex;
                if(!getAttributeData(mesh_geo_info.nodeId, partInfo.id, HAPI_ATTRIB_COLOR, HAPI_ATTROWNER_VERTEX, mesh_cd_attrib_data)){
                    getAttributeData(mesh_geo_info.nodeId, partInfo.id, HAPI_ATTRIB_COLOR, HAPI_ATTROWNER_POINT, mesh_cd_attrib_data);
                    mesh_cd_attrib_owner = HEAttribOwner::Point;
                }

                std::vector<float> mesh_N_attrib_data;
                HEAttribOwner mesh_N_attrib_owner = HEAttribOwner::Vertex;
                if(!getAttributeData(mesh_geo_info.nodeId, partInfo.id, HAPI_ATTRIB_NORMAL, HAPI_ATTROWNER_VERTEX, mesh_N_attrib_data)){
                    getAttributeData(mesh_geo_info.nodeId, partInfo.id, HAPI_ATTRIB_NORMAL, HAPI_ATTROWNER_POINT, mesh_N_attrib_data);
                    mesh_N_attrib_owner = HEAttribOwner::Point;
                }
                
                std::vector<float> mesh_uv_attrib_data;
                HEAttribOwner mesh_uv_attrib_owner = HEAttribOwner::Vertex;
                if(!getAttributeData(mesh_geo_info.nodeId, partInfo.id, HAPI_ATTRIB_UV, HAPI_ATTROWNER_VERTEX, mesh_uv_attrib_data)){
                    getAttributeData(mesh_geo_info.nodeId, partInfo.id, HAPI_ATTRIB_UV, HAPI_ATTROWNER_POINT, mesh_uv_attrib_data);
                    mesh_uv_attrib_owner = HEAttribOwner::Point;
                }
                
                std::vector<float> mesh_uv2_attrib_data;
                HEAttribOwner mesh_uv2_attrib_owner = HEAttribOwner::Vertex;
                if(!getAttributeData(mesh_geo_info.nodeId, partInfo.id, HAPI_ATTRIB_UV2, HAPI_ATTROWNER_VERTEX, mesh_uv2_attrib_data)){
                    getAttributeData(mesh_geo_info.nodeId, partInfo.id, HAPI_ATTRIB_UV2, HAPI_ATTROWNER_POINT, mesh_uv2_attrib_data);
                    mesh_uv2_attrib_owner = HEAttribOwner::Point;
                }
                
                core.geometryService().meshGeometries[id][partId] = {std::move(mesh_face_counts),std::move(mesh_p_attrib_info),std::move(mesh_vertex_list),{mesh_cd_attrib_owner,std::move(mesh_cd_attrib_data)},{mesh_N_attrib_owner,std::move(mesh_N_attrib_data)},{mesh_uv_attrib_owner,std::move(mesh_uv_attrib_data)},{mesh_uv2_attrib_owner,std::move(mesh_uv2_attrib_data)}};
            }break;
            case HEPartType::Instancer:{
                std::vector<HAPI_Transform> instancer_transforms;
                std::vector<int> instancePartIds;
                if(partInfo.instanceCount > 0){
                    instancer_transforms.resize(partInfo.instanceCount);
                    auto result = HoudiniApi::GetInstancerPartTransforms(get_session(),mesh_geo_info.nodeId,partInfo.id,HAPI_SRT,instancer_transforms.data(),0,partInfo.instanceCount);
                    if(result != HAPI_RESULT_SUCCESS){
                        printFile(HoudiniEngineUtility::getLastError().c_str());
                        continue;
                    }
                    instancePartIds.resize(partInfo.instanceCount);
                    result = HoudiniApi::GetInstancedPartIds(get_session(),mesh_geo_info.nodeId,partInfo.id,instancePartIds.data(),0,partInfo.instanceCount);
                    if(result != HAPI_RESULT_SUCCESS){
                        printFile(HoudiniEngineUtility::getLastError().c_str());
                        continue;
                    }
                }

                std::vector<HETransform> neutralTransforms;
                neutralTransforms.reserve(instancer_transforms.size());
                for(const HAPI_Transform& t : instancer_transforms){
                    HETransform nt;
                    for(int i = 0; i != 3; ++i){
                        nt.position[i] = t.position[i];
                        nt.scale[i] = t.scale[i];
                        nt.shear[i] = t.shear[i];
                    }
                    for(int i = 0; i != 4; ++i)
                        nt.rotationQuaternion[i] = t.rotationQuaternion[i];
                    nt.rstOrder = t.rstOrder;
                    neutralTransforms.push_back(nt);
                }

                core.geometryService().instancerParts[id][partId] = {std::move(instancePartIds),std::move(neutralTransforms)};
            }break;
            case HEPartType::Curve:{
                HAPI_CurveInfo info = HoudiniApi::CurveInfo_Create();
                if(HoudiniApi::GetCurveInfo(get_session(),mesh_geo_info.nodeId,partInfo.id,&info) != HAPI_RESULT_SUCCESS){
                    printError(HoudiniEngineUtility::getLastError().c_str());
                    continue;
                }
                const char* attr_name = info.isRational ? "Pw" : HAPI_ATTRIB_POSITION;
                [[maybe_unused]]int components = info.isRational ? 4 : 3;
                std::vector<float> rawControlPoints;
                //Using for debug
                HAPI_AttributeInfo attr_info = HoudiniApi::AttributeInfo_Create();
                HoudiniApi::GetAttributeInfo(get_session(),mesh_geo_info.nodeId,partInfo.id,attr_name,HAPI_ATTROWNER_POINT,&attr_info);
                assert(attr_info.count == info.vertexCount);
                assert(attr_info.tupleSize == components);

                getAttributeData(mesh_geo_info.nodeId,partInfo.id,attr_name,HAPI_ATTROWNER_POINT,rawControlPoints);
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
                std::vector<int> curveCounts;
                if(info.curveCount > 0){
                    curveCounts.resize(info.curveCount);
                    if(HoudiniApi::GetCurveCounts(get_session(),mesh_geo_info.nodeId,partInfo.id,curveCounts.data(),0,info.curveCount)!=HAPI_RESULT_SUCCESS){
                        printError(HoudiniEngineUtility::getLastError().c_str());
                        continue;
                    }
                }
                std::vector<float> knots;
                if(info.hasKnots && info.knotCount > 0){
                    knots.resize(info.knotCount);
                    HoudiniApi::GetCurveKnots(get_session(),mesh_geo_info.nodeId,partInfo.id,knots.data(),0,info.knotCount);
                }
                HECurvePartData curveData;
                switch(info.curveType){
                    case HAPI_CURVETYPE_LINEAR: curveData.curveType = HECurveType::Linear; break;
                    case HAPI_CURVETYPE_NURBS:  curveData.curveType = HECurveType::Nurbs; break;
                    case HAPI_CURVETYPE_BEZIER: curveData.curveType = HECurveType::Bezier; break;
                    default:                    curveData.curveType = HECurveType::Invalid; break;
                }
                curveData.curveCount = info.curveCount;
                curveData.vertexCount = info.vertexCount;
                curveData.knotCount = info.knotCount;
                curveData.isPeriodic = info.isPeriodic;
                curveData.isRational = info.isRational;
                curveData.order = info.order;
                curveData.hasKnots = info.hasKnots;
                curveData.isClosed = info.isClosed;
                curveData.curveCounts = std::move(curveCounts);
                curveData.controlPoints = std::move(controlPoints);
                curveData.weights = std::move(weights);
                curveData.knots = std::move(knots);

                core.geometryService().curveGeometries[id][partId] = std::move(curveData);
            }break;
            case HEPartType::Box:{
                HAPI_BoxInfo info;
                if(HoudiniApi::GetBoxInfo(get_session(),mesh_geo_info.nodeId,partInfo.id,&info) != HAPI_RESULT_SUCCESS){
                    printError(HoudiniEngineUtility::getLastError().c_str());
                    continue;
                }
                //TODO: Other part type
            }break;
            case HEPartType::Invalid:{
                printError("This part is invalid.");
            }break;
            default:{
                printError("Not yet supported.");
            }break;
            }
        }
    }
    // Background-thread phase: fetch material ids + gd_mat_path strings only.
    void fetchMaterial(int id){
        HAPI_GeoInfo mesh_geo_info = HoudiniApi::GeoInfo_Create();
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
            std::vector<HAPI_NodeId> matIds;
            if(mesh_part_info.faceCount > 0){
                matIds.resize(mesh_part_info.faceCount);
                if(HoudiniApi::GetMaterialNodeIdsOnFaces(get_session(),mesh_geo_info.nodeId,mesh_part_info.id,&all_the_same,matIds.data(),0,mesh_part_info.faceCount)!=HAPI_RESULT_SUCCESS){
                    printError(HoudiniEngineUtility::getLastError().c_str());
                    continue;
                }
            }

            materialIds[id][partId].first = all_the_same;
            materialIds[id][partId].second = std::move(matIds);
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
            HAPI_AttributeInfo materialPathAttribInfo = HoudiniApi::AttributeInfo_Create();
            
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
                resTypePaths.emplace_back(std::move(path));
            }
            materials[id][partId] = std::move(resTypePaths);
        }
    }
    // Main-thread phase: load material resources referenced by gd_mat_path.
    void buildMaterial(int id){
        for(auto& [partId, paths] : materials[id]){
            for(auto& path : paths){
                if(materialRes.find(path) == materialRes.end()){
                    materialRes[path] = godot::ResourceLoader::get_singleton()->load(string_cast(path));
                }
            }
        }
    }
    godot::Ref<godot::Mesh> createMeshRes(int nodeId, int partId){
        if(core.geometryService().meshGeometries.find(nodeId) == core.geometryService().meshGeometries.end()){
            return {};
        }
        auto& inner = core.geometryService().meshGeometries[nodeId];
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
            if(color_Attrib.first == HEAttribOwner::Vertex){
                if(i < col_num){
                    st->set_color(cols[i]);
                }
            }else if(color_Attrib.first == HEAttribOwner::Point){
                if(vertexs[i] < col_num){
                    st->set_color(cols[vertexs[i]]);
                }
            }
            if(normal_Attrib.first == HEAttribOwner::Vertex){
                if(i < nor_num){
                    st->set_normal(nors[i]);
                }
            }else if(normal_Attrib.first == HEAttribOwner::Point){
                if(vertexs[i] < nor_num){
                    st->set_normal(nors[vertexs[i]]);
                }
            }
            if(uv_Attrib.first == HEAttribOwner::Vertex){
                if(i < uv_num){
                    st->set_uv(uv_s[i]);
                }
            }else if(uv_Attrib.first == HEAttribOwner::Point){
                if(vertexs[i] < uv_num){
                    st->set_uv(uv_s[vertexs[i]]);
                }
            }
            if(uv2_Attrib.first == HEAttribOwner::Vertex){
                if(i < uv2_num){
                    st->set_uv2(uv2_s[i]);
                }
            }else if(uv2_Attrib.first == HEAttribOwner::Point){
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
        auto nodeIt = meshRefs.find(nodeId);
        if(nodeIt == meshRefs.end())
            return {};
        auto partIt = nodeIt->second.find(partId);
        if(partIt == nodeIt->second.end())
            return {};
        return partIt->second;
    }
    bool setMeshSurfaceMaterial(int nodeId, int partId, int surfaceIndex, godot::Ref<godot::Material> material){
        auto nodeIt = meshRefs.find(nodeId);
        if(nodeIt == meshRefs.end())
            return false;
        auto partIt = nodeIt->second.find(partId);
        if(partIt == nodeIt->second.end())
            return false;
        godot::Ref<godot::ArrayMesh> mesh = partIt->second;
        if(mesh.is_null() || surfaceIndex < 0 || surfaceIndex >= mesh->get_surface_count())
            return false;
        mesh->surface_set_material(surfaceIndex, material);
        return true;
    }
    godot::Ref<godot::Material> getMeshSurfaceMaterial(int nodeId, int partId, int surfaceIndex){
        auto nodeIt = meshRefs.find(nodeId);
        if(nodeIt == meshRefs.end())
            return {};
        auto partIt = nodeIt->second.find(partId);
        if(partIt == nodeIt->second.end())
            return {};
        godot::Ref<godot::ArrayMesh> mesh = partIt->second;
        if(mesh.is_null() || surfaceIndex < 0 || surfaceIndex >= mesh->get_surface_count())
            return {};
        return mesh->surface_get_material(surfaceIndex);
    }
    void setMaterialOverride(const std::string& path, godot::Ref<godot::Material> material){
        materialRes[path] = material;
    }
    godot::Ref<godot::Material> getMaterialOverride(const std::string& path){
        auto it = materialRes.find(path);
        return it == materialRes.end() ? godot::Ref<godot::Material>() : it->second;
    }
    std::unordered_map<int,godot::Ref<godot::MultiMesh>> createMultiMeshRes(int nodeId, int partId){
        if(core.geometryService().instancerParts.find(nodeId) == core.geometryService().instancerParts.end()){
            return {};
        }
        auto& inner = core.geometryService().instancerParts[nodeId];
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
            switch(core.geometryService().partType[nodeId][instancePartIds[i]]){
                case HEPartType::Mesh:{
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
                // case HEPartType::Instancer:{
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
        if (core.geometryService().curveGeometries.find(nodeId) == core.geometryService().curveGeometries.end() || core.geometryService().curveGeometries.at(nodeId).find(partId) == core.geometryService().curveGeometries.at(nodeId).end()){
            return {};
        }

        auto& curveData = core.geometryService().curveGeometries.at(nodeId).at(partId);
        auto& curveCounts = curveData.curveCounts;
        auto& controlPoints = curveData.controlPoints;

        auto& vec = curveRefs[nodeId][partId];
        vec.clear();
        vec.resize(curveData.curveCount);
        int point_offset = 0;
        for (int i = 0; i < curveData.curveCount; ++i){
            auto& ref = vec[i];
            if (ref.is_null())
                ref.instantiate();
            
            ref->clear_points();
            int num_points_in_curve = curveCounts[i];
            switch (curveData.curveType)
            {
                case HECurveType::Linear:
                case HECurveType::Nurbs:
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
                case HECurveType::Bezier:
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
        std::vector<unsigned char> buf(bufSize);
        if(HoudiniApi::GetImageMemoryBuffer(get_session(),nodeId, (char*)buf.data(), bufSize) != HAPI_RESULT_SUCCESS){
            printError("Failed to get image data from memory: ", HoudiniEngineUtility::getLastError().c_str());
            return;
        }
        core.geometryService().imageBuffers[nodeId] = std::move(buf);
    }
    godot::Ref<godot::Image> createImageRes(int nodeId){
        auto it = core.geometryService().imageBuffers.find(nodeId);
        if (it == core.geometryService().imageBuffers.end())
            return {};
        const auto& buffer = it->second;
        if (buffer.empty())
            return {};

        godot::PackedByteArray png_data;
        png_data.resize(buffer.size());
        memcpy(png_data.ptrw(), buffer.data(), buffer.size());

        godot::Ref<godot::Image> image;
        image.instantiate();

        if (image->load_png_from_buffer(png_data) != godot::Error::OK){
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
            // Do not auto-cook here. The caller normally calls cook()
            // immediately, and a hidden auto-cook creates a second cook
            // request that may never emit a cookFinished signal.
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
            colorInfo.owner = (HAPI_AttributeOwner)HEAttribOwner::Vertex;
            if(HoudiniApi::AddAttribute(get_session(),id,0,HAPI_ATTRIB_COLOR,&colorInfo) != HAPI_RESULT_SUCCESS){
                printError(HoudiniEngineUtility::getLastError().c_str());
                return -1;
            }
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
            normalInfo.owner = (HAPI_AttributeOwner)HEAttribOwner::Vertex;
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
            uvInfo.owner = (HAPI_AttributeOwner)HEAttribOwner::Vertex;
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
            uv2Info.owner = (HAPI_AttributeOwner)HEAttribOwner::Vertex;
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
            // Same as input mesh: let the caller own the cook/await cycle.
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
};

#endif