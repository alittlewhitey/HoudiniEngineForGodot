@tool
extends Control

## HEU Parameters dock. Selected HEUAssetRoot is provided by the main
## EditorPlugin. The dock owns parameter row construction, debounced auto-cook
## and cook status display.

var _root: Node = null
var _node: HEGNode = null
var _scroll: ScrollContainer
var _content: VBoxContainer
var _header_row: HBoxContainer
var _header_label: Label
var _error_label: Label
var _search_edit: LineEdit
var _auto_cook_check: CheckBox
var _rows: Dictionary = {}
var _refresh_pending := false


func _ready() -> void:
	size_flags_horizontal = SIZE_EXPAND_FILL
	size_flags_vertical = SIZE_EXPAND_FILL
	_build_ui()
	_update_dock_minimum_size()
	HEUSessionService.instance().session_changed.connect(_on_session_changed)
	_show_placeholder("No HEU Asset selected")


func _exit_tree() -> void:
	_detach_root()
	if HEUSessionService.instance().session_changed.is_connected(_on_session_changed):
		HEUSessionService.instance().session_changed.disconnect(_on_session_changed)


func set_asset_root(root: Node) -> void:
	if _root == root:
		_rebuild_parameters()
		return
	_detach_root()
	_root = root
	_attach_root()
	_rebuild_parameters()


func _build_ui() -> void:
	var vbox := VBoxContainer.new()
	vbox.set_anchors_preset(Control.PRESET_FULL_RECT)
	vbox.add_theme_constant_override("separation", 4)
	add_child(vbox)

	_header_row = HBoxContainer.new()
	vbox.add_child(_header_row)

	_header_label = Label.new()
	_header_label.text = "HEU Asset"
	_header_label.custom_minimum_size = Vector2(0, 0)
	_header_label.size_flags_horizontal = SIZE_EXPAND_FILL
	_header_label.text_overrun_behavior = TextServer.OVERRUN_TRIM_ELLIPSIS
	_header_label.clip_text = true
	_header_row.add_child(_header_label)

	_auto_cook_check = CheckBox.new()
	_auto_cook_check.text = "Auto Cook"
	_auto_cook_check.toggled.connect(_on_auto_cook_toggled)
	_header_row.add_child(_auto_cook_check)

	var recook_button := Button.new()
	recook_button.text = "Recook"
	recook_button.pressed.connect(_on_recook_pressed)
	_header_row.add_child(recook_button)

	var rebuild_button := Button.new()
	rebuild_button.text = "Rebuild"
	rebuild_button.pressed.connect(_on_rebuild_pressed)
	_header_row.add_child(rebuild_button)

	var reset_button := Button.new()
	reset_button.text = "Reset"
	reset_button.pressed.connect(_on_reset_pressed)
	_header_row.add_child(reset_button)

	var search_row := HBoxContainer.new()
	vbox.add_child(search_row)

	var search_label := Label.new()
	search_label.text = "Search"
	search_row.add_child(search_label)

	_search_edit = LineEdit.new()
	_search_edit.placeholder_text = "Filter parameters..."
	_search_edit.size_flags_horizontal = SIZE_EXPAND_FILL
	_search_edit.text_changed.connect(_on_search_changed)
	search_row.add_child(_search_edit)

	_error_label = Label.new()
	_error_label.add_theme_color_override("font_color", Color(0.90, 0.30, 0.24))
	_error_label.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	_error_label.visible = false
	vbox.add_child(_error_label)

	_scroll = ScrollContainer.new()
	_scroll.size_flags_horizontal = SIZE_EXPAND_FILL
	_scroll.size_flags_vertical = SIZE_EXPAND_FILL
	_scroll.horizontal_scroll_mode = ScrollContainer.SCROLL_MODE_DISABLED
	vbox.add_child(_scroll)

	_content = VBoxContainer.new()
	_content.size_flags_horizontal = SIZE_EXPAND_FILL
	_content.size_flags_vertical = SIZE_EXPAND_FILL
	_scroll.add_child(_content)


func _update_dock_minimum_size() -> void:
	if _header_row == null:
		return
	var min_width := 0.0
	var control_count := 0
	for child in _header_row.get_children():
		if child == _header_label:
			continue
		min_width += child.get_combined_minimum_size().x
		control_count += 1
	if control_count > 1:
		min_width += _header_row.get_theme_constant("separation") * float(control_count - 1)
	custom_minimum_size.x = min_width


func get_dock_minimum_width_for_test() -> float:
	return custom_minimum_size.x


func _attach_root() -> void:
	if _root == null:
		return
	if _root.cook_started.is_connected(_on_root_cook_started):
		return
	_root.cook_started.connect(_on_root_cook_started)
	_root.cook_finished.connect(_on_root_cook_finished)
	_root.outputs_rebuilt.connect(_on_outputs_rebuilt)


func _detach_root() -> void:
	if _root == null:
		return
	if _root.cook_started.is_connected(_on_root_cook_started):
		_root.cook_started.disconnect(_on_root_cook_started)
	if _root.cook_finished.is_connected(_on_root_cook_finished):
		_root.cook_finished.disconnect(_on_root_cook_finished)
	if _root.outputs_rebuilt.is_connected(_on_outputs_rebuilt):
		_root.outputs_rebuilt.disconnect(_on_outputs_rebuilt)


func get_visible_row_count() -> int:
	return _rows.size()


func get_folder_titles() -> Array[String]:
	var result: Array[String] = []
	_collect_folder_titles(_content, result)
	return result


func _collect_folder_titles(node: Node, result: Array[String]) -> void:
	if node == null:
		return
	for child in node.get_children():
		if child is Button and child.flat and (child.text.begins_with("▼ ") or child.text.begins_with("▶ ")):
			result.append(child.text.substr(2))
		_collect_folder_titles(child, result)


func _on_session_changed(active: bool, _session_type: int) -> void:
	if _root == null:
		if not active:
			_show_placeholder("No Session")
		return
	_rebuild_parameters()


func _on_auto_cook_toggled(enabled: bool) -> void:
	if _root != null:
		_root.auto_cook = enabled


func _on_recook_pressed() -> void:
	if _root != null and _root.has_method("recook"):
		_root.recook()


func _on_rebuild_pressed() -> void:
	if _root != null and _root.has_method("rebuild"):
		_root.rebuild()
		_rebuild_parameters()


func _on_reset_pressed() -> void:
	if _root != null and _root.has_method("reset_parameters"):
		_root.reset_parameters()
		_rebuild_parameters()
		if _root.auto_cook and _root.has_method("recook"):
			_root.recook()


func _on_search_changed(_text: String) -> void:
	_rebuild_parameters()


func _on_root_cook_started() -> void:
	_header_label.text = "Cooking %s..." % (_root.name if _root != null else "")
	_error_label.visible = false
	_set_rows_enabled(false)


func _on_root_cook_finished(success: bool) -> void:
	if _root == null:
		return
	if success:
		_header_label.text = _root.name
		_error_label.visible = false
		_refresh_row_values()
	else:
		_header_label.text = "%s (cook failed)" % _root.name
		_error_label.text = _root.get_last_error()
		_error_label.visible = not _error_label.text.is_empty()
	_set_rows_enabled(true)


func _on_outputs_rebuilt() -> void:
	if _refresh_pending:
		return


func _show_placeholder(text: String) -> void:
	if _content == null:
		return
	for child in _content.get_children():
		child.queue_free()
	_rows.clear()
	var label := Label.new()
	label.text = text
	label.modulate = Color(1, 1, 1, 0.55)
	_content.add_child(label)
	_node = null
	_auto_cook_check.set_pressed_no_signal(false)


func _rebuild_parameters() -> void:
	if _content == null:
		return
	for child in _content.get_children():
		child.queue_free()
	_rows.clear()

	if _root == null:
		_show_placeholder("No HEU Asset selected")
		return

	_auto_cook_check.set_pressed_no_signal(bool(_root.auto_cook))

	if not HEUSessionService.instance().is_active():
		_show_placeholder("No Houdini session. Use the HEU Session tab to start one.")
		return

	if not _root.has_method("ensure_houdini_node") or not _root.ensure_houdini_node():
		_show_placeholder("Failed to create Houdini node. See Output for errors.")
		return

	_node = _root.get_he_node()
	if _node == null or not _node.isValid():
		_show_placeholder("Houdini node is invalid. Try Rebuild.")
		return

	_root.refresh_parameters()
	var infos: Array = _node.getParameterInfos()
	if infos.is_empty():
		_show_placeholder("No parameters exposed by this operator.")
		return

	_header_label.text = "%s - %s" % [_root.name, _node.getName()]
	_header_label.tooltip_text = _header_label.text
	_update_error_label()

	var search_text := _search_edit.text.strip_edges().to_lower()
	if not search_text.is_empty():
		_build_filtered_parameters(infos, search_text)
	else:
		_build_parameter_tree(infos)


func _build_filtered_parameters(infos: Array, search_text: String) -> void:
	var added := 0
	for info in infos:
		var parm_name: String = info.get("name", "")
		var label: String = info.get("label", parm_name)
		if not search_text.is_empty() \
			and search_text not in parm_name.to_lower() \
			and search_text not in label.to_lower():
			continue
		if _is_layout_parameter(info):
			continue
		var row := _make_row(info)
		if row != null:
			_content.add_child(row)
			added += 1
	if added == 0:
		_show_placeholder("No parameters match '%s'" % _search_edit.text)


func _build_parameter_tree(infos: Array) -> void:
	var by_id := {}
	var children := {}
	var roots: Array = []
	for info in infos:
		var id: int = info.get("id", -1)
		by_id[id] = info
		if not children.has(id):
			children[id] = []
	for info in infos:
		var parent_id: int = info.get("parentId", -1)
		if parent_id == -1 or not by_id.has(parent_id):
			roots.append(info)
		else:
			children[parent_id].append(info)
	roots.sort_custom(func(a, b) -> bool: return int(a.get("childIndex", 0)) < int(b.get("childIndex", 0)))

	for info in roots:
		var parm_type: int = info.get("type", -1)
		if parm_type == HEGEnum.HAPI_PARMTYPE_FOLDERLIST:
			# A FOLDERLIST is not a visible group. Its child folders (for example
			# "Board Settings" under "fd_board_settings") are the real UI folders.
			_add_folderlist_children(info, children, _content)
		elif parm_type == HEGEnum.HAPI_PARMTYPE_FOLDER:
			_add_folder(info, children)
		elif parm_type == HEGEnum.HAPI_PARMTYPE_LABEL:
			var label := Label.new()
			label.text = info.get("label", info.get("name", ""))
			_content.add_child(label)
		elif parm_type == HEGEnum.HAPI_PARMTYPE_SEPARATOR:
			_content.add_child(HSeparator.new())
		else:
			var row := _make_row(info)
			if row != null:
				_content.add_child(row)
			_add_children(info.get("id", -1), children)


func _add_folder(info: Dictionary, children: Dictionary, parent: VBoxContainer = null) -> void:
	var folder_id: int = info.get("id", -1)
	var title: String = info.get("label", "")
	if title.is_empty():
		title = info.get("name", "Folder")
	var target := _content if parent == null else parent
	var toggle := Button.new()
	toggle.text = "▼ %s" % title
	toggle.alignment = HORIZONTAL_ALIGNMENT_LEFT
	toggle.flat = true
	target.add_child(toggle)

	var body := VBoxContainer.new()
	body.name = "Folder_%d" % folder_id
	target.add_child(body)

	toggle.pressed.connect(func() -> void:
		body.visible = not body.visible
		toggle.text = ("▼ " if body.visible else "▶ ") + title
	)

	var child_infos: Array = children.get(folder_id, [])
	child_infos.sort_custom(func(a, b) -> bool: return int(a.get("childIndex", 0)) < int(b.get("childIndex", 0)))
	for child_info in child_infos:
		var child_type: int = child_info.get("type", -1)
		if child_type == HEGEnum.HAPI_PARMTYPE_FOLDERLIST:
			_add_folderlist_children(child_info, children, body)
		elif child_type == HEGEnum.HAPI_PARMTYPE_FOLDER:
			_add_folder(child_info, children, body)
		elif child_type == HEGEnum.HAPI_PARMTYPE_LABEL:
			var label := Label.new()
			label.text = child_info.get("label", child_info.get("name", ""))
			body.add_child(label)
		elif child_type == HEGEnum.HAPI_PARMTYPE_SEPARATOR:
			body.add_child(HSeparator.new())
		else:
			var row := _make_row(child_info)
			if row != null:
				body.add_child(row)
			_add_children(child_info.get("id", -1), children, body)


func _add_folderlist_children(info: Dictionary, children: Dictionary, parent: VBoxContainer) -> void:
	var folder_id: int = info.get("id", -1)
	var child_infos: Array = children.get(folder_id, [])
	if child_infos.is_empty():
		return
	child_infos.sort_custom(func(a, b) -> bool: return int(a.get("childIndex", 0)) < int(b.get("childIndex", 0)))
	for child_info in child_infos:
		var child_type: int = child_info.get("type", -1)
		if child_type == HEGEnum.HAPI_PARMTYPE_FOLDERLIST:
			_add_folderlist_children(child_info, children, parent)
		elif child_type == HEGEnum.HAPI_PARMTYPE_FOLDER:
			_add_folder(child_info, children, parent)
		elif child_type == HEGEnum.HAPI_PARMTYPE_LABEL:
			var label := Label.new()
			label.text = child_info.get("label", child_info.get("name", ""))
			parent.add_child(label)
		elif child_type == HEGEnum.HAPI_PARMTYPE_SEPARATOR:
			parent.add_child(HSeparator.new())
		else:
			var row := _make_row(child_info)
			if row != null:
				parent.add_child(row)
			_add_children(child_info.get("id", -1), children, parent)


func _add_children(parent_id: int, children: Dictionary, body: VBoxContainer = _content) -> void:
	var child_infos: Array = children.get(parent_id, [])
	if child_infos.is_empty():
		return
	child_infos.sort_custom(func(a, b) -> bool: return int(a.get("childIndex", 0)) < int(b.get("childIndex", 0)))
	for child_info in child_infos:
		if _is_layout_parameter(child_info):
			continue
		var row := _make_row(child_info)
		if row != null:
			body.add_child(row)


func _is_layout_parameter(info: Dictionary) -> bool:
	var parm_type: int = info.get("type", -1)
	return parm_type == HEGEnum.HAPI_PARMTYPE_FOLDERLIST \
		or parm_type == HEGEnum.HAPI_PARMTYPE_FOLDER \
		or parm_type == HEGEnum.HAPI_PARMTYPE_LABEL \
		or parm_type == HEGEnum.HAPI_PARMTYPE_SEPARATOR


func _make_row(info: Dictionary) -> Control:
	var parm_name: String = info.get("name", "")
	if parm_name.is_empty():
		return null
	if bool(info.get("invisible", false)):
		return null

	var value: Variant = _node.getParameter(parm_name)
	var row := HEUParameterWidgetFactory.create_row(
		info,
		value,
		_node,
		Callable(self, "_on_parameter_commit"),
		Callable(self, "_on_structure_changed")
	)
	if row == null:
		return null

	if bool(info.get("disabled", false)) or int(info.get("permissions", 0)) == HEGEnum.HAPI_PERMISSIONS_READ_ONLY:
		row.set_enabled(false)

	_rows[parm_name] = row
	return row


func _on_parameter_commit(parm_name: String, value: Variant) -> void:
	if _root == null:
		return
	# HEUAssetRoot owns debounce + pending-cook handling.
	_root.set_parameter(parm_name, value)


func _on_structure_changed() -> void:
	# Multiparm add/remove changes the C++ parameter cache and usually also the
	# visible instance rows. Rebuild once, not on every mouse click.
	if _refresh_pending:
		return
	_refresh_pending = true
	call_deferred(&"_refresh_structure")


func _refresh_structure() -> void:
	_refresh_pending = false
	if _root != null:
		_rebuild_parameters()


func _refresh_row_values() -> void:
	if _node == null or not _node.isValid():
		return
	for parm_name in _rows:
		var row = _rows[parm_name]
		var value: Variant = _node.getParameter(parm_name)
		if value != null and row != null:
			row.set_value(value)


func _set_rows_enabled(enabled: bool) -> void:
	for parm_name in _rows:
		var row = _rows[parm_name]
		if row != null:
			row.set_enabled(enabled)


func _update_error_label() -> void:
	if _root == null:
		return
	var error_text: String = _root.get_last_error()
	if error_text.is_empty() and _node != null:
		error_text = _node.getLastCookError()
	_error_label.text = error_text
	_error_label.visible = not error_text.is_empty()
