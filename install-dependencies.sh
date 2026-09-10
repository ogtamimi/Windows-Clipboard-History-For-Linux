#!/usr/bin/env bash
# Install the dependencies needed to BUILD Clipboard History from source.
#
# End users do NOT need this script: install the ready-made .deb instead
# (see the README, "Install from .deb"). This helper is for contributors.
#
# Works on Debian >= 12, Ubuntu >= 22.04 and Linux Mint >= 21.
set -euo pipefail

if ! command -v apt-get >/dev/null 2>&1; then
  echo "This helper supports Debian/Ubuntu/Linux Mint systems with apt-get." >&2
  exit 1
fi

sudo apt-get update
sudo apt-get install -y \
  cmake \
  g++ \
  qt6-base-dev \
  qt6-declarative-dev \
  libx11-dev \
  libxtst-dev \
  libqt6svg6 \
  libqt6sql6-sqlite \
  qml6-module-qtquick \
  qml6-module-qtquick-controls \
  qml6-module-qtquick-layouts

echo "Build dependencies installed."
echo "Next:"
echo "  ./build-deb.sh            # build a ready-to-install .deb in dist/"
echo "  # or build in place:"
echo "  cmake -S . -B build && cmake --build build -j"
echo "  ./build/clipboard-history"