class_name HEUUtility
extends RefCounted

## Pure editor helpers shared by the browser and the asset root. Keep HAPI
## access in HEUAssetRoot/HEUAssetManager; this file only deals with the
## scene tree and HEUAssetRoot construction.


static func create_root_from_hda(path: String, operator_name: String, parent: Node = null) -> HEUAssetRoot:
    var asset := HEUAssetManager.instance().load_hda(path)
    if asset == null:
        return null

    var hda_resource := HEGHdaResource.new()
    hda_resource.path = path

    var root := HEUAssetRoot.new()
    root.name = _make_root_name(path, operator_name)
    root.hda = hda_resource
    root.operator_name = operator_name
    root.auto_cook = ProjectSettings.get_setting("houdini/option/autoCook", false)

    var scene_root := _scene_root(parent)
    _add_owned(root, scene_root if parent == null else parent, scene_root)
    return root


static func create_root_from_node(node: HEGNode, source: HEUAssetRoot) -> HEUAssetRoot:
    if node == null or not node.isValid():
        return null

    var root := HEUAssetRoot.new()
    root.name = source.name + "_Duplicate"
    root.hda = source.hda
    root.operator_name = node.getName()
    root.auto_cook = source.auto_cook
    root.parameter_snapshot = source.parameter_snapshot.duplicate(true)
    root.material_overrides = source.material_overrides.duplicate(true)

    var parent := source.get_parent()
    var scene_root := _scene_root(parent)
    _add_owned(root, parent, scene_root)
    root.bind_node(node, node.getAsset())
    return root


static func _scene_root(hint: Node) -> Node:
    var editor_interface := Engine.get_singleton("EditorInterface")
    if editor_interface != null:
        var root: Node = editor_interface.get_edited_scene_root()
        if root != null:
            return root
    if hint != null and hint.is_inside_tree():
        return hint.get_tree().current_scene
    return hint


static func _add_owned(node: Node, parent: Node, scene_root: Node) -> void:
    if parent == null:
        parent = scene_root
    if parent == null:
        push_error("HEU: cannot add asset root, no scene root available.")
        node.free()
        return
    parent.add_child(node)
    node.owner = scene_root if scene_root != null else parent


static func _make_root_name(path: String, operator_name: String) -> String:
    if not operator_name.is_empty():
        return operator_name
    return path.get_file().get_basename()
