#!/usr/bin/env python3
"""Set the version of Polyphone from a tag, before an official build (GitHub workflows).

Usage: set_version.py <tag>
The tag is "<version>[-<identifier>]", for instance "2.7.0", "2.7.0-beta" or "2.1-beta". It is written:
  * in sources/version.txt, read by the CMake projects (editor and plugin);
  * in packaging/mac/polyphone.plist (CFBundleShortVersionString, 3 numbers);
  * in packaging/io.polyphone.polyphone.metainfo.xml (first release of the list, added if it is another version).
"""

import datetime
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent


def parse_tag(tag):
    match = re.fullmatch(r"(\d+)\.(\d+)(?:\.(\d+))?(?:-([A-Za-z0-9._]+))?", tag)
    if not match:
        sys.exit(f"Invalid tag '{tag}': expected <major>.<minor>[.<patch>][-<identifier>]")
    major, minor, patch, identifier = match.groups()
    soft_version = f"{major}.{minor}" + (f".{patch}" if patch is not None else "")
    # Version of the metainfo file, as written for the previous releases: "2.6" for 2.6.0, "2.5.1"
    short_version = f"{major}.{minor}" + (f".{patch}" if patch not in (None, "0") else "")
    return soft_version, f"{major}.{minor}.{patch or 0}", short_version, identifier or ""


def write_version_file(tag):
    path = ROOT / "sources" / "version.txt"
    path.write_text(tag + "\n", encoding="utf-8", newline="\n")
    print(f"{path.relative_to(ROOT)}: {tag}")


def update_plist(numeric_version):
    path = ROOT / "packaging" / "mac" / "polyphone.plist"
    text = path.read_text(encoding="utf-8")
    text, count = re.subn(r"(<key>CFBundleShortVersionString</key>\s*<string>)[^<]*(</string>)",
                          rf"\g<1>{numeric_version}\g<2>", text)
    if count != 1:
        sys.exit(f"{path}: CFBundleShortVersionString not found")
    path.write_text(text, encoding="utf-8", newline="\n")
    print(f"{path.relative_to(ROOT)}: CFBundleShortVersionString {numeric_version}")


def update_metainfo(short_version, is_stable):
    # The first release of the list is the version built: its date is updated, or it is added before the others
    path = ROOT / "packaging" / "io.polyphone.polyphone.metainfo.xml"
    text = path.read_text(encoding="utf-8")
    today = datetime.date.today().isoformat()
    release_type = "stable" if is_stable else "development"
    first = re.search(r'^([ \t]*)<release\b[^>]*\bversion="([^"]*)"[^>]*?(/?)>', text, re.MULTILINE)
    if first is None:
        sys.exit(f"{path}: no release found")
    indent, version, _ = first.groups()
    if version == short_version:
        tag = first.group(0)
        tag = re.sub(r'\bdate="[^"]*"', f'date="{today}"', tag)
        tag = re.sub(r'\btype="[^"]*"', f'type="{release_type}"', tag)
        text = text[:first.start()] + tag + text[first.end():]
    else:
        line = f'{indent}<release type="{release_type}" version="{short_version}" date="{today}" />\n'
        text = text[:first.start()] + line + text[first.start():]
    path.write_text(text, encoding="utf-8", newline="\n")
    print(f"{path.relative_to(ROOT)}: release {short_version} ({release_type}) on {today}")


def main():
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    tag = sys.argv[1].strip()
    soft_version, numeric_version, short_version, identifier = parse_tag(tag)
    print(f"Tag {tag}: version {soft_version}, identifier '{identifier}'")
    write_version_file(tag)
    update_plist(numeric_version)
    update_metainfo(short_version, identifier == "")


if __name__ == "__main__":
    main()
