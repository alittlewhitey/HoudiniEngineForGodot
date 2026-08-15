@tool
extends Control

const HEARTBEAT_INTERVAL := 1.0

var _status_icon: ColorRect
var _status_label: Label
var _type_option: OptionButton
var _nodes_label: Label
var _cook_progress: ProgressBar
var _heartbeat: Timer


func _ready() -> void:
    custom_minimum_size = Vector2(0, 46)
    size_flags_horizontal = SIZE_EXPAND_FILL
    _build_ui()
    _build_heartbeat()

    HEUSessionService.instance().session_changed.connect(_on_session_changed)
    _on_session_changed(HEUSessionService.instance().is_active(), HEUSessionService.instance().get_session_type())


func _exit_tree() -> void:
    if HEUSessionService.instance().session_changed.is_connected(_on_session_changed):
        HEUSessionService.instance().session_changed.disconnect(_on_session_changed)


func _build_ui() -> void:
    var bar := HBoxContainer.new()
    bar.name = "SessionBar"
    bar.size_flags_horizontal = SIZE_EXPAND_FILL
    bar.add_theme_constant_override("separation", 8)
    add_child(bar)

    _status_icon = ColorRect.new()
    _status_icon.custom_minimum_size = Vector2(14, 14)
    _status_icon.size_flags_vertical = SIZE_SHRINK_CENTER
    _status_icon.color = Color(0.78, 0.16, 0.16)
    bar.add_child(_status_icon)

    _status_label = Label.new()
    _status_label.text = "No Session"
    _status_label.size_flags_vertical = SIZE_SHRINK_CENTER
    bar.add_child(_status_label)

    var spacer := Control.new()
    spacer.size_flags_horizontal = SIZE_EXPAND_FILL
    bar.add_child(spacer)

    _cook_progress = ProgressBar.new()
    _cook_progress.custom_minimum_size = Vector2(120, 18)
    _cook_progress.size_flags_vertical = SIZE_SHRINK_CENTER
    _cook_progress.show_percentage = false
    _cook_progress.visible = false
    bar.add_child(_cook_progress)

    _nodes_label = Label.new()
    _nodes_label.text = "0 nodes"
    _nodes_label.size_flags_vertical = SIZE_SHRINK_CENTER
    bar.add_child(_nodes_label)

    _type_option = OptionButton.new()
    _type_option.add_item("InProcess", HEGSession.InProcess)
    _type_option.add_item("New Named Pipe", HEGSession.NewNamedPipe)
    _type_option.add_item("New TCP", HEGSession.NewTCPSocket)
    _type_option.add_item("Existing Named Pipe", HEGSession.ExistingNamedPipe)
    _type_option.add_item("Existing TCP", HEGSession.ExistingTCPSocket)
    _type_option.add_item("Existing Shared Memory", HEGSession.ExistingSharedMemory)
    bar.add_child(_type_option)

    var start_button := Button.new()
    start_button.text = "Start"
    start_button.tooltip_text = "Start or connect the selected Houdini session type."
    start_button.pressed.connect(_on_start_pressed)
    bar.add_child(start_button)

    var restart_button := Button.new()
    restart_button.text = "Restart"
    restart_button.tooltip_text = "Restart the current session type."
    restart_button.pressed.connect(_on_restart_pressed)
    bar.add_child(restart_button)

    var close_button := Button.new()
    close_button.text = "Close"
    close_button.tooltip_text = "Close the current session."
    close_button.pressed.connect(_on_close_pressed)
    bar.add_child(close_button)


func _build_heartbeat() -> void:
    _heartbeat = Timer.new()
    _heartbeat.name = "HeartbeatTimer"
    _heartbeat.wait_time = HEARTBEAT_INTERVAL
    _heartbeat.autostart = false
    add_child(_heartbeat)
    _heartbeat.timeout.connect(_on_heartbeat)


func _on_start_pressed() -> void:
    HEUSessionService.instance().start(_type_option.get_selected_id())
    _update_node_count()


func _on_restart_pressed() -> void:
    HEUSessionService.instance().restart()
    _update_node_count()


func _on_close_pressed() -> void:
    HEUSessionService.instance().stop()
    _update_node_count()


func _on_heartbeat() -> void:
    HEUSessionService.instance().poll()
    _update_node_count()
    _update_cook_activity()


func _on_session_changed(active: bool, _session_type: int) -> void:
    if active:
        _status_icon.color = Color(0.18, 0.64, 0.24)
        _status_label.text = "Session Connected"
        if _heartbeat != null and _heartbeat.is_stopped():
            _heartbeat.start()
    else:
        _status_icon.color = Color(0.78, 0.16, 0.16)
        _status_label.text = "No Session"
        if _heartbeat != null:
            _heartbeat.stop()
        _cook_progress.visible = false
    _update_node_count()


func _update_node_count() -> void:
    var service := HEUSessionService.instance()
    if not service.is_active():
        _nodes_label.text = "0 nodes"
        return
    var session := service.get_session()
    _nodes_label.text = "%d nodes" % session.getNodes().size()


func _update_cook_activity() -> void:
    if not is_inside_tree():
        return
    var cooking := 0
    var roots := get_tree().get_nodes_in_group(&"heu_asset_roots")
    for root in roots:
        if root.has_method("is_cooking") and root.is_cooking():
            cooking += 1
    _cook_progress.visible = cooking > 0
