
#ifndef REGISTER_PARTITION_SRC
#define REGISTER_PARTITION_SRC

#include "register_types.h"

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

#include "HEBind.h"
#include "HDAImporter.h"
#include "CoreMounter.h"
#include "HECenter.h"

void initialize_module(ModuleInitializationLevel p_level){
    if (p_level != MODULE_INITIALIZATION_LEVEL_EDITOR) {
        return;
    }
    if(!godot::Engine::get_singleton()->is_editor_hint()){
        return;
    }
    GDREGISTER_CLASS(HDAResource)
    GDREGISTER_CLASS(HDAImporter)
    GDREGISTER_CLASS(HDAImportPlugin)
    GDREGISTER_CLASS(CoreMountPlugin)

    GDREGISTER_CLASS(HECenter)

    EditorPlugins::add_by_type<HDAImportPlugin>();
    EditorPlugins::add_by_type<CoreMountPlugin>();

    GDREGISTER_CLASS(Bool)
    GDREGISTER_CLASS(Int)
    GDREGISTER_CLASS(Float)
    GDREGISTER_CLASS(RefArray)
    GDREGISTER_CLASS(RefDictionary)
    GDREGISTER_CLASS(HAPI)
    GDREGISTER_CLASS(HEBindEnum)
    GDREGISTER_CLASS(HESettings)
    GDREGISTER_CLASS(HEUtil)

    GDREGISTER_ABSTRACT_CLASS(HENode)
    GDREGISTER_CLASS(HEAsset)
    GDREGISTER_CLASS(HESession)
    GDREGISTER_CLASS(HEObjNode)
    GDREGISTER_CLASS(HESopNode)
    GDREGISTER_CLASS(HECopNode)
    GDREGISTER_CLASS(HEShopNode)
    GDREGISTER_CLASS(HEChopNode)
    GDREGISTER_CLASS(HERopNode)
    GDREGISTER_CLASS(HEVopNode)
    GDREGISTER_CLASS(HEDopNode)
    GDREGISTER_CLASS(HETopNode)
    GDREGISTER_CLASS(HEGeometry)
    GDREGISTER_CLASS(HEMesh)
    GDREGISTER_CLASS(HEMultiMesh)
    GDREGISTER_CLASS(HEImage)
    auto center = HECenter::get_singleton();
    auto settings = HESettings::get_singleton();
    Engine::get_singleton()->register_singleton("HECore", center);
    Engine::get_singleton()->register_singleton("HESettings", settings);
}
void uninitialize_module(ModuleInitializationLevel p_level){
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
    if(!godot::Engine::get_singleton()->is_editor_hint()){
        return;
    }
    Engine::get_singleton()->unregister_singleton("HECore");
    Engine::get_singleton()->unregister_singleton("HESettings");
}
extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT init_library(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {

    godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);


    init_obj.register_initializer(initialize_module);
    init_obj.register_terminator(uninitialize_module);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_EDITOR);
    
    return init_obj.init();
}
}

#endif //REGISTER_PARTITION_SRC