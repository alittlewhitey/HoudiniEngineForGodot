class_name HEUSessionService
extends RefCounted

## GDScript-side session facade. All real HAPI work stays in HEGSession.
## The UI talks to this service and receives `session_changed` notifications.
signal session_changed(active: bool, session_type: int)

static var _instance: HEUSessionService

static func instance() -> HEUSessionService:
    if _instance == null:
        _instance = HEUSessionService.new()
    return _instance

var _last_active := false
var _last_type := -1


func get_session() -> HEGSession:
    return HEGSession.getSession()


func is_active() -> bool:
    var session := get_session()
    return session != null and session.valid()


func get_session_type() -> int:
    var session := get_session()
    if session == null:
        return HEGSession.None
    return session.getSessionType()


func start(session_type: int) -> bool:
    HEGSession.switchSession(session_type)
    var active := is_active()
    _emit_if_changed(active, get_session_type())
    return active


func stop() -> bool:
    var session := get_session()
    var ok := true
    if session != null and session.valid():
        ok = session.stop()
    _emit_if_changed(false, get_session_type())
    return ok


func restart() -> bool:
    var session := get_session()
    if session == null:
        return false
    var previous_type := session.getSessionType()
    if previous_type == HEGSession.None:
        previous_type = HEGSession.InProcess
    var ok := session.restart()
    _emit_if_changed(is_active(), get_session_type())
    return ok


## Cheap main-thread poll for the Session Dock. C++ currently only emits
## SessionStarted, so session loss is detected here until a SessionStopped
## signal is added in a future C++ pass.
func poll() -> void:
    _emit_if_changed(is_active(), get_session_type())


func _emit_if_changed(active: bool, session_type: int) -> void:
    if active == _last_active and session_type == _last_type:
        return
    _last_active = active
    _last_type = session_type
    session_changed.emit(active, session_type)
