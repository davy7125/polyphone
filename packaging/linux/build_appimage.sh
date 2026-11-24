#!/bin/bash
# AppImage of the editor, Polyphone_<tag>-linux_<x64|aarch64>.AppImage in the current directory.
# Usage, from the root of the repository, as root in Debian 12 (build-linux-*.yml, or by hand):
#   docker run --rm -v "$PWD:/src" -w /src debian:12 packaging/linux/build_appimage.sh <tag>
#
# Debian 12 is the oldest system supported: the AppImage needs its glibc (2.36) and the libstdc++ of its GCC (12), or
# more recent ones. Built on Ubuntu 24.04 until 2.7.3, it needed glibc 2.38 and did not start on Ubuntu 22.04, Debian
# 12 or RHEL 9. The libraries of Debian are bundled by linuxdeploy, except the ones of its exclude list that every
# desktop has (glibc, libstdc++, X11, OpenGL, ALSA, fontconfig...) and JACK: see packaging/linux/AppRun.

set -euo pipefail
if [ $# -ne 1 ]; then
    echo "Usage: $0 <tag>"
    exit 1
fi
tag=$1

# Versions of linuxdeploy, fixed so that a build gives the same AppImage
LINUXDEPLOY=1-alpha-20251107-1
LINUXDEPLOY_PLUGIN_QT=1-alpha-20250213-1

arch=$(uname -m)
case $arch in
    x86_64) suffix=x64 ;;
    aarch64) suffix=aarch64 ;;
    *) echo "Unsupported architecture: $arch"; exit 1 ;;
esac

bash tests/ci/install_dependencies.sh editor container
python3 packaging/set_version.py "$tag"
bash tests/ci/build_editor.sh . build/appimage -DCMAKE_INSTALL_PREFIX=/usr -DPOLYPHONE_OFFICIAL_BUILD=ON

appdir=build/AppDir
rm -rf "$appdir"
install -D build/appimage/bin/polyphone "$appdir/usr/bin/polyphone"
install -Dm644 packaging/io.polyphone.polyphone.desktop "$appdir/usr/share/applications/io.polyphone.polyphone.desktop"
install -Dm644 sources/resources/polyphone.png "$appdir/usr/share/icons/hicolor/512x512/apps/polyphone.png"

# libssl is loaded by Qt when a connection opens: linuxdeploy does not see it (it only follows libcrypto, linked)
libdir=/usr/lib/$(dpkg-architecture -qDEB_HOST_MULTIARCH)
install -D "$libdir/libssl.so.3" "$appdir/usr/lib/libssl.so.3"

# JACK, used by AppRun only if the system has none, with the libraries it needs that a desktop may not have (libdb)
install -D "$libdir/libjack.so.0" "$appdir/usr/lib/jack/libjack.so.0"
for library in $(readelf -d "$libdir/libjack.so.0" | awk -F'[][]' '/NEEDED/ {print $2}'); do
    case $library in
        libc.so.*|libm.so.*|libstdc++.so.*|libgcc_s.so.*|libpthread.so.*|librt.so.*|libdl.so.*|ld-linux*) ;;
        *) install -D "$libdir/$library" "$appdir/usr/lib/jack/$library" ;;
    esac
done

tools=build/linuxdeploy
mkdir -p "$tools"
curl -sSfL --retry 3 -o "$tools/linuxdeploy-$arch.AppImage" \
    "https://github.com/linuxdeploy/linuxdeploy/releases/download/$LINUXDEPLOY/linuxdeploy-$arch.AppImage"
curl -sSfL --retry 3 -o "$tools/linuxdeploy-plugin-qt-$arch.AppImage" \
    "https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/$LINUXDEPLOY_PLUGIN_QT/linuxdeploy-plugin-qt-$arch.AppImage"
chmod +x "$tools"/*.AppImage

# No FUSE in a container: the AppImages of linuxdeploy are extracted to run
export APPIMAGE_EXTRACT_AND_RUN=1
export PATH="$PWD/$tools:$PATH"
QMAKE=$(command -v qmake6)
export QMAKE
export OUTPUT="Polyphone_$tag-linux_$suffix.AppImage"
install -m755 packaging/linux/AppRun "$tools/AppRun" # Executable whatever the checkout
linuxdeploy-$arch.AppImage --appdir "$appdir" --plugin qt --custom-apprun "$tools/AppRun" --output appimage

# What the AppImage needs from the system, for the logs (packaging/linux/check_appimage.sh tests it)
echo "Most recent versions of glibc and libstdc++ needed:"
{ find "$appdir/usr" -type f \( -name '*.so*' -o -path "$appdir/usr/bin/*" \) -exec readelf -V {} + 2>/dev/null || true; } |
    grep -oE '(GLIBC|GLIBCXX)_[0-9.]+' | sort -u -t_ -k1,1 -k2,2V |
    awk -F_ '{last[$1] = $0} END {for (name in last) print "  " last[name]}' || true
ls -l "$OUTPUT"
