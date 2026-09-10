#pragma once
#include <QAbstractListModel>
#include <QClipboard>
#include <QDateTime>
#include <QSqlDatabase>
#include <QTimer>
#include <QUrl>

class ClipboardModel final : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
public:
    enum Roles { TextRole = Qt::UserRole + 1, PreviewRole, PinnedRole, TimestampRole, TypeRole, ImageRole };
    explicit ClipboardModel(QObject *parent = nullptr);
    ~ClipboardModel() override;
    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE void togglePinned(int row);
    Q_INVOKABLE void clearUnpinned();
    Q_INVOKABLE void pasteItem(int row);
    Q_INVOKABLE void setFilter(const QString &text);
    void captureClipboard();
signals:
    void countChanged();
    void itemSelected();
private:
    struct Item { qint64 id{}; QString text; bool pinned{}; QDateTime created; QString type; QString imagePath; };
    QList<Item> m_items;
    QList<Item> m_all;
    QString m_filter;
    QSqlDatabase m_db;
    QClipboard *m_clipboard{};
    void load();
    void applyFilter();
    void insertItem(const QString &text, const QString &type, const QString &imagePath = {}, const QString &forcedHash = {});
};
