#pragma once
#ifndef REGISTER_PARTITION_H
#define REGISTER_PARTITION_H


#define HOT_RELOAD_ENABLED
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/node.hpp>

using namespace godot;


void initialize_module(ModuleInitializationLevel p_level);
void uninitialize_module(ModuleInitializationLevel p_level);

#endif // REGISTER_PARTITION_H