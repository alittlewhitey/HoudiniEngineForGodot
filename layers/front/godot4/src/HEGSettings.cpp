#include"HEGSettings.h"
#include"HEGCenter.h"
void HEGSettings::_notification(int what){
    switch(what){
    case NOTIFICATION_POSTINITIALIZE:{
        godot::ProjectSettings::get_singleton()->connect("settings_changed",godot::Callable(this,"_settings_changed"));
        HEGCenter::get_singleton()->connect("SessionStarted",godot::Callable(this,"_init"));
        _init_settings();
        _settings_changed();
    }break;
    case NOTIFICATION_PREDELETE:{
        godot::ProjectSettings::get_singleton()->disconnect("settings_changed",godot::Callable(this,"_settings_changed"));
    }break;
    }
}
godot::Dictionary HEGSettings::get_cookOptions(){
    HEGCenter* core = HEGCenter::get_singleton();
    if(!core->ensureSession()){
        return {};
    }
    godot::Dictionary dic;
    dic["splitGeosByGroup"] = cookOptions.splitGeosByGroup;
    dic["splitGroup"] = string_cast(core->getString(cookOptions.splitGroupSH));
    dic["splitGeosByAttribute"] = cookOptions.splitGeosByAttribute;
    dic["splitAttr"] = string_cast(core->getString(cookOptions.splitAttrSH));
    dic["maxVerticesPerPrimitive"] = cookOptions.maxVerticesPerPrimitive;
    dic["refineCurveToLinear"] = cookOptions.refineCurveToLinear;
    dic["curveRefineLOD"] = cookOptions.curveRefineLOD;
    dic["clearErrorsAndWarnings"] = cookOptions.clearErrorsAndWarnings;
    dic["cookTemplatedGeos"] = cookOptions.cookTemplatedGeos;
    dic["splitPointsByVertexAttributes"] = cookOptions.splitPointsByVertexAttributes;
    dic["packedPrimInstancingMode"] = cookOptions.packedPrimInstancingMode;
    dic["handleBoxPartTypes"] = cookOptions.handleBoxPartTypes;
    dic["handleSpherePartTypes"] = cookOptions.handleSpherePartTypes;
    dic["checkPartChanges"] = cookOptions.checkPartChanges;
    dic["cacheMeshTopology"] = cookOptions.cacheMeshTopology;
    dic["preferOutputNodes"] = cookOptions.preferOutputNodes;
    return dic;
}
void HEGSettings::set_cookOptions(godot::Dictionary options){
    HEGCenter* core = HEGCenter::get_singleton();
    if(!core->ensureSession()){
        return;
    }
    if(options.has("splitGeosByGroup")){
        cookOptions.splitGeosByGroup = (bool)options["splitGeosByGroup"];
    }
    if(options.has("splitGroup")){
        cookOptions.splitGroupSH = core->addString(string_cast((godot::String)options["splitGroup"]));
    }
    if(options.has("splitGeosByAttribute")){
        cookOptions.splitGeosByAttribute = (bool)options["splitGeosByAttribute"];
    }
    if(options.has("splitAttr")){
        cookOptions.splitAttrSH = core->addString(string_cast((godot::String)options["splitAttr"]));
    }
    if(options.has("maxVerticesPerPrimitive")){
        cookOptions.maxVerticesPerPrimitive = (int)options["maxVerticesPerPrimitive"];
    }
    if(options.has("refineCurveToLinear")){
        cookOptions.refineCurveToLinear = (bool)options["refineCurveToLinear"];
    }
    if(options.has("curveRefineLOD")){
        cookOptions.curveRefineLOD = (float)options["curveRefineLOD"];
    }
    if(options.has("clearErrorsAndWarnings")){
        cookOptions.clearErrorsAndWarnings = (bool)options["clearErrorsAndWarnings"];
    }
    if(options.has("cookTemplatedGeos")){
        cookOptions.cookTemplatedGeos = (bool)options["cookTemplatedGeos"];
    }
    if(options.has("splitPointsByVertexAttributes")){
        cookOptions.splitPointsByVertexAttributes = (bool)options["splitPointsByVertexAttributes"];
    }
    if(options.has("packedPrimInstancingMode")){
        cookOptions.packedPrimInstancingMode = (HAPI_PackedPrimInstancingMode)(int)options["packedPrimInstancingMode"];
    }
    if(options.has("handleBoxPartTypes")){
        cookOptions.handleBoxPartTypes = (bool)options["handleBoxPartTypes"];
    }
    if(options.has("handleSpherePartTypes")){
        cookOptions.handleSpherePartTypes = (bool)options["handleSpherePartTypes"];
    }
    if(options.has("checkPartChanges")){
        cookOptions.checkPartChanges = (bool)options["checkPartChanges"];
    }
    if(options.has("cacheMeshTopology")){
        cookOptions.cacheMeshTopology = (bool)options["cacheMeshTopology"];
    }
    if(options.has("preferOutputNodes")){
        cookOptions.preferOutputNodes = (bool)options["preferOutputNodes"];
    }

    if(autoCook)
        HEGCenter::get_singleton()->cookSession();
}