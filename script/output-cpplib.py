import os, sys, re
import shutil
import time

cwd = os.path.dirname(os.path.abspath(__file__))


def _copytree_h_file(dir, files):
    l = []
    for file in files:
        _abs_path = os.path.join(dir, file)
        if os.path.isfile(_abs_path):
            if not file.endswith('.h'):
                l.append(file)
    return l


_root_dir_name = "qtontotaskbar"
_name = "qtontotaskbar"
if __name__ == "__main__":
    _platform = sys.argv[1]
    if _platform not in (
            "linux32", "linux64",
            "win32msvc", "win32mingw", "win64msvc", "win64mingw",
            "darwin",
            "android",
    ): raise ValueError(str(_platform))
    _root_path = os.path.split(cwd)[0]
    _outside_path = os.path.split(_root_path)[0]
    _src_path = os.path.abspath(os.path.join(_root_path, "src"))
    _output_path = os.path.abspath(os.path.join(_root_path, "output"))
    _bin_path = os.path.abspath(os.path.join(_output_path, "bin"))
    _include_path = os.path.abspath(os.path.join(_output_path, "include"))
    _lib_path = os.path.abspath(os.path.join(_output_path, "lib", ))
    if os.path.exists(_include_path): shutil.rmtree(_include_path)

    _include_path = os.path.join(_include_path, _name)
    if _platform in ("linux32",):
        _lib_path = os.path.join(_lib_path, "linux32")
    elif _platform in ("linux64",):
        _lib_path = os.path.join(_lib_path, "linux64")
    elif _platform in ("win32msvc",):
        _lib_path = os.path.join(_lib_path, "win32msvc")
    elif _platform in ("win32mingw",):
        _lib_path = os.path.join(_lib_path, "win32mingw")
    elif _platform in ("win64msvc",):
        _lib_path = os.path.join(_lib_path, "win64msvc")
    elif _platform in ("win64mingw",):
        _lib_path = os.path.join(_lib_path, "win64mingw")
    elif _platform in ("darwin",):
        _lib_path = os.path.join(_lib_path, "macos")
    elif _platform in ("android",):
        _lib_path = os.path.join(_lib_path, "android")
    _lib_path = os.path.join(_lib_path, _name)

    time.sleep(0.5)
    os.makedirs(_lib_path, exist_ok=True)
    os.makedirs(_include_path)
    shutil.copytree(_src_path, _include_path, dirs_exist_ok=True, ignore=_copytree_h_file)

    _build_output = []
    if _platform in ("linux64",):
        _d1 = f"build-{_root_dir_name}-Desktop_Qt_5_15_2_GCC_64bit-Release"
        _build_output.append(os.path.join(_outside_path, _d1, f"lib{_name}.so"))
    elif _platform in ("win32msvc", "win32mingw",):
        if _platform in ("win32msvc",):
            _d1 = f"build-{_root_dir_name}-Desktop_Qt_5_15_2_MSVC2019_32bit-Release"
            _build_output.append(os.path.join(_outside_path, _d1, f"{_name}.dll"))
            _build_output.append(os.path.join(_outside_path, _d1, f"{_name}.lib"))
        elif _platform in ("win32mingw",):
            _d1 = f"build-{_root_dir_name}-Desktop_Qt_5_15_2_MinGW_32_bit-Release"
            _build_output.append(os.path.join(_outside_path, _d1, f"lib{_name}.dll"))
            # _build_output.append(os.path.join(_outside_path, _d1, f"lib{_name}.dll.a"))
    elif _platform in ("android",):
        _d1 = f"build-{_root_dir_name}-Qt_5_15_2_Clang_Multi_Abi-Release"
        _build_output.append(
            os.path.join(_outside_path, _d1, "android-build", "libs", "armeabi-v7a", f"lib{_name}_armeabi-v7a.so"))
    else:
        raise ValueError(str(_platform))
    for i in _build_output:
        shutil.copy(i, _lib_path)
