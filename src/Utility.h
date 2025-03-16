#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#if defined (__linux__) || (defined (__APPLE__) && defined (__MACH__))
GDE_EXPORT
inline std::string exec_output(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) return "";
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }
    return result;
}
#elif defined (_WIN32) || defined (WIN32)
GDE_EXPORT
inline std::string exec_output(const char* cmd){
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(_popen(cmd, "r"), _pclose);
    if (!pipe) return "";
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }
    return result;
}
#endif


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
