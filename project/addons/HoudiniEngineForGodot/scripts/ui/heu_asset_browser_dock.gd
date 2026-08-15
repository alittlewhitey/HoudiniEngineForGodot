@tool
extends Control

var _tree: Tree
var _tree_root: TreeItem
var _load_dialog: EditorFileDialog
var _session_dock: Control


func _ready() -> void:
    custom_minimum_size = Vector2(240, 280)
    size_flags_horizontal = SIZE_EXPAND_FILL
    size_flags_vertical = SIZE_EXPAND_FILL
    _build_ui()
    HEUAssetManager.instance().assets_changed.connect(_refresh_tree)
    _refresh_tree()


func _exit_tree() -> void:
    if HEUAssetManager.instance().assets_changed.is_connected(_refresh_tree):
        HEUAssetManager.instance().assets_changed.disconnect(_refresh_tree)


func set_session_dock(dock: Control) -> void:
    _session_dock = dock


func load_hda_file(path: String) -> void:
    var asset := HEUAssetManager.instance().load_hda(path)
    if asset != null:
        _refresh_tree()


func _build_ui() -> void:
    var vbox := VBoxContainer.new()
    vbox.set_anchors_preset(Control.PRESET_FULL_RECT)
    vbox.size_flags_horizontal = SIZE_EXPAND_FILL
    vbox.size_flags_vertical = SIZE_EXPAND_FILL
    add_child(vbox)

    var toolbar := HBoxContainer.new()
    vbox.add_child(toolbar)

    var load_button := Button.new()
    load_button.text = "Load HDA..."
    load_button.tooltip_text = "Load a Houdini Digital Asset from file."
    load_button.pressed.connect(_on_load_pressed)
    toolbar.add_child(load_button)

    var refresh_button := Button.new()
    refresh_button.text = "Refresh"
    refresh_button.tooltip_text = "Refresh the HDA list."
    refresh_button.pressed.connect(_refresh_tree)
    toolbar.add_child(refresh_button)

    _tree = Tree.new()
    _tree.size_flags_horizontal = SIZE_EXPAND_FILL
    _tree.size_flags_vertical = SIZE_EXPAND_FILL
    _tree.columns = 2
    _tree.set_column_title(0, "HDA")
    _tree.set_column_title(1, "Path")
    _tree.set_column_expand(0, false)
    _tree.set_column_custom_minimum_width(0, 360)
    _tree.set_column_expand(1, true)
    _tree.hide_root = true
    _tree.allow_rmb_select = true
    _tree.item_activated.connect(_on_item_activated)
    _tree.item_mouse_selected.connect(_on_item_mouse_selected)
    vbox.add_child(_tree)

    _tree_root = _tree.create_item()


func _on_load_pressed() -> void:
    if _load_dialog == null:
        _load_dialog = EditorFileDialog.new()
        _load_dialog.add_filter("*.hda,*.otl,*.hdalc", "Houdini Digital Assets")
        _load_dialog.file_mode = EditorFileDialog.FILE_MODE_OPEN_FILE
        _load_dialog.access = EditorFileDialog.ACCESS_FILESYSTEM
        _load_dialog.file_selected.connect(load_hda_file)
        get_tree().root.add_child(_load_dialog)
    _load_dialog.popup_centered_ratio(0.55)


func _refresh_tree() -> void:
    if _tree == null:
        return
    _tree.clear()
    _tree_root = _tree.create_item()

    var assets := HEUAssetManager.instance().get_assets()
    var paths := assets.keys()
    paths.sort()
    for path in paths:
        var asset = assets[path]
        var item := _tree.create_item(_tree_root)
        item.set_text(0, (path as String).get_file())
        item.set_text(1, path)
        item.set_tooltip_text(0, path)
        item.set_metadata(0, {"path": path})
        item.collapsed = false

        var operators: PackedStringArray = asset.ownedNodeOperators
        for operator_name in operators:
            var child := _tree.create_item(item)
            child.set_text(0, operator_name)
            child.set_metadata(0, {"path": path, "operator": operator_name})


func _on_item_activated() -> void:
    var item := _tree.get_selected()
    if item == null:
        return
    var meta: Variant = item.get_metadata(0)
    if meta is Dictionary and meta.has("operator"):
        _instantiate_operator(meta["path"], meta["operator"])


func _on_item_mouse_selected(_position: Vector2, mouse_button_index: int) -> void:
    if mouse_button_index != MOUSE_BUTTON_RIGHT:
        return
    var item := _tree.get_item_at_position(_position)
    if item == null:
        return
    var meta: Variant = item.get_metadata(0)
    if not (meta is Dictionary):
        return
    _show_context_menu(meta)


func _show_context_menu(meta: Dictionary) -> void:
    var menu := PopupMenu.new()
    menu.name = "HEUAssetContextMenu"
    if meta.has("operator"):
        menu.add_item("Instantiate in Scene", 0)
    menu.add_item("Reload HDA", 1)
    menu.add_item("Unload HDA", 2)
    menu.id_pressed.connect(func(id: int) -> void:
        match id:
            0:
                if meta.has("operator"):
                    _instantiate_operator(meta["path"], meta["operator"])
            1:
                HEUAssetManager.instance().reload_hda(meta["path"])
                _refresh_tree()
            2:
                HEUAssetManager.instance().unload_hda(meta["path"])
                _refresh_tree()
        menu.queue_free()
    )
    get_tree().root.add_child(menu)
    menu.position = get_global_mouse_position()
    menu.popup()


func _instantiate_operator(path: String, operator_name: String) -> void:
    if not HEUSessionService.instance().is_active():
        push_error("HEU: create a Houdini session before instantiating an HDA.")
        return
    var root := HEUUtility.create_root_from_hda(path, operator_name)
    if root == null:
        return
    if not root.ensure_houdini_node():
        push_error("HEU: failed to instantiate %s" % operator_name)
        return
    if root.auto_cook:
        root.recook()
