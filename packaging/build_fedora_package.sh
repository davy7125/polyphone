#!/bin/bash
set -e

# For building a RPM package:
# * first build polyphone with CMake (official package: the notifications of a new version are shown):
#     cmake -S ../sources -B ../build/fedora -DPOLYPHONE_OFFICIAL_BUILD=ON && cmake --build ../build/fedora --parallel
#   and copy the executable "../build/fedora/bin/polyphone" in the directory "fedora"
# * then make sure you installed the rpmdevtools
# * finally run this script
# A RPM package will appear in fedora/x86_64

cd fedora
fedpkg --release f41 local
cd ..
