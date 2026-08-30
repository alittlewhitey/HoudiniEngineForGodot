# Houdini Engine for Godot

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

An open-source **Godot 4 GDExtension** that brings the SideFX **Houdini Engine**
procedural workflow into Godot through the HAPI API.

> **Status:** active early development. The core editor workflow is usable, but
> the GDScript/C++ API and the generated scene layout may still change between
> releases.

## Features

- **Houdini sessions** — start, stop, and restart sessions. Supports InProcess,
  new named pipe, new TCP socket, and connecting to existing named pipe, TCP
  socket, or shared memory sessions.
- **HDA / HIP import** — editor import plugins for `.hda`, `.otl`, `.hdalc`,
  and `.hip` files, with importable resource types.
- **Asset and node workflows** — load HDA libraries, instantiate operators,
  create/duplicate/rebuild nodes, connect and disconnect inputs, and cook
  individual nodes or the whole session.
- **Parameter editor** — inspect and edit HDA parameters from the editor with
  search, folder groups, multiparms, and widgets for int/float/string/vector,
  color, choices, file paths, node references, and arrays. Includes debounced
  auto-cook.
- **Geometry generation** — cook SOP nodes into Godot meshes, MultiMeshes, and
  curves; cook COP nodes into Godot images; apply material overrides.
- **Scene-side asset node** — `HEUAssetRoot` stores only serializable data
  (HDA resource, operator name, parameter snapshot, material overrides) and
  regenerates outputs when recooked. Supports recook, rebuild, duplicate,
  reset, and bake-to-scene.
- **Responsive editor** — cooking runs on a background worker with a
  thread-safe main-thread task queue, so the Godot editor stays responsive.
- **Layered architecture** — a HAPI 7 adapter, a host-independent C++ core,
  and a Godot 4 frontend keep the integration maintainable and testable.
- **CI builds** — GitHub Actions builds the extension for Windows, macOS, and
  Linux when a `Release-v*.*.*` tag is pushed.

## Requirements

- **Godot 4.3 or newer** (developed and tested against Godot 4.4).
- A C++20-capable compiler: Clang, GCC, or MSVC.
- **CMake 3.20+** and Python 3 (or SCons) to build `godot-cpp` and the plugin.
- **SideFX Houdini** with Houdini Engine installed and licensed. The plugin
  loads `libHAPIL` / `libHAPI` either from the Houdini environment or from a
  configured library path.
- Git submodules are initialized during the build when needed.

## Building

### 1. Clone the repository

```bash
git clone --recurse-submodules https://github.com/alittlewhitey/HoudiniEngineForGodot.git
cd HoudiniEngineForGodot
```

### 2. Build with CMake

```bash
cmake -S . -B build -DGODOT_CPP_BRANCH=4.4
cmake --build build -j
```

The shared library is copied automatically to
`project/addons/HoudiniEngineForGodot/bin/`.

### 3. Or build with SCons

```bash
# Release editor build for the current platform
scons godot_cpp_branch="4.4"

# Debug build
scons godot_cpp_branch="4.4" debug=yes

# Cross-platform examples
scons platform=windows arch=x86_64 godot_cpp_branch="4.4"
scons platform=linux arch=x86_64 godot_cpp_branch="4.4"
```

### 4. Open the example project

```bash
godot --editor --path project
```

The plugin is already enabled in the example project. You can also copy the
`project/addons/HoudiniEngineForGodot` directory into your own Godot project
and enable it from **Project > Project Settings > Plugins**.

## Houdini setup

The plugin reads its settings from **Project Settings** under the `houdini/`
section:

| Setting | Default | Description |
| --- | --- | --- |
| `houdini/config/useEnvLibPath` | `true` | Load HAPI from the Houdini environment (`HFS` / library search path). |
| `houdini/config/hapiLib` | `HAPIL` | Use `libHAPIL` (recommended) or `libHAPI`. |
| `houdini/config/houdiniRootPath` | empty | Houdini installation root, used when configuring from the editor. |
| `houdini/config/houdiniLibPath` | empty | Explicit HAPI library directory when `useEnvLibPath` is `false`. |
| `houdini/config/logFilePath` | empty | Optional log file path. |
| `houdini/config/sessionConfig` | defaults below | Named pipe, host, shared memory, TCP port, and timeout settings. |
| `houdini/config/cookOptions` | HAPI defaults | Split geometry, curve refinement, instancing, and other cook options. |
| `houdini/option/autoCook` | `false` | Cook automatically after session-wide changes. |
| `houdini/option/useCookingThread` | `true` | Run cooks on the background worker thread. |

Default session settings are `namedPipe = "hapi"`, `hostName = "127.0.0.1"`,
`sharedMemoryName = "hapi"`, `tcpPort = 9090`, and
`newSessionTimeoutSec = 120`.

> On a clean checkout, make sure Houdini is discoverable from the environment
> you launch Godot from, or set `houdini/config/useEnvLibPath` to `false` and
> point `houdini/config/houdiniLibPath` to your Houdini Engine libraries.

## Usage

1. Start a session from **HEU > Session > Start InProcess** (or one of the
   named pipe / TCP / shared memory options).
2. Load an HDA with **HEU > Load HDA...** or from the **HEU Assets** dock,
   then instantiate an operator into the scene.
3. Select the generated `HEUAssetRoot` node. The **HEU Params** dock shows its
   parameters with search, folders, and auto-cook.
4. Use **HEU > Selected Asset** for recook, rebuild, and duplicate actions.
5. Outputs are generated as child nodes and can be baked into the scene.

## Contributing

Issues, bug reports, and pull requests are welcome. For larger changes, please
open an issue first to discuss the approach.

The project is currently maintained by
[@alittlewhitey](https://github.com/alittlewhitey).

## License

This project is licensed under the [MIT License](LICENSE).

The bundled HAPI headers under `layers/back/hapi7/headers` are generated from
Side Effects Software's Houdini Engine API and retain their original Side
Effects Software copyright notice and MIT license text.
