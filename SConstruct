#!/usr/bin/env python
import os
from glob import glob
from pathlib import Path

env = SConscript("deps/godot-cpp/SConstruct")
#env["CXX"] = "/opt/homebrew/opt/llvm/bin/clang++"
env.Append(CPPPATH=["deps"])
env.Append(CPPPATH=["deps/Houdini/"])
env.Append(CPPPATH=["src/"])
if env['CC'] == 'clang':
    env.Append(CXXFLAGS=['-pthread', '-fexperimental-library', '-std=c++20', '-fexceptions'], LINKFLAGS=['-pthread', '-fexperimental-library'])
else:
    env.Append(CXXFLAGS=['-pthread', '-std=c++20', '-fexceptions'], LINKFLAGS=['-pthread'])
sources = Glob("src/*.cpp") + Glob("deps/Houdini/*.cpp")
(extension_path,) = glob("project/addons/*/*.gdextension")

addon_path = Path(extension_path).parent

project_name = Path(extension_path).stem

scons_cache_path = os.environ.get("SCONS_CACHE")
if scons_cache_path != None:
    CacheDir(scons_cache_path)
    print("Scons cache enabled... (path: '" + scons_cache_path + "')")

# Create the library target (e.g. libexample.linux.debug.x86_64.so).
debug_or_release = "release" if env["target"] == "template_release" else "debug"
# if env["platform"] == "macos":
#     library = env.SharedLibrary(
#         "{0}/bin/lib{1}.{2}.{3}.framework/{1}.{2}.{3}".format(
#             addon_path,
#             project_name,
#             env["platform"],
#             debug_or_release,
#         ),
#         source=sources,
#     )
# else:
platform = env["platform"]
arch = env["arch"]
dylib_suffix = ".so"
if platform == "macos":
    dylib_suffix = ".dylib"
elif platform == "windows":
    dylib_suffix = ".dll"
elif platform == "linux":
    dylib_suffix = ".so"
if platform == "macos":
    if arch == "universal":
        arch = "arm64"

library = env.SharedLibrary(
    "{}/bin/lib{}.{}.{}.{}{}".format(
        addon_path,
        project_name,
        platform,
        debug_or_release,
        arch,
        dylib_suffix,
    ),
    source=sources,
)

Default(library)
print(env["CXX"])