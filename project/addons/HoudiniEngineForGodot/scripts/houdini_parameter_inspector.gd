@tool
extends Control
class_name HEngineParameterInspector

var _hda_root: HEngineAssetRoot
var _data_node: HEngineAssetData
var _houdini_node: HENode

var _scroll: ScrollContainer
var _main_vbox: VBoxContainer
var _session_dock: Control

var _bake_update_targets: Array[Node3D] = []
var _preset_path: String


func _ready():
	_setup_ui()


func set_session_dock(dock: Control):
	_session_dock = dock


func set_hda_root(root: HEngineAssetRoot):
	print("[HEPI:25] set_hda_root called, root=%s (current _hda_root=%s)" % [root, _hda_root])
	if root == _hda_root:
		print("[HEPI:26] same root, skipping rebuild")
		return
	_ensure_ui()
	print("[HEPI:27] _ensure_ui done, _scroll=%s" % _scroll)
	_hda_root = root
	_data_node = root.get_data_node() if root else null
	_houdini_node = root.get_houdini_node() if root else null
	print("[HEPI:31] _hda_root=%s _data_node=%s _houdini_node=%s" % [_hda_root, _data_node, _houdini_node])
	_rebuild_ui()


func _ensure_ui():
	if _scroll == null:
		print("[HEPI:34] _scroll is null, calling _setup_ui")
		_setup_ui()


func _get_session() -> HESession:
	if _session_dock and _session_dock.has_method("get_session"):
		return _session_dock.get_session()
	var s = HESession.getSession()
	if s and s.valid():
		return s
	return null


func _setup_ui():
	print("[HEPI:47] _setup_ui START")
	size_flags_horizontal = SIZE_EXPAND_FILL
	size_flags_vertical = SIZE_EXPAND_FILL

	_main_vbox = VBoxContainer.new()
	_main_vbox.set_anchors_preset(Control.PRESET_FULL_RECT)
	add_child(_main_vbox)

	_scroll = ScrollContainer.new()
	_scroll.size_flags_horizontal = SIZE_EXPAND_FILL
	_scroll.size_flags_vertical = SIZE_EXPAND_FILL
	_scroll.horizontal_scroll_mode = ScrollContainer.SCROLL_MODE_DISABLED
	_main_vbox.add_child(_scroll)
	print("[HEPI:58] _setup_ui DONE, self=%s child_count=%d size=%s" % [self, get_child_count(), size])


func _rebuild_ui():
	print("[HEPI:61] _rebuild_ui, _hda_root=%s _scroll=%s" % [_hda_root, _scroll])
	for child in _scroll.get_children():
		child.free()
	if _hda_root == null:
		print("[HEPI:65] _hda_root is null → placeholder")
		_scroll.add_child(_placeholder("No HDA Asset selected"))
		return

	var vbox := VBoxContainer.new()
	vbox.size_flags_horizontal = SIZE_EXPAND_FILL
	vbox.size_flags_vertical = SIZE_EXPAND_FILL
	vbox.custom_minimum_size = Vector2(0, 400)
	_scroll.add_child(vbox)
	print("[HEPI:71] building sections into vbox...")

	_build_generate_section(vbox)
	_build_bake_section(vbox)
	_build_events_section(vbox)
	_build_parameters_section(vbox)
	print("[HEPI:77] _rebuild_ui DONE, vbox child_count=%d" % vbox.get_child_count())
	print("[HEPI:78] _scroll children=%d, _main_vbox children=%d" % [_scroll.get_child_count(), _main_vbox.get_child_count()])
	_dump_tree(self, 0)


func _placeholder(text: String) -> Label:
	var lbl := Label.new()
	lbl.text = text
	return lbl


# ── Generate ──

func _build_generate_section(parent: VBoxContainer):
	var fold := _make_foldout("Generate", parent)
	var vbox := _fold_body(fold)

	var btn_rebuild := Button.new()
	btn_rebuild.text = "Rebuild"
	btn_rebuild.tooltip_text = "Reload the HDA, reconnect inputs, and recook everything."
	btn_rebuild.pressed.connect(_on_rebuild_pressed)
	vbox.add_child(btn_rebuild)

	var btn_recook := Button.new()
	btn_recook.text = "Recook"
	btn_recook.tooltip_text = "Upload changed parameters and recook."
	btn_recook.pressed.connect(_on_recook_pressed)
	vbox.add_child(btn_recook)

	var btn_keep := Button.new()
	btn_keep.text = "Keep Output"
	btn_keep.tooltip_text = "Remove all Houdini Engine data, retain only output nodes."
	btn_keep.pressed.connect(_on_keep_output_pressed)
	vbox.add_child(btn_keep)

	var btn_dup := Button.new()
	btn_dup.text = "Duplicate"
	btn_dup.tooltip_text = "Create a duplicate of this asset."
	btn_dup.pressed.connect(_on_duplicate_pressed)
	vbox.add_child(btn_dup)

	var btn_reset := Button.new()
	btn_reset.text = "Reset"
	btn_reset.tooltip_text = "Reset all parameters to their default values."
	btn_reset.pressed.connect(_on_reset_parameters_pressed)
	vbox.add_child(btn_reset)


# ── Bake ──

func _build_bake_section(parent: VBoxContainer):
	var fold := _make_foldout("Bake", parent)
	var vbox := _fold_body(fold)

	var btn_bake_go := Button.new()
	btn_bake_go.text = "Bake to Scene"
	btn_bake_go.tooltip_text = "Create a copy of the output in the scene."
	btn_bake_go.pressed.connect(_on_bake_gameobject_pressed)
	vbox.add_child(btn_bake_go)

	var btn_bake_ps := Button.new()
	btn_bake_ps.text = "Bake to PackedScene"
	btn_bake_ps.tooltip_text = "Save a PackedScene (.tscn) of the output."
	btn_bake_ps.pressed.connect(_on_bake_packedscene_pressed)
	vbox.add_child(btn_bake_ps)

	var bake_update_header := Label.new()
	bake_update_header.text = "Bake Update:"
	vbox.add_child(bake_update_header)

	var targets_label := Label.new()
	targets_label.name = "BakeUpdateTargetsLabel"
	targets_label.text = "0 target(s)"
	vbox.add_child(targets_label)

	var hbox := HBoxContainer.new()
	vbox.add_child(hbox)

	var btn_add_target := Button.new()
	btn_add_target.text = "+"
	btn_add_target.tooltip_text = "Add selected node as bake update target"
	btn_add_target.pressed.connect(_on_add_bake_target_pressed)
	hbox.add_child(btn_add_target)

	var btn_clear_targets := Button.new()
	btn_clear_targets.text = "Clear"
	btn_clear_targets.pressed.connect(_on_clear_bake_targets_pressed)
	hbox.add_child(btn_clear_targets)

	var btn_update := Button.new()
	btn_update.text = "Bake Update"
	btn_update.tooltip_text = "Update target nodes by name matching with output."
	btn_update.pressed.connect(_on_bake_update_pressed)
	vbox.add_child(btn_update)

	var keep_xform := CheckBox.new()
	keep_xform.name = "KeepTransform"
	keep_xform.text = "Keep Transform"
	vbox.add_child(keep_xform)


# ── Events ──

func _build_events_section(parent: VBoxContainer):
	var fold := _make_foldout("Events", parent)
	var vbox := _fold_body(fold)

	var lbl := Label.new()
	lbl.text = "TODO: event bindings"
	vbox.add_child(lbl)



# ── Parameters ──

func _build_parameters_section(parent: VBoxContainer):
	print("[HEPI:224] _build_parameters_section, _houdini_node=%s" % _houdini_node)
	if _houdini_node == null:
		print("[HEPI:226] _houdini_node is null, returning")
		return

	var fold := _make_foldout("Asset Parameters", parent)
	var vbox := _fold_body(fold)
	vbox.name = "ParametersBody"

	var parm_list: PackedStringArray = _houdini_node.getParameterList()
	print("[HEPI:233] getParameterList returned %d params: %s" % [parm_list.size(), parm_list])
	if parm_list.is_empty():
		vbox.add_child(_placeholder("No parameters exposed"))
		return

	var session = _get_session()
	print("[HEPI:238] _get_session=%s" % session)
	if session == null:
		vbox.add_child(_placeholder("No session — parameters cannot be inspected"))
		return

	for parm_name in parm_list:
		var parm_info := _query_parm_info(session, parm_name)
		print("[HEPI:244] param '%s' info=%s" % [parm_name, parm_info])
		if parm_info.is_empty():
			continue

		var parm_type: int = parm_info.get("type", -1)
		var current_val = _houdini_node.getParameter(parm_name)
		_add_param_row(vbox, parm_name, parm_info, parm_type, current_val)


func _query_parm_info(session: HESession, parm_name: String) -> Dictionary:
	if _houdini_node == null:
		return {}

	var val = _houdini_node.getParameter(parm_name)
	if val == null:
		return {}

	var info: Dictionary = {}
	var t = typeof(val)

	if t == TYPE_ARRAY:
		info["is_array"] = true
		var arr: Array = val
		info["size"] = arr.size()
		if arr.size() > 0:
			match typeof(arr[0]):
				TYPE_INT:    info["type"] = HEBindEnum.HAPI_PARMTYPE_INT
				TYPE_FLOAT:  info["type"] = HEBindEnum.HAPI_PARMTYPE_FLOAT
				_:           info["type"] = HEBindEnum.HAPI_PARMTYPE_STRING
		else:
			info["type"] = HEBindEnum.HAPI_PARMTYPE_STRING
	elif t == TYPE_INT:
		info["type"] = HEBindEnum.HAPI_PARMTYPE_INT
	elif t == TYPE_FLOAT:
		info["type"] = HEBindEnum.HAPI_PARMTYPE_FLOAT
	else:
		info["type"] = HEBindEnum.HAPI_PARMTYPE_STRING

	info["label"] = parm_name
	return info


func _on_parameter_changed(parm_name: String, value):
	print("[HEPI:357] _on_parameter_changed: %s = %s, auto_cook=%s" % [parm_name, value, _hda_root.auto_cook if _hda_root else "N/A"])
	_houdini_node.setParameter(parm_name, value)
	if _hda_root and _hda_root.auto_cook:
		print("[HEPI:360] cooking...")
		_houdini_node.cook()
		var ok = await _houdini_node.cookFinished
		print("[HEPI:363] cook finished, ok=%s" % ok)
		if ok:
			_data_node.cook_state = HEngineAssetData.CookState.SUCCESS
			_data_node.save_parameter_preset()
			_hda_root.remove_all_outputs()
			HEngineUtility._generate_output_nodes(_hda_root, _houdini_node)


# ── Param builders ──

func _add_param_row(parent: VBoxContainer, parm_name: String, parm_info: Dictionary, parm_type: int, val):
	if parm_info.get("is_array", false):
		_build_array_section(parent, parm_name, parm_info, parm_type, val)
		return

	var row := HBoxContainer.new()
	parent.add_child(row)

	var lbl := Label.new()
	lbl.text = parm_name
	lbl.size_flags_horizontal = SIZE_EXPAND_FILL
	row.add_child(lbl)

	if parm_type == HEBindEnum.HAPI_PARMTYPE_INT:
		_add_int_field(row, parm_name, -1, val)
	elif parm_type == HEBindEnum.HAPI_PARMTYPE_FLOAT:
		_add_float_field(row, parm_name, -1, val)
	else:
		_add_string_field(row, parm_name, -1, val)


func _build_array_section(parent: VBoxContainer, parm_name: String, parm_info: Dictionary, parm_type: int, val):
	var arr: Array
	if parm_type == HEBindEnum.HAPI_PARMTYPE_INT:
		var typed: Array[int] = []
		for v in val: typed.append(int(v) if typeof(v) in [TYPE_INT, TYPE_FLOAT] else 0)
		arr = typed
	elif parm_type == HEBindEnum.HAPI_PARMTYPE_FLOAT:
		var typed: Array[float] = []
		for v in val: typed.append(float(v) if typeof(v) in [TYPE_INT, TYPE_FLOAT] else 0.0)
		arr = typed
	else:
		var typed: Array[String] = []
		for v in val: typed.append(str(v) if v != null else "")
		arr = typed

	var container := VBoxContainer.new()
	container.name = "Arr_" + parm_name

	var body := VBoxContainer.new()
	body.name = "Body"

	var title_btn := Button.new()
	title_btn.text = "▶ %s (%d)" % [parm_name, val.size()]
	title_btn.alignment = HORIZONTAL_ALIGNMENT_LEFT
	title_btn.flat = true
	title_btn.pressed.connect(func():
		body.visible = not body.visible
		title_btn.text = ("▼" if body.visible else "▶") + " %s (%d)" % [parm_name, val.size()]
	)

	container.add_child(title_btn)
	container.add_child(body)
	body.visible = false
	parent.add_child(container)

	_rebuild_array_body(body, parm_name, parm_type, val)


func _rebuild_array_body(body: VBoxContainer, parm_name: String, parm_type: int, arr: Array):
	for child in body.get_children():
		child.queue_free()

	for i in range(arr.size()):
		var row := HBoxContainer.new()
		body.add_child(row)

		var idx_lbl := Label.new()
		idx_lbl.text = "[%d]" % i
		idx_lbl.custom_minimum_size = Vector2(30, 0)
		row.add_child(idx_lbl)

		if parm_type == HEBindEnum.HAPI_PARMTYPE_INT:
			_add_int_field(row, parm_name, i, arr[i])
		elif parm_type == HEBindEnum.HAPI_PARMTYPE_FLOAT:
			_add_float_field(row, parm_name, i, arr[i])
		else:
			_add_string_field(row, parm_name, i, arr[i])

		var btn_del := Button.new()
		btn_del.text = "×"
		btn_del.custom_minimum_size = Vector2(30, 0)
		btn_del.pressed.connect(func():
			arr.remove_at(i)
			_on_parameter_changed(parm_name, arr.duplicate())
			_rebuild_array_body(body, parm_name, parm_type, arr)
		)
		row.add_child(btn_del)

	var btn_add := Button.new()
	btn_add.text = "+ Add"
	btn_add.pressed.connect(func():
		match parm_type:
			HEBindEnum.HAPI_PARMTYPE_INT: arr.append(0)
			HEBindEnum.HAPI_PARMTYPE_FLOAT: arr.append(0.0)
			_: arr.append("")
		_on_parameter_changed(parm_name, arr.duplicate())
		_rebuild_array_body(body, parm_name, parm_type, arr)
	)
	body.add_child(btn_add)


func _add_int_field(parent: HBoxContainer, parm_name: String, idx: int, val):
	var spin := SpinBox.new()
	spin.step = 1.0
	spin.allow_lesser = true
	spin.allow_greater = true
	spin.value = int(val) if typeof(val) in [TYPE_INT, TYPE_FLOAT] else 0
	spin.custom_minimum_size = Vector2(60, 0)
	spin.value_changed.connect(func(v):
		_commit_param(parm_name, idx, int(v))
	)
	parent.add_child(spin)


func _add_float_field(parent: HBoxContainer, parm_name: String, idx: int, val):
	var spin := SpinBox.new()
	spin.step = 0.01
	spin.allow_lesser = true
	spin.allow_greater = true
	spin.value = float(val) if typeof(val) in [TYPE_INT, TYPE_FLOAT] else 0.0
	spin.custom_minimum_size = Vector2(80, 0)
	spin.value_changed.connect(func(v):
		_commit_param(parm_name, idx, v)
	)
	parent.add_child(spin)


func _add_string_field(parent: HBoxContainer, parm_name: String, idx: int, val):
	var le := LineEdit.new()
	le.text = str(val) if val != null else ""
	le.custom_minimum_size = Vector2(80, 0)
	le.focus_exited.connect(func():
		_commit_param(parm_name, idx, le.text)
	)
	parent.add_child(le)


func _commit_param(parm_name: String, idx: int, value):
	print("[HEPI:commit] _commit_param: %s[%d] = %s" % [parm_name, idx, value])
	if idx < 0:
		_on_parameter_changed(parm_name, value)
		return

	var node = _get_houdini_node()
	if node == null:
		print("[HEPI:commit] node is null!")
		return

	var arr: Array = node.getParameter(parm_name)
	print("[HEPI:commit] got array from Houdini: %s (size=%d)" % [arr, arr.size()])
	if idx >= arr.size():
		print("[HEPI:commit] idx %d >= size %d, skipping" % [idx, arr.size()])
		return

	arr[idx] = value
	print("[HEPI:commit] sending back: %s" % arr)
	_on_parameter_changed(parm_name, arr.duplicate())


func _get_houdini_node() -> HENode:
	return _houdini_node


# ── Actions ──

func _on_rebuild_pressed():
	if _hda_root:
		var new_root = HEngineUtility.rebuild_asset(_hda_root)
		if new_root:
			await HEngineUtility.cook_and_generate(new_root)


func _on_recook_pressed():
	if _hda_root:
		await HEngineUtility.recook_asset(_hda_root)


func _on_keep_output_pressed():
	if _hda_root:
		_hda_root.clear_plugin_data()


func _on_duplicate_pressed():
	if _hda_root:
		var dup = HEngineUtility.duplicate_asset(_hda_root)
		if dup:
			await HEngineUtility.cook_and_generate(dup)


func _on_reset_parameters_pressed():
	if _hda_root:
		HEngineUtility.reset_parameters(_hda_root)
		await HEngineUtility.recook_asset(_hda_root)
		_rebuild_ui()


func _on_bake_gameobject_pressed():
	if _hda_root:
		HEngineUtility.bake_gameobject(_hda_root)


func _on_bake_packedscene_pressed():
	if _hda_root == null:
		return
	var output = _hda_root.find_output_nodes()
	if output.is_empty():
		return

	var bake_root := Node3D.new()
	bake_root.name = _hda_root.name + "_Baked"
	for node in output:
		var dup = node.duplicate()
		bake_root.add_child(dup)
		dup.owner = null

	var packed := PackedScene.new()
	packed.pack(bake_root)
	bake_root.queue_free()

	var dir := "res://HoudiniEngineAssetCache/Baked/"
	DirAccess.make_dir_recursive_absolute(dir)
	var fname := "%s.tscn" % _hda_root.name
	ResourceSaver.save(packed, dir + fname)


func _on_add_bake_target_pressed():
	var selection = _get_editor_selection()
	if selection == null:
		return
	var nodes = selection.get_selected_nodes()
	for node in nodes:
		if node is Node3D and node not in _bake_update_targets:
			_bake_update_targets.append(node)
	_update_bake_targets_label()


func _on_clear_bake_targets_pressed():
	_bake_update_targets.clear()
	_update_bake_targets_label()


func _on_bake_update_pressed():
	if _hda_root and not _bake_update_targets.is_empty():
		var keep_xform = _get_keep_transform()
		HEngineUtility.bake_updates(_hda_root, _bake_update_targets, keep_xform)


func _get_keep_transform() -> bool:
	for child in _scroll.get_children():
		if child is VBoxContainer:
			for c in child.get_children():
				if c is VBoxContainer:
					for cc in c.get_children():
						if cc.name == "KeepTransform" and cc is CheckBox:
							return (cc as CheckBox).button_pressed
	return false


func _update_bake_targets_label():
	for child in _scroll.get_children():
		if child is VBoxContainer:
			for c in child.get_children():
				if c is VBoxContainer:
					for cc in c.get_children():
						if cc.name == "BakeUpdateTargetsLabel" and cc is Label:
							(cc as Label).text = "%d target(s)" % _bake_update_targets.size()
							return


# ── Preset ──

func _on_save_preset_pressed():
	if _hda_root == null:
		return
	var dialog := EditorFileDialog.new()
	dialog.add_filter("*.tres", "Houdini Preset")
	dialog.file_mode = EditorFileDialog.FILE_MODE_SAVE_FILE
	dialog.access = EditorFileDialog.ACCESS_RESOURCES
	dialog.file_selected.connect(_do_save_preset)
	get_tree().get_root().add_child(dialog)
	dialog.popup_centered_ratio(0.5)


func _do_save_preset(path: String):
	if _data_node == null:
		return
	_data_node.save_parameter_preset()
	var preset := HEnginePreset.new()
	preset.parameter_values = _data_node.parameter_presets.duplicate()
	preset.hda_path = _hda_root.hda_path
	ResourceSaver.save(preset, path)


func _on_load_preset_pressed():
	var dialog := EditorFileDialog.new()
	dialog.add_filter("*.tres", "Houdini Preset")
	dialog.file_mode = EditorFileDialog.FILE_MODE_OPEN_FILE
	dialog.access = EditorFileDialog.ACCESS_RESOURCES
	dialog.file_selected.connect(_do_load_preset)
	get_tree().get_root().add_child(dialog)
	dialog.popup_centered_ratio(0.5)


func _do_load_preset(path: String):
	var preset: HEnginePreset = ResourceLoader.load(path, "HEnginePreset", ResourceLoader.CACHE_MODE_IGNORE)
	if preset == null:
		push_error("Failed to load preset: %s" % path)
		return
	if _data_node:
		_data_node.parameter_presets = preset.parameter_values.duplicate()
		if _hda_root and _hda_root.auto_cook:
			await HEngineUtility.recook_asset(_hda_root)
			_rebuild_ui()


# ── UI Helpers ──

func _make_foldout(title: String, parent: VBoxContainer) -> VBoxContainer:
	var container := VBoxContainer.new()
	container.name = "Fold_" + title.replace(" ", "_")

	var title_btn := Button.new()
	title_btn.text = "▼ " + title
	title_btn.alignment = HORIZONTAL_ALIGNMENT_LEFT
	title_btn.flat = true

	var body := VBoxContainer.new()
	body.name = "Body"

	var _toggle := func():
		body.visible = not body.visible
		if body.visible:
			title_btn.text = "▼ " + title
		else:
			title_btn.text = "▶ " + title

	title_btn.pressed.connect(_toggle)
	container.add_child(title_btn)
	container.add_child(body)
	parent.add_child(container)
	return container


func _fold_body(container: VBoxContainer) -> VBoxContainer:
	for child in container.get_children():
		if child.name == "Body":
			return child
	return container


func _get_editor_selection():
	var tree = get_tree()
	if tree:
		var editor_interface = tree.get_root()
		if editor_interface and editor_interface.has_method("get_editor_interface"):
			return null
	return null


func _dump_tree(node: Node, depth: int):
	var indent := "  ".repeat(depth)
	if node is Control:
		var ctrl = node as Control
		print("%s%s [size=%s, min=%s, visible=%s]" % [indent, ctrl.name, ctrl.size, ctrl.custom_minimum_size, ctrl.visible])
	else:
		print("%s%s" % [indent, node.name])
	for child in node.get_children():
		_dump_tree(child, depth + 1)
