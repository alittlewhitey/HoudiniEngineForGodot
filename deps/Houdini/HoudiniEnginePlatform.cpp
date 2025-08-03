/*
* Copyright (c) <2023> Side Effects Software Inc.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. The name of Side Effects Software may not be used to endorse or
*    promote products derived from this software without specific prior
*    written permission.
*
* THIS SOFTWARE IS PROVIDED BY SIDE EFFECTS SOFTWARE "AS IS" AND ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
* NO EVENT SHALL SIDE EFFECTS SOFTWARE BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "HoudiniEnginePlatform.h"

#if defined(WIN32) || defined(_WIN32)
    #include "Windows.h"
#else
    #include <dlfcn.h>
#endif

#include <iostream>
#include <string>

// Names of HAPI libraries on different platforms.
const char* HAPIL_LIB_OBJECT_WINDOWS = "libHAPIL.dll";
const char* HAPIL_LIB_OBJECT_LINUX = "libHAPIL.so";
const char* HAPIL_LIB_OBJECT_MAC = "libHAPIL.dylib";
const char* HAPI_LIB_OBJECT_WINDOWS = "libHAPI.dll";
const char* HAPI_LIB_OBJECT_LINUX = "libHAPI.so";
const char* HAPI_LIB_OBJECT_MAC = "libHAPI.dylib";

void*
HoudiniEnginePlatform::LoadLibHAPIL(bool useHAPI, std::string libDir)
{
    void* libHAPI = nullptr;
#if defined(WIN32) || defined(_WIN32)
    // // Look up the HFS environment variable

    // //Changed for non-msvcr90 environment
    // // char *buf;
    // // size_t len;
    // const char *buf = getenv("HFS");
    // //if (_dupenv_s(&buf, &len, "HFS") == 0 && buf != nullptr)
    // if(buf != nullptr)
    // {
    //     std::string libHAPI_dir(buf);
    //     //free(buf);

    //     libHAPI_dir.append("/bin/");
    //     if (SetDllDirectory(libHAPI_dir.c_str()))
    //     {
    //         if(useHAPI)
    //         libHAPI = LoadLibrary(HAPI_LIB_OBJECT_WINDOWS);
    //         else
    //             libHAPI = LoadLibrary(HAPIL_LIB_OBJECT_WINDOWS);
    //     }
    // }
    // else
    // {
    //     std::cerr << "Unable to retrieve the value of the HFS environment variable." << std::endl;
    //     return nullptr;
    // }
    std::string libHAPI_dir = libDir;

    // If libDir is not provided, fall back to HFS environment variable.
    if (libHAPI_dir.empty())
    {
        const char *buf = getenv("HFS");
        if(buf != nullptr)
        {
            libHAPI_dir = std::string(buf) + "/bin";
        }
        else
        {
            std::cerr << "HoudiniEngine: libDir was not specified and the HFS environment variable could not be found." << std::endl;
            return nullptr;
        }
    }

    // Use the modern and safer AddDllDirectory approach.
    DLL_DIRECTORY_COOKIE cookie = AddDllDirectory(std::wstring(libHAPI_dir.begin(), libHAPI_dir.end()).c_str());
    if (cookie)
    {
        const char* lib_name = useHAPI ? HAPI_LIB_OBJECT_WINDOWS : HAPIL_LIB_OBJECT_WINDOWS;
        libHAPI = LoadLibraryExA(lib_name, NULL, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
        // We can remove the path from the search list now that we are done.
        RemoveDllDirectory(cookie);
    }
#elif __linux__
    // Location of libHAPI on Mac & Linux added to the application's RPATH
    if(useHAPI)
        libHAPI = dlopen(((libDir.empty()?"":libDir+'/')+HAPI_LIB_OBJECT_LINUX).c_str(), RTLD_LAZY); 
    else
        libHAPI = dlopen(((libDir.empty()?"":libDir+'/')+HAPIL_LIB_OBJECT_LINUX).c_str(), RTLD_LAZY); 
#else
    if(useHAPI)
        libHAPI = dlopen(((libDir.empty()?"":libDir+'/')+HAPI_LIB_OBJECT_MAC).c_str(), RTLD_LAZY); 
    else
        libHAPI = dlopen(((libDir.empty()?"":libDir+'/')+HAPIL_LIB_OBJECT_MAC).c_str(), RTLD_LAZY); 
#endif

    if (libHAPI == nullptr)
        std::cerr << "Failed to load the libHAPI module." << std::endl;

    return libHAPI;
}

bool
HoudiniEnginePlatform::FreeLibHAPIL(void* libHAPIL)
{
#if defined(WIN32) || defined(_WIN32)
    return FreeLibrary((HMODULE)libHAPIL) && SetDllDirectory(nullptr);
#else
    return dlclose(libHAPIL) == 0;
#endif
}

void*
HoudiniEnginePlatform::GetDllExport(void* LibraryHandle, const char* ExportName)
{
#if defined(WIN32) || defined(_WIN32)
    return (void*)GetProcAddress((HMODULE)LibraryHandle, ExportName);
#else
    return dlsym(LibraryHandle, ExportName);
#endif
}
