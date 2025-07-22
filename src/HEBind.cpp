#include "HEBind.h"

HAPI_SessionInfo HAPI::DictToSessionInfo(godot::Dictionary objectInfo){
    HAPI_SessionInfo info;
    info.connectionCount = objectInfo["connectionCount"];
    info.portType = (HAPI_TCP_PortType)(int)objectInfo["portType"];
    info.minPort = objectInfo["minPort"];
    info.maxPort = objectInfo["maxPort"];
    godot::Array ports = objectInfo["ports"];
    for(int i = 0; i < ports.size(); i++){
        info.ports[i] = ports[i];
    }
    info.sharedMemoryBufferType = (HAPI_ThriftSharedMemoryBufferType)(int)objectInfo["sharedMemoryBufferType"];
    info.sharedMemoryBufferSize = objectInfo["sharedMemoryBufferSize"];
    return info;
}
godot::Dictionary HAPI::SessionInfoToDict(HAPI_SessionInfo info){
    godot::Dictionary dic;
    dic["connectionCount"] = info.connectionCount;
    dic["portType"] = info.portType;
    dic["minPort"] = info.minPort;
    dic["maxPort"] = info.maxPort;
    godot::Array ports;
    for(int i = 0; i < HAPI_MAX_NUM_CONNECTIONS; i++){
        ports.push_back(info.ports[i]);
    }
    dic["ports"] = ports;
    dic["sharedMemoryBufferType"] = info.sharedMemoryBufferType;
    dic["sharedMemoryBufferSize"] = info.sharedMemoryBufferSize;
    return dic;
}
godot::Dictionary HAPI::GetObjectInfo(godot::Ref<HESession> session,int nodeId){
    HAPI_ObjectInfo info;
    if(HoudiniApi::GetObjectInfo(session->get_session(),nodeId,&info) != HAPI_RESULT_SUCCESS){
        printError(HoudiniEngineUtility::getLastError().c_str());
        return {};
    }
    return ObjectInfoToDict(info);
}
HAPI_ObjectInfo HAPI::DictToObjectInfo(godot::Dictionary objectInfo){
    HAPI_ObjectInfo info;
    info.nameSH = objectInfo["nameSH"];
    info.objectInstancePathSH = objectInfo["objectInstancePathSH"];
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
godot::Dictionary HAPI::ObjectInfoToDict(HAPI_ObjectInfo info){
    godot::Dictionary dic;
    dic["nameSH"] = info.nameSH;
    dic["objectInstancePathSH"] = info.objectInstancePathSH;
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

godot::Dictionary HAPI::GetAssetInfo(godot::Ref<HESession> session, int nodeId){
    HAPI_AssetInfo info;
    if(HoudiniApi::GetAssetInfo(session->get_session(),nodeId,&info) != HAPI_RESULT_SUCCESS){
        printError(HoudiniEngineUtility::getLastError().c_str());
        return {};
    }
    return AssetInfoToDict(info);
}
HAPI_AssetInfo HAPI::DictToAssetInfo(godot::Dictionary assetInfo){
    HAPI_AssetInfo info;
    info.nodeId = assetInfo["nodeId"];
    info.objectNodeId = assetInfo["objectNodeId"];
    info.hasEverCooked = assetInfo["hasEverCooked"];
    info.nameSH = assetInfo["nameSH"];
    info.labelSH = assetInfo["labelSH"];
    info.filePathSH = assetInfo["filePathSH"];
    info.versionSH = assetInfo["versionSH"];
    info.fullOpNameSH = assetInfo["fullOpNameSH"];
    info.helpTextSH = assetInfo["helpTextSH"];
    info.helpURLSH = assetInfo["helpURLSH"];
    info.objectCount = assetInfo["objectCount"];
    info.handleCount = assetInfo["handleCount"];
    info.transformInputCount = assetInfo["transformInputCount"];
    info.geoInputCount = assetInfo["geoInputCount"];
    info.geoOutputCount = assetInfo["geoOutputCount"];
    info.haveObjectsChanged = assetInfo["haveObjectsChanged"];
    info.haveMaterialsChanged = assetInfo["haveMaterialsChanged"];
    return info;
}
godot::Dictionary HAPI::AssetInfoToDict(HAPI_AssetInfo info){
    godot::Dictionary dic;
    dic["nodeId"] = info.nodeId;
    dic["objectNodeId"] = info.objectNodeId;
    dic["hasEverCooked"] = info.hasEverCooked;
    dic["nameSH"] = info.nameSH;
    dic["labelSH"] = info.labelSH;
    dic["filePathSH"] = info.filePathSH;
    dic["versionSH"] = info.versionSH;
    dic["fullOpNameSH"] = info.fullOpNameSH;
    dic["helpTextSH"] = info.helpTextSH;
    dic["helpURLSH"] = info.helpURLSH;
    dic["objectCount"] = info.objectCount;
    dic["handleCount"] = info.handleCount;
    dic["transformInputCount"] = info.transformInputCount;
    dic["geoInputCount"] = info.geoInputCount;
    dic["geoOutputCount"] = info.geoOutputCount;
    dic["haveObjectsChanged"] = info.haveObjectsChanged;
    dic["haveMaterialsChanged"] = info.haveMaterialsChanged;
    return dic;
}
godot::Dictionary HAPI::GetNodeInfo(godot::Ref<HESession> session, int nodeId){
    HAPI_NodeInfo info;
    if(HoudiniApi::GetNodeInfo(session->get_session(),nodeId,&info) != HAPI_RESULT_SUCCESS){
        printError(HoudiniEngineUtility::getLastError().c_str());
        return {};
    }
    return NodeInfoToDict(info);
}
HAPI_NodeInfo HAPI::DictToNodeInfo(godot::Dictionary nodeInfo){
    HAPI_NodeInfo info;
    info.id = nodeInfo["id"];
    info.parentId = nodeInfo["parentId"];
    info.nameSH = nodeInfo["nameSH"];
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
godot::Dictionary HAPI::NodeInfoToDict(HAPI_NodeInfo info){
    godot::Dictionary dic;
    dic["id"] = info.id;
    dic["parentId"] = info.parentId;
    dic["nameSH"] = info.nameSH;
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
godot::Dictionary HAPI::GetGeoInfo(godot::Ref<HESession> session, int nodeId){
    HAPI_GeoInfo info;
    if(HoudiniApi::GetGeoInfo(session->get_session(),nodeId,&info) != HAPI_RESULT_SUCCESS){
        printError(HoudiniEngineUtility::getLastError().c_str());
        return {};
    }
    return GeoInfoToDict(info);
}
HAPI_GeoInfo HAPI::DictToGeoInfo(godot::Dictionary geoInfo){
    HAPI_GeoInfo info;
    info.type = (HAPI_GeoType)(int)geoInfo["type"];
    info.nameSH = geoInfo["nameSH"];
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
godot::Dictionary HAPI::GeoInfoToDict(HAPI_GeoInfo info){
    godot::Dictionary dic;
    dic["type"] = info.type;
    dic["nameSH"] = info.nameSH;
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
godot::Dictionary HAPI::GetMaterialInfo(godot::Ref<HESession> session, int nodeId){
        
    HAPI_MaterialInfo info;
    if(HoudiniApi::GetMaterialInfo(session->get_session(),nodeId,&info) != HAPI_RESULT_SUCCESS){
        printError(HoudiniEngineUtility::getLastError().c_str());
        return {};
    }
    return MaterialInfoToDict(info);
}
HAPI_MaterialInfo HAPI::DictToMaterialInfo(godot::Dictionary matInfo){
    HAPI_MaterialInfo info;
    info.nodeId = matInfo["nodeId"];
    info.exists = matInfo["exists"];
    info.hasChanged = matInfo["hasChanged"];
    return info;
}
godot::Dictionary HAPI::MaterialInfoToDict(HAPI_MaterialInfo info){
    godot::Dictionary dic;
    dic["nodeId"] = info.nodeId;
    dic["exists"] = info.exists;
    dic["hasChanged"] = info.hasChanged;
    return dic;
}
godot::Dictionary HAPI::GetPartInfo(godot::Ref<HESession> session,int nodeId,int partId){
    HAPI_PartInfo info;
    if(HoudiniApi::GetPartInfo(session->get_session(),nodeId,partId,&info) != HAPI_RESULT_SUCCESS){
        printError(HoudiniEngineUtility::getLastError().c_str());
        return {};
    }
    return PartInfoToDict(info);
}
HAPI_PartInfo HAPI::DictToPartInfo(godot::Dictionary partInfo){
    HAPI_PartInfo info;
    info.id = partInfo["id"];
    info.nameSH = partInfo["nameSH"];
    info.type = (HAPI_PartType)(int)partInfo["type"];
    info.faceCount = partInfo["faceCount"];
    info.vertexCount = partInfo["vertexCount"];
    info.pointCount = partInfo["pointCount"];
    for(int i = 0;i!=HAPI_ATTROWNER_MAX;++i){
        info.attributeCounts[i] = ((godot::Array)partInfo["attributeCounts"])[i];
    }
    info.isInstanced = partInfo["isInstanced"];
    info.instancedPartCount = partInfo["instancedPartCount"];
    info.instanceCount = partInfo["instanceCount"];
    info.hasChanged = partInfo["hasChanged"];
    return info;
}
godot::Dictionary HAPI::PartInfoToDict(HAPI_PartInfo info){
    godot::Dictionary dic;
    dic["id"] = info.id;
    dic["nameSH"] = info.nameSH;
    dic["type"] = info.type;
    dic["faceCount"] = info.faceCount;
    dic["vertexCount"] = info.vertexCount;
    dic["pointCount"] = info.pointCount;
    godot::Array attributeCounts;
    for(int i = 0;i!=HAPI_ATTROWNER_MAX;++i){
        attributeCounts.push_back(info.attributeCounts[i]);
    }
    dic["attributeCounts"] = attributeCounts;
    dic["isInstanced"] = info.isInstanced;
    dic["instancedPartCount"] = info.instancedPartCount;
    dic["instanceCount"] = info.instanceCount;
    dic["hasChanged"] = info.hasChanged;
    return dic;
}
godot::Dictionary HAPI::GetAttrInfo(godot::Ref<HESession> session,int nodeId,int partId,godot::String name,HAPI_AttributeOwner owner){
    HAPI_AttributeInfo info;
    auto res = HoudiniApi::GetAttributeInfo(session->get_session(),nodeId,partId,name.utf8().get_data(),owner,&info);
    if(res != HAPI_RESULT_SUCCESS){
        printError(HoudiniEngineUtility::getLastError().c_str());
        return {};
    }
    return AttrInfoToDict(info);
}
HAPI_AttributeInfo HAPI::DictToAttrInfo(godot::Dictionary attrInfo){
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
godot::Dictionary HAPI::AttrInfoToDict(HAPI_AttributeInfo info){
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
godot::Dictionary HAPI::GetParmInfo(godot::Ref<HESession> session,int nodeId,int parmId, godot::Ref<RefDictionary> parmInfo){
    HAPI_ParmInfo info;
    auto res = HoudiniApi::GetParmInfo(session->get_session(),nodeId,parmId,&info);
    if(res != HAPI_RESULT_SUCCESS){
        printError(HoudiniEngineUtility::getLastError().c_str());
        return {};
    }
    return ParmInfoToDict(info);
}
HAPI_ParmInfo HAPI::DictToParmInfo(godot::Dictionary parmInfo){
    HAPI_ParmInfo info;
    info.id = parmInfo["id"];
    info.parentId = parmInfo["parentId"];
    info.childIndex = parmInfo["childIndex"];
    info.type = (HAPI_ParmType)(int)parmInfo["type"];
    info.scriptType = (HAPI_PrmScriptType)(int)parmInfo["scriptType"];
    info.typeInfoSH = parmInfo["typeInfoSH"];
    info.permissions = (HAPI_Permissions)(int)parmInfo["permissions"];
    info.tagCount = parmInfo["tagCount"];
    info.size = parmInfo["size"];
    info.choiceListType = (HAPI_ChoiceListType)(int)parmInfo["choiceListType"];
    info.choiceCount = parmInfo["choiceCount"];
    info.nameSH = parmInfo["nameSH"];
    info.labelSH = parmInfo["labelSH"];
    info.templateNameSH = parmInfo["templateNameSH"];
    info.helpSH = parmInfo["helpSH"];
    info.hasMin = parmInfo["hasMin"];
    info.hasMax = parmInfo["hasMax"];
    info.hasUIMin = parmInfo["hasUIMin"];
    info.hasUIMax = parmInfo["hasUIMax"];
    info.min = parmInfo["min"];
    info.max = parmInfo["max"];
    info.UIMin = parmInfo["UIMin"];
    info.UIMax = parmInfo["UIMax"];
    info.invisible = parmInfo["invisible"];
    info.disabled = parmInfo["disabled"];
    info.spare = parmInfo["spare"];
    info.joinNext = parmInfo["joinNext"];
    info.labelNone = parmInfo["labelNone"];
    info.intValuesIndex = parmInfo["intValuesIndex"];
    info.floatValuesIndex = parmInfo["floatValuesIndex"];
    info.stringValuesIndex = parmInfo["stringValuesIndex"];
    info.choiceIndex = parmInfo["choiceIndex"];
    info.inputNodeType = (HAPI_NodeType)(int)parmInfo["inputNodeType"];
    info.inputNodeFlag = (HAPI_NodeFlags)(int)parmInfo["inputNodeFlag"];
    info.isChildOfMultiParm = parmInfo["isChildOfMultiParm"];
    info.instanceNum = parmInfo["instanceNum"];
    info.instanceLength = parmInfo["instanceLength"];
    info.instanceCount = parmInfo["instanceCount"];
    info.instanceStartOffset = parmInfo["instanceStartOffset"];
    info.rampType = (HAPI_RampType)(int)parmInfo["rampType"];
    info.visibilityConditionSH = parmInfo["visibilityConditionSH"];
    info.disabledConditionSH = parmInfo["disabledConditionSH"];
    info.useMenuItemTokenAsValue = parmInfo["useMenuItemTokenAsValue"];
    return info;
}
godot::Dictionary HAPI::ParmInfoToDict(HAPI_ParmInfo info){
    godot::Dictionary dic;
    dic["id"] = info.id;
    dic["parentId"] = info.parentId;
    dic["childIndex"] = info.childIndex;
    dic["type"] = info.type;
    dic["scriptType"] = info.scriptType;
    dic["typeInfoSH"] = info.typeInfoSH;
    dic["permissions"] = info.permissions;
    dic["tagCount"] = info.tagCount;
    dic["size"] = info.size;
    dic["choiceListType"] = info.choiceListType;
    dic["choiceCount"] = info.choiceCount;
    dic["nameSH"] = info.nameSH;
    dic["labelSH"] = info.labelSH;
    dic["templateNameSH"] = info.templateNameSH;
    dic["helpSH"] = info.helpSH;
    dic["hasMin"] = info.hasMin;
    dic["hasMax"] = info.hasMax;
    dic["hasUIMin"] = info.hasUIMin;
    dic["hasUIMax"] = info.hasUIMax;
    dic["min"] = info.min;
    dic["max"] = info.max;
    dic["UIMin"] = info.UIMin;
    dic["UIMax"] = info.UIMax;
    dic["invisible"] = info.invisible;
    dic["disabled"] = info.disabled;
    dic["spare"] = info.spare;
    dic["joinNext"] = info.joinNext;
    dic["labelNone"] = info.labelNone;
    dic["intValuesIndex"] = info.intValuesIndex;
    dic["floatValuesIndex"] = info.floatValuesIndex;
    dic["stringValuesIndex"] = info.stringValuesIndex;
    dic["choiceIndex"] = info.choiceIndex;
    dic["inputNodeType"] = (int)info.inputNodeType; 
    dic["inputNodeFlag"] =(int)info.inputNodeFlag; 
    dic["isChildOfMultiParm"] = info.isChildOfMultiParm;
    dic["instanceNum"] = info.instanceNum;
    dic["instanceLength"] = info.instanceLength;
    dic["instanceCount"] = info.instanceCount;
    dic["instanceStartOffset"] = info.instanceStartOffset;
    dic["rampType"] = info.rampType;
    dic["visibilityConditionSH"] = info.visibilityConditionSH;
    dic["disabledConditionSH"] = info.disabledConditionSH;
    dic["useMenuItemTokenAsValue"] = info.useMenuItemTokenAsValue;
    return dic;
}
HAPI_CookOptions HAPI::DictToCookOptions(godot::Dictionary options){
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
    return cookOptions;
}
godot::Dictionary HAPI::CookOptionsToDict(HAPI_CookOptions cookOptions){
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

HAPI_Transform HAPI::DictToTransform(godot::Dictionary transform){
    HAPI_Transform info;
    for(int i = 0;i!=HAPI_POSITION_VECTOR_SIZE;++i){
        info.position[i] = ((godot::Array)transform["position"])[i];        
    }
    for(int i = 0;i!=HAPI_QUATERNION_VECTOR_SIZE;++i){
        info.rotationQuaternion[i] = ((godot::Array)transform["rotationQuaternion"])[i];        
    }
    for(int i = 0;i!=HAPI_SCALE_VECTOR_SIZE;++i){
        info.scale[i] = ((godot::Array)transform["scale"])[i];        
    }
    for(int i = 0;i!=HAPI_SHEAR_VECTOR_SIZE;++i){
        info.shear[i] = ((godot::Array)transform["shear"])[i];        
    }
    info.rstOrder = (HAPI_RSTOrder)(int)transform["rstOrder"];
    return info;
}
godot::Dictionary HAPI::TransformToDict(HAPI_Transform transform){
    godot::Dictionary dic;
    godot::Array position;
    for(int i = 0;i!=HAPI_POSITION_VECTOR_SIZE;++i){
        position.push_back(transform.position[i]);        
    }
    dic["position"] = position;
    godot::Array rotationQuaternion;
    for(int i = 0;i!=HAPI_QUATERNION_VECTOR_SIZE;++i){
        rotationQuaternion.push_back(transform.rotationQuaternion[i]);        
    }
    dic["rotationQuaternion"] = rotationQuaternion;
    godot::Array scale;
    for(int i = 0;i!=HAPI_SCALE_VECTOR_SIZE;++i){
        scale.push_back(transform.scale[i]);        
    }
    dic["scale"] = scale;
    godot::Array shear;
    for(int i = 0;i!=HAPI_SHEAR_VECTOR_SIZE;++i){
        shear.push_back(transform.shear[i]);        
    }
    dic["shear"] = shear;
    dic["rstOrder"] = transform.rstOrder;
    return dic;
}

HAPI_TransformEuler HAPI::DictToTransformEuler(godot::Dictionary transform){
    HAPI_TransformEuler info;
    for(int i = 0;i!=HAPI_POSITION_VECTOR_SIZE;++i){
        info.position[i] = ((godot::Array)transform["position"])[i];        
    }
    for(int i = 0;i!=HAPI_EULER_VECTOR_SIZE;++i){
        info.rotationEuler[i] = ((godot::Array)transform["rotationEuler"])[i];        
    }
    for(int i = 0;i!=HAPI_SCALE_VECTOR_SIZE;++i){
        info.scale[i] = ((godot::Array)transform["scale"])[i];        
    }
    for(int i = 0;i!=HAPI_SHEAR_VECTOR_SIZE;++i){
        info.shear[i] = ((godot::Array)transform["shear"])[i];        
    }

    info.rstOrder = (HAPI_RSTOrder)(int)transform["rstOrder"];
    info.rotationOrder = (HAPI_XYZOrder)(int)transform["rotationOrder"];
    return info;
}
godot::Dictionary HAPI::TransformEulerToDict(HAPI_TransformEuler transform){
    godot::Dictionary dic;
    godot::Array position;
    for(int i = 0;i!=HAPI_POSITION_VECTOR_SIZE;++i){
        position.push_back(transform.position[i]);        
    }
    dic["position"] = position;
    godot::Array rotationEuler;
    for(int i = 0;i!=HAPI_EULER_VECTOR_SIZE;++i){
        rotationEuler.push_back(transform.rotationEuler[i]);        
    }
    dic["rotationEuler"] = rotationEuler;
    godot::Array scale;
    for(int i = 0;i!=HAPI_SCALE_VECTOR_SIZE;++i){
        scale.push_back(transform.scale[i]);        
    }
    dic["scale"] = scale;
    godot::Array shear;
    for(int i = 0;i!=HAPI_SHEAR_VECTOR_SIZE;++i){
        shear.push_back(transform.shear[i]);        
    }
    dic["shear"] = shear;

    dic["rstOrder"] = transform.rstOrder;
    dic["rotationOrder"] = transform.rotationOrder;
    return dic;
}
HAPI_SessionSyncInfo HAPI::DictToSessionSyncInfo(godot::Dictionary sessionSyncInfo){
    HAPI_SessionSyncInfo info;
    info.cookUsingHoudiniTime = sessionSyncInfo["cookUsingHoudiniTime"];
    info.syncViewport = sessionSyncInfo["syncViewport"];
    return info;
}
godot::Dictionary HAPI::SessionSyncInfoToDict(HAPI_SessionSyncInfo info){
    godot::Dictionary dic;
    dic["cookUsingHoudiniTime"] = info.cookUsingHoudiniTime;
    dic["syncViewport"] = info.syncViewport;
    return dic;
}
HAPI_CurveInfo HAPI::DictToCurveInfo(godot::Dictionary inputCurveInfo){
    HAPI_CurveInfo info;
    info.curveType = (HAPI_CurveType)(int)inputCurveInfo["curveType"];
    info.curveCount = inputCurveInfo["curveCount"];
    info.vertexCount = inputCurveInfo["vertexCount"];
    info.knotCount = inputCurveInfo["knotCount"];
    info.isPeriodic = inputCurveInfo["isPeriodic"];
    info.isRational = inputCurveInfo["isRational"];
    info.order = inputCurveInfo["order"];
    info.hasKnots = inputCurveInfo["hasKnots"];
    info.isClosed = inputCurveInfo["isClosed"];
    return info;
}
godot::Dictionary HAPI::CurveInfoToDict(HAPI_CurveInfo info){
    godot::Dictionary dic;
    dic["curveType"] = info.curveType;
    dic["curveCount"] = info.curveCount;
    dic["vertexCount"] = info.vertexCount;
    dic["knotCount"] = info.knotCount;
    dic["isPeriodic"] = info.isPeriodic;
    dic["isRational"] = info.isRational;
    dic["order"] = info.order;
    dic["hasKnots"] = info.hasKnots;
    dic["isClosed"] = info.isClosed;
    return dic;
}
HAPI_InputCurveInfo HAPI::DictToInputCurveInfo(godot::Dictionary inputCurveInfo){
    HAPI_InputCurveInfo info;
    info.curveType = (HAPI_CurveType)(int)inputCurveInfo["curveType"];
    info.order = (int)inputCurveInfo["order"];
    info.closed = (bool)inputCurveInfo["closed"];
    info.reverse = (bool)inputCurveInfo["reverse"];
    info.inputMethod = (HAPI_InputCurveMethod)(int)inputCurveInfo["inputMethod"];
    info.breakpointParameterization = (HAPI_InputCurveParameterization)(int)inputCurveInfo["breakpointParameterization"];
    return info;
}
godot::Dictionary HAPI::InputCurveInfoToDict(HAPI_InputCurveInfo info){
    godot::Dictionary dic;
    dic["curveType"] = info.curveType;
    dic["order"] = info.order;
    dic["closed"] = info.closed;
    dic["reverse"] = info.reverse;
    dic["inputMethod"] = info.inputMethod;
    dic["breakpointParameterization"] = info.breakpointParameterization;
    return dic;
}
HAPI_Result HAPI::AddAttribute(godot::Ref<HESession> session,int nodeId,int partId,godot::String name,godot::Ref<RefDictionary> attrInfo){
    auto info = DictToAttrInfo(attrInfo->value);
    auto res = HoudiniApi::AddAttribute(session->get_session(),nodeId,partId,name.utf8().get_data(),&info);
    attrInfo->value = AttrInfoToDict(info);
    return res;
}
HAPI_Result HAPI::Cleanup(godot::Ref<HESession> session){
    return HoudiniApi::Cleanup(session->get_session());
}
HAPI_Result HAPI::ClearConnectionError(){
    return HoudiniApi::ClearConnectionError();
}
HAPI_Result HAPI::CloseSession(godot::Ref<HESession> session){
    return HoudiniApi::CloseSession(session->get_session());
}
HAPI_Result HAPI::CommitGeo(godot::Ref<HESession> session,int nodeId){
    return HoudiniApi::CommitGeo(session->get_session(),nodeId);
}
HAPI_Result HAPI::ConnectNodeInput(godot::Ref<HESession> session, int nodeId, int inputIndex, int nodeId_toConnect, int outputIndex){
    return HoudiniApi::ConnectNodeInput(session->get_session(),nodeId,inputIndex,nodeId_toConnect,outputIndex);
}
HAPI_Result HAPI::CookNode(godot::Ref<HESession> session, int nodeId, godot::Ref<RefDictionary> cookOptions){
    HAPI_CookOptions options;
    options = DictToCookOptions(cookOptions->value);
    auto res = HoudiniApi::CookNode(session->get_session(),nodeId,&options);
    cookOptions->value = CookOptionsToDict(options);
    return res;
}
godot::Dictionary HAPI::CookOptions_Create(){
    HAPI_CookOptions options = HoudiniApi::CookOptions_Create();
    HoudiniApi::CookOptions_Init(&options);
    return CookOptionsToDict(options);
}
HAPI_Result HAPI::CreateInProcessSession(godot::Ref<HESession> session,godot::Ref<RefDictionary> sessionInfo){
    HAPI_SessionInfo info = DictToSessionInfo(sessionInfo->value);
    auto res = HoudiniApi::CreateInProcessSession(session->get_session(),&info);
    sessionInfo->value = SessionInfoToDict(info);
    return res;
}
HAPI_Result HAPI::CreateInputNode(godot::Ref<HESession> session, int parentId, godot::Ref<Int> nodeId,godot::String name){
    int temp;
    auto res = HoudiniApi::CreateInputNode(session->get_session(),parentId, &temp, name.utf8().get_data());
    nodeId->value = temp;
    return res;
}
HAPI_Result HAPI::CreateNode(godot::Ref<HESession> session, int parentId, godot::String operatorName, godot::String nodeLabel, bool cookOnCreation, godot::Ref<Int> nodeId){
    int temp;
    auto res = HoudiniApi::CreateNode(session->get_session(),parentId,operatorName.utf8().get_data(),nodeLabel.utf8().get_data(),cookOnCreation,&temp);
    nodeId->value = temp;
    return res;
}
HAPI_Result HAPI::CreateThriftNamedPipeSession(godot::Ref<HESession> session, godot::String pipeName, godot::Ref<RefDictionary> sessionInfo){
    HAPI_SessionInfo info = DictToSessionInfo(sessionInfo->value);
    auto res = HoudiniApi::CreateThriftNamedPipeSession(session->get_session(),pipeName.utf8().get_data(),&info);
    sessionInfo->value = SessionInfoToDict(info);
    return res;
}
HAPI_Result HAPI::CreateThriftSharedMemorySession(godot::Ref<HESession> session, godot::String sharedMemName, godot::Ref<RefDictionary> sessionInfo){
    HAPI_SessionInfo info = DictToSessionInfo(sessionInfo->value);
    auto res = HoudiniApi::CreateThriftSharedMemorySession(session->get_session(),sharedMemName.utf8().get_data(),&info);
    sessionInfo->value = SessionInfoToDict(info);
    return res;
}
HAPI_Result HAPI::CreateThriftSocketSession(godot::Ref<HESession> session, godot::String hostName, int port, godot::Ref<RefDictionary> sessionInfo){
    HAPI_SessionInfo info = DictToSessionInfo(sessionInfo->value);
    auto res = HoudiniApi::CreateThriftSocketSession(session->get_session(),hostName.utf8().get_data(),port,&info);
    sessionInfo->value = SessionInfoToDict(info);
    return res;
}
HAPI_Result HAPI::DeleteAttribute(godot::Ref<HESession> session, int nodeId, int partId, godot::String name, godot::Ref<RefDictionary> attrInfo){
    HAPI_AttributeInfo info = DictToAttrInfo(attrInfo->value);
    auto res = HoudiniApi::DeleteAttribute(session->get_session(),nodeId,partId,name.utf8().get_data(),&info);
    attrInfo->value = AttrInfoToDict(info);
    return res;
}
HAPI_Result HAPI::DeleteNode(godot::Ref<HESession> session, int nodeId){
    return HoudiniApi::DeleteNode(session->get_session(),nodeId);
}
HAPI_Result HAPI::DisconnectNodeInput(godot::Ref<HESession> session, int nodeId, int inputIndex){
    return HoudiniApi::DisconnectNodeInput(session->get_session(),nodeId,inputIndex);
}
godot::Dictionary HAPI::GeoInfo_Create(){
    HAPI_GeoInfo info = HoudiniApi::GeoInfo_Create();
    HoudiniApi::GeoInfo_Init(&info);
    return GeoInfoToDict(info);
}
HAPI_Result HAPI::GetAttributeFloatData(godot::Ref<HESession> session, int nodeId, int partId, godot::String name, godot::Ref<RefDictionary> attrInfo, int stride, godot::Ref<RefArray> dataArray, int start, int length){
    HAPI_AttributeInfo info = DictToAttrInfo(attrInfo->value);
    std::vector<float> data(dataArray->value.size());
    for(int i = 0,sz = dataArray->value.size();i!=sz;++i){
        data[i] = dataArray->value[i];
    }
    auto res = HoudiniApi::GetAttributeFloatData(session->get_session(),nodeId,partId,name.utf8().get_data(),&info,stride,data.data(),start,length);
    for(int i = 0,sz = dataArray->value.size();i!=sz;++i){
        dataArray->value[i] = data[i];
    }
    attrInfo->value = AttrInfoToDict(info);
    return res;
}
HAPI_Result HAPI::GetAttributeIntData(godot::Ref<HESession> session, int nodeId, int partId, godot::String name, godot::Ref<RefDictionary> attrInfo, int stride, godot::Ref<RefArray> dataArray, int start, int length){
    HAPI_AttributeInfo info = DictToAttrInfo(attrInfo->value);
    std::vector<int> data(dataArray->value.size());
    for(int i = 0,sz = dataArray->value.size();i!=sz;++i){
        data[i] = dataArray->value[i];
    }
    auto res = HoudiniApi::GetAttributeIntData(session->get_session(),nodeId,partId,name.utf8().get_data(),&info,stride,data.data(),start,length);
    for(int i = 0,sz = dataArray->value.size();i!=sz;++i){
        dataArray->value[i] = data[i];
    }
    attrInfo->value = AttrInfoToDict(info);
    return res;
}
HAPI_Result HAPI::GetAttributeStringData(godot::Ref<HESession> session, int nodeId, int partId, godot::String name, godot::Ref<RefDictionary> attrInfo, godot::Ref<RefArray> dataArray, int start, int length){
    HAPI_AttributeInfo info = DictToAttrInfo(attrInfo->value);
    std::vector<int> data(dataArray->value.size());
    for(int i = 0,sz = dataArray->value.size();i!=sz;++i){
        data[i] = dataArray->value[i];
    }
    auto res = HoudiniApi::GetAttributeStringData(session->get_session(),nodeId,partId,name.utf8().get_data(),&info,data.data(),start,length);
    for(int i = 0,sz = dataArray->value.size();i!=sz;++i){
        dataArray->value[i] = data[i];
    }
    attrInfo->value = AttrInfoToDict(info);
    return res;
}
godot::String HAPI::GetConnectionError(){
    return string_cast(HoudiniEngineUtility::getConnectionError());
}
int HAPI::GetCookingCurrentCount(godot::Ref<HESession> session){
    int count = 0;
    HoudiniApi::GetCookingCurrentCount(session->get_session(),&count);
    return count;
}
int HAPI::GetCookingTotalCount(godot::Ref<HESession> session){
    int count = 0;
    HoudiniApi::GetCookingTotalCount(session->get_session(),&count);
    return count;
}
godot::Dictionary HAPI::GetDisplayGeoInfo(godot::Ref<HESession> session, int nodeId){
    HAPI_GeoInfo info;
    if(HoudiniApi::GetDisplayGeoInfo(session->get_session(),nodeId,&info) != HAPI_RESULT_SUCCESS){
        printError(HoudiniEngineUtility::getLastError().c_str());
        return {};
    }
    return GeoInfoToDict(info);
}
HAPI_Result HAPI::GetInstanceTransformsOnPart(godot::Ref<HESession> session, int nodeId, int partId, HAPI_RSTOrder rst_order, godot::Ref<RefArray> transforms, int start, int length){
    std::vector<HAPI_Transform> data(transforms->value.size());
    for(int i = 0,sz = transforms->value.size();i!=sz;++i){
        data[i] = DictToTransform(transforms->value[i]);
    }
    auto res = HoudiniApi::GetInstanceTransformsOnPart(session->get_session(),nodeId,partId,rst_order,data.data(),start,length);
    for(int i = 0,sz = transforms->value.size();i!=sz;++i){
        transforms->value[i] = TransformToDict(data[i]);
    }
    return res;
}
HAPI_Result HAPI::GetInstancedObjectIds(godot::Ref<HESession> session, int nodeId, godot::Ref<RefArray> objectIds, int start, int length){
    std::vector<int> data(objectIds->value.size());
    for(int i = 0,sz = objectIds->value.size();i!=sz;++i){
        data[i] = objectIds->value[i];
    }
    auto res = HoudiniApi::GetInstancedObjectIds(session->get_session(),nodeId,data.data(),start,length);
    for(int i = 0,sz = objectIds->value.size();i!=sz;++i){
        objectIds->value[i] = data[i];
    }
    return res;
}
HAPI_Result HAPI::GetInstancedPartIds(godot::Ref<HESession> session, int nodeId, int partId, godot::Ref<RefArray> partIds, int start, int length){
    std::vector<int> data(partIds->value.size());
    for(int i = 0,sz = partIds->value.size();i!=sz;++i){
        data[i] = partIds->value[i];
    }
    auto res = HoudiniApi::GetInstancedPartIds(session->get_session(),nodeId,partId,data.data(),start,length);
    for(int i = 0,sz = partIds->value.size();i!=sz;++i){
        partIds->value[i] = data[i];
    }
    return res;
}
HAPI_Result HAPI::GetInstancerPartTransforms(godot::Ref<HESession> session, int nodeId, int partId, HAPI_RSTOrder rst_order, godot::Ref<RefArray> transforms, int start, int length){
    std::vector<HAPI_Transform> data(transforms->value.size());
    for(int i = 0,sz = transforms->value.size();i!=sz;++i){
        data[i] = DictToTransform(transforms->value[i]);
    }
    auto res = HoudiniApi::GetInstancerPartTransforms(session->get_session(),nodeId,partId,rst_order,data.data(),start,length);
    for(int i = 0,sz = transforms->value.size();i!=sz;++i){
        transforms->value[i] = TransformToDict(data[i]);
    }
    return res;
}
godot::String HAPI::GetLastCookError(godot::Ref<HESession> session){
    return string_cast(HoudiniEngineUtility::getLastCookError(session.is_valid()?session->get_session():nullptr));
}
godot::String HAPI::GetLastError(godot::Ref<HESession> session){
    return string_cast(HoudiniEngineUtility::getLastError(session.is_valid()?session->get_session():nullptr));
}
HAPI_Result HAPI::GetNodeCookResult(godot::Ref<HESession> session, godot::String stringValue, int length){
    return HoudiniApi::GetNodeCookResult(session->get_session(),(char*)stringValue.utf8().get_data(),length);
}
HAPI_Result HAPI::GetNodeCookResultLength(godot::Ref<HESession> session, int nodeId, HAPI_StatusVerbosity verbosity, godot::Ref<Int> length){
    int temp;
    auto res = HoudiniApi::GetNodeCookResultLength(session->get_session(),nodeId,verbosity,&temp);
    length->value = temp;
    return res;
}
HAPI_Result HAPI::GetNodeFromPath(godot::Ref<HESession> session, int parentId, godot::String path, godot::Ref<Int> nodeId){
    int temp;
    auto res = HoudiniApi::GetNodeFromPath(session->get_session(),parentId,path.utf8().get_data(),&temp);
    nodeId->value = temp;
    return res;
}
HAPI_Result HAPI::GetNodeInputName(godot::Ref<HESession> session, int nodeId, int inputIndex, godot::Ref<Int> nameHandle){
    int temp;
    auto res = HoudiniApi::GetNodeInputName(session->get_session(),nodeId,inputIndex,&temp);
    nameHandle->value = temp;
    return res;
}
HAPI_Result HAPI::GetNodeOutputName(godot::Ref<HESession> session, int nodeId, int outputIndex, godot::Ref<Int> nameHandle){
    int temp;
    auto res = HoudiniApi::GetNodeOutputName(session->get_session(),nodeId,outputIndex,&temp);
    nameHandle->value = temp;
    return res;
}
HAPI_Result HAPI::GetNodePath(godot::Ref<HESession> session, int nodeId, int relativeToNodeId, godot::Ref<Int> pathHandle){
    int temp;
    auto res = HoudiniApi::GetNodePath(session->get_session(),nodeId,relativeToNodeId,&temp);
    pathHandle->value = temp;
    return res;
}
HAPI_Result HAPI::GetObjectTransform(godot::Ref<HESession> session, int nodeId, int relativeToNodeId, HAPI_RSTOrder rst_order, godot::Ref<RefDictionary> transform){
    HAPI_Transform info;
    auto res = HoudiniApi::GetObjectTransform(session->get_session(),nodeId,relativeToNodeId,rst_order,&info);
    transform->value = TransformToDict(info);
    return res;
}
HAPI_Result HAPI::GetOutputGeoCount(godot::Ref<HESession> session, int nodeId, godot::Ref<Int> count){
    int temp;
    auto res = HoudiniApi::GetOutputGeoCount(session->get_session(),nodeId,&temp);
    count->value = temp;
    return res;
}
HAPI_Result HAPI::GetOutputGeoInfos(godot::Ref<HESession> session, int nodeId, godot::Ref<RefArray> geoInfos, int count){
    std::vector<HAPI_GeoInfo> data(count);
    auto res = HoudiniApi::GetOutputGeoInfos(session->get_session(),nodeId,data.data(),count);
    geoInfos->value.resize(count);
    for(int i = 0,sz = count;i!=sz;++i){
        geoInfos->value[i] = GeoInfoToDict(data[i]);
    }
    return res;
}
HAPI_Result HAPI::GetOutputNodeId(godot::Ref<HESession> session, int nodeId, int output, godot::Ref<Int> outputNodeId){
    int temp;
    auto res = HoudiniApi::GetOutputNodeId(session->get_session(),nodeId,output,&temp);
    outputNodeId->value = temp;
    return res;
}
HAPI_Result HAPI::GetParameters(godot::Ref<HESession> session, int nodeId, godot::Ref<RefArray> parameters, int start, int length){
    std::vector<HAPI_ParmInfo> data(length-start);
    for(int i = 0,sz = parameters->value.size();i!=sz;++i){
        data[i] = DictToParmInfo(parameters->value[i]);
    }
    auto res = HoudiniApi::GetParameters(session->get_session(),nodeId,data.data(),start,length);
    for(int i = 0,sz = parameters->value.size();i!=sz;++i){
        parameters->value[i] = ParmInfoToDict(data[i]);
    }
    return res;
}
HAPI_Result HAPI::GetParmFloatValue(godot::Ref<HESession> session, int nodeId, godot::String parmName, int index, godot::Ref<Float> value){
    float temp;
    auto res = HoudiniApi::GetParmFloatValue(session->get_session(),nodeId,parmName.utf8().get_data(),index,&temp);
    value->value = temp;
    return res;
}
HAPI_Result HAPI::GetParmIdFromName(godot::Ref<HESession> session, int nodeId, godot::String parmName, godot::Ref<Int> parmId){
    int temp;
    auto res = HoudiniApi::GetParmIdFromName(session->get_session(),nodeId,parmName.utf8().get_data(),&temp);
    parmId->value = temp;
    return res;
}
HAPI_Result HAPI::GetParmIntValue(godot::Ref<HESession> session, int nodeId, godot::String parmName, int index, godot::Ref<Int> value){
    int temp;
    auto res = HoudiniApi::GetParmIntValue(session->get_session(),nodeId,parmName.utf8().get_data(),index,&temp);
    value->value = temp;
    return res;
}
HAPI_Result HAPI::GetParmNodeValue(godot::Ref<HESession> session, int nodeId, godot::String parmName, godot::Ref<Int> value){
    int temp;
    auto res = HoudiniApi::GetParmNodeValue(session->get_session(),nodeId,parmName.utf8().get_data(),&temp);
    value->value = temp;
    return res;
}
HAPI_Result HAPI::GetParmStringValue(godot::Ref<HESession> session, int nodeId, godot::String parmName, int index, bool evaluate, godot::Ref<Int> value){
    int temp;
    auto res = HoudiniApi::GetParmStringValue(session->get_session(),nodeId,parmName.utf8().get_data(),index,evaluate,&temp);
    value->value = temp;
    return res;
}
HAPI_Result HAPI::GetStatus(godot::Ref<HESession> session, HAPI_StatusType statusType, godot::Ref<Int> status){
    int temp;
    auto res = HoudiniApi::GetStatus(session->get_session(),statusType,&temp);
    status->value = temp;
    return res;
}
godot::String HAPI::GetStatusString(godot::Ref<HESession> session, HAPI_StatusType status_type){
    char buf[256];
    HoudiniApi::GetStatusString(session->get_session(),status_type,buf,256);
    return buf;
}
godot::String HAPI::GetString(godot::Ref<HESession> session, int stringHandle){
    std::string str = HoudiniEngineUtility::getString(session->get_session(), stringHandle);
    return string_cast(str);
}
HAPI_Result HAPI::Initialize(godot::Ref<HESession> session, godot::Dictionary cookOptions, bool useCookingThread, int cookingThreadStackSize, godot::String houdiniEnvironmentFiles, godot::String otlSearchPath, godot::String dsoSearchPath, godot::String imageDsoSearchPath, godot::String audioDsoSearchPath){
    HAPI_CookOptions options = DictToCookOptions(cookOptions);
    auto otlSearch = otlSearchPath.is_empty()?nullptr:otlSearchPath.utf8().get_data();
    auto dsoSearch = dsoSearchPath.is_empty()?nullptr:dsoSearchPath.utf8().get_data();
    auto imageDsoSearch = imageDsoSearchPath.is_empty()?nullptr:imageDsoSearchPath.utf8().get_data();
    auto audioDsoSearch = audioDsoSearchPath.is_empty()?nullptr:audioDsoSearchPath.utf8().get_data();
    auto res = HoudiniApi::Initialize(session->get_session(),&options,useCookingThread,cookingThreadStackSize,houdiniEnvironmentFiles.utf8().get_data(),otlSearch,dsoSearch,imageDsoSearch,audioDsoSearch);
    return res;
}
HAPI_Result HAPI::Interrupt(godot::Ref<HESession> session){
    return HoudiniApi::Interrupt(session->get_session());
}
HAPI_Result HAPI::IsInitialized(godot::Ref<HESession> session){
    return HoudiniApi::IsInitialized(session->get_session());
}
HAPI_Result HAPI::IsSessionValid(godot::Ref<HESession> session){
    return HoudiniApi::IsSessionValid(session->get_session());
}
HAPI_Result HAPI::IsNodeValid(godot::Ref<HESession> session, int nodeId, int uniqueNodeId, godot::Ref<Bool> answer){
    bool temp;
    auto res = HoudiniApi::IsNodeValid(session->get_session(),nodeId,uniqueNodeId,&temp);
    answer->value = temp;
    return res;
}
HAPI_Result HAPI::LoadAssetLibraryFromFile(godot::Ref<HESession> session, godot::String filePath, bool allowOverwrite, godot::Ref<Int> assetId){
    int temp;
    auto res = HoudiniApi::LoadAssetLibraryFromFile(session->get_session(),filePath.utf8().get_data(),allowOverwrite,&temp);
    assetId->value = temp;
    return res;
}
HAPI_Result HAPI::LoadAssetLibraryFromMemory(godot::Ref<HESession> session, godot::String buffer, bool allowOverwrite, godot::Ref<Int> assetId){
    int temp;
    std::string buf;
    for(int i = 0,sz = buffer.length();i!=sz;++i){
        buf.push_back((int8_t)buffer[i]);
    }
    auto res = HoudiniApi::LoadAssetLibraryFromMemory(session->get_session(),buf.data(),buf.size(),allowOverwrite,&temp);
    assetId->value = temp;
    return res;
}
HAPI_Result HAPI::LoadGeoFromFile(godot::Ref<HESession> session, int nodeId, godot::String filePath){
    return HoudiniApi::LoadGeoFromFile(session->get_session(),nodeId,filePath.utf8().get_data());
}
HAPI_Result HAPI::LoadGeoFromMemory(godot::Ref<HESession> session, int nodeId, godot::String format, godot::String buffer){
    std::string buf;
    for(int i = 0,sz = buffer.length();i!=sz;++i){
        buf.push_back((int8_t)buffer[i]);
    }
    return HoudiniApi::LoadGeoFromMemory(session->get_session(),nodeId,format.utf8().get_data(),buf.data(),buf.size());
}
HAPI_Result HAPI::LoadHIPFile(godot::Ref<HESession> session, godot::String fileName, bool cookOnLoad){
    return HoudiniApi::LoadHIPFile(session->get_session(),fileName.utf8().get_data(),cookOnLoad);
}
HAPI_Result HAPI::LoadNodeFromFile(godot::Ref<HESession> session, godot::String fileName, int parentId, godot::String nodeLabel, bool cookOnLoad, godot::Ref<Int> newNodeId){
    int temp;
    auto res = HoudiniApi::LoadNodeFromFile(session->get_session(),fileName.utf8().get_data(),parentId,nodeLabel.utf8().get_data(),cookOnLoad,&temp);
    newNodeId->value = temp;
    return res;
}
godot::Dictionary HAPI::MaterialInfo_Create(){
    HAPI_MaterialInfo info = HoudiniApi::MaterialInfo_Create();
    HoudiniApi::MaterialInfo_Init(&info);
    return MaterialInfoToDict(info);
}
godot::Dictionary HAPI::NodeInfo_Create(){
    HAPI_NodeInfo info = HoudiniApi::NodeInfo_Create();
    HoudiniApi::NodeInfo_Init(&info);
    return NodeInfoToDict(info);
}
godot::Dictionary HAPI::ObjectInfo_Create(){
    HAPI_ObjectInfo info = HoudiniApi::ObjectInfo_Create();
    HoudiniApi::ObjectInfo_Init(&info);
    return ObjectInfoToDict(info);
}
godot::Dictionary HAPI::ParmInfo_Create(){
    HAPI_ParmInfo info = HoudiniApi::ParmInfo_Create();
    HoudiniApi::ParmInfo_Init(&info);
    return ParmInfoToDict(info);
}
godot::Dictionary HAPI::PartInfo_Create(){
    HAPI_PartInfo info = HoudiniApi::PartInfo_Create();
    HoudiniApi::PartInfo_Init(&info);
    return PartInfoToDict(info);
}
HAPI_Result HAPI::RemoveCustomString(godot::Ref<HESession> session, int stringHandle){
    return HoudiniApi::RemoveCustomString(session->get_session(),stringHandle);
}
HAPI_Result HAPI::RenameNode(godot::Ref<HESession> session, int nodeId, godot::String newName){
    return HoudiniApi::RenameNode(session->get_session(),nodeId,newName.utf8().get_data());
}
HAPI_Result HAPI::RevertGeo(godot::Ref<HESession> session, int nodeId){
    return HoudiniApi::RevertGeo(session->get_session(),nodeId);
}
HAPI_Result HAPI::RevertParmToDefault(godot::Ref<HESession> session, int nodeId, godot::String parmName, int index){
    return HoudiniApi::RevertParmToDefault(session->get_session(),nodeId,parmName.utf8().get_data(),index);
}
HAPI_Result HAPI::RevertParmToDefaults(godot::Ref<HESession> session, int nodeId, godot::String parmName){
    return HoudiniApi::RevertParmToDefaults(session->get_session(),nodeId,parmName.utf8().get_data());
}
HAPI_Result HAPI::SaveGeoToFile(godot::Ref<HESession> session, int nodeId, godot::String filePath){
    return HoudiniApi::SaveGeoToFile(session->get_session(),nodeId,filePath.utf8().get_data());
}
bool HAPI::SaveToHip(godot::Ref<HESession> session, godot::String filename){
    return HoudiniEngineUtility::saveToHip(session->get_session(), string_cast(filename));
}
HAPI_Result HAPI::SaveNodeToFile(godot::Ref<HESession> session, int nodeId, godot::String fileName){
    return HoudiniApi::SaveNodeToFile(session->get_session(),nodeId,fileName.utf8().get_data());
}
godot::Dictionary HAPI::SessionInfo_Create(){
    HAPI_SessionInfo info = HoudiniApi::SessionInfo_Create();
    HoudiniApi::SessionInfo_Init(&info);
    return SessionInfoToDict(info);
}
HAPI_Result HAPI::SetAttributeFloatData(godot::Ref<HESession> session, int nodeId, int partId, godot::String name, godot::Dictionary attrInfo, godot::Ref<RefArray> dataArray, int start, int length){
    HAPI_AttributeInfo info = DictToAttrInfo(attrInfo);
    std::vector<float> data(dataArray->value.size());
    for(int i = 0,sz = dataArray->value.size();i!=sz;++i){
        data[i] = dataArray->value[i];
    }
    auto res = HoudiniApi::SetAttributeFloatData(session->get_session(),nodeId,partId,name.utf8().get_data(),&info,data.data(),start,length);
    for(int i = 0,sz = dataArray->value.size();i!=sz;++i){
        dataArray->value[i] = data[i];
    }
    return res;
}
HAPI_Result HAPI::SetAttributeIntData(godot::Ref<HESession> session, int nodeId, int partId, godot::String name, godot::Dictionary attrInfo, godot::Ref<RefArray> dataArray, int start, int length){
    HAPI_AttributeInfo info = DictToAttrInfo(attrInfo);
    std::vector<int> data(dataArray->value.size());
    for(int i = 0,sz = dataArray->value.size();i!=sz;++i){
        data[i] = dataArray->value[i];
    }
    auto res = HoudiniApi::SetAttributeIntData(session->get_session(),nodeId,partId,name.utf8().get_data(),&info,data.data(),start,length);
    for(int i = 0,sz = dataArray->value.size();i!=sz;++i){
        dataArray->value[i] = data[i];
    }
    return res;
}
HAPI_Result HAPI::SetAttributeStringData(godot::Ref<HESession> session, int nodeId, int partId, godot::String name, godot::Dictionary attrInfo, godot::Ref<RefArray> dataArray, int start, int length){
    HAPI_AttributeInfo info = DictToAttrInfo(attrInfo);
    std::vector<const char*> data(dataArray->value.size());
    for(int i = 0,sz = dataArray->value.size();i!=sz;++i){
        data[i] = keep_alive_string((godot::String)dataArray->value[i]);
    }
    return HoudiniApi::SetAttributeStringData(session->get_session(),nodeId,partId,name.utf8().get_data(),&info,data.data(),start,length);
}
HAPI_Result HAPI::SetCurveCounts(godot::Ref<HESession> session, int nodeId, int partId, godot::Array curveCounts, int start, int length){
    std::vector<int> data(curveCounts.size());
    for(int i = 0,sz = curveCounts.size();i!=sz;++i){
        data[i] = curveCounts[i];
    }
    auto res = HoudiniApi::SetCurveCounts(session->get_session(),nodeId,partId,data.data(),start,length);
    return res;
}
HAPI_Result HAPI::SetCurveInfo(godot::Ref<HESession> session, int nodeId, int partId, godot::Dictionary curveInfo){
    HAPI_CurveInfo info = DictToCurveInfo(curveInfo);
    return HoudiniApi::SetCurveInfo(session->get_session(),nodeId,partId,&info);
}
HAPI_Result HAPI::SetCurveKnots(godot::Ref<HESession> session, int nodeId, int partId, godot::Array knots, int start, int length){
    std::vector<float> data(knots.size());
    for(int i = 0,sz = knots.size();i!=sz;++i){
        data[i] = knots[i];
    }
    auto res = HoudiniApi::SetCurveKnots(session->get_session(),nodeId,partId,data.data(),start,length);
    return res;
}
HAPI_Result HAPI::SetCurveOrders(godot::Ref<HESession> session, int nodeId, int partId, godot::Array orders, int start, int length){
    std::vector<int> data(orders.size());
    for(int i = 0,sz = orders.size();i!=sz;++i){
        data[i] = orders[i];
    }
    auto res = HoudiniApi::SetCurveOrders(session->get_session(),nodeId,partId,data.data(),start,length);
    return res;
}
HAPI_Result HAPI::SetCustomString(godot::Ref<HESession> session, godot::String string_value, godot::Ref<Int> handle_value){
    int temp;
    auto res = HoudiniApi::SetCustomString(session->get_session(),keep_alive_string(string_value.utf8().get_data()),&temp);
    handle_value->value = temp;
    return res;
}
HAPI_Result HAPI::SetFaceCounts(godot::Ref<HESession> session, int nodeId, int partId, godot::Array faceCounts, int start, int length){
    std::vector<int> data(faceCounts.size());
    for(int i = 0,sz = faceCounts.size();i!=sz;++i){
        data[i] = faceCounts[i];
    }
    auto res = HoudiniApi::SetFaceCounts(session->get_session(),nodeId,partId,data.data(),start,length);
    return res;
}
HAPI_Result HAPI::SetInputCurveInfo(godot::Ref<HESession> session, int nodeId, int partId, godot::Dictionary curveInfo){
    HAPI_InputCurveInfo info = DictToInputCurveInfo(curveInfo);
    return HoudiniApi::SetInputCurveInfo(session->get_session(),nodeId,partId,&info);
}
HAPI_Result HAPI::SetInputCurvePositions(godot::Ref<HESession> session, int nodeId, int partId, godot::Array positions, int start, int length){
    std::vector<float> data(positions.size());
    for(int i = 0,sz = positions.size();i!=sz;++i){
        data[i] = positions[i];
    }
    return HoudiniApi::SetInputCurvePositions(session->get_session(),nodeId,partId,data.data(),start,length);
}
HAPI_Result HAPI::SetNodeDisplay(godot::Ref<HESession> session, int nodeId, int onOff){
    return HoudiniApi::SetNodeDisplay(session->get_session(),nodeId,onOff);
}
HAPI_Result HAPI::SetObjectTransform(godot::Ref<HESession> session, int nodeId, godot::Dictionary transform){
    HAPI_TransformEuler info = DictToTransformEuler(transform);
    return HoudiniApi::SetObjectTransform(session->get_session(),nodeId,&info);
}
HAPI_Result HAPI::SetServerEnvInt(godot::Ref<HESession> session, godot::String envName, int value){
    return HoudiniApi::SetServerEnvInt(session->get_session(), envName.utf8().get_data(), value);
}
HAPI_Result HAPI::SetServerEnvString(godot::Ref<HESession> session, godot::String envName, godot::String value){
    return HoudiniApi::SetServerEnvString(session->get_session(), envName.utf8().get_data(), keep_alive_string(value.utf8().get_data()));
}

HAPI_Result HAPI::SetSessionSync(godot::Ref<HESession> session, bool enable){
    return HoudiniApi::SetSessionSync(session->get_session(), enable);
}
HAPI_Result HAPI::SetSessionSyncInfo(godot::Ref<HESession> session, godot::Dictionary syncInfo){
    HAPI_SessionSyncInfo info = DictToSessionSyncInfo(syncInfo);
    return HoudiniApi::SetSessionSyncInfo(session->get_session(), &info);
}