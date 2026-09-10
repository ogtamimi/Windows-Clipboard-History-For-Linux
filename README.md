# Clipboard History

A lightweight, native **Clipboard History** app for Linux that brings the familiar
*Windows clipboard history* experience to your desktop. Built from scratch with
**C++23**, **Qt 6 / QML**, and **SQLite**, it runs as a background tray app and
opens a fast, searchable overlay popup with the `Ctrl+Alt+V` global hotkey.

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

> The product/application **name** is **Clipboard History**; the window and tray
> title is **Clipboard**.

---

## Features

- **Instant access** — `Ctrl+Alt+V` opens a Windows-style clipboard popup anywhere
  on the screen, positioned near the pointer.
- **True popup window** — override-redirect, always-on-top, and **not** present in
  the taskbar or the Alt+Tab list.
- **Keyboard-first navigation** — while the popup is open, the arrow keys move the
  selection, typing filters the history, and `Enter` pastes the selected item.
- **Real paste injection** — pasting writes the item to the system clipboard, closes
  the popup, restores focus to the app you were using, then injects a genuine
  `Ctrl+V` keystroke so the item lands in your document.
- **Click to paste** — click any row with the mouse and it is pasted immediately.
- **Search as you type** — type to filter; `Backspace` edits the query.
- **Images** — copied images and image files copied from the file manager
  (Nautilus/Caja/Files) are saved automatically and shown as thumbnails; pasting
  an image item puts the image back on the clipboard.
- **Text & rich text** — both are captured and displayed with a clean preview.
- **Pin items** — star a row to pin it; pinned rows are never removed by
  **Clear all**.
- **Clear all** — one click clears the whole unpinned history (with a confirmation
  dialog); pinned items stay.
- **Duplicate detection** — copying something already in the history moves it to the
  top instead of adding a duplicate.
- **Persistent history** — everything is stored in a SQLite database and survives
  restarts.
- **System tray** — the app keeps running in the background; the tray menu opens the
  popup or quits.
- **Focus management** — the popup takes keyboard focus while open and returns it to
  your previous window when dismissed.

---

## Requirements

- A **64-bit (amd64)** Debian-based distribution: Debian **12+**, Ubuntu **22.04+**,
  Linux Mint **21+**.
- An **X11** session for the global hotkey, popup focus handling and paste injection.

> **Wayland note:** the native hotkey and popup focus handling require X11. On Wayland
> the app can still run from the tray, but the full overlay experience is limited.
> `Ctrl+Alt+V` works on X11, including XWayland sessions.

---

## Install from .deb

Download `clipboard-history_<version>_amd64.deb` from the
[Releases](https://github.com/ogtamimi/clipboard-history/releases) page.

### Option A — double click

1. **Download** the `.deb` file.
2. **Double click** it — the package manager (Software Manager / GDebi) opens.
3. Click **Install** and enter your password.
4. Launch **Clipboard History** from the Applications menu.
5. Press **Ctrl+Alt+V** to open the history popup.

### Option B — terminal

```bash
sudo apt install ./clipboard-history_<version>_amd64.deb
```

Then launch it (or find it in the Applications menu):

```bash
clipboard-history
```

It starts in the background with a system tray icon and **starts automatically
at every login** (the package installs an autostart entry). Open the popup with
**Ctrl+Alt+V**; use the tray menu to open or quit.

> The package is built for **amd64** only. On other architectures, build from
> source instead.

### Uninstall

```bash
sudo apt remove clipboard-history
```

This removes the program, menu entries, icons and autostart entry. Your personal
history database in `~/.local/share` is kept.

### Update / upgrade

Install the new `.deb` the same way you installed the first one:

```bash
sudo apt install ./clipboard-history_<new-version>_amd64.deb
```

The new version replaces the old one cleanly; your history is untouched.

---

## Build & package (contributors)

End users don't need this section — it's for people building from source.

### Manual dependencies

Installing a `.deb` pulls in everything automatically. To build from source, run:

```bash
chmod +x install-dependencies.sh
./install-dependencies.sh
```

(this installs `cmake`, a C++ compiler, Qt 6 development packages, X11/XTest
headers, QML modules and the SQLite driver).

### Build a .deb (recommended)

```bash
./build-deb.sh
```

The package is written to:

```text
dist/clipboard-history_<version>_amd64.deb
```

The `<version>` comes from the single version source, `CMakeLists.txt`
(`project(ClipboardHistory VERSION x.y.z)`).

### Build in place

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j"$(nproc)"
```

Run the resulting binary:

```bash
./build/clipboard-history
```

> While the popup is open it takes keyboard focus (like Windows clipboard history),
> so you can't type into the app behind it until it's dismissed.

### Continuous delivery

A [GitHub Actions workflow](.github/workflows/release.yml) builds
`clipboard-history_<version>_amd64.deb` automatically whenever a `v*` tag is
pushed; the package is attached to a GitHub Release. Example:

```bash
git tag v1.0.0
git push origin v1.0.0
```

### Package contents & metadata

The `.deb` installs:

| Item | Location |
|------|----------|
| Executable | `/usr/bin/clipboard-history` |
| Application menu entry | `/usr/share/applications/clipboard-history.desktop` |
| Autostart entry | `/etc/xdg/autostart/clipboard-history.desktop` |
| Icon | `/usr/share/icons/hicolor/scalable/apps/clipboard-history.svg` |
| License | `/usr/share/doc/clipboard-history/copyright` |

The package declares its dependencies (`libqt6*`, `libx11-6`, `libxtst6`,
QML modules, SQLite driver), so `apt` resolves every library automatically on
Debian 12 / Ubuntu 22.04+ / Linux Mint 21+.

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

1. the item is placed on the system clipboard and moved to the top of the history,
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

An integration harness drives a **running instance** over X11 and verifies the popup
end to end: popup properties, keyboard focus, paste delivery, focus restoration,
`Escape` dismissal, hotkey toggling, and outside-click dismissal.

```bash
cmake --build build -j"$(nproc)"
CLIPBOARD_DEBUG=1 ./build/clipboard-history &
python3 tests/integration_harness.py
```

It exits `0` when every check passes. It needs `python3-tk`, `python3-xlib`, and the
`xwininfo` utility on the same X11 display.

---

## Troubleshooting

### `module "QtQuick" is not installed`

With the `.deb`, `apt` installs the QML modules automatically. For source builds,
`./install-dependencies.sh` covers this; restart the app afterwards.

### `Ctrl+Alt+V` does not open the popup

The global shortcut requires an X11 session (see the Wayland note). Some desktop
environments reserve `Ctrl+Alt+V` or block global shortcuts; you can always open the
popup from the tray icon instead.

### The app disappears after clicking `×`

This is intentional. The `×` button hides the panel while the background process
keeps running. Reopen it with `Ctrl+Alt+V` or from the tray.

### Paste does not insert text

Paste injects a real `Ctrl+V` via XTest, which a few applications deliberately ignore.
The selected item is still placed on the system clipboard, so a manual `Ctrl+V` pastes
it.

---

## Project structure

```text
clipboard-history/
├── .github/workflows/release.yml # Auto-builds a .deb for every v* tag
├── CMakeLists.txt                # Build config, install rules, CPack metadata
├── LICENSE                       # MIT license
├── build-deb.sh                  # Build dist/clipboard-history_<ver>_amd64.deb
├── install-autostart.sh          # Optional per-user autostart helper
├── install-dependencies.sh       # apt helper for source builds
├── qml/Main.qml                  # Popup UI (header, search, list, dialogs)
├── resources/
│   ├── clipboard-history.desktop # Menu + autostart entry
│   └── clipboard-history.svg     # App icon
├── src/
│   ├── main.cpp                  # Entry point, popup control, focus & XTest paste
│   ├── clipboardmodel.cpp/.h     # SQLite-backed list model + clipboard capture
│   └── globalhotkey.cpp/.h       # X11 global hotkey (Ctrl+Alt+V)
└── tests/
    └── integration_harness.py    # X11 end-to-end test suite
```

---

## License

[MIT](LICENSE) © 2026 Ogtamimi. The implementation contains no proprietary assets
or branding — "Windows clipboard history" refers only to the user-interface concept.