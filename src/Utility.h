#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <set>
#include <source_location>
#include <godot_cpp/variant/utility_functions.hpp>
namespace _houdini_engine_log{
    inline std::string logFilePath = "";
    inline std::ofstream logFile;
}
enum class _godot_msg_type{
    log,warning,error
};
template <typename ...T>
GDE_EXPORT
void _output_log(T... output){
    using namespace _houdini_engine_log;
    if(!logFilePath.empty()){
        logFile << " ### " << std::chrono::system_clock::now() << " ### \n";
        int _[] = {((logFile << output),0)...};
        logFile << std::endl;
    }
}
template <typename ...T>
GDE_EXPORT
void _print_godot_msg(_godot_msg_type type, T... output){
    switch (type)
    {
    case _godot_msg_type::log:
        godot::UtilityFunctions::print(output...);
        break;
    case _godot_msg_type::warning:
        godot::UtilityFunctions::push_warning(output...);
        break;
    case _godot_msg_type::error:
        godot::UtilityFunctions::push_error(output...);
        break;
    default:
        break;
    }
}
#define printFile(...) do{std::source_location _houdini_engine_source_loc = std::source_location::current();_output_log("\nFile: ",_houdini_engine_source_loc.file_name(),"(",_houdini_engine_source_loc.line(),":",_houdini_engine_source_loc.column(),") `",_houdini_engine_source_loc.function_name(),"`: \n",__VA_ARGS__);}while(0)
#define printLog(...) do{std::source_location _houdini_engine_source_loc = std::source_location::current();_output_log("\nFile: ",_houdini_engine_source_loc.file_name(),"(",_houdini_engine_source_loc.line(),":",_houdini_engine_source_loc.column(),") `",_houdini_engine_source_loc.function_name(),"`: \n",__VA_ARGS__);_print_godot_msg(_godot_msg_type::log,"\nFile: ",_houdini_engine_source_loc.file_name(),"(",_houdini_engine_source_loc.line(),":",_houdini_engine_source_loc.column(),") `",_houdini_engine_source_loc.function_name(),"`: \n",__VA_ARGS__);}while(0)
#define printWarning(...) do{std::source_location _houdini_engine_source_loc = std::source_location::current();_output_log("\nFile: ",_houdini_engine_source_loc.file_name(),"(",_houdini_engine_source_loc.line(),":",_houdini_engine_source_loc.column(),") `",_houdini_engine_source_loc.function_name(),"`: \n",__VA_ARGS__);_print_godot_msg(_godot_msg_type::warning,"\nFile: ",_houdini_engine_source_loc.file_name(),"(",_houdini_engine_source_loc.line(),":",_houdini_engine_source_loc.column(),") `",_houdini_engine_source_loc.function_name(),"`: \n",__VA_ARGS__);}while(0)
#define printError(...) do{std::source_location _houdini_engine_source_loc = std::source_location::current();_output_log("\nFile: ",_houdini_engine_source_loc.file_name(),"(",_houdini_engine_source_loc.line(),":",_houdini_engine_source_loc.column(),") `",_houdini_engine_source_loc.function_name(),"`: \n",__VA_ARGS__);_print_godot_msg(_godot_msg_type::error,"\nFile: ",_houdini_engine_source_loc.file_name(),"(",_houdini_engine_source_loc.line(),":",_houdini_engine_source_loc.column(),") `",_houdini_engine_source_loc.function_name(),"`: \n",__VA_ARGS__);}while(0)

GDE_EXPORT 
inline godot::String string_cast(std::string s){
    return godot::String::utf8(s.c_str());
}
GDE_EXPORT 
inline std::string string_cast(godot::String s){
    return s.utf8().get_data();
}

static inline std::set<std::string> _houdini_endine_string_buffer;
GDE_EXPORT
inline const char* keep_alive_string(std::string s){
    return _houdini_endine_string_buffer.insert(std::move(s)).first->c_str();
}
GDE_EXPORT
inline const char* keep_alive_string(const char* s){
    return keep_alive_string(std::string(s));
}
GDE_EXPORT
inline const char* keep_alive_string(godot::String s){
    return keep_alive_string(string_cast(s));
}
#if defined (__linux__) || (defined (__APPLE__) && defined (__MACH__))
GDE_EXPORT
inline bool execute(std::string cmd){
    if(std::system(cmd.c_str()) == -1){
        return false;
    }

    return true;
}
#elif defined (_WIN32) || defined (WIN32)
GDE_EXPORT

inline bool execute(std::string cmd){
    if(std::system(cmd.c_str()) == -1){
        if(std::system(("./"+cmd).c_str()) == -1){
            if(std::system(("./"+cmd+".exe").c_str()) == -1){
                return false;
            }
        }
    }
    return true;
}
#endif

#if defined (__linux__) || (defined (__APPLE__) && defined (__MACH__))
GDE_EXPORT
inline std::string exec_output(std::string cmd , std::string find = "") {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen((cmd + (find.empty()?"":(" | grep " + find + " | grep -v grep"))).c_str(), "r"), pclose);
    if (!pipe) return "";
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }
    return result;
}
#elif defined (_WIN32) || defined (WIN32)
GDE_EXPORT
inline std::string exec_output(std::string cmd , std::string find = ""){
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(_popen((cmd + (find.empty()?"":(" | findstr \"" + find + "\" | findstr /V findstr"))).c_str(), "r"), _pclose);
    if (!pipe) return "";
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }
    return result;
}
#endif

#if defined (__linux__) || (defined (__APPLE__) && defined (__MACH__))
GDE_EXPORT
inline bool findproc(std::string procName){
    return !exec_output("ps -ef",procName).empty();
}
#elif defined (_WIN32) || defined (WIN32)
GDE_EXPORT
inline bool findproc(std::string procName){
    return !exec_output("tasklist",procName).empty();
}
#endif
GDE_EXPORT
inline bool findenv(std::string key){
    return getenv(key.c_str()) != nullptr;
}
#if defined (__linux__)
GDE_EXPORT
inline void addenv(std::string key,std::string value){
    putenv(keep_alive_string(key+"="+value));
}
#elif defined (_WIN32) || defined (WIN32)
GDE_EXPORT
inline void addenv(std::string key,std::string value){
    _putenv(keep_alive_string(key+"="+value));
}
#elif defined (__APPLE__) && defined (__MACH__)
GDE_EXPORT
inline void addenv(std::string key,std::string value){
    setenv(keep_alive_string(key),keep_alive_string(value),1);
}
#endif
