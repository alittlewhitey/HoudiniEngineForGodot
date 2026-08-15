@tool
class_name HEUAssetRoot
extends Node3D

## Scene-side Houdini asset root. This is the Godot counterpart of Unity's
## HEU_HoudiniAssetRoot/HEU_HoudiniAsset pair.
##
## Only serializable data is stored in this node:
##   hda resource + operator name + parameter snapshot + material overrides.
## HEGNode/HEGAsset are transient RefCounted objects and are rebound after a
## scene reload or a session restart.

signal cook_started
signal cook_finished(success: bool)
signal outputs_rebuilt

const GENERATED_META := &"_heu_generated"

@export var hda: HEGHdaResource
@export var operator_name: String = ""
@export var auto_cook: bool = true
@export var parameter_snapshot: Dictionary = {}
@export var material_overrides: Dictionary = {}

const COOK_DEBOUNCE_SEC := 0.25

var _he_node: HEGNode = null
var _asset: HEGAsset = null
var _cooking: bool = false
var _cook_pending: bool = false
var _pending_parameter_changes: Dictionary = {}
var _cook_debounce: Timer = null
var _last_error: String = ""
var _cook_generation: int = 0


func _ready() -> void:
    set_meta(&"HEUAssetRoot", true)
    add_to_group(&"heu_asset_roots")
    _ensure_cook_timer()
    if Engine.is_editor_hint():
        HEUAssetManager.instance().register_root(self)
        call_deferred(&"_restore_from_scene")


func _exit_tree() -> void:
    if Engine.is_editor_hint():
        HEUAssetManager.instance().unregister_root(self)


func get_hda_path() -> String:
    if hda == null:
        return ""
    return hda.path


func get_he_node() -> HEGNode:
    return _he_node


func get_he_asset() -> HEGAsset:
    return _asset


func is_cooking() -> bool:
    return _cooking


func get_cook_generation() -> int:
    return _cook_generation


func get_last_error() -> String:
    return _last_error


## Bind an already-created HEGNode (used by the asset browser and by
## duplicate/rebuild flows).
func bind_node(node: HEGNode, asset: HEGAsset) -> bool:
    if node == null or not node.isValid():
        push_error("HEU: cannot bind invalid HEGNode.")
        return false
    _disconnect_node()
    _he_node = node
    _asset = asset
    _connect_node()
    _refresh_parameters_from_houdini()
    _restore_parameter_snapshot()
    return true


## Rebind after C++ reload_hda recreated instances. The old node is invalid;
## the C++ HEGAsset object stays valid and `asset.getInstances()` contains the
## new roots.
func bind_existing_instances(instances: Array) -> void:
    if instances.is_empty():
        return
    var chosen: HEGNode = null
    for instance in instances:
        if instance == null:
            continue
        var node: HEGNode = instance
        if operator_name.is_empty() or node.getName() == operator_name:
            chosen = node
            break
    if chosen == null:
        chosen = instances[0] as HEGNode
    if chosen != null:
        bind_node(chosen, _asset if _asset != null and _asset.isValid() else chosen.getAsset())


func ensure_houdini_node() -> bool:
    if _he_node != null and _he_node.isValid():
        return true

    if not HEUSessionService.instance().is_active():
        push_warning("HEU: no active Houdini session, cannot create node for %s." % name)
        return false

    var path := get_hda_path()
    if path.is_empty():
        push_error("HEU: no HDA assigned to %s." % name)
        return false

    var asset := HEUAssetManager.instance().load_hda(path)
    if asset == null:
        return false

    if operator_name.is_empty():
        var operators: PackedStringArray = asset.ownedNodeOperators
        if operators.is_empty():
            push_error("HEU: HDA has no available operators: %s" % path)
            return false
        operator_name = operators[0]

    var node := asset.instantiate(operator_name)
    if node == null or not node.isValid():
        push_error("HEU: failed to instantiate operator '%s' from %s." % [operator_name, path])
        return false

    bind_node(node, asset)
    return true


func request_cook() -> void:
    if not auto_cook:
        return
    _ensure_cook_timer()
    if _cooking:
        # A parameter changed while Houdini is already cooking. The current
        # cook may read a half-applied value, so schedule exactly one more
        # cook after cook_finished.
        _cook_pending = true
        return
    _cook_debounce.start()


func recook() -> bool:
    if _cooking:
        _cook_pending = true
        return false
    if not ensure_houdini_node():
        return false

    _cooking = true
    _cook_pending = false
    _last_error = ""
    _cook_generation += 1
    cook_started.emit()

    if not _he_node.cook():
        _finish_cook(false)
        return false
    return true


func rebuild() -> bool:
    if _he_node != null and _he_node.isValid():
        _capture_parameter_snapshot()
        var new_node: HEGNode = _he_node.rebuild()
        if new_node != null and new_node.isValid():
            bind_node(new_node, new_node.getAsset())
            return true
        return false
    return ensure_houdini_node()


func duplicate_asset() -> HEUAssetRoot:
    if _he_node != null and _he_node.isValid():
        _capture_parameter_snapshot()
        var dup_node: HEGNode = _he_node.duplicate()
        if dup_node != null and dup_node.isValid():
            return HEUUtility.create_root_from_node(dup_node, self)
    return null


func reset_parameters() -> bool:
    if not ensure_houdini_node():
        return false
    _capture_parameter_snapshot()
    var ok: bool = _he_node.resetParameters()
    if ok:
        parameter_snapshot.clear()
        _refresh_parameters_from_houdini()
    return ok


func set_parameter(parm_name: String, value: Variant) -> bool:
    if not ensure_houdini_node():
        return false

    # HAPI parameter writes must not race with the cook worker. If a cook is
    # running, queue the value and apply it immediately after cook_finished.
    parameter_snapshot[parm_name] = value
    if _cooking:
        _pending_parameter_changes[parm_name] = value
        if auto_cook:
            _cook_pending = true
        return true

    _apply_parameter_change(parm_name, value)
    if auto_cook:
        request_cook()
    return true


func _apply_parameter_change(parm_name: String, value: Variant) -> void:
    if _he_node == null or not _he_node.isValid():
        return
    # HEUAssetRoot owns auto-cook scheduling. Suppress the C++ global
    # auto-cook to avoid two cooks (one unsignaled C++ cook + one UI cook).
    _he_node.setParameter(parm_name, value, false)
    parameter_snapshot[parm_name] = value


func _flush_pending_parameter_changes() -> void:
    if _pending_parameter_changes.is_empty():
        return
    var pending := _pending_parameter_changes.duplicate()
    _pending_parameter_changes.clear()
    for parm_name in pending:
        _apply_parameter_change(parm_name, pending[parm_name])


func get_parameter(parm_name: String) -> Variant:
    if not ensure_houdini_node():
        return null
    return _he_node.getParameter(parm_name)


func get_parameter_infos() -> Array:
    if not ensure_houdini_node():
        return []
    return _he_node.getParameterInfos()


func get_parameter_choices(parm_name: String) -> Array:
    if not ensure_houdini_node():
        return []
    return _he_node.getParameterChoices(parm_name)


## Refresh the C++ parameter cache without a cook. This is the reason for the
## small refreshParameters() C++ addition.
func refresh_parameters() -> void:
    if _he_node == null or not _he_node.isValid():
        return
    if _he_node.has_method(&"refreshParameters"):
        _he_node.refreshParameters()


func generate_outputs() -> void:
    _clear_generated_outputs()
    if _he_node == null or not _he_node.isValid():
        return

    if not _he_node is HEGSopNode:
        return

    var sop := _he_node as HEGSopNode
    var scene_root := get_tree().get_edited_scene_root() if Engine.is_editor_hint() else get_tree().current_scene
    if scene_root == null:
        scene_root = get_parent()

    var part_count: int = sop.getGeometryCount()
    for part_id in range(part_count):
        var geometry := sop.getGeometry(part_id)
        if geometry == null:
            continue

        if geometry is HEGMesh:
            _add_mesh_output(geometry as HEGMesh, part_id, scene_root)
        elif geometry is HEGMultiMesh:
            _add_multimesh_output(geometry as HEGMultiMesh, part_id, scene_root)
        elif geometry is HEGCurve:
            _add_curve_output(geometry as HEGCurve, part_id, scene_root)

    outputs_rebuilt.emit()


func clear_outputs() -> void:
    _clear_generated_outputs()


func bake_to_scene() -> Node3D:
    var generated := _generated_output_nodes()
    if generated.is_empty():
        push_warning("HEU: no generated outputs to bake.")
        return null

    var parent := get_parent()
    var scene_root := get_tree().get_edited_scene_root() if Engine.is_editor_hint() else get_tree().current_scene
    if scene_root == null:
        scene_root = parent

    var bake_root: Node3D
    if generated.size() == 1:
        bake_root = (generated[0] as Node).duplicate() as Node3D
    else:
        bake_root = Node3D.new()
        bake_root.name = name + "_Baked"
        for output in generated:
            var copy := (output as Node).duplicate()
            bake_root.add_child(copy)
            copy.owner = scene_root

    bake_root.name = name + "_Baked"
    parent.add_child(bake_root)
    bake_root.owner = scene_root
    bake_root.set_meta(&"_heu_baked", true)
    return bake_root


func _restore_from_scene() -> void:
    if not Engine.is_editor_hint():
        return
    if not is_inside_tree():
        return
    if not HEUSessionService.instance().is_active():
        return

    # The asset browser binds the HEGNode immediately after adding the root.
    # In that case the browser owns the initial recook. Only auto-cook here
    # when this deferred restore created the node itself (true scene reload).
    var was_already_bound: bool = _he_node != null and _he_node.isValid()
    if ensure_houdini_node():
        if auto_cook and not was_already_bound and not _cooking:
            recook()


func _connect_node() -> void:
    if _he_node != null and _he_node.cookFinished.is_connected(_on_node_cook_finished):
        return
    if _he_node != null:
        _he_node.cookFinished.connect(_on_node_cook_finished)


func _disconnect_node() -> void:
    if _he_node != null and _he_node.cookFinished.is_connected(_on_node_cook_finished):
        _he_node.cookFinished.disconnect(_on_node_cook_finished)


func _on_node_cook_finished(success: bool) -> void:
    if not success:
        _last_error = _he_node.getLastCookError() if _he_node != null else ""
    _finish_cook(success)


func _finish_cook(success: bool) -> void:
    _cooking = false

    # The cook worker has exited; it is safe to apply values that arrived
    # while it was running. They will be picked up by the follow-up cook.
    _flush_pending_parameter_changes()

    if success and _he_node != null and _he_node.isValid():
        _capture_parameter_snapshot()
        generate_outputs()
    cook_finished.emit(success)

    if _cook_pending:
        _cook_pending = false
        request_cook()


func _ensure_cook_timer() -> void:
    if _cook_debounce != null:
        return
    _cook_debounce = Timer.new()
    _cook_debounce.name = "HEUCookDebounce"
    _cook_debounce.one_shot = true
    _cook_debounce.wait_time = COOK_DEBOUNCE_SEC
    _cook_debounce.timeout.connect(_on_cook_debounce_timeout)
    add_child(_cook_debounce)


func _on_cook_debounce_timeout() -> void:
    if _cooking:
        _cook_pending = true
        return
    recook()


func _capture_parameter_snapshot() -> void:
    if _he_node == null or not _he_node.isValid():
        return
    var infos := _he_node.getParameterInfos()
    if infos.is_empty():
        return
    for info in infos:
        var parm_name: String = info.get("name", "")
        if parm_name.is_empty():
            continue
        var value: Variant = _he_node.getParameter(parm_name)
        if value != null:
            parameter_snapshot[parm_name] = value


func _restore_parameter_snapshot() -> void:
    if _he_node == null or parameter_snapshot.is_empty():
        return
    for parm_name in parameter_snapshot:
        # Snapshot restore is not a user edit. The caller decides when to cook.
        _he_node.setParameter(parm_name, parameter_snapshot[parm_name], false)


func _refresh_parameters_from_houdini() -> void:
    if _he_node == null or not _he_node.isValid():
        return
    if _he_node.has_method(&"refreshParameters"):
        _he_node.refreshParameters()


func _clear_generated_outputs() -> void:
    for child in get_children():
        if child.has_meta(GENERATED_META):
            remove_child(child)
            child.queue_free()


func _generated_output_nodes() -> Array[Node]:
    var result: Array[Node] = []
    for child in get_children():
        if child.has_meta(GENERATED_META):
            result.append(child)
    return result


func _add_mesh_output(mesh_geo: HEGMesh, part_id: int, scene_root: Node) -> void:
    var mesh := mesh_geo.bakeAsMesh()
    if mesh == null:
        return
    var mesh_instance := MeshInstance3D.new()
    mesh_instance.name = "Mesh_%d" % part_id
    mesh_instance.mesh = mesh
    for surface_index in mesh.get_surface_count():
        var material := mesh_geo.getMaterial(surface_index)
        if material != null:
            mesh_instance.set_surface_override_material(surface_index, material)
    add_generated_child(mesh_instance, scene_root)


func _add_multimesh_output(multimesh_geo: HEGMultiMesh, part_id: int, scene_root: Node) -> void:
    var multimeshes: Dictionary = multimesh_geo.bakeAsMultiMesh()
    if multimeshes.is_empty():
        return
    var index := 0
    for key in multimeshes:
        var instance := MultiMeshInstance3D.new()
        instance.name = "MultiMesh_%d_%d" % [part_id, index]
        instance.multimesh = multimeshes[key]
        add_generated_child(instance, scene_root)
        index += 1


func _add_curve_output(curve_geo: HEGCurve, part_id: int, scene_root: Node) -> void:
    var curves: Array = curve_geo.bakeAsCurve()
    var index := 0
    for curve in curves:
        var path := Path3D.new()
        path.name = "Curve_%d_%d" % [part_id, index]
        path.curve = curve
        add_generated_child(path, scene_root)
        index += 1


func add_generated_child(child: Node, scene_root: Node) -> void:
    child.set_meta(GENERATED_META, true)
    add_child(child)
    if scene_root != null and scene_root != self and scene_root.is_ancestor_of(child):
        child.owner = scene_root
