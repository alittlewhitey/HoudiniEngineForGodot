@tool
extends RefCounted
class_name HEngineUtility


static func create_hda_root(hda_path: String, session: HESession, parent_node: Node = null) -> HEngineAssetRoot:
	if session == null or not session.valid():
		push_error("Cannot instantiate HDA: no valid session")
		return null

	var asset: HEAsset = session.loadHDAExternal(hda_path)
	if asset == null:
		push_error("Failed to load HDA: %s" % hda_path)
		return null

	var operator_names: PackedStringArray = asset.get_ownedNodeOperators()
	if operator_names.is_empty():
		push_error("HDA has no operators: %s" % hda_path)
		return null

	var scene_root = _get_scene_root(parent_node)
	var label: String = operator_names[0]

	var sop_node: HESopNode = HENode.createNode(label, label)
	if sop_node == null:
		push_error("Failed to create node for operator: %s" % label)
		return null

	var root := HEngineAssetRoot.new()
	root.name = hda_path.get_file().get_basename()
	root.hda_path = hda_path
	root.asset_id = 0
	root.set_houdini_node(sop_node)

	root.auto_cook = ProjectSettings.get_setting("houdini/option/autoCook", true)
	root.use_cooking_thread = ProjectSettings.get_setting("houdini/option/useCookingThread", false)
	root.push_transform_to_houdini = ProjectSettings.get_setting("houdini/option/pushTransform", false)
	root.generate_normals = ProjectSettings.get_setting("houdini/option/generateNormals", true)
	root.generate_tangents = ProjectSettings.get_setting("houdini/option/generateTangents", true)

	var data_node := HEngineAssetData.new()
	data_node.name = HEngineAssetRoot.DATA_NODE_NAME
	data_node.set_hda_metadata(hda_path, 0)
	root.add_child(data_node)

	if parent_node:
		parent_node.add_child(root)
	else:
		scene_root.add_child(root)

	# owner must be set AFTER the node is in the scene tree
	data_node.owner = scene_root
	root.owner = scene_root

	root.set_meta("HEngineAssetRoot", true)

	data_node.save_parameter_preset()

	return root


static func cook_and_generate(root: HEngineAssetRoot) -> bool:
	if root == null:
		return false

	var node: HENode = root.get_houdini_node()
	if node == null:
		return false

	var data_node = root.get_data_node()
	node.cook()
	var ok = await node.cookFinished
	if ok:
		if data_node:
			data_node.cook_state = HEngineAssetData.CookState.SUCCESS
		root.remove_all_outputs()
		_generate_output_nodes(root, node)
	else:
		if data_node:
			data_node.cook_state = HEngineAssetData.CookState.FAILED
		push_warning("Cook failed for: %s" % node.getName())

	return ok


static func recook_asset(root: HEngineAssetRoot) -> bool:
	var data_node = root.get_data_node()
	if data_node == null:
		return false

	var node: HENode = root.get_houdini_node()
	if node == null:
		return false

	if not data_node.parameter_presets.is_empty():
		for parm_name in data_node.parameter_presets:
			node.setParameter(parm_name, data_node.parameter_presets[parm_name])

	return await cook_and_generate(root)


static func rebuild_asset(root: HEngineAssetRoot) -> HEngineAssetRoot:
	if root == null:
		return null

	var hda_path := root.hda_path
	var parent := root.get_parent()
	root.queue_free()
	return create_hda_root(hda_path, HESession.getSession(), parent)


static func duplicate_asset(root: HEngineAssetRoot) -> HEngineAssetRoot:
	var hda_path := root.hda_path
	var parent := root.get_parent()
	return create_hda_root(hda_path, HESession.getSession(), parent)


static func reset_parameters(root: HEngineAssetRoot) -> bool:
	var data_node = root.get_data_node()
	if data_node == null:
		return false

	data_node.parameter_presets.clear()
	return true


static func bake_gameobject(root: HEngineAssetRoot) -> Node3D:
	if root == null:
		return null

	var output_nodes = root.find_output_nodes()
	if output_nodes.is_empty():
		return null

	var scene_root = root.get_tree().get_edited_scene_root()
	var is_multi = output_nodes.size() > 1
	var bake_root: Node3D

	if is_multi:
		bake_root = Node3D.new()
		bake_root.name = root.name + "_Baked"
		bake_root.set_meta("_HEngineBaked", true)
	else:
		bake_root = output_nodes[0].duplicate()
		root.get_parent().add_child(bake_root)
		bake_root.owner = scene_root
		return bake_root

	for node in output_nodes:
		var dup = node.duplicate()
		bake_root.add_child(dup)
		dup.owner = scene_root

	root.get_parent().add_child(bake_root)
	bake_root.owner = scene_root
	return bake_root


static func bake_updates(root: HEngineAssetRoot, targets: Array[Node3D], keep_transform: bool):
	var output_nodes = root.find_output_nodes()
	if output_nodes.is_empty():
		return

	for target in targets:
		for out_node in output_nodes:
			if out_node.name == target.name:
				_copy_node_components(out_node, target, keep_transform)


static func upload_parameters(root: HEngineAssetRoot):
	var data_node = root.get_data_node()
	if data_node == null:
		return

	var node: HENode = root.get_houdini_node()
	if node == null:
		return

	if not data_node.parameter_presets.is_empty():
		for parm_name in data_node.parameter_presets:
			node.setParameter(parm_name, data_node.parameter_presets[parm_name])


# ── Internal ──

static func _get_scene_root(hint_node: Node) -> Node:
	if hint_node:
		var tree = hint_node.get_tree()
		if tree:
			return tree.get_edited_scene_root()
	var editor_interface = Engine.get_singleton("EditorInterface")
	if editor_interface:
		return editor_interface.get_edited_scene_root()
	return null


static func _generate_output_nodes(root: HEngineAssetRoot, he_node: HENode):
	if not he_node is HESopNode:
		return

	var sop_node: HESopNode = he_node as HESopNode
	var geo_count: int = sop_node.getGeometryCount()

	for i in range(geo_count):
		var geo = sop_node.getGeometry(i)
		if geo == null:
			continue

		var scene_root = root.owner
		if geo is HEMesh:
			var mesh: Mesh = (geo as HEMesh).bakeAsMesh()
			if mesh:
				var mi := MeshInstance3D.new()
				mi.name = "Mesh_%d" % i
				mi.mesh = mesh
				_try_apply_material(mi, he_node.getId())
				root.add_child(mi)
				mi.owner = scene_root

		elif geo is HEMultiMesh:
			var mm_dict: Dictionary = (geo as HEMultiMesh).bakeAsMultiMesh()
			if not mm_dict.is_empty():
				var mmi := MultiMeshInstance3D.new()
				mmi.name = "MultiMesh_%d" % i
				mmi.multimesh = mm_dict.get("multimesh")
				root.add_child(mmi)
				mmi.owner = scene_root

		elif geo is HECurve:
			var curve_arr: Array = (geo as HECurve).bakeAsCurve()
			if not curve_arr.is_empty():
				var path := Path3D.new()
				path.name = "Curve_%d" % i
				var curve_3d := Curve3D.new()
				for point_data in curve_arr:
					if point_data is Dictionary:
						var pos: Vector3 = point_data.get("position", Vector3.ZERO)
						var in_tan: Vector3 = point_data.get("in_tangent", Vector3.ZERO)
						var out_tan: Vector3 = point_data.get("out_tangent", Vector3.ZERO)
						curve_3d.add_point(pos, in_tan, out_tan)
				path.curve = curve_3d
				root.add_child(path)
				path.owner = scene_root


static func _copy_node_components(source: Node, target: Node3D, keep_transform: bool):
	var saved_transform = target.transform if keep_transform else source.transform
	if source is MeshInstance3D and target is MeshInstance3D:
		target.mesh = (source as MeshInstance3D).mesh
	target.transform = saved_transform


# ── Material (skeleton — depends on C++ material mapping API) ──

static func _try_apply_material(mesh_instance: MeshInstance3D, node_id: int):
	_apply_fallback_material(mesh_instance)


static func _apply_fallback_material(mesh_instance: MeshInstance3D):
	var mat := StandardMaterial3D.new()
	mat.vertex_color_use_as_albedo = true
	mat.vertex_color_is_srgb = true
	mesh_instance.set_surface_override_material(0, mat)


# ── Object node traversal (skeleton) ──

static func generate_full_scene(root: HEngineAssetRoot, he_node: HENode):
	_generate_output_nodes(root, he_node)
