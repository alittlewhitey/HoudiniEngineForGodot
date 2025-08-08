cmake_minimum_required(VERSION 3.12)
project(HoudiniEngineForGodot)
find_package(Python3 REQUIRED)
if(NOT DEFINED GODOT_CPP_BRANCH)
    message(WARNING "Use branch 'master' of godot-cpp")
    message(WARNING "You'd better define branch name like this: \ncmake .. -DGODOT_CPP_BRANCH=\"4.4\"")
    set(GODOT_CPP_BRANCH "master")
    execute_process(COMMAND ${CMAKE_COMMAND} -E sleep 5)
else() 
    message("Use branch ${GODOT_CPP_BRANCH} of godot-cpp.")
    string(REGEX REPLACE "^\"(.*)\"$" "\\1" GODOT_CPP_BRANCH "${GODOT_CPP_BRANCH}")
endif()
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    if(${USE_MINGW} MATCHES yes)
        if(${USE_LLVM} MATCHES yes)
            set(CMAKE_C_COMPILER_ID Clang)
            set(CMAKE_CXX_COMPILER_ID Clang)
            set(CMAKE_C_COMPILER clang)
            set(CMAKE_CXX_COMPILER clang++)
        else()
            set(CMAKE_C_COMPILER_ID GNU)
            set(CMAKE_CXX_COMPILER_ID GNU)
            set(CMAKE_C_COMPILER gcc)
            set(CMAKE_CXX_COMPILER g++)
        endif()
    endif()
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(${DEBUG} MATCHES yes)
    set(BuildType "debug") 
else()
if(${CMAKE_BUILD_TYPE} MATCHES "Release")
    set(BuildType "release") 
else()
    set(BuildType "debug") 
endif()
endif()
if(${BuildType} MATCHES "debug")
message("Debug mode opening")
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
if(${CMAKE_CXX_COMPILER_ID} MATCHES "MSVC")
    add_compile_options("-EHsc")
    if(${BuildType} MATCHES "debug")
        add_compile_options("-DFORCE_DEBUG")
        add_compile_options("-Z7")
    endif()
else()
    if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
        add_compile_options("-fexperimental-library")
    endif()
    if(${BuildType} MATCHES "debug")
        add_compile_options("-DFORCE_DEBUG")
        add_compile_options("-g")
    endif()
    add_compile_options("-Wall")
    add_compile_options("-fexceptions")
    add_compile_options("-ftls-model=global-dynamic")
endif()
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deps/godot-cpp/include")
    execute_process(
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMAND git submodule update --init --recursive
    )
    execute_process(
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/deps/godot-cpp"
        COMMAND git checkout ${GODOT_CPP_BRANCH}
    )
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.godot_cpp_branch")
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/.godot_cpp_branch" OLD_BRANCH_NAME)
    if(NOT ${OLD_BRANCH_NAME} STREQUAL ${GODOT_CPP_BRANCH})
        execute_process(
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/deps/godot-cpp"
            COMMAND git checkout ${GODOT_CPP_BRANCH}
        )
    endif()
endif()
file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/.godot_cpp_branch" ${GODOT_CPP_BRANCH})
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deps/godot-cpp/bin/libgodot-cpp.${Platform}.editor.${Arch}.a")
    if(${BuildType} MATCHES "debug")
        if(${Platform} MATCHES "windows")
            if(${USE_MINGW} MATCHES yes)
                if(${USE_LLVM} MATCHES yes)
                    execute_process(
                        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/deps/godot-cpp"
                        COMMAND scons target=editor platform=${Platform} arch=${Arch} use_mingw=yes use_llvm=yes dev_build=yes
                    ) 
                else()
                    execute_process(
                        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/deps/godot-cpp"
                        COMMAND scons target=editor platform=${Platform} arch=${Arch} use_mingw=yes dev_build=yes
                    ) 
                endif()
            else()
                execute_process(
                    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/deps/godot-cpp"
                    COMMAND scons target=editor platform=${Platform} arch=${Arch} debug_crt=yes dev_build=yes 
                ) 
            endif()
        else()
            execute_process(
                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/deps/godot-cpp"
                COMMAND scons target=editor platform=${Platform} arch=${Arch} dev_build=yes
            ) 
        endif()
    else()
        if(${Platform} MATCHES "windows")
            if(${USE_MINGW} MATCHES yes)
                if(${USE_LLVM} MATCHES yes)
                    execute_process(
                        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/deps/godot-cpp"
                        COMMAND scons target=editor platform=${Platform} arch=${Arch} use_mingw=yes use_llvm=yes production=yes
                    ) 
                else()
                    execute_process(
                        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/deps/godot-cpp"
                        COMMAND scons target=editor platform=${Platform} arch=${Arch} use_mingw=yes production=yes
                    ) 
                endif()
            else()
                execute_process(
                    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/deps/godot-cpp"
                    COMMAND scons target=editor platform=${Platform} arch=${Arch} production=yes
                ) 
            endif()
        else()
            execute_process(
                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/deps/godot-cpp"
                COMMAND scons target=editor platform=${Platform} arch=${Arch} production=yes
            ) 
        endif()
    endif()
endif()
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/gen/doc_data.gen.cpp")
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/gen)
    file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/src/gen/doc_data.gen.cpp "")
endif()
execute_process(
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/cmake_docgen.py
)
set(TARGET_NAME "HoudiniEngineForGodot.${Platform}.${Arch}")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/Houdini)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/godot-cpp/gdextension)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/godot-cpp/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/godot-cpp/gen/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/godot-cpp/bin)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/deps/Houdini DEP_SRC)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src BIND_SRC)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/gen DOCDATA_SRC)
add_library(${TARGET_NAME} SHARED ${DEP_SRC} ${BIND_SRC} ${DOCDATA_SRC})
target_link_libraries(${TARGET_NAME} PRIVATE "godot-cpp.${Platform}.editor.${Arch}")
if(${Platform} MATCHES "windows")
    set(STATIC_CPP "$<BOOL:${GODOTCPP_USE_STATIC_CPP}>")
    set_target_properties(${TARGET_NAME} PROPERTIES PDB_OUTPUT_DIRECTORY "$<1:${CMAKE_SOURCE_DIR}/bin>")
    add_custom_command(
        TARGET ${TARGET_NAME}
        POST_BUILD
        COMMAND copy build/lib${TARGET_NAME}.dll project/addons/HoudiniEngineForGodot/bin
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