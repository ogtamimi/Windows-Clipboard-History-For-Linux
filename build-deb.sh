#!/usr/bin/env bash
# Build a Debian package:  dist/clipboard-history_<version>_amd64.deb
#
#        ./build-deb.sh
#
# Result:  dist/clipboard-history_1.0.0_amd64.deb
# Requires: cmake, a C++23 compiler and the Qt 6 development packages
#         (see install-dependencies.sh / the CI workflow).
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$ROOT/build-deb"
DIST_DIR="$ROOT/dist"
VERSION="$(sed -n 's/^project(ClipboardHistory VERSION \([0-9.]*\).*/\1/p' "$ROOT/CMakeLists.txt")"
DEB_NAME="clipboard-history_${VERSION}_amd64.deb"

if [[ -z "$VERSION" ]]; then
    echo "error: could not read VERSION from CMakeLists.txt" >&2
    exit 1
fi

echo "== Building Clipboard History $VERSION (amd64) =="
cmake -S "$ROOT" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release
cmake --build "$BUILD_DIR" -j"$(nproc)"

echo "== Packaging .deb =="
rm -rf "$DIST_DIR"
mkdir -p "$DIST_DIR"
cpack --config "$BUILD_DIR/CPackConfig.cmake" -B "$DIST_DIR"

if [[ -f "$DIST_DIR/$DEB_NAME" ]]; then
    echo
    echo "OK: $DIST_DIR/$DEB_NAME"
else
    echo "error: expected $DIST_DIR/$DEB_NAME not produced" >&2
    ls -1 "$DIST_DIR" >&2
    exit 1
fi