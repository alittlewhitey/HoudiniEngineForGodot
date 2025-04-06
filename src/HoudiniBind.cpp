#include "HoudiniBind.h"

GDE_EXPORT
godot::Dictionary HoudiniEngine::GetObjectInfo(HoudiniEngineManager* session,int nodeId){
    HAPI_ObjectInfo info;
    if(HoudiniApi::GetObjectInfo(session->get_session(),nodeId,&info) != HAPI_RESULT_SUCCESS){
        printError(HoudiniEngineUtility::getLastError().c_str());
        return {};
    }
    return ObjectInfoToDict(session,info);
}
HAPI_ObjectInfo HoudiniEngine::DictToObjectInfo(HoudiniEngineManager* session, godot::Dictionary objectInfo){
    HAPI_ObjectInfo info;
    godot::Ref<Int> nameSH;
    nameSH.instantiate();
    SetCustomString(session,objectInfo["name"],nameSH);
    info.nameSH = (int)nameSH->value;
    godot::Ref<Int> objectInstancePathSH;
    objectInstancePathSH.instantiate();
    SetCustomString(session,objectInfo["objectInstancePath"],objectInstancePathSH);
    info.objectInstancePathSH = (int)objectInstancePathSH->value;
    info.hasTransformChanged = objectInfo["hasTransformChanged"];
    info.haveGeosChanged = objectInfo["haveGeosChanged"];
    info.isVisible = objectInfo["isVisible"];
    info.isInstancer = objectInfo["isInstancer"];
    info.isInstanced = objectInfo["isInstanced"];
    info.geoCount = objectInfo["geoCount"];
    info.nodeId = objectInfo["nodeId"];
    info.objectToInstanceId = objectInfo["objectToInstanceId"];
    return info;
}
godot::Dictionary HoudiniEngine::ObjectInfoToDict(HoudiniEngineManager* session, HAPI_ObjectInfo info){
    godot::Dictionary dic;
    auto name = HoudiniEngineUtility::getString(session->get_session(), info.nameSH);
    dic["name"] = godot::String::utf8(name.c_str());
    auto objectInstancePath = HoudiniEngineUtility::getString(session->get_session(), info.objectInstancePathSH);
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

godot::Dictionary HoudiniEngine::GetAssetInfo(HoudiniEngineManager* session, int nodeId){
    HAPI_AssetInfo info;
    if(HoudiniApi::GetAssetInfo(session->get_session(),nodeId,&info) != HAPI_RESULT_SUCCESS){
        printError(HoudiniEngineUtility::getLastError().c_str());
        return {};
    }
    return AssetInfoToDict(session,info);
}
HAPI_AssetInfo HoudiniEngine::DictToAssetInfo(HoudiniEngineManager* session, godot::Dictionary assetInfo){
    HAPI_AssetInfo info;
    info.nodeId = assetInfo["nodeId"];
    info.objectNodeId = assetInfo["objectNodeId"];
    info.hasEverCooked = assetInfo["hasEverCooked"];
    godot::Ref<Int> nameSH;
    nameSH.instantiate();
    SetCustomString(session,assetInfo["name"],nameSH);
    info.nameSH = (int)nameSH->value;
    godot::Ref<Int> labelSH;
    labelSH.instantiate();
    SetCustomString(session,assetInfo["label"],labelSH);
    info.labelSH = (int)labelSH->value;
    godot::Ref<Int> filePathSH;
    filePathSH.instantiate();
    SetCustomString(session,assetInfo["filePath"],filePathSH);
    info.filePathSH = (int)filePathSH->value;
    godot::Ref<Int> versionSH;
    versionSH.instantiate();
    SetCustomString(session,assetInfo["version"],versionSH);
    info.versionSH = (int)versionSH->value;
    godot::Ref<Int> fullOpNameSH;
    fullOpNameSH.instantiate();
    SetCustomString(session,assetInfo["fullOpName"],fullOpNameSH);
    info.fullOpNameSH = (int)fullOpNameSH->value;
    godot::Ref<Int> helpTextSH;
    helpTextSH.instantiate();
    SetCustomString(session,assetInfo["helpText"],helpTextSH);
    info.helpTextSH = (int)helpTextSH->value;
    godot::Ref<Int> helpURLSH;
    helpURLSH.instantiate();
    SetCustomString(session,assetInfo["helpURL"],helpURLSH);
    info.helpURLSH = (int)helpURLSH->value;
    info.objectCount = assetInfo["objectCount"];
    info.handleCount = assetInfo["handleCount"];
    info.transformInputCount = assetInfo["transformInputCount"];
    info.geoInputCount = assetInfo["geoInputCount"];
    info.geoOutputCount = assetInfo["geoOutputCount"];
    info.haveObjectsChanged = assetInfo["haveObjectsChanged"];
    info.haveMaterialsChanged = assetInfo["haveMaterialsChanged"];
    return info;
}
godot::Dictionary HoudiniEngine::AssetInfoToDict(HoudiniEngineManager* session, HAPI_AssetInfo info){
    godot::Dictionary dic;
    dic["nodeId"] = info.nodeId;
    dic["objectNodeId"] = info.objectNodeId;
    dic["hasEverCooked"] = info.hasEverCooked;
    auto name = HoudiniEngineUtility::getString(session->get_session(), info.nameSH);
    dic["name"] = godot::String::utf8(name.c_str());
    auto label = HoudiniEngineUtility::getString(session->get_session(), info.labelSH);
    dic["label"] = godot::String::utf8(label.c_str());
    auto filePath = HoudiniEngineUtility::getString(session->get_session(), info.filePathSH);
    dic["filePath"] = godot::String::utf8(filePath.c_str());
    auto version = HoudiniEngineUtility::getString(session->get_session(), info.versionSH);
    dic["version"] = godot::String::utf8(version.c_str());
    auto fullOpName = HoudiniEngineUtility::getString(session->get_session(), info.fullOpNameSH);
    dic["fullOpName"] = godot::String::utf8(fullOpName.c_str());
    auto helpText = HoudiniEngineUtility::getString(session->get_session(), info.helpTextSH);
    dic["helpText"] = godot::String::utf8(helpText.c_str());
    auto helpURL = HoudiniEngineUtility::getString(session->get_session(), info.helpURLSH);
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
godot::Dictionary HoudiniEngine::GetNodeInfo(HoudiniEngineManager* session, int nodeId){
    HAPI_NodeInfo info;
    if(HoudiniApi::GetNodeInfo(session->get_session(),nodeId,&info) != HAPI_RESULT_SUCCESS){
        printError(HoudiniEngineUtility::getLastError().c_str());
        return {};
    }
    return NodeInfoToDict(session,info);
}
HAPI_NodeInfo HoudiniEngine::DictToNodeInfo(HoudiniEngineManager* session, godot::Dictionary nodeInfo){
    HAPI_NodeInfo info;
    info.id = nodeInfo["id"];
    info.parentId = nodeInfo["parentId"];
    godot::Ref<Int> nameSH;
    nameSH.instantiate();
    SetCustomString(session,nodeInfo["name"],nameSH);
    info.nameSH = (int)nameSH->value;
    info.type = (HAPI_NodeType)(int)nodeInfo["type"];
    info.isValid = nodeInfo["isValid"];
    info.totalCookCount = nodeInfo["totalCookCount"];
    info.uniqueHoudiniNodeId = nodeInfo["uniqueHoudiniNodeId"];
    info.parmCount = nodeInfo["parmCount"];
    info.parmChoiceCount = nodeInfo["parmChoiceCount"];
    info.childNodeCount = nodeInfo["childNodeCount"];
    info.inputCount = nodeInfo["inputCount"];
    info.outputCount = nodeInfo["outputCount"];
    info.createdPostAssetLoad = nodeInfo["createdPostAssetLoad"];
    info.isTimeDependent = nodeInfo["isTimeDependent"];
    info.parmIntValueCount = nodeInfo["parmIntValueCount"];
    info.parmFloatValueCount = nodeInfo["parmFloatValueCount"];
    info.parmStringValueCount = nodeInfo["parmStringValueCount"];
    return info;
}
godot::Dictionary HoudiniEngine::NodeInfoToDict(HoudiniEngineManager* session, HAPI_NodeInfo info){
    godot::Dictionary dic;
    dic["id"] = info.id;
    dic["parentId"] = info.parentId;
    auto name = HoudiniEngineUtility::getString(session->get_session(), info.nameSH);
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
godot::Dictionary HoudiniEngine::GetGeoInfo(HoudiniEngineManager* session, int nodeId){
    HAPI_GeoInfo info;
    if(HoudiniApi::GetGeoInfo(session->get_session(),nodeId,&info) != HAPI_RESULT_SUCCESS){
        printError(HoudiniEngineUtility::getLastError().c_str());
        return {};
    }
    return GeoInfoToDict(session,info);
}
HAPI_GeoInfo HoudiniEngine::DictToGeoInfo(HoudiniEngineManager* session, godot::Dictionary geoInfo){
    HAPI_GeoInfo info;
    info.type = (HAPI_GeoType)(int)geoInfo["type"];
    godot::Ref<Int> nameSH;
    nameSH.instantiate();
    SetCustomString(session,geoInfo["name"],nameSH);
    info.nameSH = (int)nameSH->value;
    info.nodeId = geoInfo["nodeId"];
    info.isEditable = geoInfo["isEditable"];
    info.isTemplated = geoInfo["isTemplated"];
    info.isDisplayGeo = geoInfo["isDisplayGeo"];
    info.hasGeoChanged = geoInfo["hasGeoChanged"];
    info.hasMaterialChanged = geoInfo["hasMaterialChanged"];
    info.pointGroupCount = geoInfo["pointGroupCount"];
    info.primitiveGroupCount = geoInfo["primitiveGroupCount"];
    info.edgeGroupCount = geoInfo["edgeGroupCount"];
    info.partCount = geoInfo["partCount"];
    return info;
}
godot::Dictionary HoudiniEngine::GeoInfoToDict(HoudiniEngineManager* session, HAPI_GeoInfo info){
    godot::Dictionary dic;
    dic["type"] = info.type;
    auto name = HoudiniEngineUtility::getString(session->get_session(), info.nameSH);
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
godot::Dictionary HoudiniEngine::GetMaterialInfo(HoudiniEngineManager* session, int nodeId){
        
    HAPI_MaterialInfo info;
    if(HoudiniApi::GetMaterialInfo(session->get_session(),nodeId,&info) != HAPI_RESULT_SUCCESS){
        printError(HoudiniEngineUtility::getLastError().c_str());
        return {};
    }
    return MaterialInfoToDict(session,info);
}
HAPI_MaterialInfo HoudiniEngine::DictToMaterialInfo(HoudiniEngineManager* session, godot::Dictionary matInfo){
    HAPI_MaterialInfo info;
    info.nodeId = matInfo["nodeId"];
    info.exists = matInfo["exists"];
    info.hasChanged = matInfo["hasChanged"];
    return info;
}
godot::Dictionary HoudiniEngine::MaterialInfoToDict(HoudiniEngineManager* session, HAPI_MaterialInfo info){
    godot::Dictionary dic;
    dic["nodeId"] = info.nodeId;
    dic["exists"] = info.exists;
    dic["hasChanged"] = info.hasChanged;
    return dic;
}
godot::Dictionary HoudiniEngine::GetAttrInfo(HoudiniEngineManager* session,int nodeId,int partId,godot::String name,HAPI_AttributeOwner owner){
    HAPI_AttributeInfo info;
    auto res = HoudiniApi::GetAttributeInfo(session->get_session(),nodeId,partId,name.utf8().get_data(),owner,&info);
    if(res != HAPI_RESULT_SUCCESS){
        printError(HoudiniEngineUtility::getLastError().c_str());
        return {};
    }
    return AttrInfoToDict(session,info);
}
HAPI_AttributeInfo HoudiniEngine::DictToAttrInfo(HoudiniEngineManager* session, godot::Dictionary attrInfo){
    HAPI_AttributeInfo info;
    info.count = attrInfo["count"];
    info.exists = attrInfo["exists"];
    info.originalOwner = (HAPI_AttributeOwner)(int)attrInfo["originalOwner"];
    info.owner = (HAPI_AttributeOwner)(int)attrInfo["owner"];
    info.storage = (HAPI_StorageType)(int)attrInfo["storage"];
    info.totalArrayElements = attrInfo["totalArrayElements"];
    info.tupleSize = attrInfo["tupleSize"];
    info.typeInfo = (HAPI_AttributeTypeInfo)(int)attrInfo["typeInfo"];
    return info;
}
godot::Dictionary HoudiniEngine::AttrInfoToDict(HoudiniEngineManager* session, HAPI_AttributeInfo info){
    godot::Dictionary dic;
    dic["count"] = info.count;
    dic["exists"] = info.exists;
    dic["originalOwner"] = info.originalOwner;
    dic["owner"] = info.owner;
    dic["storage"] = info.storage;
    dic["totalArrayElements"] = info.totalArrayElements;
    dic["tupleSize"] = info.tupleSize;
    dic["typeInfo"] = info.typeInfo;
    return dic;
}
HAPI_CookOptions HoudiniEngine::DictToCookOptions(godot::Dictionary options){
    HAPI_CookOptions cookOptions;

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
godot::Dictionary HoudiniEngine::CookOptionsToDict(HAPI_CookOptions cookOptions){
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
HAPI_Result HoudiniEngine::AddAttribute(HoudiniEngineManager* session,int nodeId,int partId,godot::String name,godot::Ref<RefDictionary> attrInfo){
    auto info = DictToAttrInfo(session,attrInfo->value);
    auto res = HoudiniApi::AddAttribute(session->get_session(),nodeId,partId,name.utf8().get_data(),&info);
    attrInfo->value = AttrInfoToDict(session,info);
    return res;
}
HAPI_Result HoudiniEngine::Cleanup(HoudiniEngineManager* session){
    return HoudiniApi::Cleanup(session->get_session());
}
HAPI_Result HoudiniEngine::ClearConnectionError(){
    return HoudiniApi::ClearConnectionError();
}
HAPI_Result HoudiniEngine::CloseSession(HoudiniEngineManager* session){
    return HoudiniApi::CloseSession(session->get_session());
}
HAPI_Result HoudiniEngine::CommitGeo(HoudiniEngineManager* session,int nodeId){
    return HoudiniApi::CommitGeo(session->get_session(),nodeId);
}
HAPI_Result HoudiniEngine::ConnectNodeInput(HoudiniEngineManager* session, int nodeId, int inputIndex, int nodeId_toConnect, int outputIndex){
    return HoudiniApi::ConnectNodeInput(session->get_session(),nodeId,inputIndex,nodeId_toConnect,outputIndex);
}
HAPI_Result HoudiniEngine::CookNode(HoudiniEngineManager* session, int nodeId, godot::Ref<RefDictionary> cookOptions){
    HAPI_CookOptions options;
    options = DictToCookOptions(cookOptions->value);
    auto res = HoudiniApi::CookNode(session->get_session(),nodeId,&options);
    cookOptions->value = CookOptionsToDict(options);
    return res;
}
godot::Dictionary HoudiniEngine::CookOptions_Create(){
    HAPI_CookOptions options = HoudiniApi::CookOptions_Create();
    HoudiniApi::CookOptions_Init(&options);
    return CookOptionsToDict(options);
}
HAPI_Result HoudiniEngine::SetCustomString(HoudiniEngineManager* session, godot::String string_value, godot::Ref<Int> handle_value){
    int temp;
    auto res = HoudiniApi::SetCustomString(session->get_session(),keep_alive_string(string_value.utf8().get_data()),&temp);
    handle_value->value = temp;
    return res;
}