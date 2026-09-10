#include "globalhotkey.h"
#include <QGuiApplication>
#include <QByteArray>
#ifdef CLIPBOARD_HAS_X11
#include <X11/Xlib.h>
#include <X11/keysym.h>
#endif

GlobalHotkey::GlobalHotkey(QObject *parent) : QObject(parent) {
#ifdef CLIPBOARD_HAS_X11
    if (QGuiApplication::platformName() != QStringLiteral("xcb")) return;
    Display *display = XOpenDisplay(nullptr);
    if (!display) return;
    m_display = display;
    m_root = DefaultRootWindow(display);
    m_keycode = XKeysymToKeycode(display, XK_v);
    if (!m_keycode) return;
    const unsigned int masks[] = {
        ControlMask | Mod1Mask,
        ControlMask | Mod1Mask | LockMask,
        ControlMask | Mod1Mask | Mod2Mask,
        ControlMask | Mod1Mask | LockMask | Mod2Mask
    };
    bool ok = false;
    for (unsigned int mask : masks) {
        if (XGrabKey(display, m_keycode, mask, m_root, False, GrabModeAsync, GrabModeAsync) == Success)
            ok = true;
    }
    XSelectInput(display, m_root, KeyPressMask);
    XFlush(display);
    m_available = ok;
    connect(&m_timer, &QTimer::timeout, this, &GlobalHotkey::poll);
    m_timer.start(20);
#endif
}
GlobalHotkey::~GlobalHotkey() {
#ifdef CLIPBOARD_HAS_X11
    if (m_display) {
        Display *display = static_cast<Display *>(m_display);
        XUngrabKey(display, AnyKey, AnyModifier, static_cast<Window>(m_root));
        XCloseDisplay(display);
    }
#endif
}
bool GlobalHotkey::isAvailable() const { return m_available; }
void GlobalHotkey::poll() {
#ifdef CLIPBOARD_HAS_X11
    Display *display = static_cast<Display *>(m_display);
    while (display && XPending(display)) {
        XEvent event; XNextEvent(display, &event);
        const unsigned int modifiers = event.xkey.state & (ControlMask | Mod1Mask);
        if (event.type == KeyPress && event.xkey.keycode == m_keycode
            && modifiers == (ControlMask | Mod1Mask)
            && event.xkey.time != m_lastEventTime) {
            m_lastEventTime = event.xkey.time;
            emit activated();
        }
    }
#endif
}
