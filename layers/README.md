# Layers

- `front/`: host frontends. Current frontend is `godot4`. All public classes use the `HEG` prefix and may depend on godot-cpp.
- `back/core/`: host-independent core. All types use the `HE` prefix and must not depend on godot-cpp or a specific frontend.
- `back/hapi7/`: HAPI 7 adapter. HAPI version-specific types and calls are confined here.

`HEGHapi` is a deliberate low-level escape hatch exposed to GDScript and may depend on `back/hapi7` directly.
