#!/bin/bash
set -e

# For building a DEB package, first copy this file and the directory "debian" next to the sources:
# root directory
# |- debian
# |  |- changelog
# |  |- ...
# |- sources
# |  |- ... (lot of things)
# |- build-debian-package
#
# Then, please take care of the following points:
#  * adapt debian/changelog so that the version and date are correct
#  * make sure "devscripts" package is installed (sudo apt-get install devscripts)
#  * the filesystem allow the execution flags to be removed (no NTFS)
#
# Finally, run this script by opening a terminal in the root directory
# For version 2.1 write "build-debian-package 2.1"

if [ -z "$1" ]
then
  echo "Usage: build-debian-package VERSION"
  exit $E_NOARGS
fi

# Remove the execution flags
chmod -R -x+X debian
chmod -R -x+X sources

# Rename folder sources
mv sources polyphone-$1

# Create archive
tar -zcvf polyphone_$1.orig.tar.gz polyphone-$1

# Copy debian directory in sources
cp -r debian polyphone-$1

# Build package
cd polyphone-$1
dpkg-buildpackage
cd ..

# Revert rename
mv polyphone-$1 sources

# Delete directories
rm -r sources/debian
rm -r sources/generated_files
rm -r sources/bin
rm sources/Makefile
rm polyphone_$1-1.debian.tar.xz
rm polyphone_$1-1.dsc
rm polyphone_$1-1_amd64.changes
