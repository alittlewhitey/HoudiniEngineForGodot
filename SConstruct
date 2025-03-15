#!/usr/bin/env python
import os
from glob import glob
from pathlib import Path

HAPI_PATH = "/Applications/Houdini/Houdini20.5.487/Frameworks/Houdini.framework/Versions/20.5/Resources/toolkit/include/"

env = SConscript("deps/godot-cpp/SConstruct")
env.Append(CPPPATH=["deps/Houdini/"])
env.Append(CPPPATH=[HAPI_PATH])
env.Append(CPPPATH=["src/"])
env.Append(CXXFLAGS=['-pthread', '-fexperimental-library', '-std=c++20', '-fexceptions'], LINKFLAGS=['-pthread', '-fexperimental-library'])
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
if env["platform"] == "macos":
    library = env.SharedLibrary(
        "{0}/bin/lib{1}.{2}.{3}.framework/{1}.{2}.{3}".format(
            addon_path,
            project_name,
            env["platform"],
            debug_or_release,
        ),
        source=sources,
    )
else:
    library = env.SharedLibrary(
        "{}/bin/lib{}.{}.{}.{}{}".format(
            addon_path,
            project_name,
            env["platform"],
            debug_or_release,
            env["arch"],
            env["SHLIBSUFFIX"],
        ),
        source=sources,
    )

Default(library)
