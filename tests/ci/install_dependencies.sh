#!/bin/bash
# Ubuntu packages needed by the GitHub workflows.
# Usage: install_dependencies.sh <group>...
#   editor      build of the editor with the Qt 6 of the system (and its execution without display)
#   qt5         build of the editor with the Qt 5 of the system
#   synth_core  build of the synth core alone (tests/synth_core)
#   plugin      build of the plugin and of its test host (JUCE), execution without display
#   tests       soundfonts of the rendering tests

set -e
if [ $# -eq 0 ]; then
    echo "Usage: $0 <group>..."
    exit 1
fi

common=(build-essential cmake ccache libasound2-dev libjack-jackd2-dev libsndfile1-dev libogg-dev libvorbis-dev libflac-dev
        zlib1g-dev libssl-dev librtaudio-dev librtmidi-dev ca-certificates)
packages=()
for group in "$@"; do
    case $group in
        editor)
            packages+=("${common[@]}" qt6-base-dev qt6-svg-dev qt6-tools-dev qt6-tools-dev-tools qt6-l10n-tools)
            ;;
        qt5)
            packages+=("${common[@]}" qtbase5-dev libqt5svg5-dev qttools5-dev qttools5-dev-tools)
            ;;
        synth_core)
            packages+=(build-essential cmake libvorbis-dev libsndfile1-dev)
            ;;
        plugin)
            packages+=(build-essential cmake ccache libvorbis-dev libsndfile1-dev libasound2-dev libfreetype-dev
                       libfontconfig1-dev libx11-dev libxcomposite-dev libxcursor-dev libxext-dev libxinerama-dev
                       libxi-dev libxrandr-dev libxrender-dev libgl-dev xvfb)
            ;;
        tests)
            packages+=(fluid-soundfont-gm timgm6mb-soundfont python3)
            ;;
        *)
            echo "Unknown group: $group"
            exit 1
            ;;
    esac
done

sudo apt-get update
sudo apt-get install -y "${packages[@]}"
