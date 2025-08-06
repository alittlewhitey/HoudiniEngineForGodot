#ifndef CORE_MOUNTER_H
#define CORE_MOUNTER_H
#include "DebugSymbol.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/editor_plugin.hpp>
#include <godot_cpp/classes/v_box_container.hpp>

#include "Utility.h"
#include "HECenter.h"


class CoreMountPlugin : public godot::EditorPlugin{
    GDCLASS(CoreMountPlugin,godot::EditorPlugin)
    static void _bind_methods(){}
public:
    void _enter_tree() override {
        godot::UtilityFunctions::print("[Houdini Engine] EditorPlugin entered tree, adding HECenter.");

        auto center = HECenter::get_singleton();

        if (!center->is_inside_tree()) {
            get_editor_interface()->get_editor_main_screen()->add_child(center);
        }
    }
    // void _exit_tree() override {
    //     godot::UtilityFunctions::print("[Houdini Engine] EditorPlugin exiting tree, removing HECenter.");
        
    //     auto center = HECenter::get_singleton();
    //     if (center && center->is_inside_tree()) {
    //         center->get_parent()->remove_child(center);
    //     }
    // }
};

#endif