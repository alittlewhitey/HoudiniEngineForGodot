#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <set>

static inline std::set<std::string> _houdini_endine_string_buffer;
GDE_EXPORT
inline const char* keep_alive_string(std::string s){
    return _houdini_endine_string_buffer.insert(std::move(s)).first->c_str();
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
inline std::string exec_output(std::string cmd){
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
    if(getenv(key.c_str()) == NULL)
        return false;
    return true;
}
#if defined (__linux__)
GDE_EXPORT
inline void addenv(std::string key,std::string value){
    putenv((key+"="+value).c_str());
}
#elif defined (_WIN32) || defined (WIN32)
GDE_EXPORT
inline void addenv(std::string key,std::string value){
    _putenv((key+"="+value).c_str());
}
#elif defined (__APPLE__) && defined (__MACH__)
GDE_EXPORT
inline void addenv(std::string key,std::string value){
    setenv(key.c_str(),value.c_str(),1);
}
#endif
