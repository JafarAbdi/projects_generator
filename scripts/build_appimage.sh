#! /bin/bash

# https://docs.appimage.org/packaging-guide/from-source/native-binaries.html#examples

set -x
set -e

# building in temporary directory to keep system clean
# use RAM disk if possible (as in: not building on CI system like Travis, and RAM disk is available)
if [ -d /dev/shm ]; then
    TEMP_BASE=/dev/shm
else
    TEMP_BASE=/tmp
fi

BUILD_DIR=$(mktemp -d -p "$TEMP_BASE" appimage-build-XXXXXX)

# make sure to clean up build dir, even if errors occur
cleanup () {
    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
    fi
}
trap cleanup EXIT

# store repo root as variable
REPO_ROOT=$(readlink -f $(dirname $(dirname $0)))
OLD_CWD=$(readlink -f .)

# switch to build dir
pushd "$BUILD_DIR"

# configure build files with CMake
# we need to explicitly set the install prefix, as CMake's default is /usr/local for some reason...
CC=clang-10 CXX=clang++-10 cmake "$REPO_ROOT" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_TOOLCHAIN_FILE="$VCPKG_DIR"/scripts/buildsystems/vcpkg.cmake

# build project and install files into AppDir
make -j$(nproc)
make install DESTDIR=AppDir

# now, build AppImage using linuxdeploy
wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage

# make them executable
chmod +x linuxdeploy-x86_64.AppImage

# TODO: Should override this variable or keep the current behavior by appending .?
export LD_LIBRARY_PATH="$($REPO_ROOT/scripts/findlink.sh projects_generator)"$LD_LIBRARY_PATH
# initialize AppDir, bundle shared libraries for projects_generator and build AppImage
cp $REPO_ROOT/scripts/apprun.sh $BUILD_DIR/AppRun
chmod +x $BUILD_DIR/AppRun
./linuxdeploy-x86_64.AppImage --appdir AppDir --output appimage --custom-apprun AppRun

# move built AppImage back into original CWD
chmod +x projects_generator*.AppImage
mv projects_generator*.AppImage "$OLD_CWD"
