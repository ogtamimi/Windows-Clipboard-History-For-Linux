#!/usr/bin/env bash
# Enable Clipboard History to start automatically when you log in.
#
# Installs an XDG autostart entry (per user) into ~/.config/autostart.
# It prefers the freshly built binary (./build/clipboard-history); if that is
# not available it falls back to the system-installed `clipboard-history`.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BIN_PATH="$SCRIPT_DIR/build/clipboard-history"

if [[ -x "$BIN_PATH" ]]; then
  EXEC="$BIN_PATH"
else
  EXEC="clipboard-history"
fi

AUTOSTART_DIR="${XDG_CONFIG_HOME:-$HOME/.config}/autostart"
mkdir -p "$AUTOSTART_DIR"

DESKTOP_FILE="$AUTOSTART_DIR/clipboard-history.desktop"
cat > "$DESKTOP_FILE" <<EOF
[Desktop Entry]
Type=Application
Name=Clipboard
GenericName=Clipboard History
Comment=Windows-style clipboard history for Linux
Exec=$EXEC
Icon=clipboard-history
StartupNotify=true
Terminal=false
X-GNOME-Autostart-enabled=true
EOF

echo "Autostart enabled: $DESKTOP_FILE"
echo "Exec=$EXEC"
echo "It will start automatically the next time you log in."