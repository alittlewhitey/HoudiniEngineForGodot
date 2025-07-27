#ifndef HOUDINI_ENGINE_SETTINGS
#define HOUDINI_ENGINE_SETTINGS
#include "DebugSymbol.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <HoudiniApi.h>
#include <HoudiniEnginePlatform.h>

#include "Utility.h"
#include "HEClass.h"
class HESettings: public godot::Object{
    GDCLASS(HESettings,godot::Object)
    friend class HECenter;
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("_settings_changed"),&HESettings::_settings_changed);
        godot::ClassDB::bind_method(godot::D_METHOD("set_logFilePath","path"),&HESettings::set_logFilePath);
    }
    void _notification(int what){
        switch(what){
        case NOTIFICATION_POSTINITIALIZE:{
            _init_settings();
            godot::ProjectSettings::get_singleton()->connect("settings_changed",godot::Callable(this,"_settings_changed"));
            _settings_changed();
        }break;
        case NOTIFICATION_PREDELETE:{
            godot::ProjectSettings::get_singleton()->disconnect("settings_changed",godot::Callable(this,"_settings_changed"));
        }break;
        }
    }
    void _init_settings(){
        
        auto tempDic = godot::Dictionary();
        godot::ProjectSettings* settings = godot::ProjectSettings::get_singleton();
        auto addSetting = [&tempDic,settings](godot::String name,godot::Variant value,godot::Variant::Type type,godot::PropertyHint hint = godot::PROPERTY_HINT_NONE,godot::String hint_string = ""){
            if(!settings->has_setting(name)){
                settings->set_setting(name,value);
            }
            tempDic["name"] = name;
            tempDic["type"] = type;
            tempDic["hint"] = hint;
            tempDic["hint_string"] = hint_string;
            settings->add_property_info(tempDic);
            settings->set_initial_value(name,value);
            tempDic.clear();
        };
        addSetting("houdini/config/useEnvLibPath",true,godot::Variant::BOOL);

        settings->set_restart_if_changed("houdini/config/useEnvLibPath",true);
        
        addSetting("houdini/config/hapiLib",HAPILibType::HAPIL,godot::Variant::INT,godot::PROPERTY_HINT_ENUM,"HAPIL:0,HAPI:1");

        settings->set_restart_if_changed("houdini/config/hapiLib",true);

        addSetting("houdini/config/houdiniRootPath","",godot::Variant::STRING,godot::PROPERTY_HINT_GLOBAL_DIR);

        settings->set_restart_if_changed("houdini/config/houdiniRootPath",true);

        addSetting("houdini/config/houdiniLibPath","",godot::Variant::STRING,godot::PROPERTY_HINT_GLOBAL_DIR);
        
        settings->set_restart_if_changed("houdini/config/houdiniLibPath",true);

        addSetting("houdini/config/logFilePath","",godot::Variant::STRING,godot::PROPERTY_HINT_SAVE_FILE);

        addSetting("houdini/config/sessionConfig",get_sessionConfig(),godot::Variant::DICTIONARY);

        addSetting("houdini/config/cookOptions",default_cookOptions(),godot::Variant::DICTIONARY);
    }
    void _settings_changed(){
        using namespace _houdini_engine_log;
        godot::Variant value;
        std::string tempStr;

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/useEnvLibPath");
        if(useEnvLibPath != (bool)value){
            useEnvLibPath = (bool)value;
        }

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/hapiLib");
        if(libType != (HAPILibType)(int)value){
            libType = (HAPILibType)(int)value;
        }

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/houdiniRootPath");
        tempStr = string_cast((godot::String)value);
        if(tempStr != houdiniRootPath){
            set_houdiniRootPath((godot::String)value);
        }

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/houdiniLibPath");
        tempStr = string_cast((godot::String)value);
        if(tempStr != houdiniRootPath){
            set_houdiniLibPath((godot::String)value);
        }

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/logFilePath");
        tempStr = string_cast((godot::String)value);
        if(tempStr != logFilePath){
            set_logFilePath((godot::String)value);
        }

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/sessionConfig");
        set_sessionConfig((godot::Dictionary)value);

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/cookOptions");
        set_cookOptions((godot::Dictionary)value);
    }
    void set_logFilePath(godot::String path){
        using namespace _houdini_engine_log;
        path = godot::ProjectSettings::get_singleton()->globalize_path(path);
        if(path.is_absolute_path()){
            if(!logFilePath.empty()){
                logFile.close();
                logFile.clear();
            }
            logFile.open(string_cast(path),std::ios::app|std::ios::out);
            logFilePath = string_cast(path);
        }else if(path.is_empty()){
            logFile.close();
            logFile.clear();
            logFilePath.clear();
        }
    }

    bool useEnvLibPath = true;
    std::string houdiniRootPath = "";
    void set_houdiniRootPath(godot::String path){
        path = godot::ProjectSettings::get_singleton()->globalize_path(path);
        if(path == ""){
            houdiniRootPath.clear();
            return;
        }
        std::string houdiniPath = string_cast(path);
        std::string hconfigPath = houdiniPath+"/bin/hconfig";
        if(!std::filesystem::exists(hconfigPath)){
            hconfigPath += ".exe";
            if(!std::filesystem::exists(hconfigPath)){
                printError("Invalid houdini root path. Make sure \"${HoudiniRootPath}/bin/hconfig\" is exist.");
                return;
            }
        }
        if(findenv("HFS"))
            return;
        // Add Environment
        std::string cmd = "\"" + hconfigPath + "\"";
        std::string output = exec_output(cmd.c_str());
        if(output.empty()){
            printError("Houdini environment is null");
            return;
        }
        std::istringstream iss(output);
        std::string envLine,envKey,envValue,temp;
        while(std::getline(iss,envLine)){
            if(!iss)
                break;
            std::istringstream iss2(envLine);
            iss2 >> envKey >> temp >> envValue;
            if(envKey.empty()||envValue.empty())
                continue;
            if(envValue[0] == '\'')
                envValue.erase(0,1);
            if(envValue[envValue.size()-1] == '\'')
                envValue.erase(envValue.size()-1,1);
            addenv(envKey,envValue);
        }
        houdiniRootPath = houdiniPath;
        if(useEnvLibPath)
            initHoudini();
    }

    std::string houdiniLibPath = "";
    void set_houdiniLibPath(godot::String path){
        path = godot::ProjectSettings::get_singleton()->globalize_path(path);
        if(path == ""){
            houdiniLibPath.clear();
            return;
        }
        std::string houdiniPath = string_cast(path);
        if(!std::filesystem::exists(houdiniPath)){
            printError("Invalid houdini lib path. Make sure the dir exists.");
            return;
        }
        houdiniLibPath = houdiniPath;
        if(!useEnvLibPath)
            initHoudini();
    }
    void initHoudini(){
        if(putenv((char*)"HAPI_CLIENT_NAME=godot")){
            printWarning("Failed to change env \"HAPI_CLIENT_NAME\" to \"godot\".\n");
        }
        if(useEnvLibPath)
            hapiLib = HoudiniEnginePlatform::LoadLibHAPIL(libType == HAPI);
        else 
            hapiLib = HoudiniEnginePlatform::LoadLibHAPIL(libType == HAPI, houdiniLibPath);
        if(hapiLib != nullptr){
            HoudiniApi::InitializeHAPI(hapiLib);
        }else{
            printError("Failed to initialize hapi");
        }
        if(!HoudiniApi::IsHAPIInitialized()){
            if(libType == HAPIL){
                printError("Failed to load and initialize the "
                        "Houdini Engine API from libHAPIL.\n");
            }else{
                printError("Failed to load and initialize the "
                        "Houdini Engine API from libHAPI.\n");  
            }
        }
    }
    void set_sessionConfig(godot::Dictionary config){
        if(config.has("namedPipe"))
            sessionConfig.namedPipe = string_cast(static_cast<godot::String>(config["namedPipe"]));
        if(config.has("hostName"))
            sessionConfig.hostName = string_cast(static_cast<godot::String>(config["hostName"]));
        if(config.has("sharedMemoryName"))
            sessionConfig.sharedMemoryName = string_cast(static_cast<godot::String>(config["sharedMemoryName"]));
        if(config.has("tcpPort"))
            sessionConfig.tcpPort = (int)config["tcpPort"];
        if(config.has("newSessionTimeoutSec"))
            sessionConfig.newSessionTimeoutSec = (int)config["newSessionTimeoutSec"];
    }
    godot::Dictionary get_sessionConfig(){
        godot::Dictionary dic;
        dic["namedPipe"] = string_cast(sessionConfig.namedPipe);
        dic["hostName"] = string_cast(sessionConfig.hostName);
        dic["sharedMemoryName"] = string_cast(sessionConfig.sharedMemoryName);
        dic["tcpPort"] = sessionConfig.tcpPort;
        dic["newSessionTimeoutSec"] = sessionConfig.newSessionTimeoutSec;
        return dic;
    }
    godot::Dictionary default_cookOptions(){
        #ifdef HE_DEBUG_MODE
        #warning "Be careful of cook options"
        #endif
        HAPI_CookOptions cookOptions = HoudiniApi::CookOptions_Create();
        cookOptions.curveRefineLOD = 8.0f;
        cookOptions.clearErrorsAndWarnings = false;
        cookOptions.maxVerticesPerPrimitive = 3;
        cookOptions.splitGeosByGroup = false;
        cookOptions.refineCurveToLinear = true;
        cookOptions.handleBoxPartTypes = false;
        cookOptions.handleSpherePartTypes = false;
        cookOptions.splitPointsByVertexAttributes = false;
        cookOptions.packedPrimInstancingMode = HAPI_PACKEDPRIM_INSTANCING_MODE_FLAT;
        cookOptions.preferOutputNodes = false;
        this->cookOptions = cookOptions;
        return get_cookOptions();
    }
    godot::Dictionary get_cookOptions();
    void set_cookOptions(godot::Dictionary options);

    void* hapiLib = nullptr;
public:
    //Datas:

    static HESettings* get_singleton(){
        static HESettings* singleton = nullptr;
        if(!singleton)
            singleton = memnew(HESettings());
        return singleton;
    }
    enum HAPILibType{
        HAPIL = 0,
        HAPI = 1
    } libType = HAPIL;
    constexpr static const char* DefaultNamedPipe = "hapi";
    constexpr static const char* DefaultSharedMemoryName = "hapi";
    constexpr static const char* DefaultHostName = "127.0.0.1";
    constexpr static int DefaultTcpPort = 9090;
    constexpr static int DefaultNewSessionTimeoutSec = 120;
    constexpr static std::chrono::milliseconds defaultFreeTimeout = std::chrono::milliseconds(100);
    struct {
        std::string namedPipe = DefaultNamedPipe;
        std::string hostName = DefaultHostName;
        std::string sharedMemoryName = DefaultSharedMemoryName;
        int tcpPort = DefaultTcpPort;
        int newSessionTimeoutSec = DefaultNewSessionTimeoutSec;
    } sessionConfig;
    HAPI_CookOptions cookOptions;
    bool autoCook = 1;
    bool useCookingThread = true;
    



    void _update_settings(){
        using namespace _houdini_engine_log;
        godot::Variant value;
        godot::String tempStr;

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/useEnvLibPath");
        if(useEnvLibPath != (bool)value){
            godot::ProjectSettings::get_singleton()->set_setting("houdini/config/useEnvLibPath",useEnvLibPath);
        }

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/hapiLib");
        if(libType != (HAPILibType)(int)value){
            godot::ProjectSettings::get_singleton()->set_setting("houdini/config/hapiLib",libType);
        }

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/houdiniRootDirPath");
        tempStr = string_cast(houdiniRootPath);
        if(tempStr != value){
            godot::ProjectSettings::get_singleton()->set_setting("houdini/config/houdiniRootDirPath",tempStr);
        }

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/houdiniLibDirPath");
        tempStr = string_cast(houdiniLibPath);
        if(tempStr != value){
            godot::ProjectSettings::get_singleton()->set_setting("houdini/config/houdiniLibDirPath",tempStr);
        }

        value = godot::ProjectSettings::get_singleton()->get_setting("houdini/config/logFilePath");
        tempStr = string_cast(logFilePath);
        if(tempStr != value){
            set_logFilePath((godot::String)value);
            godot::ProjectSettings::get_singleton()->set_setting("houdini/config/logFilePath",tempStr);
        }
    }
};
#endif