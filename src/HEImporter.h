
#ifndef HDA_IMPORTER
#define HDA_IMPORTER
#include "DebugSymbol.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/resource_saver.hpp>
#include <godot_cpp/classes/editor_import_plugin.hpp>
#include <godot_cpp/classes/editor_plugin.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "Utility.h"
class HDAResource : public godot::Resource{
    GDCLASS(HDAResource,godot::Resource)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("get_path"),&HDAResource::get_path);
        godot::ClassDB::bind_method(godot::D_METHOD("set_path","path"),&HDAResource::set_path);
        godot::ClassDB::add_property("HDAResource",godot::PropertyInfo(godot::Variant::STRING,"path",godot::PROPERTY_HINT_FILE,"*.hda,*.otl"),"set_path","get_path");
        godot::ClassDB::bind_method(godot::D_METHOD("get_assetId"),&HDAResource::get_assetId);
        godot::ClassDB::bind_method(godot::D_METHOD("set_assetId","assetId"),&HDAResource::set_assetId);
        godot::ClassDB::add_property("HDAResource",godot::PropertyInfo(godot::Variant::INT,"assetId"),"set_assetId","get_assetId");
    }
    godot::String get_path(){
        return godot::String::utf8(path.c_str());
    }
    void set_path(godot::String s){
        path = s.utf8().get_data();
    }
    int get_assetId(){
        return assetId;
    }
    void set_assetId(int assetId){
        //this->assetId = assetId;
        godot::UtilityFunctions::push_warning("Forbitten change assetId by inspector for safe");
    }
public:
    std::string path;
    // If id is -1, this instance is inactive.
    int assetId = -1;
};
class HDAImporter : public godot::EditorImportPlugin{
    GDCLASS(HDAImporter,godot::EditorImportPlugin)
    static void _bind_methods(){}
public:
    godot::String _get_importer_name()const override{
        return "littlewhite.houdini_digital_assets";
    }
    godot::String _get_visible_name()const override{
        return "Houdini Digital Assets";
    }
    godot::PackedStringArray _get_recognized_extensions()const override{
        godot::PackedStringArray arr;
        arr.push_back("hda");
        arr.push_back("otl");
        arr.push_back("hdalc");
        return arr;
    }
    godot::String _get_save_extension()const override{
        return "res";
    }
    godot::String _get_resource_type()const override{
        return "HDAResource";
    }
    enum Presets{
        PresetSize
    };
    int32_t _get_preset_count()const override{
        return PresetSize;
    }
    float _get_priority()const override{
        return 1.0;
    }
    int32_t _get_import_order()const override{
        return 1;
    }
    godot::String _get_preset_name(int32_t p_preset_index)const override{
        switch((Presets)p_preset_index){
        default:{
            return "Unknown";
        }break;
        }
    }
    godot::TypedArray<godot::Dictionary> _get_import_options(const godot::String &p_path, int32_t p_preset_index)const override{
        return {};
    }
    bool _get_option_visibility(const godot::String &p_path, const godot::StringName &p_option_name, const godot::Dictionary &p_options)const override{
        return true;
    }
    godot::Error _import(const godot::String &p_source_file, const godot::String &p_save_path, const godot::Dictionary &p_options, const godot::TypedArray<godot::String> &p_platform_variants, const godot::TypedArray<godot::String> &p_gen_files)const override{
        godot::Ref<HDAResource> res;
        res.instantiate();
        res->path = godot::ProjectSettings::get_singleton()->globalize_path(p_source_file).utf8().get_data();
        std::string save_path = p_save_path.utf8().get_data();
        std::string all_path = save_path+"."+_get_save_extension().utf8().get_data();
        return godot::ResourceSaver::get_singleton()->save(res,godot::String::utf8(all_path.c_str()));
    }
};
class HIPResource : public godot::Resource{
    GDCLASS(HIPResource,godot::Resource)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("get_path"),&HIPResource::get_path);
        godot::ClassDB::bind_method(godot::D_METHOD("set_path","path"),&HIPResource::set_path);
        godot::ClassDB::add_property("HIPResource",godot::PropertyInfo(godot::Variant::STRING,"path",godot::PROPERTY_HINT_FILE,"*.hip"),"set_path","get_path");
    }
    godot::String get_path(){
        return godot::String::utf8(path.c_str());
    }
    void set_path(godot::String s){
        path = s.utf8().get_data();
    }
public:
    std::string path;
};
class HIPImporter : public godot::EditorImportPlugin{
    GDCLASS(HIPImporter,godot::EditorImportPlugin)
    static void _bind_methods(){}
public:
    godot::String _get_importer_name()const override{
        return "littlewhite.houdini_scene_file";
    }
    godot::String _get_visible_name()const override{
        return "Houdini Scene File";
    }
    godot::PackedStringArray _get_recognized_extensions()const override{
        godot::PackedStringArray arr;
        arr.push_back("hip");
        return arr;
    }
    godot::String _get_save_extension()const override{
        return "res";
    }
    godot::String _get_resource_type()const override{
        return "HIPResource";
    }
    enum Presets{
        PresetSize
    };
    int32_t _get_preset_count()const override{
        return PresetSize;
    }
    float _get_priority()const override{
        return 1.0;
    }
    int32_t _get_import_order()const override{
        return 1;
    }
    godot::String _get_preset_name(int32_t p_preset_index)const override{
        switch((Presets)p_preset_index){
        default:{
            return "Unknown";
        }break;
        }
    }
    godot::TypedArray<godot::Dictionary> _get_import_options(const godot::String &p_path, int32_t p_preset_index)const override{
        return {};
    }
    bool _get_option_visibility(const godot::String &p_path, const godot::StringName &p_option_name, const godot::Dictionary &p_options)const override{
        return true;
    }
    godot::Error _import(const godot::String &p_source_file, const godot::String &p_save_path, const godot::Dictionary &p_options, const godot::TypedArray<godot::String> &p_platform_variants, const godot::TypedArray<godot::String> &p_gen_files)const override{
        godot::Ref<HIPResource> res;
        res.instantiate();
        res->path = godot::ProjectSettings::get_singleton()->globalize_path(p_source_file).utf8().get_data();
        std::string save_path = p_save_path.utf8().get_data();
        std::string all_path = save_path+"."+_get_save_extension().utf8().get_data();
        return godot::ResourceSaver::get_singleton()->save(res,godot::String::utf8(all_path.c_str()));
    }
};
class HEImportPlugin : public godot::EditorPlugin{
    GDCLASS(HEImportPlugin,godot::EditorPlugin)
    static void _bind_methods(){}
    godot::Ref<HDAImporter> HDA_importer;
    godot::Ref<HIPImporter> HIP_importer;
public:
    virtual void _enter_tree()override{
        HDA_importer.instantiate();
        HIP_importer.instantiate();
        add_import_plugin(HDA_importer);
        add_import_plugin(HIP_importer);
    }
    virtual void _exit_tree()override{
        remove_import_plugin(HDA_importer);
        remove_import_plugin(HIP_importer);
    }
};

#endif //!HDA_IMPORTER