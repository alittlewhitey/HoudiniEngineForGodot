#!/usr/bin/env python
import os
from glob import glob
from pathlib import Path
main_env = Environment()
debug = main_env["debug"]
if debug == None:
    debug = False
godot_cpp_branch = main_env["godot_cpp_branch"]
if godot_cpp_branch == None:
    Warning("Use branch 'master' of godot-cpp")
    Warning("You'd better define branch name like this: \nscons godot_cpp_branch=\"4.4\"")
    godot_cpp_branch = "master"
    
if not os.path.isdir("deps/godot-cpp/include"):
    main_env.Execute("git submodule update --init --recursive")
    main_env.Execute(f"cd deps/godot-cpp && git checkout {godot_cpp_branch} && cd ../..")

options = {
    "target": "editor",
    "CC": main_env["CC"],
    "CXX": main_env["CXX"],
    "use_mingw": main_env["use_mingw"],
    "use_llvm": main_env["use_llvm"]
}
godot_env = Environment().Clone()
godot_env.Replace(**options)
env = SConscript("deps/godot-cpp/SConstruct",exports={"env": godot_env})
env.Append(CPPPATH=["deps"])
env.Append(CPPPATH=["deps/Houdini/"])
env.Append(CPPPATH=["src/"])

compiler = env['CXX']
if compiler == "$CC":
    compiler = env['CC']
print(compiler)
if(compiler == "clang++"):
        env.Append(CXXFLAGS=['-pthread', '-fexperimental-library', '-std=c++20', '-fexceptions'], LINKFLAGS=['-pthread', '-fexperimental-library'])
elif(compiler == "g++"):
        env.Append(CXXFLAGS=['-pthread', '-std=c++20', '-fexceptions'], LINKFLAGS=['-pthread'])
elif(compiler == "cl"):
        env.Append(CXXFLAGS=['-EHsc', '-std:c++20'])
else:
    pass

sources = Glob("src/*.cpp") + Glob("deps/Houdini/*.cpp")

try:
    doc_data = env.GodotCPPDocData("src/gen/doc_data.gen.cpp", source=Glob("docs/*.xml"))
    sources.append(doc_data)
except AttributeError:
    print("Not including class reference as we're targeting a pre-4.3 baseline.")

(extension_path,) = glob("project/addons/HoudiniEngineForGodot/HoudiniEngineForGodot.gdextension")
addon_path = Path(extension_path).parent
project_name = Path(extension_path).stem
scons_cache_path = os.environ.get("SCONS_CACHE")
if scons_cache_path != None:
    CacheDir(scons_cache_path)
    print("Scons cache enabled... (path: '" + scons_cache_path + "')")

if debug:
    if(compiler == "cl"):
        env.Append(CXXFLAGS=["-DFORCE_DEBUG","-Z7"])
    else:
        env.Append(CXXFLAGS=["-DFORCE_DEBUG","-g"])

platform = env["platform"]
arch = env["arch"]
dylib_suffix = ".so"
if(platform == "macos"):
    dylib_suffix = ".dylib"
elif(platform == "windows"):
    dylib_suffix = ".dll"
elif(platform == "linux"):
    dylib_suffix = ".so"
if platform == "macos":
    if arch == "universal":
        arch = "arm64"

library = env.SharedLibrary(
    "{}/bin/lib{}.{}.{}.{}{}".format(
        addon_path,
        project_name,
        platform,
        "debug" if debug else "release",
        arch,
        dylib_suffix,
    ),
    source=sources,
)
Default(library)