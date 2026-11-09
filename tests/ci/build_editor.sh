#!/bin/bash
# Build the editor for the tests of the GitHub workflows (Linux), as the official builds: release, local RtAudio.
# Usage: build_editor.sh <repository root> <build directory> [options of CMake or arguments of qmake]
# Built with CMake, or with qmake if BUILD_SYSTEM=qmake or if the sources have no CMakeLists.txt (previous versions).
# The executable is <build directory>/bin/polyphone. QMAKE gives qmake (qmake6 by default), JOBS the number of
# parallel jobs (all processors by default), LOCAL_RTMIDI=1 the copy of RtMidi in sources/lib; ccache is used if found.

set -e
if [ $# -lt 2 ]; then
    echo "Usage: $0 <repository root> <build directory> [options of CMake or arguments of qmake]"
    exit 1
fi
root=$(cd "$1" && pwd)
build=$2
shift 2
mkdir -p "$build"
jobs=${JOBS:-$(nproc)}

if [ "$BUILD_SYSTEM" != qmake ] && [ -f "$root/sources/CMakeLists.txt" ]; then
    options=(-DCMAKE_BUILD_TYPE=Release -DPOLYPHONE_LOCAL_RTAUDIO=ON)
    if [ "$LOCAL_RTMIDI" = 1 ]; then
        options+=(-DPOLYPHONE_LOCAL_RTMIDI=ON)
    fi
    if command -v ccache > /dev/null; then
        options+=(-DCMAKE_C_COMPILER_LAUNCHER=ccache -DCMAKE_CXX_COMPILER_LAUNCHER=ccache)
    fi
    cmake -S "$root/sources" -B "$build" "${options[@]}" "$@"
    cmake --build "$build" --parallel "$jobs"
else
    cd "$build"
    options=("DEFINES+=USE_LOCAL_RTAUDIO")
    if [ "$LOCAL_RTMIDI" = 1 ]; then
        options+=("DEFINES+=USE_LOCAL_RTMIDI")
    fi
    if command -v ccache > /dev/null; then
        options+=("CONFIG+=ccache")
    fi
    "${QMAKE:-qmake6}" "$root/sources/polyphone.pro" CONFIG+=release "${options[@]}" "$@"
    make -j"$jobs"
    cd - > /dev/null
fi
ls -l "$build/bin/polyphone"
