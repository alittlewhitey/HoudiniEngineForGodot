#!/usr/bin/env python
import os
from glob import glob
from pathlib import Path
main_env = Environment(ENV=os.environ)
debug = ARGUMENTS.get("debug", "no").lower() in ("yes", "true", "1")
if debug == None:
    debug = False
godot_cpp_branch = ARGUMENTS.get("godot_cpp_branch", None)
if godot_cpp_branch == None:
    Warning("Use branch 'master' of godot-cpp")
    Warning("You'd better define branch name like this: \nscons godot_cpp_branch=\"4.4\"")
    godot_cpp_branch = "master"
else:
    print(f"Use branch ${godot_cpp_branch} of godot-cpp.")
if not os.path.isdir("deps/godot-cpp/include"):
    main_env.Execute("git submodule update --init --recursive")
    main_env.Execute(f"cd deps/godot-cpp && git checkout {godot_cpp_branch} && cd ../..")
rewrite_branch_name = 0
try:
    with open(".godot_cpp_branch", "r", encoding="utf-8") as f:
        branch_name = f.read()
        if branch_name != godot_cpp_branch:
            rewrite_branch_name = main_env.Execute(f"cd deps/godot-cpp && git pull && git checkout {godot_cpp_branch} && cd ../..")
except Exception as e:
    pass
if rewrite_branch_name == 0:
    try:
        with open(".godot_cpp_branch", "w", encoding="utf-8") as f:
            f.write(godot_cpp_branch)
    except Exception as e:
        pass
filtered_args = {}
custom_args = {"godot_cpp_branch", "debug"}
for key, value in ARGUMENTS.items():
    if key not in custom_args:
        filtered_args[key] = value
original_args = ARGUMENTS.copy()
ARGUMENTS.clear()
ARGUMENTS.update(filtered_args)
ARGUMENTS['target'] = 'editor'
if debug:
    ARGUMENTS['dev_build'] = "yes"
    import platform as py_platform
    if py_platform.system() == "Windows":
        ARGUMENTS['debug_crt'] = "yes"
else: 
    ARGUMENTS['production'] = "yes"
env = SConscript("deps/godot-cpp/SConstruct")
ARGUMENTS.clear()
ARGUMENTS.update(original_args)

env.Append(CPPPATH=["deps"])
env.Append(CPPPATH=["deps/Houdini/"])
env.Append(CPPPATH=["src/"])

compiler = env['CXX']
if compiler == "$CC":
    compiler = env['CC']
print(f"Use compiler: {compiler}")
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
        env.Append(CXXFLAGS=["-DFORCE_DEBUG","-Zi", "-MDd"])
        env.Append(LINKFLAGS=["-DEBUG"])
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
    "{}/bin/lib{}.{}.{}{}".format(
        addon_path,
        project_name,
        platform,
        arch,
        dylib_suffix,
    ),
    source=sources,
)
Default(library)