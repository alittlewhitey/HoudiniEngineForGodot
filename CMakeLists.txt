cmake_minimum_required(VERSION 3.10.2)

project(HoudiniEngineForGodot)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_BUILD_TYPE "Debug")

set(CMAKE_BUILD_PARALLEL_LEVEL 19)

include_directories(deps/Houdini)
include_directories(src)

aux_source_directory(deps/Houdini DEP_SRC)
aux_source_directory(src BIND_SRC)
set(LIBRARY_OUTPUT_DIRECTORY out)

add_library(HoudiniEngine_windows_4.3_editor SHARED ${DEP_SRC} ${BIND_SRC})

target_link_libraries(HoudiniEngine_windows_4.3_editor  "${CMAKE_CURRENT_SOURCE_DIR}/lib/libgodot-cpp.windows.editor.x86_64.lib")