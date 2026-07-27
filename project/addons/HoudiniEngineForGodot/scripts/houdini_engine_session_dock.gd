@tool
extends Control

const HEARTBEAT_INTERVAL: float = 3.0

var _heartbeat_timer: Timer
var _status_label: Label
var _status_icon: ColorRect
var _progress_bar: ProgressBar


func _ready():
	custom_minimum_size = Vector2(0, 48)
	size_flags_horizontal = SIZE_EXPAND_FILL
	_setup_ui()
	_setup_heartbeat()


func _setup_ui():
	custom_minimum_size = Vector2(0, 48)
	size_flags_horizontal = SIZE_EXPAND_FILL

	var container := HBoxContainer.new()
	container.name = "SessionBar"
	container.size_flags_horizontal = SIZE_EXPAND_FILL
	add_child(container)

	_status_icon = ColorRect.new()
	_status_icon.custom_minimum_size = Vector2(16, 16)
	_status_icon.color = Color.RED
	container.add_child(_status_icon)

	_status_label = Label.new()
	_status_label.text = "No Session"
	container.add_child(_status_label)

	container.add_child(_spacer())

	_progress_bar = ProgressBar.new()
	_progress_bar.custom_minimum_size = Vector2(120, 16)
	_progress_bar.hide()
	container.add_child(_progress_bar)

	var btn_inprocess := Button.new()
	btn_inprocess.text = "InProcess"
	btn_inprocess.tooltip_text = "Create an in-process Houdini session"
	btn_inprocess.pressed.connect(func(): create_session(HESession.InProcess))
	container.add_child(btn_inprocess)

	var btn_pipe := Button.new()
	btn_pipe.text = "Pipe"
	btn_pipe.tooltip_text = "Create a named pipe Houdini session"
	btn_pipe.pressed.connect(func(): create_session(HESession.NewNamedPipe))
	container.add_child(btn_pipe)

	var btn_tcp := Button.new()
	btn_tcp.text = "TCP"
	btn_tcp.tooltip_text = "Create a TCP socket Houdini session"
	btn_tcp.pressed.connect(func(): create_session(HESession.NewTCPSocket))
	container.add_child(btn_tcp)

	var btn_close := Button.new()
	btn_close.text = "Close"
	btn_close.tooltip_text = "Close the current session"
	btn_close.pressed.connect(close_session)
	container.add_child(btn_close)


func _spacer() -> Control:
	var spacer := Control.new()
	spacer.size_flags_horizontal = SIZE_EXPAND_FILL
	return spacer


func _setup_heartbeat():
	_heartbeat_timer = Timer.new()
	_heartbeat_timer.name = "HeartbeatTimer"
	_heartbeat_timer.wait_time = HEARTBEAT_INTERVAL
	_heartbeat_timer.timeout.connect(_on_heartbeat)
	_heartbeat_timer.autostart = false
	add_child(_heartbeat_timer)


func auto_create_session():
	var session_type = HESession.InProcess
	if ProjectSettings.has_setting("houdini/config/sessionConfig"):
		var config: Dictionary = ProjectSettings.get_setting("houdini/config/sessionConfig")
		var type_str: String = config.get("sessionType", "InProcess")
		match type_str:
			"NamedPipe": session_type = HESession.NewNamedPipe
			"TCP": session_type = HESession.NewTCPSocket
	create_session(session_type)


func create_session(type: int):
	HESession.switchSession(type)
	_update_status()


func close_session():
	# Note: HEClass has no closeSession() — switchSession would re-create.
	_heartbeat_timer.stop()
	_update_status()


func _on_heartbeat():
	_update_status()


func _update_status():
	var session = HESession.getSession()
	if session and session.valid():
		_status_icon.color = Color.GREEN
		_status_label.text = "Session Connected"
		if not _heartbeat_timer.is_stopped() == false:
			_heartbeat_timer.start()
	else:
		_status_icon.color = Color.RED
		_status_label.text = "No Session"


func start_cook_progress_polling():
	_progress_bar.show()
	_progress_bar.value = 0.0


func stop_cook_progress_polling():
	_progress_bar.hide()
	_progress_bar.value = 0.0


func get_session() -> HESession:
	return HESession.getSession()
