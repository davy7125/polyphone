#!/usr/bin/env python3
"""Check the dependencies between the parts of the sources (GitHub workflows, or by hand before a commit).

Usage: check_dependencies.py [<repository root>]

The headers are found by their name (the build gives all source directories to the compiler) or by the end of
their path. A part may only include the headers of the parts it is allowed to use:
  * synth_core: itself, without Qt (the plugin and a web player use it);
  * core (soundfonts, samples, import and export): itself, synth_core and lib, without the widgets of Qt (QtCore
    and QtGui only): the user interface derives its classes to ask questions (InteractiveSampleLoader,
    InteractiveDuplicator) and gives it the preferences it reads (CoreSettings);
  * plugin: itself and synth_core;
  * mcp (server for the AI assistants): no interface except the tools (the main window gives it the editor through
    McpEditor).
Allowed parts are directories of sources/ ("editor/tools" allows this directory of editor only).
"""

import re
import subprocess
import sys
from pathlib import Path

ALLOWED = {
    "synth_core": {"synth_core"},
    "core": {"core", "synth_core", "lib"},
    "plugin": {"plugin", "synth_core"},
    "mcp": {"mcp", "core", "synth_core", "lib", "context", "sound_engine", "editor/tools"},
}

# Headers of the modules QtWidgets and QtPrintSupport, forbidden in core
QT_WIDGETS = {
    "QtWidgets", "QApplication", "QWidget", "QDialog", "QMessageBox", "QFileDialog", "QInputDialog", "QProgressDialog",
    "QAbstractButton", "QPushButton", "QMenu", "QLabel", "QLayout", "QStyle", "QToolTip", "QPrinter", "QPrintDialog",
}

INCLUDE = re.compile(r'^\s*#\s*include\s*([<"])([^>"]+)[>"]')


def part_of(path):
    """Part of a file: first directory under sources/ ("." for the files at the root of sources/)"""
    parts = path.parts[1:]
    return parts[0] if len(parts) > 1 else "."


def is_allowed(header, allowed):
    """True if a header is in one of the allowed directories of sources/"""
    relative = Path(*header.parts[1:]).as_posix()
    return part_of(header) in allowed or any(relative.startswith(prefix + "/") for prefix in allowed)


def main():
    root = Path(sys.argv[1] if len(sys.argv) > 1 else Path(__file__).resolve().parents[2])
    # Files of the repository, and the new ones not ignored
    files = subprocess.run(["git", "-C", str(root), "ls-files", "--cached", "--others", "--exclude-standard", "sources"],
                           capture_output=True, text=True, check=True).stdout.split()
    files = [f for f in files if (root / f).exists()]
    files = [Path(f) for f in files if f.endswith((".h", ".hpp", ".cpp", ".c"))]
    headers = [f for f in files if f.suffix in (".h", ".hpp")]
    by_name = {}
    for header in headers:
        by_name.setdefault(header.name, []).append(header)

    def resolve(name):
        """Header of the sources included with this name, None if it is not one of them"""
        if "/" not in name:
            found = by_name.get(name, [])
        else:
            found = [h for h in headers if h.as_posix().endswith("/" + name)]
        return found[0] if len(found) == 1 else None

    violations = []
    for file in files:
        part = part_of(file)
        if part not in ALLOWED:
            continue
        for number, line in enumerate((root / file).read_text(encoding="utf-8", errors="replace").splitlines(), 1):
            match = INCLUDE.match(line)
            if not match:
                continue
            name = match.group(2)
            base = name.split("/")[-1]
            if part == "synth_core" and (base.startswith("Q") or base.startswith("q")) and resolve(name) is None:
                violations.append(f"{file}:{number}: {name} (Qt in synth_core)")
                continue
            if part == "core" and base.split(".")[0] in QT_WIDGETS | {w.lower() for w in QT_WIDGETS}:
                violations.append(f"{file}:{number}: {name} (widgets of Qt in core)")
                continue
            header = resolve(name)
            if header is not None and not is_allowed(header, ALLOWED[part]):
                violations.append(f"{file}:{number}: {name} ({part} cannot use {part_of(header)})")

    for violation in violations:
        print(violation)
    if violations:
        print(f"{len(violations)} forbidden dependencies (see tests/ci/check_dependencies.py)")
        return 1
    print("Dependencies OK: " + ", ".join(f"{part} -> {', '.join(sorted(allowed))}" for part, allowed in ALLOWED.items()))
    return 0


if __name__ == "__main__":
    sys.exit(main())
