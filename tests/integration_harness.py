#!/usr/bin/env python3
"""Integration harness for the Clipboard History X11 popup.

Run against a built and already-running binary:

    cmake -B build && cmake --build build -j
    ./build/clipboard-history &
    python3 tests/integration_harness.py

The harness must run on the same X11 display as the app (DISPLAY set).
It opens a small Tk "text input" window to act as the original focused app,
then drives the real clipboard-history binary via XTest and verifies the
Windows-Clipboard-History behavior end to end:

  T1  Ctrl+Alt+V opens a popup that is override-redirect (not in the taskbar
      or alt-tab list), holds keyboard focus, appears near the pointer.
  T2  Down+Enter selects the second item: popup closes, item text is set on
      the system clipboard, real Ctrl+V is injected into the original app
      (which receives it), focus is restored, item moves to the top of history.
  T3  Escape closes the popup and restores focus.
  T4  Repeated hotkey toggles the popup without duplicating windows.
  T5  A click outside the popup dismisses it.
  T6  The app keeps running in the background after all of the above.

Exit code is 0 when every test passes.
"""
import os, re, sqlite3, subprocess, sys, time
import tkinter as tk
from Xlib import display as xldisplay, X, XK
from Xlib.ext import xtest

DB = os.path.expanduser("~/.local/share/Clipboard History/Clipboard/clipboard-history.db")

PASS = []
FAIL = []
def check(name, cond, detail=""):
    if cond:
        PASS.append(name); print(f"  PASS  {name}" + (f"  ({detail})" if detail else ""), flush=True)
    else:
        FAIL.append(name); print(f"  FAIL  {name}  ({detail})", flush=True)

d = xldisplay.Display()
root = d.screen().root

def kc(name):
    return d.keysym_to_keycode(XK.string_to_keysym(name))

def press_hold(name):
    xtest.fake_input(d, X.KeyPress, kc(name)); d.sync()
def release(name):
    xtest.fake_input(d, X.KeyRelease, kc(name)); d.sync()

def combo(name):
    xtest.fake_input(d, X.KeyPress, kc(name)); d.sync()
    xtest.fake_input(d, X.KeyRelease, kc(name)); d.sync()

def ctrl_alt_v():
    press_hold("Control_L"); press_hold("Alt_L"); press_hold("v")
    release("v"); release("Alt_L"); release("Control_L")

def escape():
    combo("Escape")

def type_text(s):
    for ch in s:
        combo(ch)
    time.sleep(0.05)

def move_pointer(x, y):
    xtest.fake_input(d, X.MotionNotify, x=x, y=y); d.sync(); time.sleep(0.05)

def click(x, y):
    move_pointer(x, y)
    xtest.fake_input(d, X.ButtonPress, 1); d.sync(); time.sleep(0.03)
    xtest.fake_input(d, X.ButtonRelease, 1); d.sync(); time.sleep(0.03)

def active_focus_id():
    try:
        f = d.get_input_focus().focus
        return f.id if f else 0
    except Exception:
        return 0

def win_tree():
    out = subprocess.run(["xwininfo", "-root", "-tree"], capture_output=True, text=True).stdout
    wins = {}
    for line in out.splitlines():
        m = re.match(r"\s*0x([0-9a-f]+)\s+\"(.*?)\"", line)
        if m:
            gm = re.search(r"(\d+)x(\d+)\+(-?\d+)\+(-?\d+)", line)
            if gm:
                wid = int(m.group(1), 16)
                wins[wid] = dict(name=m.group(2), w=int(gm.group(1)), h=int(gm.group(2)),
                                 x=int(gm.group(3)), y=int(gm.group(4)))
    return wins

def root_client_list():
    prop = root.get_full_property(d.intern_atom("_NET_CLIENT_LIST"), 0)
    return [int(x) for x in prop.value] if prop else []

def db_rows():
    con = sqlite3.connect(DB)
    rows = con.execute("SELECT id, text FROM clips ORDER BY pinned DESC, created DESC").fetchall()
    con.close()
    return rows

def clipboard_text():
    try:
        return tkroot.clipboard_get()
    except tk.TclError:
        return None

# ---------------------------------------------------------------- target window
state = {"ctrl_v_seen": False, "pastes": 0}
tkroot = tk.Tk()
tkroot.title("TKRxTarget")
tkroot.geometry("620x300+60+750")
text = tk.Text(tkroot, font=("DejaVuSansMono", 11))
text.pack(fill=tk.BOTH, expand=True)
text.focus_set()

def on_ctrl_v(event):
    state["ctrl_v_seen"] = True
    state["pastes"] += 1
    text.insert("insert", clipboard_text() or "")
    return "break"
text.bind("<Control-Key-v>", on_ctrl_v)
tkroot.update(); tkroot.update_idletasks()

def find_target_id():
    for wid, info in win_tree().items():
        if info["name"] == "TKRxTarget":
            return wid
    return None

def popup_visible():
    out = []
    for w in win_tree():
        if win_tree()[w]["name"] != "Clipboard":
            continue
        s = subprocess.run(["xwininfo", "-id", hex(w)], capture_output=True, text=True).stdout
        if any("Map State: IsViewable" in l for l in s.splitlines()):
            out.append(w)
    return out

def run():
    tid = find_target_id()
    assert tid, "target window not found"
    from Xlib.xobject.drawable import Window
    d.set_input_focus(tid, X.RevertToPointerRoot, X.CurrentTime); d.sync()
    time.sleep(0.2)
    type_text("Hello ")
    time.sleep(0.1)

    rows_before = db_rows()
    second_before = rows_before[1] if len(rows_before) > 1 else None
    print(f"  target={hex(tid)} second_item={second_before[0] if second_before else None}",
          flush=True)

    # T1 hotkey -> popup, popup-like, no taskbar, takes keyboard focus, near pointer
    move_pointer(400, 800)
    d.set_input_focus(tid, X.RevertToPointerRoot, X.CurrentTime); d.sync(); time.sleep(0.1)
    ctrl_alt_v(); time.sleep(0.8)
    vis = popup_visible()
    check("T1 hotkey shows popup", len(vis) == 1, str([hex(w) for w in vis]))
    if vis:
        pw = d.create_resource_object("window", vis[0])
        check("T1 popup override-redirect", bool(pw.get_attributes().override_redirect))
        check("T1 popup not in taskbar client list", vis[0] not in root_client_list(), hex(vis[0]))
        check("T1 popup holds keyboard focus", active_focus_id() == vis[0], hex(active_focus_id()))
        info = win_tree()[vis[0]]
        check("T1 popup near pointer", abs(info["x"] + info["w"]/2 - 400) < 420,
              f"center x={info['x']+info['w']/2:.0f}, ptr=400")

    # T2 Down + Enter selects 2nd item -> closes, pastes into target, clipboard & focus good
    if vis:
        combo("Down"); time.sleep(0.25)
    combo("Return")
    t0 = time.time()
    while time.time() - t0 < 5.0:
        tkroot.update(); time.sleep(0.1)
        if state["ctrl_v_seen"]:
            time.sleep(0.2)
            break
    time.sleep(0.3)
    vis_after = popup_visible()
    check("T2 popup closed after select", len(vis_after) == 0, str([hex(w) for w in vis_after]))
    got = text.get("1.0", "end-1c")
    if second_before:
        check("T2 pasted into original input", got.endswith(second_before[1]), repr(got[-45:]))
    check("T2 real Ctrl+V delivered to original app", state["ctrl_v_seen"], f"{state['pastes']} pastes")
    check("T2 clipboard == selected item", clipboard_text() == second_before[1],
          repr((clipboard_text() or "")[:40]))
    check("T2 focus restored to original app", active_focus_id() in (tid, find_target_id()),
          hex(active_focus_id()))
    if second_before:
        check("T2 selected item moved to top", db_rows()[0][0] == second_before[0],
              str([r[0] for r in db_rows()[:4]]))

    # T3 Escape closes popup and restores focus
    d.set_input_focus(tid, X.RevertToPointerRoot, X.CurrentTime); d.sync(); time.sleep(0.1)
    ctrl_alt_v(); time.sleep(0.8)
    check("T3 popup reopened", len(popup_visible()) == 1)
    escape(); time.sleep(0.6)
    check("T3 Escape closes popup", len(popup_visible()) == 0)
    check("T3 Escape restores original focus", active_focus_id() in (tid, find_target_id()),
          hex(active_focus_id()))

    # T4 repeated hotkey toggles without duplicate windows
    d.set_input_focus(tid, X.RevertToPointerRoot, X.CurrentTime); d.sync(); time.sleep(0.1)
    ctrl_alt_v(); time.sleep(0.7)
    check("T4 toggle-on single popup", len(popup_visible()) == 1)
    ctrl_alt_v(); time.sleep(0.7)
    check("T4 toggle-off hides popup", len(popup_visible()) == 0)
    check("T4 toggle-off restores focus", active_focus_id() in (tid, find_target_id()),
          hex(active_focus_id()))

    # T5 outside click closes popup and does not yank focus back
    ctrl_alt_v(); time.sleep(0.8)
    vis = popup_visible()
    ok = False
    if len(vis) == 1:
        info = win_tree()[vis[0]]
        tx, ty = 80, 1010
        inside_popup = info["x"] <= tx <= info["x"] + info["w"] and info["y"] <= ty <= info["y"] + info["h"]
        if inside_popup:
            tx = 40
        click(tx, ty); time.sleep(0.6)
        check("T5 outside click closes popup", len(popup_visible()) == 0, f"clicked {tx},{ty}")
        ok = True
    if not ok:
        check("T5 outside click closes popup", False, "popup not present")

    # T6 closing/hiding does not quit the app
    time.sleep(0.3)
    alive = subprocess.run(["pgrep", "-af", "clipboard-history"],
                           capture_output=True, text=True).stdout
    check("T6 app stays running in background", "clipboard-history" in alive)

    tkroot.destroy()
    print("\n==== SUMMARY ====", flush=True)
    print(f"PASS {len(PASS)}  FAIL {len(FAIL)}", flush=True)
    for f in FAIL:
        print("  FAILED:", f, flush=True)
    sys.exit(1 if FAIL else 0)

tkroot.after(400, run)
tkroot.mainloop()