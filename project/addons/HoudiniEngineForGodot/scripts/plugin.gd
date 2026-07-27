@tool
extends EditorPlugin

const DOCK_NAME = "Houdini Engine"

var _session_dock: Control
var _asset_browser_dock: Control
var _parameter_inspector_dock: Control


func _enter_tree():
	# Session dock — bottom panel, left tab
	_session_dock = _create_session_dock()
	add_control_to_bottom_panel(_session_dock, "Houdini Session")

	# Asset Browser dock — bottom panel, middle tab
	_asset_browser_dock = _create_asset_browser_dock()
	add_control_to_bottom_panel(_asset_browser_dock, "Houdini Assets")

	# Parameter Inspector — right panel, next to Inspector
	_parameter_inspector_dock = _create_parameter_inspector_dock()
	add_control_to_dock(EditorPlugin.DOCK_SLOT_RIGHT_UL, _parameter_inspector_dock)

	# Menu items
	add_tool_menu_item("HoudiniEngine/Load Houdini Digital Asset...", _on_menu_load_hda)
	add_tool_menu_item("HoudiniEngine/Session/Create (InProcess)", func(): _on_menu_create_session("InProcess"))
	add_tool_menu_item("HoudiniEngine/Session/Create (Named Pipe)", func(): _on_menu_create_session("NamedPipe"))
	add_tool_menu_item("HoudiniEngine/Session/Create (TCP)", func(): _on_menu_create_session("TCP"))
	add_tool_menu_item("HoudiniEngine/Session/Close", _on_menu_close_session)
	add_tool_menu_item("HoudiniEngine/Session/Restart", _on_menu_restart_session)

	# Connect selection signal
	get_editor_interface().get_selection().selection_changed.connect(_on_selection_changed)


func _exit_tree():
	remove_control_from_bottom_panel(_session_dock)
	remove_control_from_bottom_panel(_asset_browser_dock)
	remove_control_from_docks(_parameter_inspector_dock)

	remove_tool_menu_item("HoudiniEngine/Load Houdini Digital Asset...")
	remove_tool_menu_item("HoudiniEngine/Session/Create (InProcess)")
	remove_tool_menu_item("HoudiniEngine/Session/Create (Named Pipe)")
	remove_tool_menu_item("HoudiniEngine/Session/Create (TCP)")
	remove_tool_menu_item("HoudiniEngine/Session/Close")
	remove_tool_menu_item("HoudiniEngine/Session/Restart")

	if _session_dock:
		_session_dock.queue_free()
	if _asset_browser_dock:
		_asset_browser_dock.queue_free()
	if _parameter_inspector_dock:
		_parameter_inspector_dock.queue_free()


func _handles(object) -> bool:
	return object is Node and object.has_meta("HEngineAssetRoot")


func _edit(object):
	if _parameter_inspector_dock and _parameter_inspector_dock.has_method("set_hda_root"):
		_parameter_inspector_dock.set_hda_root(object)


func _create_session_dock() -> Control:
	var script = load("res://addons/HoudiniEngineForGodot/scripts/houdini_engine_session_dock.gd")
	var dock = Control.new()
	dock.set_script(script)
	dock.name = "HESessionDock"
	dock.custom_minimum_size = Vector2(0, 48)
	return dock


func _create_asset_browser_dock() -> Control:
	var script = load("res://addons/HoudiniEngineForGodot/scripts/houdini_asset_browser.gd")
	var dock = Control.new()
	dock.set_script(script)
	dock.name = "HEAssetBrowserDock"
	dock.custom_minimum_size = Vector2(220, 350)
	dock.set_session_dock(_session_dock)
	return dock


func _create_parameter_inspector_dock() -> Control:
	var script = load("res://addons/HoudiniEngineForGodot/scripts/houdini_parameter_inspector.gd")
	var dock = Control.new()
	dock.set_script(script)
	dock.name = "HEParameter"
	dock.custom_minimum_size = Vector2(280, 350)
	dock.set_session_dock(_session_dock)
	return dock


func _auto_start_session():
	if _session_dock and _session_dock.has_method("auto_create_session"):
		_session_dock.auto_create_session()


func _on_menu_load_hda():
	var dialog = EditorFileDialog.new()
	dialog.add_filter("*.hda,*.otl,*.hdalc", "Houdini Digital Assets")
	dialog.file_mode = EditorFileDialog.FILE_MODE_OPEN_FILE
	dialog.access = EditorFileDialog.ACCESS_FILESYSTEM
	dialog.file_selected.connect(_on_hda_file_selected)
	get_editor_interface().get_base_control().add_child(dialog)
	dialog.popup_centered_ratio(0.5)


func _on_hda_file_selected(path: String):
	if _asset_browser_dock and _asset_browser_dock.has_method("load_hda_file"):
		_asset_browser_dock.load_hda_file(path)


func _on_menu_create_session(session_type: String):
	if _session_dock and _session_dock.has_method("create_session"):
		_session_dock.create_session(session_type)


func _on_menu_close_session():
	if _session_dock and _session_dock.has_method("close_session"):
		_session_dock.close_session()


func _on_menu_restart_session():
	if _session_dock and _session_dock.has_method("restart_session"):
		_session_dock.restart_session()


func _on_selection_changed():
	var selection = get_editor_interface().get_selection()
	var nodes = selection.get_selected_nodes()
	print("[HE] selection changed: %d nodes" % nodes.size())
	for n in nodes:
		print("[HE]   %s (has_meta=%s)" % [n.name, n.has_meta("HEngineAssetRoot")])
	if nodes.size() == 1 and nodes[0].has_meta("HEngineAssetRoot"):
		_edit(nodes[0])
