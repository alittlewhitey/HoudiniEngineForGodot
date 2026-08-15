@tool
extends EditorPlugin

const SessionDockScene = preload("res://addons/HoudiniEngineForGodot/scripts/ui/heu_session_dock.gd")
const AssetBrowserScene = preload("res://addons/HoudiniEngineForGodot/scripts/ui/heu_asset_browser_dock.gd")
const ParameterDockScene = preload("res://addons/HoudiniEngineForGodot/scripts/ui/heu_parameter_dock.gd")
const HEUAssetRootScene = preload("res://addons/HoudiniEngineForGodot/scripts/scene/heu_asset_root.gd")

var _session_dock: Control
var _asset_browser_dock: Control
var _parameter_dock: Control
var _load_dialog: EditorFileDialog


func _enter_tree() -> void:
    _session_dock = _create_dock(SessionDockScene, "HEUSessionDock", "HEU Session")
    add_control_to_bottom_panel(_session_dock, "HEU Session")

    _asset_browser_dock = _create_dock(AssetBrowserScene, "HEUAssetBrowserDock", "HEU Assets")
    add_control_to_bottom_panel(_asset_browser_dock, "HEU Assets")

    _parameter_dock = _create_dock(ParameterDockScene, "HEU Params", "HEU Params")
    _parameter_dock.custom_minimum_size = Vector2(300, 360)
    add_control_to_dock(EditorPlugin.DOCK_SLOT_RIGHT_UL, _parameter_dock)

    _add_menu_items()
    get_editor_interface().get_selection().selection_changed.connect(_on_selection_changed)
    _on_selection_changed()

    if OS.has_environment("HEU_SMOKE_TEST"):
        call_deferred(&"_run_smoke_test")
    if OS.has_environment("HEU_UI_SMOKE_TEST"):
        call_deferred(&"_run_ui_smoke_test")
    if OS.has_environment("HEU_AUTOCOOK_SMOKE_TEST"):
        call_deferred(&"_run_autocook_smoke_test")
    if OS.has_environment("HEU_ARRAY_SMOKE_TEST"):
        call_deferred(&"_run_array_smoke_test")


func _exit_tree() -> void:
    if _session_dock != null:
        remove_control_from_bottom_panel(_session_dock)
    if _asset_browser_dock != null:
        remove_control_from_bottom_panel(_asset_browser_dock)
    if _parameter_dock != null:
        remove_control_from_docks(_parameter_dock)

    _remove_menu_items()

    if get_editor_interface().get_selection().selection_changed.is_connected(_on_selection_changed):
        get_editor_interface().get_selection().selection_changed.disconnect(_on_selection_changed)

    if _session_dock != null:
        _session_dock.queue_free()
    if _asset_browser_dock != null:
        _asset_browser_dock.queue_free()
    if _parameter_dock != null:
        _parameter_dock.queue_free()


func _create_dock(script: GDScript, dock_name: String, dock_title: String) -> Control:
    var dock := Control.new()
    dock.set_script(script)
    dock.name = dock_name
    dock.set_meta(&"heu_dock_title", dock_title)
    return dock


func _add_menu_items() -> void:
    add_tool_menu_item("HEU/Load HDA...", _menu_load_hda)
    add_tool_menu_item("HEU/Session/Start InProcess", _start_session.bind(HEGSession.InProcess))
    add_tool_menu_item("HEU/Session/Start Named Pipe", _start_session.bind(HEGSession.NewNamedPipe))
    add_tool_menu_item("HEU/Session/Start TCP", _start_session.bind(HEGSession.NewTCPSocket))
    add_tool_menu_item("HEU/Session/Connect Existing Named Pipe", _start_session.bind(HEGSession.ExistingNamedPipe))
    add_tool_menu_item("HEU/Session/Connect Existing TCP", _start_session.bind(HEGSession.ExistingTCPSocket))
    add_tool_menu_item("HEU/Session/Connect Existing Shared Memory", _start_session.bind(HEGSession.ExistingSharedMemory))
    add_tool_menu_item("HEU/Session/Restart", _menu_restart_session)
    add_tool_menu_item("HEU/Session/Close", _menu_close_session)
    add_tool_menu_item("HEU/Selected Asset/Recook", _menu_recook_selected)
    add_tool_menu_item("HEU/Selected Asset/Rebuild", _menu_rebuild_selected)
    add_tool_menu_item("HEU/Selected Asset/Duplicate", _menu_duplicate_selected)


func _remove_menu_items() -> void:
    remove_tool_menu_item("HEU/Load HDA...")
    remove_tool_menu_item("HEU/Session/Start InProcess")
    remove_tool_menu_item("HEU/Session/Start Named Pipe")
    remove_tool_menu_item("HEU/Session/Start TCP")
    remove_tool_menu_item("HEU/Session/Connect Existing Named Pipe")
    remove_tool_menu_item("HEU/Session/Connect Existing TCP")
    remove_tool_menu_item("HEU/Session/Connect Existing Shared Memory")
    remove_tool_menu_item("HEU/Session/Restart")
    remove_tool_menu_item("HEU/Session/Close")
    remove_tool_menu_item("HEU/Selected Asset/Recook")
    remove_tool_menu_item("HEU/Selected Asset/Rebuild")
    remove_tool_menu_item("HEU/Selected Asset/Duplicate")


func _start_session(session_type: int) -> void:
    HEUSessionService.instance().start(session_type)


func _menu_restart_session() -> void:
    HEUSessionService.instance().restart()


func _menu_close_session() -> void:
    HEUSessionService.instance().stop()


func _menu_load_hda() -> void:
    if _load_dialog == null:
        _load_dialog = EditorFileDialog.new()
        _load_dialog.add_filter("*.hda,*.otl,*.hdalc", "Houdini Digital Assets")
        _load_dialog.file_mode = EditorFileDialog.FILE_MODE_OPEN_FILE
        _load_dialog.access = EditorFileDialog.ACCESS_FILESYSTEM
        _load_dialog.file_selected.connect(_on_load_hda_file_selected)
        get_editor_interface().get_base_control().add_child(_load_dialog)
    _load_dialog.popup_centered_ratio(0.55)


func _on_load_hda_file_selected(path: String) -> void:
    if _asset_browser_dock != null and _asset_browser_dock.has_method("load_hda_file"):
        _asset_browser_dock.load_hda_file(path)


func _on_selection_changed() -> void:
    if _parameter_dock == null or not is_instance_valid(_parameter_dock):
        return
    var selected := get_editor_interface().get_selection().get_selected_nodes()
    var target: Node = null
    for node in selected:
        if node != null and node.has_meta(&"HEUAssetRoot") and node.has_method("get_he_node"):
            target = node
            break
    _parameter_dock.set_asset_root(target)


func _selected_asset_root() -> Node:
    var selected := get_editor_interface().get_selection().get_selected_nodes()
    for node in selected:
        if node != null and node.has_meta(&"HEUAssetRoot"):
            return node
    return null


func _run_ui_smoke_test() -> void:
    await get_tree().process_frame

    if not HEUSessionService.instance().is_active():
        if not HEUSessionService.instance().start(HEGSession.InProcess):
            print("[HEU-UI-SMOKE] FAIL: cannot start session")
            return

    var root := HEUAssetRoot.new()
    root.name = "HEUSmokeRoot"
    var hda_resource := HEGHdaResource.new()
    hda_resource.path = ProjectSettings.globalize_path("res://hda/hexagona_lite.hda")
    root.hda = hda_resource
    root.operator_name = ""
    root.auto_cook = false
    get_tree().root.add_child(root)

    if not root.ensure_houdini_node():
        print("[HEU-UI-SMOKE] FAIL: ensure_houdini_node")
        root.queue_free()
        return

    _parameter_dock.set_asset_root(root)
    await get_tree().process_frame
    await get_tree().process_frame
    var visible_rows: int = _parameter_dock.get_visible_row_count()
    var folder_titles: Array[String] = _parameter_dock.get_folder_titles()
    var folder_ok: bool = folder_titles.has("Board Settings") \
        and folder_titles.has("Landscape Settings") \
        and folder_titles.has("Set Dressing")
    for title in folder_titles:
        if title.begins_with("fd_"):
            folder_ok = false
    print("[HEU-UI-SMOKE] visible_parameter_rows=", visible_rows)
    print("[HEU-UI-SMOKE] folder_titles=", folder_titles, " folder_ok=", folder_ok)

    var old_toggle: Variant = root.get_parameter("flat_board")
    var toggle_target := not bool(old_toggle)
    var parm_set := root.set_parameter("flat_board", toggle_target)
    var new_toggle: Variant = root.get_parameter("flat_board")
    var parm_roundtrip: bool = parm_set and bool(new_toggle) == toggle_target
    print("[HEU-UI-SMOKE] parameter_roundtrip=", parm_roundtrip, " old=", old_toggle, " new=", new_toggle)

    var cook_ok := root.recook()
    var cooked := false
    if cook_ok:
        cooked = await root.cook_finished
    await get_tree().process_frame
    var generated := 0
    for child in root.get_children():
        if child.has_meta(&"_heu_generated"):
            generated += 1
    print("[HEU-UI-SMOKE] recook_started=", cook_ok, " cooked=", cooked, " generated_children=", generated)

    var ui_ok: bool = visible_rows > 0 and parm_roundtrip and cook_ok and cooked and generated > 0 and folder_ok
    print("[HEU-UI-SMOKE] %s" % ("PASS" if ui_ok else "FAIL"))
    root.queue_free()


func _run_autocook_smoke_test() -> void:
    await get_tree().process_frame

    if not HEUSessionService.instance().is_active():
        if not HEUSessionService.instance().start(HEGSession.InProcess):
            print("[HEU-AUTOCOOK] FAIL: cannot start session")
            return

    var root := HEUAssetRoot.new()
    root.name = "HEUAutoCookRoot"
    var hda_resource := HEGHdaResource.new()
    hda_resource.path = ProjectSettings.globalize_path("res://hda/hexagona_lite.hda")
    root.hda = hda_resource
    root.operator_name = ""
    root.auto_cook = true
    get_tree().root.add_child(root)

    if not root.ensure_houdini_node():
        print("[HEU-AUTOCOOK] FAIL: ensure_houdini_node")
        root.queue_free()
        return

    var generation_before: int = root.get_cook_generation()
    var set_ok: bool = root.set_parameter("flat_board", true)
    var cooked: bool = await root.cook_finished
    await get_tree().process_frame

    var generated := 0
    for child in root.get_children():
        if child.has_meta(&"_heu_generated"):
            generated += 1
    var generation_after_first: int = root.get_cook_generation()

    # Rapid parameter changes must be debounced into a single extra cook.
    root.set_parameter("flat_board", false)
    root.set_parameter("flat_board", true)
    var second_cook: bool = await root.cook_finished
    await get_tree().process_frame
    var generation_after_debounce: int = root.get_cook_generation()

    var first_ok: bool = set_ok and cooked and generated > 0 and generation_after_first == generation_before + 1
    var debounce_ok: bool = second_cook and generation_after_debounce == generation_after_first + 1

    # A parameter change arriving while a cook is running must be queued and
    # must trigger exactly one follow-up cook.
    var explicit_started: bool = root.recook()
    var during_cook_set: bool = root.set_parameter("flat_board", false)
    var first_follow_signal: bool = await root.cook_finished
    var second_follow_signal: bool = await root.cook_finished
    await get_tree().process_frame
    var generation_after_pending: int = root.get_cook_generation()
    var final_value: Variant = root.get_parameter("flat_board")
    var pending_ok: bool = explicit_started and during_cook_set and first_follow_signal and second_follow_signal \
        and generation_after_pending == generation_after_debounce + 2 and bool(final_value) == false

    var ok: bool = first_ok and debounce_ok and pending_ok

    print("[HEU-AUTOCOOK] first_auto_cook=%s generations=%d->%d generated=%d" % [first_ok, generation_before, generation_after_first, generated])
    print("[HEU-AUTOCOOK] debounce_single_cook=%s generation_after=%d" % [debounce_ok, generation_after_debounce])
    print("[HEU-AUTOCOOK] cook_during_edit_queue=%s generations=%d final_flat_board=%s" % [pending_ok, generation_after_pending, final_value])
    print("[HEU-AUTOCOOK] %s" % ("PASS" if ok else "FAIL"))
    root.queue_free()


func _run_array_smoke_test() -> void:
    await get_tree().process_frame

    var state := {"last": null}
    var commit := func(_name: String, value: Variant) -> void:
        state["last"] = value

    var info := {
        "name": "test_array",
        "label": "Test Array",
        "help": "",
        "type": HEGEnum.HAPI_PARMTYPE_FLOAT,
        "scriptType": 0,
        "permissions": HEGEnum.HAPI_PERMISSIONS_READ_WRITE,
    }
    var row := HEUParameterWidgetFactory.create_row(info, [], null, commit)
    get_tree().root.add_child(row)
    await get_tree().process_frame

    var add_button := row.get_array_add_button()
    var add_visible: bool = add_button.is_visible_in_tree()
    add_button.pressed.emit()
    await get_tree().process_frame
    var empty_add_ok: bool = add_visible and row.get_array_item_count() == 1 \
        and state["last"] is Array \
        and (state["last"] as Array).size() == 1

    row.set_value([1.5, 2.5])
    await get_tree().process_frame
    var nonempty_ok: bool = row.get_array_item_count() == 2

    var remove_button := row.get_array_remove_button(0)
    var remove_visible: bool = remove_button.is_visible_in_tree()
    remove_button.pressed.emit()
    await get_tree().process_frame
    var remove_ok: bool = remove_visible and row.get_array_item_count() == 1 \
        and state["last"] is Array \
        and (state["last"] as Array).size() == 1

    # The rebuild after add/remove frees old buttons; fetch current instances
    # before simulating the cook-time disabled state.
    var current_add_button := row.get_array_add_button()
    var current_remove_button := row.get_array_remove_button(0)
    row.set_enabled(false)
    var add_during_cook: bool = current_add_button != null and not current_add_button.disabled
    var remove_during_cook: bool = current_remove_button != null and not current_remove_button.disabled

    var ok: bool = empty_add_ok and nonempty_ok and remove_ok and add_during_cook and remove_during_cook
    print("[HEU-ARRAY] empty_add=%s nonempty_count=%s remove=%s visible=%s/%s enabled_during_cook=%s/%s" % [
        empty_add_ok, nonempty_ok, remove_ok, add_visible, remove_visible, add_during_cook, remove_during_cook
    ])
    print("[HEU-ARRAY] %s" % ("PASS" if ok else "FAIL"))
    row.queue_free()


func _run_smoke_test() -> void:
    var result := HEUSmokeTest.run()
    print("[HEU-SMOKE] result=", result)
    if result.get("ok", false):
        print("[HEU-SMOKE] PASS")
    else:
        print("[HEU-SMOKE] FAIL: ", result.get("error", "unknown"))
        push_error("[HEU-SMOKE] FAIL: %s" % result.get("error", "unknown"))


func _menu_recook_selected() -> void:
    var root := _selected_asset_root()
    if root != null and root.has_method("recook"):
        root.recook()


func _menu_rebuild_selected() -> void:
    var root := _selected_asset_root()
    if root != null and root.has_method("rebuild"):
        root.rebuild()


func _menu_duplicate_selected() -> void:
    var root := _selected_asset_root()
    if root != null and root.has_method("duplicate_asset"):
        root.duplicate_asset()
