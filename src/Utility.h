#ifndef HOUDINI_ENGINE_UTILITY
#define HOUDINI_ENGINE_UTILITY
#include "DebugSymbol.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <format>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <set>
#include <filesystem>
#include <source_location>
#include <godot_cpp/variant/utility_functions.hpp>
#if defined (_WIN32) || defined (WIN32)
#include <windows.h>
#undef GetGeoInfo
#elif defined (__linux__) || (defined (__APPLE__) && defined (__MACH__))
#include <dlfcn.h>
#endif
namespace _houdini_engine_log{
    inline std::string logFilePath = "";
    inline std::ofstream logFile;
}
enum class _godot_msg_type{
    log,warning,error
};
template <typename ...T>
void _output_log(T... output){
    using namespace _houdini_engine_log;
    if(!logFilePath.empty()){
        logFile.open(logFilePath,std::ios::app|std::ios::out);
        logFile << std::format("### {0:%F} {0:%R%z} ###\n",std::chrono::system_clock::now());
        int _[] = {((logFile << output),0)...};
        logFile << std::endl;
        logFile.close();
    }
}
template <typename ...T>
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
#ifdef HE_DEBUG_MODE
#define printFile(...) do{std::source_location _houdini_engine_source_loc = std::source_location::current();_output_log("\nFile: ",_houdini_engine_source_loc.file_name(),"(",_houdini_engine_source_loc.line(),":",_houdini_engine_source_loc.column(),") `",_houdini_engine_source_loc.function_name(),"`: \n",__VA_ARGS__);}while(0)
#define printLog(...) do{std::source_location _houdini_engine_source_loc = std::source_location::current();_output_log("\nFile: ",_houdini_engine_source_loc.file_name(),"(",_houdini_engine_source_loc.line(),":",_houdini_engine_source_loc.column(),") `",_houdini_engine_source_loc.function_name(),"`: \n",__VA_ARGS__);_print_godot_msg(_godot_msg_type::log,"\nFile: ",_houdini_engine_source_loc.file_name(),"(",_houdini_engine_source_loc.line(),":",_houdini_engine_source_loc.column(),") `",_houdini_engine_source_loc.function_name(),"`: \n",__VA_ARGS__);}while(0)
#define printWarning(...) do{std::source_location _houdini_engine_source_loc = std::source_location::current();_output_log("\nFile: ",_houdini_engine_source_loc.file_name(),"(",_houdini_engine_source_loc.line(),":",_houdini_engine_source_loc.column(),") `",_houdini_engine_source_loc.function_name(),"`: \n",__VA_ARGS__);_print_godot_msg(_godot_msg_type::warning,"\nFile: ",_houdini_engine_source_loc.file_name(),"(",_houdini_engine_source_loc.line(),":",_houdini_engine_source_loc.column(),") `",_houdini_engine_source_loc.function_name(),"`: \n",__VA_ARGS__);}while(0)
#define printError(...) do{std::source_location _houdini_engine_source_loc = std::source_location::current();_output_log("\nFile: ",_houdini_engine_source_loc.file_name(),"(",_houdini_engine_source_loc.line(),":",_houdini_engine_source_loc.column(),") `",_houdini_engine_source_loc.function_name(),"`: \n",__VA_ARGS__);_print_godot_msg(_godot_msg_type::error,"\nFile: ",_houdini_engine_source_loc.file_name(),"(",_houdini_engine_source_loc.line(),":",_houdini_engine_source_loc.column(),") `",_houdini_engine_source_loc.function_name(),"`: \n",__VA_ARGS__);}while(0)
#else
#define printFile(...) do{std::source_location _houdini_engine_source_loc = std::source_location::current();_output_log("\nFile: ",_houdini_engine_source_loc.file_name(),"(",_houdini_engine_source_loc.line(),":",_houdini_engine_source_loc.column(),") `",_houdini_engine_source_loc.function_name(),"`: \n",__VA_ARGS__);}while(0)
#define printLog(...) do{std::source_location _houdini_engine_source_loc = std::source_location::current();_output_log("\nFile: ",_houdini_engine_source_loc.file_name(),"(",_houdini_engine_source_loc.line(),":",_houdini_engine_source_loc.column(),") `",_houdini_engine_source_loc.function_name(),"`: \n",__VA_ARGS__);_print_godot_msg(_godot_msg_type::log,"\n",__VA_ARGS__);}while(0)
#define printWarning(...) do{std::source_location _houdini_engine_source_loc = std::source_location::current();_output_log("\nFile: ",_houdini_engine_source_loc.file_name(),"(",_houdini_engine_source_loc.line(),":",_houdini_engine_source_loc.column(),") `",_houdini_engine_source_loc.function_name(),"`: \n",__VA_ARGS__);_print_godot_msg(_godot_msg_type::warning,"\n",__VA_ARGS__);}while(0)
#define printError(...) do{std::source_location _houdini_engine_source_loc = std::source_location::current();_output_log("\nFile: ",_houdini_engine_source_loc.file_name(),"(",_houdini_engine_source_loc.line(),":",_houdini_engine_source_loc.column(),") `",_houdini_engine_source_loc.function_name(),"`: \n",__VA_ARGS__);_print_godot_msg(_godot_msg_type::error,"\n",__VA_ARGS__);}while(0)
#endif
inline godot::String string_cast(std::string s){
    return godot::String::utf8(s.c_str());
}
inline std::string string_cast(godot::String s){
    return s.utf8().get_data();
}

static inline std::set<std::string> _houdini_endine_string_buffer;
inline const char* keep_alive_string(std::string s){
    return _houdini_endine_string_buffer.insert(std::move(s)).first->c_str();
}
inline const char* keep_alive_string(const char* s){
    return keep_alive_string(std::string(s));
}
inline const char* keep_alive_string(godot::String s){
    return keep_alive_string(string_cast(s));
}
#if defined (__linux__) || (defined (__APPLE__) && defined (__MACH__))
inline bool execute(std::string cmd){
    if(std::system(cmd.c_str()) == -1){
        return false;
    }

    return true;
}
#elif defined (_WIN32) || defined (WIN32)
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
#else
#error "Unsupported platform"
#endif

#if defined (__linux__) || (defined (__APPLE__) && defined (__MACH__))
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
#else
#error "Unsupported platform"
#endif

#if defined (__linux__) || (defined (__APPLE__) && defined (__MACH__))
inline bool findproc(std::string procName){
    return !exec_output("ps -ef",procName).empty();
}
#elif defined (_WIN32) || defined (WIN32)
inline bool findproc(std::string procName){
    return !exec_output("tasklist",procName).empty();
}
#else
#error "Unsupported platform"
#endif
inline bool findenv(std::string key){
    return getenv(key.c_str()) != nullptr;
}
#if defined (__linux__)
inline void addenv(std::string key,std::string value){
    putenv((char*)keep_alive_string(key+"="+value));
}
#elif defined (_WIN32) || defined (WIN32)
inline void addenv(std::string key,std::string value){
    _putenv(keep_alive_string(key+"="+value));
}
#elif defined (__APPLE__) && defined (__MACH__)
inline void addenv(std::string key,std::string value){
    setenv(keep_alive_string(key),keep_alive_string(value),1);
}
#else
#error "Unsupported platform"
#endif

#if defined (__linux__) || (defined (__APPLE__) && defined (__MACH__))
inline std::string get_current_dylib_path(){
    std::string path;
    Dl_info dl_info;
    if(dladdr((void*)&get_current_dylib_path,&dl_info)){
        path = dl_info.dli_fname;
    }
    return path;
}
#elif defined (_WIN32) || defined (WIN32)
inline std::string get_current_dylib_path(){
    std::string path;
    HMODULE hModule = NULL;
    if(GetModuleHandleExA(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCSTR) &get_current_dylib_path,
        &hModule)){
            char buffer[1024];
            DWORD length = GetModuleFileNameA(hModule, buffer, 1024);
            if(length > 0){
                path = std::string(buffer,length);
            }
        }
    return path;
}
#else
#error "Unsupported platform"
#endif
inline std::string to_windows_path(std::string path)
{
    if (path.empty())
    {
        return "";
    }

    std::string result;
    result.reserve(path.length());

    for (char c : path)
    {
        if (c == '/')
        {
            c = '\\';
        }
        if (c == '\\' && !result.empty() && result.back() == '\\')
        {
            continue;
        }
        result += c;
    }
    return result;
}
#endif//!HOUDINI_ENGINE_UTILITY