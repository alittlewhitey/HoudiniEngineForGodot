cmake_minimum_required(VERSION 3.10.2)
project(HoudiniEngineForGodot)

if(NOT DEFINED GODOT_CPP_BRANCH)
    set(GODOT_CPP_BRANCH "master")
    message("Use branch ${GODOT_CPP_BRANCH} of godot-cpp. Define GODOT_CPP_BRANCH to change it.")
endif()
message("Use branch ${GODOT_CPP_BRANCH} of godot-cpp.")

set(CMAKE_CXX_STANDARD 20)

string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
if(${BUILD_TYPE} MATCHES "release")
    set(BuildType "release") 
else()
    set(BuildType "debug")
endif()
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(Platform "windows")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(Platform "linux")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(Platform "macos")
else()
    set(Platform "unknown")
endif()
string(TOLOWER ${CMAKE_SYSTEM_PROCESSOR} SYSTEM_ARCH)
if(${SYSTEM_ARCH} MATCHES "x86_64" OR ${SYSTEM_ARCH} MATCHES "amd64")
    set(Arch "x86_64")
elseif(${SYSTEM_ARCH} MATCHES "x86_32" OR ${SYSTEM_ARCH} MATCHES "i386")
    set(Arch "x86_32")
elseif(${SYSTEM_ARCH} MATCHES "arm64" OR ${SYSTEM_ARCH} MATCHES "aarch64") 
    set(Arch "arm64")
elseif(${SYSTEM_ARCH} MATCHES "arm")
    set(Arch "arm32")
elseif(${SYSTEM_ARCH} MATCHES "riscv64" OR ${SYSTEM_ARCH} MATCHES "rv64") 
    set(Arch "rv64")
else()
    set(Arch "unknown")
endif()

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    add_compile_options("-fexperimental-library")
endif()

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deps/godot-cpp/include")
    execute_process(
        COMMAND git "submodule update --init --recursive"
        TIMEOUT 600
    )
    execute_process(
        COMMAND git "checkout ${GODOT_CPP_BRANCH}"
        TIMEOUT 60
    )
endif()
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deps/godot-cpp/bin/libgodot-cpp.${Platform}.editor.${Arch}.a")
    execute_process(
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/deps/godot-cpp"
        COMMAND scons "target=editor"
    )
endif()
include_directories(deps/godot-cpp/gdextension)
include_directories(deps/Houdini)
include_directories(deps/godot-cpp/include)
include_directories(deps/godot-cpp/gen/include)
include_directories(src)

aux_source_directory(deps/Houdini DEP_SRC)
aux_source_directory(src BIND_SRC)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/project/addons/HoudiniEngineForGodot/bin")

add_library(HoudiniEngine_${Platform}_${BuildType}_${Arch} SHARED ${DEP_SRC} ${BIND_SRC})

target_link_libraries(HoudiniEngine_${Platform}_${BuildType}_${Arch}  "${CMAKE_CURRENT_SOURCE_DIR}/deps/godot-cpp/bin/libgodot-cpp.${Platform}.editor.${Arch}.a")
