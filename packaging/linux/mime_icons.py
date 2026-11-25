#!/usr/bin/env python3
"""Icons of the file types for Linux, extracted from the icons of Windows (run by sources/CMakeLists.txt).

Usage: mime_icons.py <polyphone.xml> <directory of the .ico files> <output directory>

For each MIME type of polyphone.xml having a glob "*.<extension>", the images of "file_<extension>.ico" (extension in
lower case) are written in "<output directory>/hicolor/<size>x<size>/mimetypes/<icon>.png", <icon> being the name
given by <icon name="..."/> or the type with "/" replaced by "-" (audio/x-sf2 -> audio-x-sf2), as the desktops look for
it. The images of the .ico files are PNG: they are copied without decoding, nothing but the standard library is needed.
The output directory is emptied first.
"""

import shutil
import struct
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

NAMESPACE = "{http://www.freedesktop.org/standards/shared-mime-info}"
PNG_SIGNATURE = b"\x89PNG\r\n\x1a\n"


def file_types(mime_file):
    """(icon name, extension) of the types having a glob."""
    result = []
    for mime_type in ET.parse(mime_file).getroot().findall(NAMESPACE + "mime-type"):
        globs = [glob.get("pattern") for glob in mime_type.findall(NAMESPACE + "glob")]
        if not globs:
            continue  # Parent type without files (audio/x-soundfont)
        icon = mime_type.find(NAMESPACE + "icon")
        name = icon.get("name") if icon is not None else mime_type.get("type").replace("/", "-")
        if len(globs) != 1 or not globs[0].startswith("*."):
            sys.exit(f"{mime_file}: one glob '*.<extension>' expected for {mime_type.get('type')}")
        result.append((name, globs[0][2:].lower()))
    return result


def png_images(ico_file):
    """{size: PNG data} of an .ico file."""
    data = ico_file.read_bytes()
    reserved, kind, count = struct.unpack_from("<HHH", data, 0)
    if reserved != 0 or kind != 1:
        sys.exit(f"{ico_file}: not an icon file")
    images = {}
    for index in range(count):
        size, offset = struct.unpack_from("<II", data, 6 + 16 * index + 8)
        image = data[offset:offset + size]
        if not image.startswith(PNG_SIGNATURE):
            sys.exit(f"{ico_file}: image {index + 1} is not a PNG image (save the icon with PNG images)")
        width, height = struct.unpack_from(">II", image, 16)  # Header IHDR, more reliable than the directory
        if width != height:
            sys.exit(f"{ico_file}: image {index + 1} is not square ({width}x{height})")
        images[width] = image
    return images


def main():
    if len(sys.argv) != 4:
        sys.exit(__doc__)
    mime_file, ico_directory, output = Path(sys.argv[1]), Path(sys.argv[2]), Path(sys.argv[3])
    shutil.rmtree(output / "hicolor", ignore_errors=True)
    for name, extension in file_types(mime_file):
        ico_file = ico_directory / f"file_{extension}.ico"
        if not ico_file.is_file():
            sys.exit(f"{ico_file} not found, needed for the type of the files *.{extension} ({mime_file})")
        for size, image in sorted(png_images(ico_file).items()):
            path = output / "hicolor" / f"{size}x{size}" / "mimetypes" / f"{name}.png"
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(image)


if __name__ == "__main__":
    main()
