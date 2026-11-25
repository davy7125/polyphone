#!/bin/bash
# Debian package of the editor, built with the compiler and the libraries of this system (Debian, Ubuntu...).
# Usage, from any directory:
#   packaging/build_debian_package.sh [<tag>]
# <tag> is the version as for the releases ("2.7.3", "2.8.0-beta"), by default the last tag reachable in git.
# Result: build/polyphone_<version>-1_<architecture>.deb, then checked by packaging/linux/check_package.sh
#
# The files of the working tree (uncommitted changes included, files ignored by git excluded) are copied in
# build/debian/polyphone-<tag>, with packaging/debian as "debian" and the version written by set_version.py: the
# repository is not modified. This copy is removed once the package is built, kept if the build fails.
# The changelog of the package is sources/changelog, its first entry taking the version of the package ("-beta" of a
# tag becomes "~beta", so that 2.8.0~beta-1 comes before 2.8.0-1).
#
# Needed: dpkg-dev, debhelper and the build dependencies of packaging/debian/control (the script lists the missing
# ones). Also used by the checks if installed: lintian, appstream, desktop-file-utils. JOBS gives the number of
# parallel jobs (all processors by default).

set -euo pipefail

root=$(cd "$(dirname "$0")/.." && pwd)
cd "$root"

for command in dpkg-buildpackage dh python3; do
    if ! command -v $command > /dev/null; then
        echo "Error: $command not found (packages dpkg-dev, debhelper and python3)"
        exit 1
    fi
done

if [ $# -gt 1 ]; then
    echo "Usage: $0 [<tag>]"
    exit 1
fi
tag=${1:-$(git describe --tags --abbrev=0)}
if ! [[ $tag =~ ^[0-9]+\.[0-9]+(\.[0-9]+)?(-[A-Za-z0-9.]+)?$ ]]; then
    echo "Error: invalid tag '$tag', expected <major>.<minor>[.<patch>][-<identifier>]"
    exit 1
fi
deb_version=$(sed 's/-/~/' <<< "$tag")-1
echo "Version $tag, package $deb_version"

# What is packaged: a working tree that is not exactly the tag is reported
described=$(git describe --tags --always)
if [ "$described" != "$tag" ]; then
    echo "Note: the working tree is at $described"
fi
if [ -n "$(git status --porcelain)" ]; then
    echo "Note: uncommitted changes are included"
fi

# Copy of the working tree: the CMake project in sources/ installs files of packaging/ and the README of the root
work=$root/build/debian
src=$work/polyphone-$tag
rm -rf "$work"
mkdir -p "$src"
git ls-files -z --cached --others --exclude-standard |
    while IFS= read -r -d '' file; do
        # Files deleted but not committed yet are skipped
        if [ -e "$file" ] || [ -L "$file" ]; then
            printf '%s\0' "$file"
        fi
    done |
    tar --null --files-from=- -cf - | tar -C "$src" -xf -
cp -r packaging/debian "$src/debian"
chmod +x "$src/debian/rules" # Executable whatever the checkout
python3 "$src/packaging/set_version.py" "$tag" > /dev/null

# Changelog: first entry of sources/changelog with the version of the package and the date of the build, lines
# wrapped at 80 columns (one line per change in sources/changelog, read by the editor)
changelog=sources/changelog
first=$(sed -n '1s/^polyphone (\([^)]*\)).*/\1/p' "$changelog")
if [[ $tag != "$first" && $tag != "$first".* && $tag != "$first"-* ]]; then
    echo "Warning: the first entry of $changelog is for the version '$first', not $tag"
fi
signature=$(grep -m1 '^ -- ' "$changelog" | sed 's/>  .*/>/')
{
    echo "polyphone ($deb_version) unstable; urgency=low"
    sed -n '2,/^ -- /{/^ -- /!p}' "$changelog"
    echo "$signature  $(LC_ALL=C date -R)"
    sed '1,/^ -- /d' "$changelog"
} | python3 -c '
import sys, textwrap
for line in sys.stdin:
    line = line.rstrip("\n")
    if line.startswith("  * ") and len(line) > 80:
        line = textwrap.fill(line[4:], width=80, initial_indent="  * ", subsequent_indent="    ",
                             break_long_words=False, break_on_hyphens=False)
    print(line)
' > "$src/debian/changelog"

if ! dpkg-checkbuilddeps "$src/debian/control"; then
    echo "Install the missing packages, for instance with: sudo apt build-dep \"$src\""
    exit 1
fi

# Binary package only (no source package, unsigned), without the package of debug symbols
cd "$src"
DEB_BUILD_OPTIONS="noautodbgsym ${DEB_BUILD_OPTIONS:-}" dpkg-buildpackage -b -us -uc --jobs-try="${JOBS:-auto}"
cd "$root"

deb=$(find "$work" -maxdepth 1 -name "polyphone_${deb_version}_*.deb")
if [ -z "$deb" ]; then
    echo "Error: no package found in $work"
    exit 1
fi
mv "$deb" build/
rm -rf "$work"
deb=build/$(basename "$deb")
echo
dpkg-deb --show --showformat='${Package} ${Version} (${Architecture}), ${Installed-Size} kB installed\nDepends: ${Depends}\n' "$deb"
echo "Package: $root/$deb"
echo
packaging/linux/check_package.sh "$deb"
