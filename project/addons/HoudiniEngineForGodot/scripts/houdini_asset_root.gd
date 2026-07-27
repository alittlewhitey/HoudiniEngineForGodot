@tool
extends Node3D
class_name HEngineAssetRoot

const DATA_NODE_NAME: String = "HDA_Data"

var hda_path: String
var asset_id: int

var auto_cook: bool
var use_cooking_thread: bool
var push_transform_to_houdini: bool
var generate_normals: bool
var generate_tangents: bool
var split_geos_by_groups: bool

var _houdini_node: HENode


func _ready():
	set_meta("HEngineAssetRoot", true)


func get_houdini_node() -> HENode:
	return _houdini_node


func set_houdini_node(node: HENode):
	_houdini_node = node


func get_data_node() -> HEngineAssetData:
	for child in get_children():
		if child.name == DATA_NODE_NAME:
			return child
	return null


func find_output_nodes() -> Array[Node]:
	var data_node = get_data_node()
	var output: Array[Node] = []
	for child in get_children():
		if child != data_node:
			output.append(child)
	return output


func remove_all_outputs():
	var data_node = get_data_node()
	for child in get_children():
		if child != data_node:
			child.queue_free()


func clear_plugin_data():
	var data_node = get_data_node()
	var output_nodes = find_output_nodes()
	if data_node:
		data_node.queue_free()
	for node in output_nodes:
		node.reparent(get_parent())

	var scene_root = get_tree().get_edited_scene_root()
	if scene_root == self:
		for node in output_nodes:
			node.owner = null
	else:
		for node in output_nodes:
			node.owner = get_parent()

	remove_meta("HEngineAssetRoot")
	queue_free()
