#include "HEGRegister.h"

#include <iostream>
#include <cstring>
#include <gdextension_interface.h>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/classes/window.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/editor_plugin.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/editor_plugin_registration.hpp>

#include "HEGHapi.h"
#include "HEGImporter.h"
#include "HEGCoreMounter.h"
#include "HEGCenter.h"

void initialize_module(godot::ModuleInitializationLevel p_level){
    if (p_level != godot::MODULE_INITIALIZATION_LEVEL_EDITOR) {
        return;
    }
    if(!godot::Engine::get_singleton()->is_editor_hint()){
        return;
    }
    GDREGISTER_CLASS(HEGHdaResource)
    GDREGISTER_CLASS(HEGHdaImporter)
    GDREGISTER_CLASS(HEGHipResource)
    GDREGISTER_CLASS(HEGHipImporter)
    GDREGISTER_CLASS(HEGImportPlugin)
    GDREGISTER_CLASS(HEGCoreMountPlugin)

    GDREGISTER_CLASS(HEGCenter)

    godot::EditorPlugins::add_by_type<HEGImportPlugin>();
    godot::EditorPlugins::add_by_type<HEGCoreMountPlugin>();

    GDREGISTER_CLASS(HEGBool)
    GDREGISTER_CLASS(HEGInt)
    GDREGISTER_CLASS(HEGFloat)
    GDREGISTER_CLASS(HEGRefArray)
    GDREGISTER_CLASS(HEGRefDictionary)
    GDREGISTER_CLASS(HEGHapi)
    GDREGISTER_CLASS(HEGEnum)
    GDREGISTER_CLASS(HEGSettings)
    GDREGISTER_CLASS(HEGUtil)

    GDREGISTER_ABSTRACT_CLASS(HEGNode)
    GDREGISTER_CLASS(HEGAsset)
    GDREGISTER_CLASS(HEGSession)
    GDREGISTER_CLASS(HEGObjNode)
    GDREGISTER_CLASS(HEGSopNode)
    GDREGISTER_CLASS(HEGCopNode)
    GDREGISTER_CLASS(HEGShopNode)
    GDREGISTER_CLASS(HEGChopNode)
    GDREGISTER_CLASS(HEGRopNode)
    GDREGISTER_CLASS(HEGVopNode)
    GDREGISTER_CLASS(HEGDopNode)
    GDREGISTER_CLASS(HEGTopNode)
    GDREGISTER_CLASS(HEGGeometry)
    GDREGISTER_CLASS(HEGMesh)
    GDREGISTER_CLASS(HEGMultiMesh)
    GDREGISTER_CLASS(HEGCurve)
    GDREGISTER_CLASS(HEGImage)
    auto center = HEGCenter::get_singleton();
    auto settings = HEGSettings::get_singleton();
    godot::Engine::get_singleton()->register_singleton("HEGCenter", center);
    godot::Engine::get_singleton()->register_singleton("HEGSettings", settings);
}
void uninitialize_module(godot::ModuleInitializationLevel p_level){
    if (p_level != godot::MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
    if(!godot::Engine::get_singleton()->is_editor_hint()){
        return;
    }
    godot::Engine::get_singleton()->unregister_singleton("HEGCenter");
    godot::Engine::get_singleton()->unregister_singleton("HEGSettings");
}
extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT init_library(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {

    godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);


    init_obj.register_initializer(initialize_module);
    init_obj.register_terminator(uninitialize_module);
    init_obj.set_minimum_library_initialization_level(godot::MODULE_INITIALIZATION_LEVEL_EDITOR);
    
    return init_obj.init();
}
}
