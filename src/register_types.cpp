
#ifndef REGISTER_PARTITION_SRC
#define REGISTER_PARTITION_SRC

#define ENABLE_HOT_RELOAD


#include "register_types.h"


#include <iostream>
#include <cstring>
#include <gdextension_interface.h>
#include <godot_cpp/classes/editor_plugin_registration.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/editor_plugin.hpp>
#include <godot_cpp/classes/project_settings.hpp>

#include "HoudiniBind.h"
#include "HDAImporter.h"

void initialize_module(ModuleInitializationLevel p_level){
    if (p_level != MODULE_INITIALIZATION_LEVEL_EDITOR) {
        return;
    }
    // GDREGISTER_CLASS(EditorPlugin)
    // GDREGISTER_CLASS(EditorImportPlugin)

    GDREGISTER_CLASS(NodeId)
    GDREGISTER_CLASS(PartId)

    GDREGISTER_ABSTRACT_CLASS(Action)
    GDREGISTER_ABSTRACT_CLASS(SessionAction)
    GDREGISTER_ABSTRACT_CLASS(AssetAction)
    GDREGISTER_ABSTRACT_CLASS(NodeAction)
    GDREGISTER_CLASS(CookSessionAction)
    GDREGISTER_CLASS(StartSessionAction)
    GDREGISTER_CLASS(StopSessionAction)
    GDREGISTER_CLASS(CookAssetAction)
    GDREGISTER_CLASS(LoadAssetAction)
    GDREGISTER_CLASS(LoadInputNodeAction)
    GDREGISTER_CLASS(CookNodeAction)
    GDREGISTER_CLASS(DeleteNodeAction)
    GDREGISTER_CLASS(CreateMeshAction)

    GDREGISTER_CLASS(HoudiniEngineManager)

    GDREGISTER_CLASS(HDANode)
    
    GDREGISTER_CLASS(HDAResource)
    GDREGISTER_CLASS(HDAImporter)
    GDREGISTER_CLASS(HDAImportPlugin)

    EditorPlugins::add_by_type<HDAImportPlugin>();
}
void uninitialize_module(ModuleInitializationLevel p_level){
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

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