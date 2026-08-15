@tool
class_name HEUSmokeTest
extends RefCounted

## Headless runtime smoke test for the GDScript UI layer and the
## HEGNode.refreshParameters() C++ addition.
## Run the editor with HEU_SMOKE_TEST=1; plugin.gd invokes this automatically.

static func run() -> Dictionary:
    var result := {"ok": false, "session": false, "asset": false, "node": false, "refresh": false, "parm_before": -1, "parm_after": -1, "operators": 0, "error": ""}

    var session_started := HEUSessionService.instance().start(HEGSession.InProcess)
    result["session"] = session_started
    if not session_started:
        result["error"] = "failed to start in-process session"
        return result

    var session := HEUSessionService.instance().get_session()
    var path := ProjectSettings.globalize_path("res://hda/hexagona_lite.hda")
    var asset := session.loadHDAExternal(path)
    result["asset"] = asset != null and asset.isValid()
    if not result["asset"]:
        result["error"] = "failed to load " + path
        return result

    var operators: PackedStringArray = asset.ownedNodeOperators
    result["operators"] = operators.size()
    if operators.is_empty():
        result["error"] = "HDA exposes no operators"
        return result

    var node := asset.instantiate(operators[0])
    result["node"] = node != null and node.isValid()
    if not result["node"]:
        result["error"] = "failed to instantiate " + operators[0]
        return result

    result["parm_before"] = node.getParameterList().size()
    result["refresh"] = node.refreshParameters()
    result["parm_after"] = node.getParameterList().size()
    result["ok"] = result["refresh"] and result["parm_after"] > 0

    var infos: Array = node.getParameterInfos()
    print("[HEU-SMOKE] session=%s asset=%s node=%s refresh=%s parm_count=%d infos=%d" % [
        result["session"], result["asset"], result["node"], result["refresh"], result["parm_after"], infos.size()
    ])
    for info in infos:
        var label: Variant = info.get("label", "<missing>")
        var help: Variant = info.get("help", "<missing>")
        print("[HEU-SMOKE]   parm=%s label=<%s> help=<%s> type=%s scriptType=%s" % [
            info.get("name", "?"), label, help, info.get("type", "?"), info.get("scriptType", "?")
        ])
    return result
