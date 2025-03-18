cmake_minimum_required(VERSION 3.10.2)

project(HoudiniEngineForGodot)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_BUILD_TYPE "Debug")

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(Platform "windows")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(Platform "linux")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(Platform "macos")
else()
    set(Platform "unknown")
endif()
string(TOLOWER ${CMAKE_SYSTEM_PROCESSOR} CMAKE_SYSTEM_ARCH)
if(${CMAKE_SYSTEM_ARCH} MATCHES "x86_64")
    set(Arch "x86_64")
elseif(${CMAKE_SYSTEM_ARCH} MATCHES "x86_32" OR ${CMAKE_SYSTEM_ARCH} MATCHES "i386")
    set(Arch "x86_32")
elseif(${CMAKE_SYSTEM_ARCH} MATCHES "arm64" OR ${CMAKE_SYSTEM_ARCH} MATCHES "aarch64") 
    set(Arch "arm64")
elseif(${CMAKE_SYSTEM_ARCH} MATCHES "arm")
    set(Arch "arm32")
else()
    set(Arch "unknown")
endif()

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    add_compile_options("-fexperimental-library")
endif()


include_directories(deps/Houdini)
include_directories(src)

aux_source_directory(deps/Houdini DEP_SRC)
aux_source_directory(src BIND_SRC)
set(LIBRARY_OUTPUT_DIRECTORY out)

add_library(HoudiniEngine_${Platform}_${Arch} SHARED ${DEP_SRC} ${BIND_SRC})

target_link_libraries(HoudiniEngine_${Platform}_${Arch}  "${CMAKE_CURRENT_SOURCE_DIR}/lib/libgodot-cpp.${Platform}.editor.${Arch}.a")