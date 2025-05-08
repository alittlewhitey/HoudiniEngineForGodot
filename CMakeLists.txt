cmake_minimum_required(VERSION 3.10.2)
project(HoudiniEngineForGodot)

if(NOT DEFINED GODOT_CPP_BRANCH)
    message(FATAL_ERROR "You must define GODOT_CPP_BRANCH for what version you use.")
else() 
    message("Use branch ${GODOT_CPP_BRANCH} of godot-cpp.")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


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
endif()
add_compile_options("-fexceptions")
add_compile_options("-ftls-model=global-dynamic")
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
set(TARGET_NAME "HoudiniEngine.${Platform}.${BuildType}.${Arch}")
include_directories(src)
include_directories(deps)
include_directories(deps/Houdini)
include_directories(deps/godot-cpp/gdextension)
include_directories(deps/godot-cpp/include)
include_directories(deps/godot-cpp/gen/include)
link_directories(deps/godot-cpp/bin)
aux_source_directory(deps/Houdini DEP_SRC)
aux_source_directory(src BIND_SRC)
add_library(${TARGET_NAME} SHARED ${DEP_SRC} ${BIND_SRC})
target_link_libraries(${TARGET_NAME} PRIVATE "godot-cpp.${Platform}.editor.${Arch}")
if(${Platform} MATCHES "windows")
    set(STATIC_CPP "$<BOOL:${GODOTCPP_USE_STATIC_CPP}>")
    set_target_properties(${TARGET_NAME} PROPERTIES PDB_OUTPUT_DIRECTORY "$<1:${CMAKE_SOURCE_DIR}/bin>")
    target_compile_definitions(
        ${TARGET_NAME}
        PUBLIC WINDOWS_ENABLED $<${IS_MSVC}: TYPED_METHOD_BIND NOMINMAX >
    )
    target_link_options(
        ${TARGET_NAME}
        PUBLIC
            $<${NOT_MSVC}:
                -Wl,--no-undefined
                $<${STATIC_CPP}:
                    -static
                    -static-libgcc
                    -static-libstdc++
                >
            >

            $<${IS_CLANG}:-lstdc++>
    )
    add_custom_command(
        TARGET ${TARGET_NAME}
        POST_BUILD
        COMMAND cp build/lib${TARGET_NAME}.dll project/addons/HoudiniEngineForGodot/bin
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
elseif(${Platform} MATCHES "linux")
    target_compile_definitions(${TARGET_NAME} PUBLIC LINUX_ENABLED UNIX_ENABLED)
    add_custom_command(
        TARGET ${TARGET_NAME}
        POST_BUILD
        COMMAND cp build/lib${TARGET_NAME}.so project/addons/HoudiniEngineForGodot/bin
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
elseif(${Platform} MATCHES "macos")
    target_compile_definitions(${TARGET_NAME} PUBLIC MACOS_ENABLED UNIX_ENABLED)
    target_link_options(${TARGET_NAME} PUBLIC -Wl,-undefined,dynamic_lookup)
    add_custom_command(
        TARGET ${TARGET_NAME}
        POST_BUILD
        COMMAND cp build/lib${TARGET_NAME}.dylib project/addons/HoudiniEngineForGodot/bin
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()