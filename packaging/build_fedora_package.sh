#!/bin/bash
# RPM package of the editor, built with the compiler and the libraries of this system (Fedora).
# Usage, from any directory:
#   packaging/build_fedora_package.sh [<tag>]
# <tag> is the version as for the releases ("2.7.3", "2.8.0-beta"), by default the last tag reachable in git.
# Result: build/polyphone-<version>-1.<distribution>.<architecture>.rpm, then checked by packaging/linux/check_package.sh
#
# The files of the working tree (uncommitted changes included, files ignored by git excluded) are archived in
# build/fedora/rpmbuild/SOURCES/polyphone-<tag>.tar.gz and built with packaging/fedora/polyphone.spec, whose copy
# receives the changelog: the repository is not modified. build/fedora is removed once the package is built, kept if
# the build fails. The changelog of the package is the first entry of sources/changelog.
#
# Needed: rpm-build and the build dependencies of the spec, for instance:
#   sudo dnf install rpm-build && sudo dnf builddep packaging/fedora/polyphone.spec
# Also used by the checks if installed: rpmlint, appstream. JOBS gives the number of parallel jobs (all processors by
# default).

set -euo pipefail

root=$(cd "$(dirname "$0")/.." && pwd)
cd "$root"

for command in rpmbuild python3; do
    if ! command -v $command > /dev/null; then
        echo "Error: $command not found (packages rpm-build and python3)"
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
rpm_version=$(sed 's/-/~/' <<< "$tag")
echo "Version $tag, package $rpm_version-1"

# What is packaged: a working tree that is not exactly the tag is reported
described=$(git describe --tags --always)
if [ "$described" != "$tag" ]; then
    echo "Note: the working tree is at $described"
fi
if [ -n "$(git status --porcelain)" ]; then
    echo "Note: uncommitted changes are included"
fi

# Archive of the working tree, as the archive of a tag on GitHub (the spec writes the version with set_version.py)
work=$root/build/fedora
src=$work/polyphone-$tag
rm -rf "$work"
mkdir -p "$src" "$work/rpmbuild/SOURCES" "$work/rpmbuild/SPECS"
git ls-files -z --cached --others --exclude-standard |
    while IFS= read -r -d '' file; do
        # Files deleted but not committed yet are skipped
        if [ -e "$file" ] || [ -L "$file" ]; then
            printf '%s\0' "$file"
        fi
    done |
    tar --null --files-from=- -cf - | tar -C "$src" -xf -
tar -C "$work" -czf "$work/rpmbuild/SOURCES/polyphone-$tag.tar.gz" "polyphone-$tag"
rm -rf "$src"

# Spec with the changelog: first entry of sources/changelog, dated of the build, lines wrapped at 80 columns, "%"
# escaped (macros are expanded in the changelog)
changelog=sources/changelog
first=$(sed -n '1s/^polyphone (\([^)]*\)).*/\1/p' "$changelog")
if [[ $tag != "$first" && $tag != "$first".* && $tag != "$first"-* ]]; then
    echo "Warning: the first entry of $changelog is for the version '$first', not $tag"
fi
maintainer=$(grep -m1 '^ -- ' "$changelog" | sed 's/^ -- //; s/>  .*/>/')
spec=$work/rpmbuild/SPECS/polyphone.spec
cp packaging/fedora/polyphone.spec "$spec"
{
    echo "* $(LC_ALL=C date '+%a %b %d %Y') $maintainer - $rpm_version-1"
    sed -n '2,/^ -- /{/^ -- /!p}' "$changelog" | sed -n 's/^  \* //p'
} | python3 -c '
import sys, textwrap
print(sys.stdin.readline().rstrip("\n"))
for line in sys.stdin:
    print(textwrap.fill(line.rstrip("\n"), width=80, initial_indent="- ", subsequent_indent="  ",
                        break_long_words=False, break_on_hyphens=False))
' | sed 's/%/%%/g' >> "$spec"

# Binary package only, without the package of debug symbols
options=(--define "_topdir $work/rpmbuild" --define "polyphone_tag $tag" --define "debug_package %{nil}")
if [ -n "${JOBS:-}" ]; then
    options+=(--define "_smp_build_ncpus $JOBS")
fi
if ! rpmbuild -bb "${options[@]}" "$spec"; then
    echo "Build failed. If build dependencies are missing: sudo dnf builddep packaging/fedora/polyphone.spec"
    exit 1
fi

rpm=$(find "$work/rpmbuild/RPMS" -name "polyphone-${rpm_version}-1.*.rpm")
if [ -z "$rpm" ]; then
    echo "Error: no package found in $work/rpmbuild/RPMS"
    exit 1
fi
mv "$rpm" build/
rm -rf "$work"
rpm=build/$(basename "$rpm")
echo
# Query formats of rpm 4 and 6 (no SIZE in the packages of format 6, no option --requires with some builds)
rpm -qp --qf '%{NAME} %{VERSION}-%{RELEASE} (%{ARCH}), %|LONGSIZE?{%{LONGSIZE}}:{%{SIZE}}| bytes installed\n' "$rpm"
echo "Requires: $(rpm -qp --qf '[%{REQUIRENAME}\n]' "$rpm" | grep -v '^rpmlib(' | sort -u | paste -sd ' ')"
echo "Package: $root/$rpm"
echo
packaging/linux/check_package.sh "$rpm"
