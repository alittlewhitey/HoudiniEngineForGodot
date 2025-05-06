cmake_minimum_required(VERSION 3.10.2)
project(HoudiniEngineForGodot)

if(NOT DEFINED GODOT_CPP_BRANCH)
    message(FATAL_ERROR "You must define GODOT_CPP_BRANCH for what version you use.")
else() 
    message("Use branch ${GODOT_CPP_BRANCH} of godot-cpp.")
endif()

set(CMAKE_CXX_STANDARD 20)
if(${CMAKE_BUILD_TYPE} MATCHES "Release")
    set(BuildType "release") 
else()
    set(BuildType "debug") 
endif()
message("Build for platform: ${CMAKE_SYSTEM_NAME}.")
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(Platform "windows")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(Platform "linux")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(Platform "macos")
else()
    set(Platform "unknown")
endif()
message("Build for arch: ${CMAKE_SYSTEM_PROCESSOR}.")
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
message("Use compiler: ${CMAKE_CXX_COMPILER_ID}.")
if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    add_compile_options("-fexperimental-library")
    add_compile_options("-fexceptions")
    add_link_options("-v")
endif()

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deps/godot-cpp/include")
    execute_process(
        COMMAND git submodule update --init --recursive
        TIMEOUT 600
    )
    execute_process(
        COMMAND git checkout ${GODOT_CPP_BRANCH}
        TIMEOUT 60
    )
endif()
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deps/godot-cpp/bin/libgodot-cpp.${Platform}.editor.${Arch}.a")
    execute_process(
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/deps/godot-cpp"
        COMMAND scons target=editor platform=${Platform} arch=${Arch} 
    ) 
endif()
include_directories(src)
include_directories(deps)
include_directories(deps/Houdini)
include_directories(deps/godot-cpp/gdextension)
include_directories(deps/godot-cpp/include)
include_directories(deps/godot-cpp/gen/include)
link_directories(deps/godot-cpp/bin)
aux_source_directory(deps/Houdini DEP_SRC)
aux_source_directory(src BIND_SRC)
add_library(HoudiniEngine_${Platform}_${BuildType}_${Arch} SHARED ${DEP_SRC} ${BIND_SRC})
target_link_libraries(HoudiniEngine_${Platform}_${BuildType}_${Arch} PUBLIC "godot-cpp.${Platform}.editor.${Arch}")
if(${Platform} MATCHES "windows")
    add_custom_command(
        TARGET HoudiniEngine_${Platform}_${BuildType}_${Arch}
        POST_BUILD
        COMMAND cp build/libHoudiniEngine_${Platform}_${BuildType}_${Arch}.dll project/addons/HoudiniEngineForGodot/bin
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
elseif(${Platform} MATCHES "linux")
    add_custom_command(
        TARGET HoudiniEngine_${Platform}_${BuildType}_${Arch}
        POST_BUILD
        COMMAND cp build/libHoudiniEngine_${Platform}_${BuildType}_${Arch}.so project/addons/HoudiniEngineForGodot/bin
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
elseif(${Platform} MATCHES "macos")
    add_custom_command(
        TARGET HoudiniEngine_${Platform}_${BuildType}_${Arch}
        POST_BUILD
        COMMAND cp build/libHoudiniEngine_${Platform}_${BuildType}_${Arch}.dylib project/addons/HoudiniEngineForGodot/bin
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()
