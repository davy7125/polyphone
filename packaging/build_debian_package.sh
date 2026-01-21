#!/bin/bash
set -e

# For building a DEB package, please take care of the following points:
#  * adapt ../sources/changelog so that the version and date are correct
#  * make sure "devscripts" package is installed (sudo apt-get install devscripts)
#  * the filesystem allow the execution flags to be removed (no NTFS)
#
# Finally, run this script by opening a terminal in this current directory

# Copy the changelog
cp ../sources/changelog debian/changelog

# Current version
VERSION=$(dpkg-parsechangelog -l debian/changelog -S Version | sed 's/-[0-9]*$//')
if [ -z "$VERSION" ]; then
  echo "Error: cannot find version in debian/changelog"
  exit 1
fi
echo "Detected version: $VERSION"

# Remove the execution flags
cd ..
chmod -R -x+X packaging/debian
chmod -R -x+X sources

# Rename folder sources
mv sources polyphone-$VERSION

# Create archive
tar -zcvf polyphone_$VERSION.orig.tar.gz polyphone-$VERSION

# Copy debian directory in sources
cp -r packaging/debian polyphone-$VERSION

# Build package
cd polyphone-$VERSION
dpkg-buildpackage || true
cd ..

# Revert rename
mv polyphone-$VERSION sources

# Delete files
rm -f polyphone_$VERSION-1.debian.tar.xz
rm -f polyphone_$VERSION-1.dsc
rm -f *.changes
rm -f *.buildinfo
rm -f polyphone_$VERSION.orig.tar.gz
rm -f polyphone-dbg*

# Clean the source directory
rm -rf sources/debian
rm -rf sources/generated_files
rm -rf sources/bin
rm -rf sources/.qm
rm -f sources/*.o
rm -f sources/ui_*
rm -f sources/moc_*
rm -f sources/qrc_*
rm -f sources/qmake_qmake_qm_files.qrc
rm -f sources/Makefile
rm -f sources/.qmake.stash

# Clean the packaging directory
rm -f packaging/debian/changelog
