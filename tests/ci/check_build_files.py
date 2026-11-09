#!/usr/bin/env python3
"""Check that polyphone.pro (qmake) and the CMake files build the same sources, while both builds exist.

Usage: check_build_files.py [<repository root>]

The files named in sources/polyphone.pro must be the ones named in sources/CMakeLists.txt, sources/cmake/files.cmake
and sources/synth_core/CMakeLists.txt (the headers of the synth core are only listed by polyphone.pro). The sources of
the repository that are not built are listed too.
"""

import re
import subprocess
import sys
from pathlib import Path

FILE = re.compile(r"(?<![\w./-])((?:\.\./)?[\w./-]+\.(?:cpp|c|h|ui))(?![\w.])")


def files_in(path, prefix=""):
    text = "\n".join(line.split("#", 1)[0] for line in path.read_text(encoding="utf-8").splitlines())
    return {prefix + name for name in FILE.findall(text)}


def main():
    root = Path(sys.argv[1] if len(sys.argv) > 1 else Path(__file__).resolve().parents[2])
    sources = root / "sources"
    qmake = files_in(sources / "polyphone.pro")
    cmake = (files_in(sources / "CMakeLists.txt") | files_in(sources / "cmake" / "files.cmake") |
             files_in(sources / "synth_core" / "CMakeLists.txt", "synth_core/"))
    qmake = {f for f in qmake if not (f.startswith("synth_core/") and f.endswith(".h"))}

    status = 0
    for name, missing in (("CMake", sorted(qmake - cmake)), ("polyphone.pro", sorted(cmake - qmake))):
        for file in missing:
            print(f"{file}: missing in {name}")
            status = 1
    for file in sorted(qmake | cmake):
        if not (sources / file).exists():
            print(f"{file}: listed but not found")
            status = 1

    # Sources of the repository built by neither (the plugin has its own CMake project)
    tracked = subprocess.run(["git", "-C", str(root), "ls-files", "--cached", "--others", "--exclude-standard",
                              "sources"], capture_output=True, text=True, check=True).stdout.split()
    built = qmake | cmake | {"precompiled_header.h"}
    for file in sorted(tracked):
        relative = file[len("sources/"):]
        if (relative.endswith((".cpp", ".c", ".h", ".ui")) and relative not in built and
                not relative.startswith(("plugin/", "synth_core/", "lib/")) and (root / file).exists()):
            print(f"{relative}: in the repository but built by neither polyphone.pro nor CMake")
            status = 1

    if status == 0:
        print(f"Same files built by polyphone.pro and CMake ({len(qmake)} files)")
    return status


if __name__ == "__main__":
    sys.exit(main())
