from importlib import import_module
import sys
import os
def collect_files(dir_path):
    file_list = []
    for root, _, files in os.walk(dir_path):
        for file in files:
            file_path = os.path.join(root, file)
            file_list.append(file_path)
    return file_list
def call_function_from_file(relative_path, function_name, *args, **kwargs):
    current_dir = os.path.dirname(os.path.abspath(__file__))
    target_dir = os.path.dirname(os.path.join(current_dir, relative_path))
    file_name = os.path.basename(relative_path)
    module_name = os.path.splitext(file_name)[0]
    if target_dir not in sys.path:
        sys.path.insert(0, target_dir)
    module = import_module(module_name)
    if hasattr(module, function_name):
        func = getattr(module, function_name)
        return func(*args, **kwargs)
    else:
        raise AttributeError(f"Couldn't file function {function_name} in module {module_name}")

if __name__ == "__main__":
    doc_dir = "docs"
    all_files = collect_files(doc_dir)
    call_function_from_file("deps/godot-cpp/doc_source_generator.py","generate_doc_source","layers/front/godot4/src/gen/doc_data.gen.cpp",all_files)
