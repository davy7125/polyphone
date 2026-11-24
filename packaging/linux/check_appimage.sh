#!/bin/bash
# Checks that the AppImage starts on the distribution of the container where it runs, as root (build-linux-*.yml):
#   docker run --rm -v "$PWD:/w" -w /w <image> packaging/linux/check_appimage.sh Polyphone_<tag>-linux_<arch>.AppImage
# Only what every desktop has is installed: the libraries that linuxdeploy does not bundle, and Xvfb as the display.
# JACK is not installed first, so that the copy of the AppImage is used (packaging/linux/AppRun); on Debian and Ubuntu,
# polyphone then starts again with the JACK of the system, which must be the one loaded.

set -euo pipefail
if [ $# -ne 1 ]; then
    echo "Usage: $0 <AppImage>"
    exit 1
fi
appimage=$(readlink -f "$1")
chmod +x "$appimage" # Lost by the artifacts of GitHub
. /etc/os-release
echo "System: $PRETTY_NAME"

if command -v apt-get > /dev/null; then
    export DEBIAN_FRONTEND=noninteractive
    apt-get update -qq
    alsa=libasound2
    if apt-cache show libasound2t64 > /dev/null 2>&1; then
        alsa=libasound2t64 # Ubuntu 24.04
    fi
    apt-get install -y -qq --no-install-recommends xvfb "$alsa" libharfbuzz0b libice6 libsm6 libgl1 libglx0 libegl1 \
        libopengl0 libx11-6 libx11-xcb1 libxcb1 libfontconfig1 libfreetype6 fonts-dejavu-core libgmp10 libgpg-error0 \
        libcom-err2 zlib1g > /dev/null
elif command -v dnf > /dev/null; then
    # By the names of the libraries, the same in every version of Fedora
    libraries=(libasound.so.2 libharfbuzz.so.0 libICE.so.6 libSM.so.6 libGL.so.1 libGLX.so.0 libEGL.so.1 libOpenGL.so.0
               libX11.so.6 libX11-xcb.so.1 libxcb.so.1 libfontconfig.so.1 libfreetype.so.6 libgmp.so.10
               libgpg-error.so.0 libcom_err.so.2 libz.so.1)
    dnf install -y -q xorg-x11-server-Xvfb dejavu-sans-fonts "${libraries[@]/%/()(64bit)}"
elif command -v pacman > /dev/null; then
    pacman -Syu --noconfirm --needed xorg-server-xvfb alsa-lib harfbuzz libice libsm libglvnd libx11 libxcb fontconfig \
        freetype2 ttf-dejavu gmp libgpg-error e2fsprogs zlib > /dev/null
else
    echo "Unknown package manager"
    exit 1
fi

Xvfb :99 -screen 0 1280x1024x24 -nolisten tcp > /tmp/xvfb.log 2>&1 &
export DISPLAY=:99
for _ in $(seq 100); do
    if [ -S /tmp/.X11-unix/X99 ]; then
        break
    fi
    sleep 0.1
done
if [ ! -S /tmp/.X11-unix/X99 ]; then
    cat /tmp/xvfb.log
    exit 1
fi

# Extracted: no FUSE in a container
workdir=$(mktemp -d)
cd "$workdir"
"$appimage" --appimage-extract > /dev/null

# Starts polyphone --version and checks the library of JACK that it loads: "bundled" or "system"
start() {
    local expected=$1
    if ! LD_DEBUG=libs timeout 120 ./squashfs-root/AppRun --version > out.txt 2> err.txt; then
        echo "::error::Polyphone does not start on $PRETTY_NAME (JACK: $expected)"
        grep -v '^ *[0-9]*:' err.txt | tail -20 || true
        echo "Libraries not found:"
        LD_LIBRARY_PATH="$workdir/squashfs-root/usr/lib/jack" ldd squashfs-root/usr/bin/polyphone | grep 'not found' || true
        exit 1
    fi
    cat out.txt
    local jack
    jack=$(grep -oE 'calling init: [^ ]*/libjack\.so\.0' err.txt | head -1 | awk '{print $3}')
    case $jack in
        "$workdir"/squashfs-root/usr/lib/jack/*) used=bundled ;;
        /*) used=system ;;
        *) used=none ;;
    esac
    echo "JACK: $used ($jack)"
    if [ "$used" != "$expected" ]; then
        echo "::error::The library of JACK loaded on $PRETTY_NAME is not the $expected one"
        exit 1
    fi
}

if { ldconfig -p || /sbin/ldconfig -p; } 2> /dev/null | grep -q 'libjack\.so\.0 '; then
    echo "JACK is installed in the image: the test needs a system without it first"
    exit 1
fi
start bundled

if command -v apt-get > /dev/null; then
    apt-get install -y -qq --no-install-recommends libjack-jackd2-0 > /dev/null
    start system
fi
echo "Polyphone starts on $PRETTY_NAME"
