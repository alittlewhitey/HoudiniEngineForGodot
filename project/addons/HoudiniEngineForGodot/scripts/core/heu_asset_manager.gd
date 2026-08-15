class_name HEUAssetManager
extends RefCounted

## path -> HEGAsset cache. C++ owns the authoritative library id registry,
## this layer adds the path-keyed cache and scene-root bookkeeping needed by
## the editor UI.
signal assets_changed

static var _instance: HEUAssetManager

static func instance() -> HEUAssetManager:
    if _instance == null:
        _instance = HEUAssetManager.new()
    return _instance

var _assets: Dictionary = {}          # absolute path -> HEGAsset
var _roots: Dictionary = {}           # absolute path -> Array[Node]


func get_assets() -> Dictionary:
    var result := {}
    for path in _assets:
        if _is_asset_valid(_assets[path]):
            result[path] = _assets[path]
    return result


func get_asset(path: String) -> HEGAsset:
    var key := _normalize_path(path)
    if key.is_empty():
        return null
    if _assets.has(key) and _is_asset_valid(_assets[key]):
        return _assets[key]
    return null


func has_asset(path: String) -> bool:
    return get_asset(path) != null


func load_hda(path: String) -> HEGAsset:
    if not HEUSessionService.instance().is_active():
        push_error("HEU: cannot load HDA, no active Houdini session.")
        return null

    var key := _normalize_path(path)
    if key.is_empty():
        push_error("HEU: invalid HDA path: %s" % path)
        return null

    var existing := get_asset(key)
    if existing != null:
        return existing

    var asset := HEUSessionService.instance().get_session().loadHDAExternal(key)
    if asset == null or not asset.isValid():
        push_error("HEU: failed to load HDA: %s" % key)
        return null

    _assets[key] = asset
    assets_changed.emit()
    return asset


func reload_hda(path: String) -> bool:
    var key := _normalize_path(path)
    if not _assets.has(key) or not _is_asset_valid(_assets[key]):
        push_error("HEU: cannot reload unknown HDA: %s" % key)
        return false

    var asset: HEGAsset = _assets[key]
    if not asset.reload():
        push_error("HEU: failed to reload HDA: %s" % key)
        return false

    assets_changed.emit()
    _rebind_roots_for_path(key, asset)
    return true


func unload_hda(path: String, force: bool = false) -> bool:
    var key := _normalize_path(path)
    if not _assets.has(key):
        return true

    if not force and get_root_count(key) > 0:
        push_warning("HEU: HDA has scene instances. Rebuild/delete them before unloading.")
        return false

    var asset: HEGAsset = _assets[key]
    if _is_asset_valid(asset):
        if not asset.unload():
            push_error("HEU: failed to unload HDA: %s" % key)
            return false
    _assets.erase(key)
    assets_changed.emit()
    return true


func register_root(root: Node) -> void:
    if root == null:
        return
    var key := _normalize_path(root.get_hda_path())
    if key.is_empty():
        return
    if not _roots.has(key):
        _roots[key] = []
    var roots: Array = _roots[key]
    if not roots.has(root):
        roots.append(root)


func unregister_root(root: Node) -> void:
    if root == null:
        return
    var key := _normalize_path(root.get_hda_path())
    if not _roots.has(key):
        return
    _roots[key].erase(root)
    if (_roots[key] as Array).is_empty():
        _roots.erase(key)


func get_roots(path: String) -> Array:
    var key := _normalize_path(path)
    if not _roots.has(key):
        return []
    return (_roots[key] as Array).duplicate()


func get_root_count(path: String) -> int:
    return get_roots(path).size()


func all_roots() -> Array:
    var result: Array = []
    for path in _roots:
        for root in _roots[path]:
            if is_instance_valid(root):
                result.append(root)
    return result


func _rebind_roots_for_path(path: String, asset: HEGAsset) -> void:
    var roots := get_roots(path)
    if roots.is_empty() or asset == null or not asset.isValid():
        return
    var instances: Array = asset.getInstances()
    for root in roots:
        if root != null and root.has_method("bind_existing_instances"):
            root.bind_existing_instances(instances)


func _normalize_path(path: String) -> String:
    if path.is_empty():
        return ""
    if path.begins_with("res://") or path.begins_with("user://"):
        return ProjectSettings.globalize_path(path)
    return path


func _is_asset_valid(asset) -> bool:
    return asset != null and is_instance_valid(asset) and asset.isValid()
