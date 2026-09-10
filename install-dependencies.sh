#!/usr/bin/env bash
set -euo pipefail

if ! command -v apt-get >/dev/null 2>&1; then
  echo "This helper currently supports Debian/Ubuntu/Linux Mint systems with apt-get." >&2
  exit 1
fi

sudo apt-get update
sudo apt-get install -y \
  libqt6core6 \
  libqt6gui6 \
  libqt6qml6 \
  libqt6quick6 \
  libqt6quickcontrols2-6 \
  libqt6sql6-sqlite \
  qml6-module-qtqml-models \
  qml6-module-qtqml-workerscript \
  qml6-module-qtquick \
  qml6-module-qtquick-controls \
  qml6-module-qtquick-layouts \
  qml6-module-qtquick-templates \
  qml6-module-qtquick-window

echo "Dependencies installed. Start Clipboard with:"
echo "  cmake -S . -B build && cmake --build build -j"
echo "  ./build/clipboard-history"
