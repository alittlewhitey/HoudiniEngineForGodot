@tool
class_name HEUParameterWidgetFactory
extends RefCounted

## Builds editor controls from HAPI_ParmInfo dictionaries returned by
## HEGNode.getParameterInfos(). All writes go through a commit callable so the
## parameter dock owns debounce + cook scheduling.



const EDITOR_MIN_WIDTH := 80.0
const LABEL_MIN_WIDTH := 56.0


static func create_row(info: Dictionary, value: Variant, node: HEGNode, commit: Callable, refresh_requested: Callable = Callable()) -> HEUParameterRow:
	var parm_name: String = info.get("name", "")
	var label: String = info.get("label", parm_name)
	if label.is_empty():
		label = parm_name
	var help: String = info.get("help", "")
	return HEUParameterRow.new(parm_name, label, help, info, value, node, commit, refresh_requested)


class HEUParameterRow:
	extends HBoxContainer

	var parm_name: String
	var info: Dictionary
	var node: HEGNode
	var commit: Callable
	var refresh_requested: Callable
	var _suppress_callbacks := false
	var _label: Label
	var _editors: Array[Control] = []
	var _array_values: Array = []
	var _array_rows: VBoxContainer = null
	var _array_header: Label = null
	var _array_item_lines: Array[LineEdit] = []
	var _array_remove_buttons: Array[Button] = []
	var _array_add_button: Button = null

	func _init(p_name: String, p_label: String, p_help: String, p_info: Dictionary, p_value: Variant, p_node: HEGNode, p_commit: Callable, p_refresh: Callable) -> void:
		parm_name = p_name
		info = p_info
		node = p_node
		commit = p_commit
		refresh_requested = p_refresh

		custom_minimum_size = Vector2(0, 30)
		size_flags_horizontal = SIZE_EXPAND_FILL

		_label = Label.new()
		_label.text = p_label
		_label.custom_minimum_size = Vector2(HEUParameterWidgetFactory.LABEL_MIN_WIDTH, 0)
		_label.size_flags_horizontal = SIZE_EXPAND_FILL
		_label.size_flags_stretch_ratio = 1.0
		_label.size_flags_vertical = SIZE_SHRINK_CENTER
		_label.text_overrun_behavior = TextServer.OVERRUN_TRIM_ELLIPSIS
		if not p_help.is_empty():
			_label.tooltip_text = p_help
		add_child(_label)

		_create_editor(p_value)


	func set_value(value: Variant) -> void:
		_suppress_callbacks = true
		_write_value(value)
		_suppress_callbacks = false


	func set_enabled(enabled: bool) -> void:
		for editor in _editors:
			if editor is SpinBox:
				editor.editable = enabled
			elif editor is Button or editor is LineEdit or editor is OptionButton or editor is CheckButton or editor is ColorPickerButton:
				editor.disabled = not enabled
		for line in _array_item_lines:
			if is_instance_valid(line):
				line.editable = enabled


	func _create_editor(value: Variant) -> void:
		var parm_type: int = info.get("type", -1)
		var script_type: int = info.get("scriptType", -1)

		if parm_type == HEGEnum.HAPI_PARMTYPE_LABEL:
			var editor := Label.new()
			editor.text = info.get("label", parm_name)
			editor.size_flags_vertical = SIZE_SHRINK_CENTER
			add_child(editor)
			_editors.append(editor)
			return

		if parm_type == HEGEnum.HAPI_PARMTYPE_SEPARATOR:
			var separator := HSeparator.new()
			separator.size_flags_horizontal = SIZE_EXPAND_FILL
			separator.size_flags_vertical = SIZE_SHRINK_CENTER
			add_child(separator)
			_editors.append(separator)
			return

		if parm_type == HEGEnum.HAPI_PARMTYPE_TOGGLE or script_type == HEGEnum.HAPI_PRM_SCRIPT_TYPE_TOGGLE:
			var check := CheckButton.new()
			check.text = ""
			check.button_pressed = bool(value)
			check.toggled.connect(_on_check_toggled)
			add_child(check)
			_editors.append(check)
			return

		if parm_type == HEGEnum.HAPI_PARMTYPE_BUTTON or script_type == HEGEnum.HAPI_PRM_SCRIPT_TYPE_BUTTON:
			var button := Button.new()
			button.text = info.get("label", parm_name)
			button.pressed.connect(_on_button_pressed)
			add_child(button)
			_editors.append(button)
			return

		if parm_type == HEGEnum.HAPI_PARMTYPE_COLOR or script_type == HEGEnum.HAPI_PRM_SCRIPT_TYPE_COLOR or script_type == HEGEnum.HAPI_PRM_SCRIPT_TYPE_COLOR4:
			var picker := ColorPickerButton.new()
			picker.color = _variant_to_color(value)
			picker.color_changed.connect(_on_color_changed)
			add_child(picker)
			_editors.append(picker)
			return

		match script_type:
			HEGEnum.HAPI_PRM_SCRIPT_TYPE_VECTOR2:
				_create_vector_editor(2, value)
				return
			HEGEnum.HAPI_PRM_SCRIPT_TYPE_VECTOR3:
				_create_vector_editor(3, value)
				return
			HEGEnum.HAPI_PRM_SCRIPT_TYPE_VECTOR4:
				_create_vector_editor(4, value)
				return

		if value is Array:
			_create_array_editor(value)
			return

		if parm_type == HEGEnum.HAPI_PARMTYPE_INT:
			var spin := _make_int_spin(value)
			add_child(spin)
			_editors.append(spin)
			return

		if parm_type == HEGEnum.HAPI_PARMTYPE_FLOAT:
			var spin := _make_float_spin(value)
			add_child(spin)
			_editors.append(spin)
			return

		if parm_type == HEGEnum.HAPI_PARMTYPE_PATH_FILE \
			or parm_type == HEGEnum.HAPI_PARMTYPE_PATH_FILE_DIR \
			or parm_type == HEGEnum.HAPI_PARMTYPE_PATH_FILE_GEO \
			or parm_type == HEGEnum.HAPI_PARMTYPE_PATH_FILE_IMAGE:
			_create_path_editor(value, script_type)
			return

		if parm_type == HEGEnum.HAPI_PARMTYPE_NODE:
			_create_node_editor(value)
			return

		if parm_type == HEGEnum.HAPI_PARMTYPE_STRING:
			var choices: Array = node.getParameterChoices(parm_name)
			if not choices.is_empty():
				_create_choice_editor(value, choices)
			else:
				_create_string_editor(value)
			return

		if parm_type == HEGEnum.HAPI_PARMTYPE_MULTIPARMLIST:
			_create_multiparm_editor(value)
			return

		if value is Array:
			_create_array_editor(value)
			return

		_create_string_editor(value)


	func _make_int_spin(value: Variant) -> SpinBox:
		var spin := SpinBox.new()
		spin.allow_lesser = true
		spin.allow_greater = true
		spin.step = 1.0
		spin.custom_minimum_size = Vector2(HEUParameterWidgetFactory.EDITOR_MIN_WIDTH, 0)
		spin.size_flags_horizontal = SIZE_SHRINK_END
		if info.get("hasMin", false):
			spin.min_value = float(info.get("min", -2147483648))
		else:
			spin.min_value = -2147483648.0
		if info.get("hasMax", false):
			spin.max_value = float(info.get("max", 2147483647))
		else:
			spin.max_value = 2147483647.0
		spin.value = float(_variant_to_int(value))
		spin.value_changed.connect(_on_int_changed)
		return spin


	func _make_float_spin(value: Variant) -> SpinBox:
		var spin := SpinBox.new()
		spin.allow_lesser = true
		spin.allow_greater = true
		spin.step = 0.01
		spin.custom_minimum_size = Vector2(HEUParameterWidgetFactory.EDITOR_MIN_WIDTH, 0)
		spin.size_flags_horizontal = SIZE_SHRINK_END
		var lo := -1.0e30
		var hi := 1.0e30
		if info.get("hasMin", false):
			lo = float(info.get("min", lo))
		if info.get("hasUIMin", false):
			lo = float(info.get("UIMin", lo))
		if info.get("hasMax", false):
			hi = float(info.get("max", hi))
		if info.get("hasUIMax", false):
			hi = float(info.get("UIMax", hi))
		spin.min_value = lo
		spin.max_value = hi
		spin.value = float(_variant_to_float(value))
		spin.value_changed.connect(_on_float_changed)
		return spin


	func _create_vector_editor(component_count: int, value: Variant) -> void:
		var holder := HBoxContainer.new()
		holder.size_flags_horizontal = SIZE_SHRINK_END
		add_child(holder)
		var components := _variant_to_vector_components(value, component_count)
		for i in range(component_count):
			var spin := SpinBox.new()
			spin.allow_lesser = true
			spin.allow_greater = true
			spin.step = 0.01
			spin.value = float(components[i])
			spin.custom_minimum_size = Vector2(64, 0)
			spin.value_changed.connect(_on_vector_component_changed)
			holder.add_child(spin)
			_editors.append(spin)


	func _create_path_editor(value: Variant, script_type: int) -> void:
		var line := LineEdit.new()
		line.text = str(value) if value != null and not (value is Array) else ""
		line.custom_minimum_size = Vector2(140, 0)
		line.size_flags_horizontal = SIZE_SHRINK_END
		line.text_submitted.connect(func(_text: String) -> void: _commit_from_string(_text))
		line.focus_exited.connect(func() -> void: _commit_from_string(line.text))
		add_child(line)
		_editors.append(line)

		var browse := Button.new()
		browse.text = "..."
		browse.custom_minimum_size = Vector2(34, 0)
		browse.pressed.connect(func() -> void: _browse_path(script_type))
		add_child(browse)
		_editors.append(browse)


	func _create_node_editor(value: Variant) -> void:
		var line := LineEdit.new()
		var node_value: int = node.getParameterNodeValue(parm_name)
		line.text = str(node_value) if node_value >= 0 else str(value)
		line.custom_minimum_size = Vector2(90, 0)
		line.size_flags_horizontal = SIZE_SHRINK_END
		line.text_submitted.connect(func(text: String) -> void:
			if text.is_valid_int():
				node.setParameterNodeValue(parm_name, text.to_int())
				_emit_commit(text.to_int())
				_request_refresh()
		)
		add_child(line)
		_editors.append(line)


	func _create_choice_editor(value: Variant, choices: Array) -> void:
		var option := OptionButton.new()
		option.custom_minimum_size = Vector2(100, 0)
		option.size_flags_horizontal = SIZE_SHRINK_END
		var selected_index := 0
		var index := 0
		for choice in choices:
			var choice_dict: Dictionary = choice
			option.add_item(choice_dict.get("label", ""), index)
			if _values_equal(value, choice_dict.get("value", "")) or _values_equal(value, index):
				selected_index = index
			index += 1
		option.selected = selected_index
		option.item_selected.connect(_on_choice_selected)
		add_child(option)
		_editors.append(option)


	func _create_string_editor(value: Variant) -> void:
		var line := LineEdit.new()
		line.text = str(value) if value != null else ""
		line.custom_minimum_size = Vector2(100, 0)
		line.size_flags_horizontal = SIZE_SHRINK_END
		line.text_submitted.connect(func(_text: String) -> void: _commit_from_string(_text))
		line.focus_exited.connect(func() -> void: _commit_from_string(line.text))
		add_child(line)
		_editors.append(line)


	func _create_array_editor(value: Variant) -> void:
		_array_values = (value as Array).duplicate() if value is Array else []

		_array_rows = VBoxContainer.new()
		_array_rows.size_flags_horizontal = SIZE_SHRINK_END
		add_child(_array_rows)

		_array_header = Label.new()
		_array_header.text = "Array (%d)" % _array_values.size()
		_array_header.add_theme_color_override("font_color", Color(1, 1, 1, 0.65))
		add_child(_array_header)

		_rebuild_array_editor()
		_editors.append(_array_header)


	func _rebuild_array_editor() -> void:
		if _array_rows == null:
			return
		for child in _array_rows.get_children():
			_array_rows.remove_child(child)
			child.queue_free()
		_array_item_lines.clear()
		_array_remove_buttons.clear()

		_update_array_header_text()

		for i in range(_array_values.size()):
			var row := HBoxContainer.new()
			_array_rows.add_child(row)

			var index_label := Label.new()
			index_label.text = "[%d]" % i
			index_label.custom_minimum_size = Vector2(36, 0)
			row.add_child(index_label)

			var item_line := LineEdit.new()
			item_line.text = _array_item_to_text(_array_values[i])
			item_line.custom_minimum_size = Vector2(100, 0)
			item_line.text_submitted.connect(_on_array_item_text.bind(i))
			item_line.focus_exited.connect(_on_array_item_focus_exited.bind(i, item_line))
			row.add_child(item_line)
			_array_item_lines.append(item_line)

			var remove_button := Button.new()
			remove_button.text = "×"
			remove_button.custom_minimum_size = Vector2(32, 0)
			remove_button.tooltip_text = "Remove element"
			remove_button.pressed.connect(_on_array_remove_pressed.bind(i))
			row.add_child(remove_button)
			_array_remove_buttons.append(remove_button)

		var add_row := HBoxContainer.new()
		_array_rows.add_child(add_row)
		var add_button := Button.new()
		add_button.text = "+ Add"
		add_button.tooltip_text = "Add array element"
		add_button.pressed.connect(_on_array_add_pressed)
		add_row.add_child(add_button)
		_array_add_button = add_button


	func _update_array_header_text() -> void:
		if _array_header == null:
			return
		_array_header.text = "Array (%d)" % _array_values.size()


	func get_array_item_count() -> int:
		return _array_values.size()


	func get_array_add_button() -> Button:
		return _array_add_button


	func get_array_remove_button(index: int) -> Button:
		if index < 0 or index >= _array_remove_buttons.size():
			return null
		return _array_remove_buttons[index]


	func _on_array_add_pressed() -> void:
		_array_values.append(_array_item_default())
		_emit_commit(_array_values.duplicate())
		_rebuild_array_editor()


	func _on_array_remove_pressed(index: int) -> void:
		if index < 0 or index >= _array_values.size():
			return
		_array_values.remove_at(index)
		_emit_commit(_array_values.duplicate())
		_rebuild_array_editor()


	func _on_array_item_text(index: int, text: String) -> void:
		if _suppress_callbacks:
			return
		if index < 0 or index >= _array_values.size():
			return
		_array_values[index] = _array_item_from_text(text)
		_emit_commit(_array_values.duplicate())


	func _on_array_item_focus_exited(index: int, line: LineEdit) -> void:
		_on_array_item_text(index, line.text)


	func _array_item_default() -> Variant:
		if not _array_values.is_empty():
			match typeof(_array_values[0]):
				TYPE_INT:
					return 0
				TYPE_FLOAT:
					return 0.0
				TYPE_STRING:
					return ""
		var parm_type: int = info.get("type", -1)
		if parm_type == HEGEnum.HAPI_PARMTYPE_INT:
			return 0
		if parm_type == HEGEnum.HAPI_PARMTYPE_FLOAT:
			return 0.0
		return ""


	func _array_item_from_text(text: String) -> Variant:
		var default: Variant = _array_item_default()
		match typeof(default):
			TYPE_INT:
				return text.to_int() if text.is_valid_int() else 0
			TYPE_FLOAT:
				return text.to_float()
			_:
				return text


	func _array_item_to_text(value: Variant) -> String:
		if value == null:
			return ""
		return str(value)


	func _create_multiparm_editor(value: Variant) -> void:
		var holder := HBoxContainer.new()
		holder.size_flags_horizontal = SIZE_SHRINK_END
		add_child(holder)

		var count_label := Label.new()
		count_label.text = "%d instance(s)" % int(value) if value != null else "0 instance(s)"
		holder.add_child(count_label)

		var add_button := Button.new()
		add_button.text = "+"
		add_button.pressed.connect(func() -> void:
			node.insertMultiparmInstance(parm_name, int(value) if value != null else 0)
			_request_refresh()
		)
		holder.add_child(add_button)

		var remove_button := Button.new()
		remove_button.text = "-"
		remove_button.pressed.connect(func() -> void:
			var count := int(value) if value != null else 0
			if count > 0:
				node.removeMultiparmInstance(parm_name, count - 1)
				_request_refresh()
		)
		holder.add_child(remove_button)
		_editors.append(count_label)


	func _browse_path(script_type: int) -> void:
		var dialog := EditorFileDialog.new()
		dialog.access = EditorFileDialog.ACCESS_FILESYSTEM
		match script_type:
			HEGEnum.HAPI_PRM_SCRIPT_TYPE_IMAGE:
				dialog.add_filter("*.png,*.jpg,*.jpeg,*.exr,*.tga,*.bmp", "Images")
				dialog.file_mode = EditorFileDialog.FILE_MODE_OPEN_FILE
			HEGEnum.HAPI_PRM_SCRIPT_TYPE_GEOMETRY:
				dialog.add_filter("*.bgeo,*.bgeo.sc,*.obj,*.fbx", "Geometry")
				dialog.file_mode = EditorFileDialog.FILE_MODE_OPEN_FILE
			HEGEnum.HAPI_PRM_SCRIPT_TYPE_DIRECTORY:
				dialog.file_mode = EditorFileDialog.FILE_MODE_OPEN_DIR
			_:
				dialog.file_mode = EditorFileDialog.FILE_MODE_OPEN_FILE
		dialog.file_selected.connect(func(path: String) -> void:
			_commit_from_string(path)
			_write_string_value(path)
		)
		get_tree().root.add_child(dialog)
		dialog.popup_centered_ratio(0.55)


	func _on_check_toggled(pressed: bool) -> void:
		if _suppress_callbacks:
			return
		_emit_commit(pressed)


	func _on_button_pressed() -> void:
		if _suppress_callbacks:
			return
		_emit_commit(0)


	func _on_color_changed(color: Color) -> void:
		if _suppress_callbacks:
			return
		_emit_commit(color)


	func _on_int_changed(value: float) -> void:
		if _suppress_callbacks:
			return
		_emit_commit(int(value))


	func _on_float_changed(value: float) -> void:
		if _suppress_callbacks:
			return
		_emit_commit(value)


	func _on_vector_component_changed(_value: float) -> void:
		if _suppress_callbacks:
			return
		var components: Array[float] = []
		for editor in _editors:
			components.append(float((editor as SpinBox).value))
		match components.size():
			2:
				_emit_commit(Vector2(components[0], components[1]))
			3:
				_emit_commit(Vector3(components[0], components[1], components[2]))
			4:
				_emit_commit(Vector4(components[0], components[1], components[2], components[3]))


	func _on_choice_selected(index: int) -> void:
		if _suppress_callbacks:
			return
		var choices: Array = node.getParameterChoices(parm_name)
		if index < 0 or index >= choices.size():
			return
		var choice: Dictionary = choices[index]
		if info.get("useMenuItemTokenAsValue", false):
			_emit_commit(choice.get("value", ""))
		else:
			_emit_commit(index)


	func _commit_from_string(text: String) -> void:
		if _suppress_callbacks:
			return
		_emit_commit(text)


	func _emit_commit(value: Variant) -> void:
		if commit.is_valid():
			commit.call(parm_name, value)


	func _request_refresh() -> void:
		if refresh_requested.is_valid():
			refresh_requested.call()


	func _write_value(value: Variant) -> void:
		if _array_rows != null:
			_array_values = (value as Array).duplicate() if value is Array else []
			_rebuild_array_editor()
			return
		if _editors.is_empty():
			return
		var editor := _editors[0]
		if editor is CheckButton:
			editor.button_pressed = bool(value)
		elif editor is ColorPickerButton:
			editor.color = _variant_to_color(value)
		elif editor is SpinBox and _editors.size() == 1:
			if info.get("type", -1) == HEGEnum.HAPI_PARMTYPE_INT:
				editor.value = float(_variant_to_int(value))
			else:
				editor.value = float(_variant_to_float(value))
		elif editor is LineEdit:
			editor.text = str(value) if value != null else ""
		elif editor is OptionButton:
			var choices: Array = node.getParameterChoices(parm_name)
			var index := 0
			for i in range(choices.size()):
				var choice: Dictionary = choices[i]
				if _values_equal(value, choice.get("value", "")) or _values_equal(value, i):
					index = i
					break
			editor.selected = index
		elif editor is Label and info.get("type", -1) == HEGEnum.HAPI_PARMTYPE_MULTIPARMLIST:
			editor.text = "%d instance(s)" % int(value) if value != null else "0 instance(s)"
		elif _editors.size() > 1:
			var components := _variant_to_vector_components(value, _editors.size())
			for i in range(_editors.size()):
				var control := _editors[i]
				if control is SpinBox:
					control.value = float(components[i])
				elif control is LineEdit:
					control.text = str(value) if value != null else ""


	func _write_string_value(text: String) -> void:
		for editor in _editors:
			if editor is LineEdit:
				editor.text = text
				break


	func _variant_to_int(value: Variant) -> int:
		if value == null:
			return 0
		if value is Array:
			var arr: Array = value
			return int(arr[0]) if not arr.is_empty() else 0
		return int(value)


	func _variant_to_float(value: Variant) -> float:
		if value == null:
			return 0.0
		if value is Array:
			var arr: Array = value
			return float(arr[0]) if not arr.is_empty() else 0.0
		return float(value)


	func _variant_to_color(value: Variant) -> Color:
		if value is Color:
			return value
		if value is Array:
			var arr: Array = value
			if arr.size() >= 3:
				return Color(float(arr[0]), float(arr[1]), float(arr[2]), float(arr[3]) if arr.size() >= 4 else 1.0)
		return Color.WHITE


	func _variant_to_vector_components(value: Variant, component_count: int) -> Array[float]:
		var result: Array[float] = []
		if value is Vector2 or value is Vector3 or value is Vector4:
			result.append(float(value.x))
			result.append(float(value.y))
			if component_count >= 3:
				result.append(float(value.z) if value is Vector3 or value is Vector4 else 0.0)
			if component_count >= 4:
				result.append(float(value.w) if value is Vector4 else 0.0)
			return result
		if value is Array:
			var arr: Array = value
			for i in range(component_count):
				result.append(float(arr[i]) if i < arr.size() else 0.0)
			return result
		result.append(float(value))
		for i in range(1, component_count):
			result.append(0.0)
		return result


	func _values_equal(a: Variant, b: Variant) -> bool:
		if a == null or b == null:
			return a == b
		var na := str(a)
		var nb := str(b)
		if na == nb:
			return true
		if na.is_valid_float() and nb.is_valid_float():
			return float(na) == float(nb)
		return false
