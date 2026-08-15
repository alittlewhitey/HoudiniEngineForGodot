#pragma once
#ifndef HOUDINI_ENGINE_REGISTER
#define HOUDINI_ENGINE_REGISTER
#include "DebugSymbol.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/node.hpp>

void initialize_module(godot::ModuleInitializationLevel p_level);
void uninitialize_module(godot::ModuleInitializationLevel p_level);

#endif // HOUDINI_ENGINE_REGISTER