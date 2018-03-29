#!/bin/bash

set -e

# Configure the application in the current directory
cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=out \
    `dirname $0`

# Build and install the application
make
make install

# Run macdeployqt
macdeployqt \
    out/nitroshare.app
    -always-overwrite

# Create a symlink to /Applications
rm -f out/Applications
ln -s /Applications out/Applications

# Create a DMG from the app and symlink
hdiutil create \
    -srcfolder out \
    -fs HFS+ \
    -volname NitroShare \
    nitroshare.dmg
