@tool
extends Control
class_name HEngineAssetBrowser

var _loaded_assets: Array = []  # Array[Dictionary{path: String, asset: HEAsset}]
var _tree: Tree
var _tree_root: TreeItem
var _session_dock: Control


func _ready():
	_setup_ui()


func _setup_ui():
	var vbox := VBoxContainer.new()
	vbox.set_anchors_preset(Control.PRESET_FULL_RECT)
	vbox.size_flags_horizontal = SIZE_EXPAND_FILL
	vbox.size_flags_vertical = SIZE_EXPAND_FILL
	add_child(vbox)

	var toolbar := HBoxContainer.new()
	vbox.add_child(toolbar)

	var btn_load := Button.new()
	btn_load.text = "Load HDA..."
	btn_load.tooltip_text = "Load a Houdini Digital Asset from file"
	btn_load.pressed.connect(_on_load_pressed)
	toolbar.add_child(btn_load)

	var btn_refresh := Button.new()
	btn_refresh.text = "Refresh"
	btn_refresh.tooltip_text = "Refresh the asset list"
	btn_refresh.pressed.connect(_refresh_tree)
	toolbar.add_child(btn_refresh)

	_tree = Tree.new()
	_tree.size_flags_horizontal = SIZE_EXPAND_FILL
	_tree.size_flags_vertical = SIZE_EXPAND_FILL
	_tree.hide_root = true
	_tree.allow_rmb_select = true
	_tree.item_activated.connect(_on_item_activated)
	_tree.item_mouse_selected.connect(_on_item_mouse_selected)
	vbox.add_child(_tree)

	_tree_root = _tree.create_item()


func set_session_dock(dock: Control):
	_session_dock = dock


func load_hda_file(path: String):
	for entry in _loaded_assets:
		if entry.path == path:
			push_warning("HDA already loaded: %s" % path)
			return

	var session = _get_session()
	if session == null:
		push_error("No Houdini session. Create a session first.")
		return

	var asset: HEAsset = session.loadHDAExternal(path)
	if asset == null:
		push_error("Failed to load HDA: %s" % path)
		return

	var entry := {path = path, asset = asset}
	_loaded_assets.append(entry)
	_add_hda_to_tree(entry)


func _get_session() -> HESession:
	if _session_dock and _session_dock.has_method("get_session"):
		return _session_dock.get_session()
	var s = HESession.getSession()
	if s and s.valid():
		return s
	return null


func _add_hda_to_tree(entry: Dictionary):
	var path: String = entry.path
	var asset: HEAsset = entry.asset

	var item := _tree.create_item(_tree_root)
	item.set_text(0, path.get_file())
	item.set_tooltip_text(0, path)
	item.set_metadata(0, entry)

	var operators: PackedStringArray = asset.get_ownedNodeOperators()
	for op_name in operators:
		var child := _tree.create_item(item)
		child.set_text(0, op_name)
		child.set_metadata(0, {"asset": asset, "operator": op_name, "path": path})
		child.set_icon(0, _get_operator_icon(op_name))


func _get_operator_icon(_op_name: String) -> Texture2D:
	return null


func _on_load_pressed():
	var dialog := EditorFileDialog.new()
	dialog.add_filter("*.hda,*.otl,*.hdalc", "Houdini Digital Assets")
	dialog.file_mode = EditorFileDialog.FILE_MODE_OPEN_FILE
	dialog.access = EditorFileDialog.ACCESS_FILESYSTEM
	dialog.file_selected.connect(_on_file_selected)
	get_tree().get_root().add_child(dialog)
	dialog.popup_centered_ratio(0.5)


func _on_file_selected(path: String):
	load_hda_file(path)


func _on_item_activated():
	var item := _tree.get_selected()
	if item == null:
		return

	var meta = item.get_metadata(0)
	if meta is Dictionary and meta.has("operator"):
		_instantiate_operator(meta.get("path"), meta.get("operator"))


func _on_item_mouse_selected(position: Vector2, mouse_button_index: int):
	if mouse_button_index != MOUSE_BUTTON_RIGHT:
		return

	var item := _tree.get_item_at_position(position)
	if item == null:
		return

	var meta = item.get_metadata(0)
	if meta is Dictionary and meta.has("path"):
		_show_context_menu(item, meta)


func _show_context_menu(item: TreeItem, meta: Dictionary):
	var menu := PopupMenu.new()
	menu.add_item("Instantiate in Scene", 0)
	menu.add_separator()
	menu.add_item("Reload HDA", 1)
	menu.add_item("Close HDA", 2)
	menu.id_pressed.connect(func(id: int):
		match id:
			0:
				if meta.has("operator"):
					_instantiate_operator(meta.path, meta.operator)
			1:
				_reload_hda(meta.path)
			2:
				_close_hda(meta.path)
		menu.queue_free()
	)
	get_tree().get_root().add_child(menu)
	menu.position = get_global_mouse_position()
	menu.popup()


func _instantiate_operator(hda_path: String, _operator: String):
	var session = _get_session()
	if session == null:
		push_error("No Houdini session")
		return

	var root: HEngineAssetRoot = HEngineUtility.create_hda_root(hda_path, session)
	if root:
		await HEngineUtility.cook_and_generate(root)
		print("Instantiated HDA: %s" % root.name)


func _reload_hda(path: String):
	_close_hda(path)
	load_hda_file(path)


func _close_hda(path: String):
	for i in range(_loaded_assets.size()):
		if _loaded_assets[i].path == path:
			_loaded_assets.remove_at(i)
			break
	_refresh_tree()


func _refresh_tree():
	_tree.clear()
	_tree_root = _tree.create_item()
	for entry in _loaded_assets:
		_add_hda_to_tree(entry)


func get_loaded_assets() -> Array:
	return _loaded_assets
