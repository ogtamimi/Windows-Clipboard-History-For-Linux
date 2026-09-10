#include "clipboardmodel.h"
#include "globalhotkey.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QQuickWindow>
#include <QCursor>
#include <QScreen>
#include <QTimer>
#include <QDebug>
#ifdef CLIPBOARD_HAS_X11
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#endif

#ifdef CLIPBOARD_HAS_X11
static bool debugEnabled() {
    const char *e = getenv("CLIPBOARD_DEBUG");
    return e && *e == '1';
}
static void debugMsg(const char *msg, long v = -1) {
    if (!debugEnabled()) return;
    if (v >= 0) qDebug().noquote() << "[clip] " << msg << v;
    else qDebug().noquote() << "[clip] " << msg;
}
#endif

struct PopupController {
    QQuickWindow *window{};
#ifdef CLIPBOARD_HAS_X11
    Display *display{};
    Window previousWindow{};
    Window root{};
    bool avoidRestoreOnHide{false};
    bool pendingInject{false};
    QTimer pollTimer;
#endif
    ~PopupController() {
#ifdef CLIPBOARD_HAS_X11
        if (display) XCloseDisplay(display);
#endif
    }

    bool isPopupVisible() const { return window && window->isVisible(); }

    void init() {
#ifdef CLIPBOARD_HAS_X11
        display = XOpenDisplay(nullptr);
        if (!display) return;
        root = DefaultRootWindow(display);
        XFlush(display);
        if (window)
            QObject::connect(&pollTimer, &QTimer::timeout, window, [this] { drainEvents(); });
        pollTimer.start(20);
#endif
    }

    #ifdef CLIPBOARD_HAS_X11
    bool isWindowValid(Window w) const {
        XWindowAttributes attr{};
        if (!display || !w || w == PointerRoot) return false;
        return XGetWindowAttributes(display, w, &attr) != 0;
    }

    void setInputFocus(Window w) {
        if (!display || !w || w == PointerRoot) return;
        XSetInputFocus(display, w, RevertToPointerRoot, CurrentTime);
        XFlush(display);
    }

    void capturePreviousWindow() {
        if (!display || !window) return;
        Window focused{}; int revert{};
        XGetInputFocus(display, &focused, &revert);
        debugMsg("raw focused window", static_cast<long>(focused));
        if (focused && focused != window->winId() && isWindowValid(focused))
            previousWindow = focused;
        debugMsg("captured previous window", static_cast<long>(previousWindow));
    }

    void restoreFocus() {
        if (!display) return;
        if (isWindowValid(previousWindow)) {
            setInputFocus(previousWindow);
            debugMsg("restored input focus", static_cast<long>(previousWindow));
        } else {
            previousWindow = None;
        }
    }

    void focusPopup() {
        if (!display || !window) return;
        const Window wid = window->winId();
        setInputFocus(wid);
        XRaiseWindow(display, wid);
        XFlush(display);
        debugMsg("focused popup window", static_cast<long>(wid));
    }

    void injectPaste() {
        if (!display || !isWindowValid(previousWindow)) { debugMsg("injectPaste skipped (no focus target)"); return; }
        int ev{}, erm{}, major{}, minor{};
        if (!XTestQueryExtension(display, &ev, &erm, &major, &minor)) { debugMsg("XTest not available"); return; }
        setInputFocus(previousWindow);
        const KeyCode ctrl = XKeysymToKeycode(display, XK_Control_L);
        const KeyCode v = XKeysymToKeycode(display, XK_v);
        if (!ctrl || !v) { debugMsg("injectPaste skipped (keycodes unavailable)"); return; }
        debugMsg("injecting real Ctrl+V via XTest");
        XTestFakeKeyEvent(display, ctrl, True, CurrentTime);
        XTestFakeKeyEvent(display, v, True, CurrentTime);
        XTestFakeKeyEvent(display, v, False, CurrentTime);
        XTestFakeKeyEvent(display, ctrl, False, CurrentTime);
        XFlush(display);
    }

    bool pointOutsidePopup(int xRoot, int yRoot) const {
        if (!display || !window) return true;
        int ax = 0, ay = 0; Window child{};
        XTranslateCoordinates(display, window->winId(), root, 0, 0, &ax, &ay, &child);
        Window rw = None; int rx = 0, ry = 0;
        unsigned int w = 0, h = 0, bw = 0, depth = 0;
        XGetGeometry(display, window->winId(), &rw, &rx, &ry, &w, &h, &bw, &depth);
        return xRoot < ax || xRoot >= ax + (int)w || yRoot < ay || yRoot >= ay + (int)h;
    }

    void hidePopup() {
        if (!window || !window->isVisible()) return;
        window->hide();
    }

    void forwardClick(int x, int y, unsigned button) {
        if (!display) return;
        debugMsg("forwarding outside click", button);
        XTestFakeMotionEvent(display, -1, x, y, CurrentTime);
        XTestFakeButtonEvent(display, button, True, CurrentTime);
        XTestFakeButtonEvent(display, button, False, CurrentTime);
        XFlush(display);
    }

    void dismissOutsideClick(int x, int y, unsigned button) {
        if (!window || !window->isVisible()) return;
        debugMsg("dismissing from outside click");
        avoidRestoreOnHide = true;
        previousWindow = None;
        hidePopup();
        QTimer::singleShot(80, window, [this, x, y, button] {
#ifdef CLIPBOARD_HAS_X11
            forwardClick(x, y, button);
#endif
        });
    }

    void toggle() {
        if (isPopupVisible()) { debugMsg("toggle: hiding popup"); avoidRestoreOnHide = false; hidePopup(); }
        else { debugMsg("toggle: showing popup"); show(); }
    }

    void show() {
        if (!window || window->isVisible()) return;
        capturePreviousWindow();
        const QPoint cursor = QCursor::pos();
        if (const auto *screen = QGuiApplication::screenAt(cursor)) {
            const QRect area = screen->availableGeometry();
            const int x = qBound(area.left(), cursor.x() - window->width() / 2, area.right() - window->width());
            const int y = qBound(area.top(), cursor.y() - window->height() / 2, area.bottom() - window->height());
            window->setPosition(x, y);
            debugMsg("positioned popup", x);
        }
        window->show();
        window->raise();
        focusPopup();
    }

    void drainEvents() {
#ifdef CLIPBOARD_HAS_X11
        if (!display || !window || !window->isVisible()) return;
        Window rootret = None, childret = None;
        int rx = 0, ry = 0, wx = 0, wy = 0;
        unsigned int mask = 0;
        XQueryPointer(display, root, &rootret, &childret, &rx, &ry, &wx, &wy, &mask);
        if (pointOutsidePopup(rx, ry) && (mask & (Button1Mask | Button2Mask | Button3Mask))) {
            const unsigned button =
                (mask & Button1Mask) ? 1u : (mask & Button2Mask) ? 2u : (mask & Button3Mask) ? 3u : 0u;
            if (button) dismissOutsideClick(rx, ry, button);
        }
#endif
    }
#endif

    void onItemSelected() {
        if (!window) return;
        pendingInject = true;
        hidePopup();
        restoreFocus();
    }

    void onHidden() {
#ifdef CLIPBOARD_HAS_X11
        if (avoidRestoreOnHide) { avoidRestoreOnHide = false; return; }
        restoreFocus();
        if (pendingInject) {
            pendingInject = false;
            if (display)
                QTimer::singleShot(120, window, [this] { injectPaste(); });
        }
#endif
    }
};

static void toggleWindow(PopupController &popup) { popup.toggle(); }

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
    app.setApplicationName(QStringLiteral("Clipboard"));
    app.setApplicationDisplayName(QStringLiteral("Clipboard"));
    app.setOrganizationName(QStringLiteral("Clipboard History"));
    const QIcon icon = QIcon::fromTheme(QStringLiteral("edit-paste"));
    app.setWindowIcon(icon);

    ClipboardModel model;
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("clipboardModel"), &model);
    const QUrl url(QStringLiteral("qrc:/ClipboardHistory/qml/Main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, &app, [] { QCoreApplication::exit(1); }, Qt::QueuedConnection);
    engine.load(url);
    const auto windows = engine.rootObjects();
    if (windows.isEmpty()) return 1;
    PopupController popup;
    popup.window = qobject_cast<QQuickWindow *>(windows.first());
    popup.init();
    QObject::connect(&model, &ClipboardModel::itemSelected, &app, [&] { popup.onItemSelected(); });
    QObject::connect(popup.window, &QWindow::visibleChanged, &app, [&] {
        if (!popup.window->isVisible()) popup.onHidden();
    });

    QSystemTrayIcon tray(icon, &app);
    QMenu trayMenu;
    QAction openAction(QStringLiteral("Open Clipboard"), &trayMenu);
    QAction quitAction(QStringLiteral("Quit"), &trayMenu);
    trayMenu.addAction(&openAction);
    trayMenu.addSeparator();
    trayMenu.addAction(&quitAction);
    tray.setContextMenu(&trayMenu);
    tray.setToolTip(QStringLiteral("Clipboard History"));
    QObject::connect(&openAction, &QAction::triggered, &app, [&] { toggleWindow(popup); });
    QObject::connect(&tray, &QSystemTrayIcon::activated, &app, [&](QSystemTrayIcon::ActivationReason reason) { if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) toggleWindow(popup); });
    QObject::connect(&quitAction, &QAction::triggered, &app, &QCoreApplication::quit);
    tray.show();

    GlobalHotkey hotkey(&app);
    QObject::connect(&hotkey, &GlobalHotkey::activated, &app, [&] {
#ifdef CLIPBOARD_HAS_X11
        debugMsg("global hotkey activated (Ctrl+Alt+V)");
#endif
        toggleWindow(popup);
    });
    return app.exec();
}