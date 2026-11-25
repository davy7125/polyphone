#!/bin/bash
# Checks of a package of the editor (.deb or .rpm) without installing it, run at the end of build_debian_package.sh
# and build_fedora_package.sh, or by hand:
#   packaging/linux/check_package.sh <package>
# The package is extracted in a temporary directory, then:
#   * files: executable, desktop file, metainfo, MIME types, icon of the application, icons of the file types and
#     manual pages as in packaging/, documentation and license;
#   * the executable starts with the libraries of this system and gives the version of the package;
#   * desktop file, metainfo (without network) and manual pages valid;
#   * the types of the desktop file and of the metainfo are the ones of polyphone.xml, each one with an icon;
#   * files detected by their extension and by their content, Polyphone registered for each type (gio, with the MIME
#     database of the package only, whatever is installed);
#   * lintian (.deb) or rpmlint (.rpm).
# A missing tool skips its checks, which are reported. Exit code 1 if a check failed.

set -uo pipefail # Not -e: all the checks run
shopt -s nullglob

if [ $# -ne 1 ] || [ ! -f "$1" ]; then
    echo "Usage: $0 <package .deb or .rpm>"
    exit 1
fi
package=$(realpath "$1")
root=$(cd "$(dirname "$0")/../.." && pwd)
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT
tree=$tmp/tree
mkdir -p "$tree"

failures=0
skipped=0
pass() { echo "  ok    $*"; }
fail() { echo "  FAIL  $*"; failures=$((failures + 1)); }
skip() { echo "  skip  $*"; skipped=$((skipped + 1)); }
has() { command -v "$1" > /dev/null; }

case $package in
    *.deb)
        dpkg-deb -x "$package" "$tree"
        version=$(dpkg-deb -f "$package" Version)
        version=${version%-*} # Without the Debian revision
        license=usr/share/doc/polyphone/copyright
        lint=(lintian --pedantic -I)
        ;;
    *.rpm)
        if ! has rpm2cpio || ! has cpio; then
            echo "Error: rpm2cpio and cpio are needed"
            exit 1
        fi
        (cd "$tree" && rpm2cpio "$package" | cpio -idm --quiet)
        version=$(rpm -qp --qf '%{VERSION}' "$package")
        license="usr/share/licenses/polyphone*/LICENSE.txt" # "polyphone-<version>" if %_docdir_fmt is not Fedora's
        lint=(rpmlint)
        ;;
    *)
        echo "Error: .deb or .rpm package expected"
        exit 1
        ;;
esac
numeric=${version%%~*} # "2.8.0~beta": the executable writes "2.8.0 beta"
echo "Checking $(basename "$package") (version $version)"

# Files, the expected ones being given by packaging/
echo "Files"
check_files() { # <description> <pattern relative to the root of the package>
    # Each match tested: a pattern without wildcard is kept by the shell even if the file does not exist
    local file
    for file in "$tree"/$2; do
        if [ -e "$file" ]; then
            pass "$1"
            return
        fi
    done
    fail "$1: no /$2"
}
if [ -x "$tree/usr/bin/polyphone" ]; then pass "executable"; else fail "executable: no /usr/bin/polyphone"; fi
check_files "desktop file" usr/share/applications/io.polyphone.polyphone.desktop
check_files "metainfo" usr/share/metainfo/io.polyphone.polyphone.metainfo.xml
check_files "MIME types" usr/share/mime/packages/polyphone.xml
check_files "icon of the application" usr/share/icons/hicolor/512x512/apps/polyphone.png
# Icons of the file types: the ones extracted from packaging/windows by the script of the build
if python3 -I "$root/packaging/linux/mime_icons.py" "$root/packaging/polyphone.xml" "$root/packaging/windows" \
        "$tmp/icons" > /dev/null; then
    missing=()
    count=0
    while IFS= read -r icon; do
        count=$((count + 1))
        [ -f "$tree/usr/share/icons/$icon" ] || missing+=("$icon")
    done < <(cd "$tmp/icons" && find . -type f -name '*.png' | sed 's|^\./||' | sort)
    if [ ${#missing[@]} -eq 0 ]; then pass "icons of the file types ($count)"; else fail "icons of the file types, missing: ${missing[*]}"; fi
else
    fail "icons of the file types: packaging/linux/mime_icons.py failed"
fi
missing=()
count=0
for page in "$root"/packaging/man/man1/polyphone.1 "$root"/packaging/man/*/man1/polyphone.1; do
    count=$((count + 1))
    relative=${page#"$root/packaging/man/"}
    found=("$tree/usr/share/man/$relative"*)
    [ ${#found[@]} -gt 0 ] || missing+=("$relative")
done
if [ ${#missing[@]} -eq 0 ]; then pass "manual pages ($count)"; else fail "manual pages, missing: ${missing[*]}"; fi
check_files "README" "usr/share/doc/polyphone/README.md*"
check_files "changelog" "usr/share/doc/polyphone/changelog*"
check_files "license" "$license"

# Executable
echo "Executable"
if [ -x "$tree/usr/bin/polyphone" ]; then
    mkdir -p "$tmp/home"
    output=$(HOME=$tmp/home QT_QPA_PLATFORM=offscreen timeout 30 "$tree/usr/bin/polyphone" --version 2>&1)
    if [[ $output == *"Polyphone $numeric"* ]]; then
        pass "--version: $output"
    else
        fail "--version: '$output', expected 'Polyphone $numeric'"
    fi
else
    skip "no executable"
fi

# Validity of the files
echo "Validity"
desktop=$tree/usr/share/applications/io.polyphone.polyphone.desktop
metainfo=$tree/usr/share/metainfo/io.polyphone.polyphone.metainfo.xml
mime=$tree/usr/share/mime/packages/polyphone.xml
if [ ! -f "$desktop" ]; then
    skip "desktop file: absent"
elif has desktop-file-validate; then
    if output=$(desktop-file-validate "$desktop" 2>&1) && [ -z "$output" ]; then pass "desktop file"; else fail "desktop file: $output"; fi
else
    skip "desktop file: desktop-file-validate not found (desktop-file-utils)"
fi
if [ ! -f "$metainfo" ]; then
    skip "metainfo: absent"
elif has appstreamcli; then
    if output=$(appstreamcli validate --no-net --pedantic "$metainfo" 2>&1); then pass "metainfo"; else fail "metainfo: $output"; fi
elif has appstream-util; then
    if output=$(appstream-util validate-relax --nonet "$metainfo" 2>&1); then pass "metainfo"; else fail "metainfo: $output"; fi
else
    skip "metainfo: appstreamcli (appstream) and appstream-util (libappstream-glib) not found"
fi
if has man; then
    pages=("$tree"/usr/share/man/man1/polyphone.1* "$tree"/usr/share/man/*/man1/polyphone.1*)
    invalid=()
    for page in "${pages[@]}"; do
        # Formatted, with a NAME section readable by whatis / apropos (output captured: with pipefail, "| grep -q" would
        # fail when the writer is stopped by SIGPIPE)
        if [ -z "$(LC_ALL=C.UTF-8 MANWIDTH=80 man -l "$page" 2>/dev/null)" ]; then
            invalid+=("${page#"$tree/usr/share/man/"}")
        elif has lexgrog && ! lexgrog "$page" > /dev/null 2>&1; then
            invalid+=("${page#"$tree/usr/share/man/"} (NAME)")
        fi
    done
    if [ ${#invalid[@]} -eq 0 ]; then pass "manual pages (${#pages[@]})"; else fail "manual pages: ${invalid[*]}"; fi
else
    skip "manual pages: man not found"
fi

# Types of files: the same in the desktop file, the metainfo and polyphone.xml, each one with an icon
echo "Types of files"
desktop_types=""
if [ -f "$desktop" ]; then
    desktop_types=$(sed -n 's/^MimeType=//p' "$desktop" | tr ';' '\n' | sed '/^$/d' | sort)
fi
if [ -f "$desktop" ] && [ -f "$metainfo" ] && [ -f "$mime" ]; then
    metainfo_types=$(grep -o '<mediatype>[^<]*' "$metainfo" | sed 's/<mediatype>//' | sort)
    # Types defined, and the ones with a glob (files) with their icon name
    defined=$(python3 -I - "$mime" <<'EOF'
import sys
import xml.etree.ElementTree as ET
ns = '{http://www.freedesktop.org/standards/shared-mime-info}'
for mime_type in ET.parse(sys.argv[1]).getroot().findall(ns + 'mime-type'):
    name = mime_type.get('type')
    icon = mime_type.find(ns + 'icon')
    icon = icon.get('name') if icon is not None else name.replace('/', '-')
    print(name, icon if mime_type.find(ns + 'glob') is not None else '')
EOF
)
    defined_types=$(cut -d' ' -f1 <<< "$defined" | sort)
    if [ "$desktop_types" = "$defined_types" ]; then
        pass "desktop file: $(echo $desktop_types)"
    else
        fail "types of the desktop file ($(echo $desktop_types)) and of polyphone.xml ($(echo $defined_types)) differ"
    fi
    if [ "$metainfo_types" = "$defined_types" ]; then
        pass "metainfo: same types"
    else
        fail "types of the metainfo ($(echo $metainfo_types)) and of polyphone.xml ($(echo $defined_types)) differ"
    fi
    sizes=("$tree"/usr/share/icons/hicolor/*/mimetypes)
    missing=()
    while read -r name icon; do
        [ -n "$icon" ] || continue
        [ ${#sizes[@]} -gt 0 ] || missing+=("$icon")
        for size in "${sizes[@]}"; do
            [ -f "$size/$icon.png" ] || [ -f "$size/$icon.svg" ] || missing+=("${size#"$tree/usr/share/icons/"}/$icon")
        done
    done <<< "$defined"
    if [ ${#missing[@]} -eq 0 ]; then
        pass "icons of the types with files, in ${#sizes[@]} sizes"
    else
        fail "icons missing: ${missing[*]}"
    fi
else
    skip "desktop file, metainfo or polyphone.xml absent"
fi

# Detection of files and association with Polyphone, with the MIME database of the package only
echo "Detection of the files"
if [ -f "$mime" ] && [ -f "$desktop" ] && has gio && has update-mime-database; then
    xdg=$tmp/xdg
    mkdir -p "$xdg/share/mime/packages" "$xdg/share/applications" "$xdg/home" "$xdg/config" "$tmp/files"
    cp "$mime" "$xdg/share/mime/packages/"
    cp "$desktop" "$xdg/share/applications/"
    update-mime-database "$xdg/share/mime" > /dev/null 2>&1
    has update-desktop-database && update-desktop-database "$xdg/share/applications" > /dev/null 2>&1
    export XDG_DATA_DIRS=$xdg/share XDG_DATA_HOME=$xdg/home XDG_CONFIG_HOME=$xdg/config
    cd "$tmp/files"
    printf 'RIFF\x1c\x00\x00\x00sfbkLIST\x10\x00\x00\x00INFOifil\x04\x00\x00\x00\x02\x00\x01\x00' > test.sf2
    cp test.sf2 test.sf3
    cp test.sf2 sf2_without_extension
    printf 'RIFF\x14\x00\x00\x00DLS LIST\x04\x00\x00\x00lins' > test.dls
    cp test.dls dls_without_extension
    printf '<region> sample=sample.wav\n' > test.sfz
    printf 'sfArk test\n' > test.sfArk
    printf '[Organ]\nChurchName=Test\n' > test.organ
    for expected in test.sf2:audio/x-sf2 test.sf3:audio/x-sf3 test.sfz:audio/x-sfz test.dls:audio/dls \
                    test.sfArk:audio/x-sfark test.organ:audio/x-organ sf2_without_extension:audio/x-sf2 \
                    dls_without_extension:audio/dls; do
        file=${expected%%:*}
        type=${expected#*:}
        detected=$(gio info -a standard::content-type "$file" 2>/dev/null | sed -n 's/^ *standard::content-type: //p')
        if [ "$detected" = "$type" ]; then pass "$file: $type"; else fail "$file: '$detected' instead of $type"; fi
    done
    unregistered=()
    for type in $desktop_types; do
        output=$(gio mime "$type" 2>/dev/null)
        [[ $output == *io.polyphone.polyphone.desktop* ]] || unregistered+=("$type")
    done
    if [ ${#unregistered[@]} -eq 0 ]; then pass "Polyphone registered for each type"; else fail "Polyphone not registered for: ${unregistered[*]}"; fi
    cd "$root"
    unset XDG_DATA_DIRS XDG_DATA_HOME XDG_CONFIG_HOME
else
    skip "gio (glib) or update-mime-database (shared-mime-info) not found"
fi

# Lint of the package
echo "${lint[0]}"
if has "${lint[0]}"; then
    if output=$("${lint[@]}" "$package" 2>&1); then
        pass "no error"
    else
        fail "errors found"
    fi
    [ -z "$output" ] || sed 's/^/        /' <<< "$output"
else
    skip "${lint[0]} not found"
fi

echo
if [ $failures -gt 0 ]; then
    echo "$failures check(s) failed, $skipped skipped"
    exit 1
fi
echo "All the checks passed ($skipped skipped)"
