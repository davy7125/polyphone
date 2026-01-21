#!/bin/bash
set -e

# For building a RPM package:
# * first build polyphone and copy the executable "polyphone" in the directory "fedora"
# * then make sure you installed the rpmdevtools
# * finally run this script
# A RPM package will appear in fedora/x86_64

cd fedora
fedpkg --release f41 local
cd ..
