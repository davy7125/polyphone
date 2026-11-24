#!/usr/bin/env python3
"""Libraries imported by Windows executables and DLLs (import table and delayed imports), without any tool of Windows.

Usage: pe_imports.py [--forbid <regex>] <file>...
Prints the libraries of each file. With --forbid, fails (exit code 1) when a library matches the regular expression,
case ignored: the plugin is checked this way for the runtime of Visual C++ (.github/workflows/build-plugin.yml).
"""

import argparse
import re
import struct
import sys

IMPORT_DIRECTORY = 1
DELAY_IMPORT_DIRECTORY = 13


def read_imports(path):
    with open(path, "rb") as file:
        data = file.read()
    if data[:2] != b"MZ":
        raise ValueError("not an executable")
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    if data[pe:pe + 4] != b"PE\0\0":
        raise ValueError("no PE header")
    section_count, optional_size = struct.unpack_from("<H12xH", data, pe + 6)
    optional = pe + 24
    magic = struct.unpack_from("<H", data, optional)[0]
    directories = optional + (112 if magic == 0x20B else 96)  # PE32+ or PE32

    sections = []
    for index in range(section_count):
        offset = optional + optional_size + 40 * index
        virtual_size, virtual_address, raw_size, raw_offset = struct.unpack_from("<4I", data, offset + 8)
        sections.append((virtual_address, max(virtual_size, raw_size), raw_offset))

    def file_offset(rva):
        for virtual_address, size, raw_offset in sections:
            if virtual_address <= rva < virtual_address + size:
                return rva - virtual_address + raw_offset
        raise ValueError(f"address {rva:#x} outside the sections")

    def name_at(rva):
        start = file_offset(rva)
        return data[start:data.index(b"\0", start)].decode("ascii")

    def directory(index):
        return struct.unpack_from("<II", data, directories + 8 * index)

    names = []
    rva, size = directory(IMPORT_DIRECTORY)
    if rva:
        offset = file_offset(rva)
        while True:
            name_rva = struct.unpack_from("<I", data, offset + 12)[0]
            if not name_rva:
                break
            names.append(name_at(name_rva))
            offset += 20
    rva, size = directory(DELAY_IMPORT_DIRECTORY)
    if rva:
        offset = file_offset(rva)
        while True:
            name_rva = struct.unpack_from("<I", data, offset + 4)[0]
            if not name_rva:
                break
            names.append(name_at(name_rva))
            offset += 32
    return names


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--forbid", help="regular expression of the libraries that must not be imported")
    parser.add_argument("files", nargs="+")
    arguments = parser.parse_args()
    forbidden = re.compile(arguments.forbid, re.IGNORECASE) if arguments.forbid else None

    failed = False
    for path in arguments.files:
        names = read_imports(path)
        print(f"{path}: {' '.join(names)}")
        matches = [name for name in names if forbidden and forbidden.search(name)]
        if matches:
            print(f"::error::{path} imports {', '.join(matches)}")
            failed = True
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
