@tool
extends Node
class_name HEngineAssetData

var hda_path: String
var asset_id: int

var parameter_presets: Dictionary = {}
var material_overrides: Dictionary = {}
var input_connections: Array = []

var cook_state: int = 0
enum CookState {
	NONE = 0,
	COOKING = 1,
	SUCCESS = 2,
	FAILED = 3,
	NODE_INVALID = 4,
}

var _hda_root: HEngineAssetRoot


func _ready():
	_hda_root = _find_hda_root()


func _find_hda_root() -> HEngineAssetRoot:
	var node = get_parent()
	while node:
		if node is HEngineAssetRoot:
			return node
		node = node.get_parent()
	return null


func set_hda_metadata(path: String, id: int):
	hda_path = path
	asset_id = id


func get_houdini_node() -> HENode:
	if _hda_root:
		return _hda_root.get_houdini_node()
	return null


func save_parameter_preset():
	parameter_presets.clear()
	var node = get_houdini_node()
	if node == null:
		return
	var parm_list: PackedStringArray = node.getParameterList()
	for parm_name in parm_list:
		parameter_presets[parm_name] = node.getParameter(parm_name)


func load_parameter_preset(preset: Dictionary = {}):
	var node = get_houdini_node()
	if node == null:
		return
	var target = preset if not preset.is_empty() else parameter_presets
	for parm_name in target:
		node.setParameter(parm_name, target[parm_name])


func restore_parameter_preset():
	load_parameter_preset()
