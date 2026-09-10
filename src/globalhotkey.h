#pragma once
#include <QObject>
#include <QTimer>

class GlobalHotkey final : public QObject {
    Q_OBJECT
public:
    explicit GlobalHotkey(QObject *parent = nullptr);
    ~GlobalHotkey() override;
    bool isAvailable() const;
signals:
    void activated();
private slots:
    void poll();
private:
    void *m_display = nullptr;
    unsigned long m_root = 0;
    unsigned int m_keycode = 0;
    unsigned long m_lastEventTime{};
    bool m_available = false;
    QTimer m_timer;
};
