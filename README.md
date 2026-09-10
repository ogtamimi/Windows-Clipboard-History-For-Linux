# Clipboard History

A lightweight, native **Clipboard History** app for Linux that brings the familiar
*Windows clipboard history* experience to your desktop. Built from scratch with
**C++23**, **Qt 6 / QML**, and **SQLite**, it runs as a background tray app and
opens a fast, searchable overlay popup with a global hotkey.

> The product is **Clipboard History**; the application (window/tray) title is **Clipboard**.

![License: MIT-style placeholder — add your license](#)

---

## Features

- **Instant access** — `Ctrl+Alt+V` opens a Windows-style clipboard popup anywhere
  on the screen, positioned near the pointer.
- **True popup window** — override-redirect, always-on-top, and **not** present in
  the taskbar or the Alt+Tab list.
- **Keyboard-first navigation** — while the popup is open, the arrow keys move the
  selection, typing filters the history, and `Enter` pastes the selected item,
  exactly like Windows clipboard history.
- **Real paste injection** — pasting writes the item to the system clipboard, closes
  the popup, restores focus to the app you were using, and injects a genuine
  `Ctrl+V` keystroke so the item lands in your document.
- **Click to paste** — click any row with the mouse and it is pasted immediately.
- **Search as you type** — type to filter; `Backspace` edits the query.
- **Images** — copied images and **image files copied from the file manager**
  (file operations in Nautilus/Caja/Files) are saved automatically and shown as
  thumbnails; pasting an image item puts the image back on the clipboard.
- **Text & rich text** — both are captured and displayed with a clean preview.
- **Pin items** — star a row to pin it; pinned rows are never removed by
  **Clear all**.
- **Clear all** — one click clears the whole unpinned history (with a
  confirmation dialog); pinned items stay.
- **Duplicate detection** — copying something that is already in the history moves
  it to the top instead of adding a copy.
- **Persistent history** — everything is stored in a SQLite database and survives
  restarts.
- **System tray** — the app keeps running in the background; use the tray menu to
  open the popup or quit.
- **Focus management** — the popup takes keyboard focus while open and returns it
  to your previous window when dismissed.

---

## Requirements

- A Debian-based distribution (Debian, Ubuntu, Linux Mint).
- Qt 6 runtime and QML modules.
- An **X11** session for the global hotkey, popup focus handling and paste
  injection.

> **Wayland note:** the native global hotkey and popup focus handling require X11.
> On Wayland the app can still run from the tray, but the full overlay experience
> is limited. (`Ctrl+Alt+V` works on X11, including XWayland sessions.)

---

## Install runtime dependencies

From the project directory, either run the helper script:

```bash
chmod +x install-dependencies.sh
./install-dependencies.sh
```

or install the packages manually:

```bash
sudo apt update
sudo apt install -y \
  libqt6core6 libqt6gui6 libqt6qml6 libqt6quick6 \
  libqt6quickcontrols2-6 libqt6sql6-sqlite \
  qml6-module-qtqml-models qml6-module-qtqml-workerscript \
  qml6-module-qtquick qml6-module-qtquick-controls \
  qml6-module-qtquick-layouts qml6-module-qtquick-templates \
  qml6-module-qtquick-window
```

---

## Build from source

Install the build dependencies, then configure and compile:

```bash
sudo apt install -y cmake g++ qt6-base-dev qt6-declarative-dev \
  libqt6sql6-sqlite libqt6svg6-dev libx11-dev libxtst-dev

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j"$(nproc)"
```

The binaries are produced inside the `build/` directory.

### Run

```bash
./build/clipboard-history
```

On X11 press **Ctrl+Alt+V** to open the popup.

> While the popup is open it takes keyboard focus (this is how Windows clipboard
> history behaves), so you cannot type into the application behind it until the
> popup is dismissed.

### Optional: install to the system

```bash
sudo cmake --install build
```

This copies the executable to `/usr/local/bin`, creates a **.desktop** entry,
installs the SVG icon, **and adds an autostart entry** (see below).

### Start automatically at login

Clipboard History can start by itself as soon as you log in. Two ways:

- **System-wide (requires install):** `sudo cmake --install build` writes an
  autostart entry to `/etc/xdg/autostart`, so every user session starts it.
- **Per user (no admin rights):**

  ```bash
  ./install-autostart.sh
  ```

  This installs `~/.config/autostart/clipboard-history.desktop` and prefers the
  freshly built `build/clipboard-history` binary (falling back to the
  system-installed `clipboard-history`).

The app starts in the background and is ready for `Ctrl+Alt+V` at your next
login. To disable autostart, remove the entry:

```bash
rm ~/.config/autostart/clipboard-history.desktop          # per user
sudo rm /etc/xdg/autostart/clipboard-history.desktop      # system-wide
```

---

## Usage

Open the popup with **Ctrl+Alt+V** (or from the system tray), then:

| Action | How |
|---|---|
| Open / hide popup | `Ctrl+Alt+V`, or click the tray icon |
| Select item | `↑` / `↓`, or click a row |
| Paste selected item | `Enter`, or **click** any row |
| Search / filter | type while the popup is open; `Backspace` to edit |
| Pin / unpin item | click the **★** button on the row (★ = pinned, ☆ = not) |
| Clear unpinned history | **Clear all** button (top right), then confirm |
| Hide popup | `Escape`, the **×** button, an outside click, or `Ctrl+Alt+V` again |

Pasting performs these four steps:

1. The item is placed on the system clipboard and moved to the top of the history,
2. the popup closes,
3. focus returns to the app you were using, and
4. a real **Ctrl+V** keystroke is injected so the text actually appears.

---

## Data location

History is stored in a SQLite database:

```text
~/.local/share/Clipboard History/Clipboard/clipboard-history.db
```

Copied images are saved next to it, in:

```text
~/.local/share/Clipboard History/Clipboard/images/
```

---

## Tests

An integration harness drives a running build over X11 and verifies the popup
behavior end to end: popup properties, keyboard focus, paste delivery, focus
restoration, Escape dismissal, hotkey toggling, and outside-click dismissal.

```bash
cmake --build build -j"$(nproc)"
CLIPBOARD_DEBUG=1 ./build/clipboard-history &
python3 tests/integration_harness.py
```

It exits `0` when every check passes. It needs `python3-tk`, `python3-xlib`, and
the `xwininfo` utility on the same X11 display.

---

## Troubleshooting

### `module "QtQuick" is not installed`

Install the runtime dependencies with `./install-dependencies.sh` (see above),
then restart the app.

### `Ctrl+Alt+V` does not open the popup

The global shortcut requires an X11 session (see the Wayland note above).
Some desktop environments reserve `Ctrl+Alt+V` or prevent apps from registering
global shortcuts; you can always open the popup from the tray icon instead.

### The app disappears after clicking `×`

This is intentional. The `×` button hides the panel while the background process
keeps running. Reopen it with `Ctrl+Alt+V` or from the tray.

### Paste does not insert text

Paste injects a real `Ctrl+V` via XTest, which a few applications deliberately
ignore. The selected item is still set on the system clipboard, so a manual
`Ctrl+V` pastes it.

### The whole right edge of the popup closes it as "outside"

This was a real bug, fixed in 1.0.0: the outside-click boundary now uses the real
window geometry instead of Qt's logical size. If you still see it, please open an
issue.

---

## Project structure

```text
clipboard-history/
├── CMakeLists.txt              # Build configuration (Qt 6, C++23) + install rules
├── install-dependencies.sh     # apt helper for runtime packages
├── install-autostart.sh        # enable per-user autostart at login
├── qml/
│   └── Main.qml                # Popup UI (header, search, list, dialogs)
├── resources/
│   ├── clipboard-history.desktop
│   └── clipboard-history.svg
├── src/
│   ├── main.cpp                # Entry point, popup control, focus & XTest paste
│   ├── clipboardmodel.cpp/.h   # SQLite-backed list model + clipboard capture
│   └── globalhotkey.cpp/.h     # X11 global hotkey (Ctrl+Alt+V)
└── tests/
    └── integration_harness.py  # X11 end-to-end test suite
```

---

## License

Add your license of choice (e.g. `LICENSE` file) before publishing. The
implementation contains no Microsoft proprietary assets or branding — "windows
clipboard history" refers only to the user-interface concept.