#include "clipboardmodel.h"
#include <QGuiApplication>
#include <QMimeData>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QBuffer>
#include <QCryptographicHash>
#include <QUrl>
#include <QImage>

ClipboardModel::ClipboardModel(QObject *parent) : QAbstractListModel(parent) {
    const auto dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));
    m_db.setDatabaseName(dir + QStringLiteral("/clipboard-history.db"));
    if (m_db.open()) {
        QSqlQuery q(m_db);
        q.exec("CREATE TABLE IF NOT EXISTS clips (id INTEGER PRIMARY KEY AUTOINCREMENT, text TEXT NOT NULL, pinned INTEGER DEFAULT 0, created TEXT NOT NULL, type TEXT NOT NULL, hash TEXT UNIQUE, image TEXT)");
        bool hasImageColumn = false;
        q.exec("PRAGMA table_info(clips)");
        while (q.next()) if (q.value(1).toString() == QStringLiteral("image")) hasImageColumn = true;
        if (!hasImageColumn) q.exec("ALTER TABLE clips ADD COLUMN image TEXT");
        load();
    }
    m_clipboard = QGuiApplication::clipboard();
    connect(m_clipboard, &QClipboard::dataChanged, this, &ClipboardModel::captureClipboard);
}
ClipboardModel::~ClipboardModel() { if (m_db.isOpen()) m_db.close(); }
int ClipboardModel::rowCount(const QModelIndex &parent) const { return parent.isValid() ? 0 : m_items.size(); }
QVariant ClipboardModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.size()) return {};
    const auto &i = m_items.at(index.row());
    switch (role) { case TextRole: return i.text; case PreviewRole: return i.text.left(280).replace('\n', ' '); case PinnedRole: return i.pinned; case TimestampRole: return i.created.toLocalTime().toString(QStringLiteral("h:mm AP")); case TypeRole: return i.type; case ImageRole: return i.imagePath.isEmpty() ? QVariant() : QVariant::fromValue(QUrl::fromLocalFile(i.imagePath)); default: return {}; }
}
QHash<int, QByteArray> ClipboardModel::roleNames() const { return {{TextRole,"text"},{PreviewRole,"preview"},{PinnedRole,"pinned"},{TimestampRole,"timestamp"},{TypeRole,"type"},{ImageRole,"imagePath"}}; }
void ClipboardModel::load() {
    QSqlQuery q(m_db); q.prepare("SELECT id,text,pinned,created,type,image FROM clips ORDER BY pinned DESC, created DESC"); q.exec();
    m_all.clear(); while (q.next()) m_all.push_back({q.value(0).toLongLong(),q.value(1).toString(),q.value(2).toBool(),QDateTime::fromString(q.value(3).toString(),Qt::ISODate),q.value(4).toString(),q.value(5).toString()});
    applyFilter();
}
void ClipboardModel::applyFilter() {
    beginResetModel(); m_items.clear();
    for (const auto &i : m_all) if (m_filter.isEmpty() || i.text.contains(m_filter, Qt::CaseInsensitive)) m_items.push_back(i);
    endResetModel(); emit countChanged();
}
void ClipboardModel::setFilter(const QString &text) { m_filter = text; applyFilter(); }
void ClipboardModel::insertItem(const QString &text, const QString &type, const QString &imagePath, const QString &forcedHash) {
    const QString clean = text.trimmed(); if (clean.isEmpty() && imagePath.isEmpty()) return;
    const bool dbg = qEnvironmentVariable("CLIPBOARD_DEBUG") == QByteArrayLiteral("1");
    if (dbg) qDebug().noquote() << "[clip] insertItem type=" << type << " imageEmpty=" << imagePath.isEmpty() << " textLen=" << clean.size() << " hashGiven=" << !forcedHash.isEmpty();
    if (!m_db.isOpen()) { if (dbg) qDebug().noquote() << "[clip] db closed"; return; }
    const QString hash = forcedHash.isEmpty() ? QString::fromLatin1(QCryptographicHash::hash(clean.toUtf8(), QCryptographicHash::Sha256).toHex()) : forcedHash;
    const QString textValue = (!clean.isEmpty() || imagePath.isEmpty()) ? clean : QStringLiteral("Image");
    QSqlQuery q(m_db); q.prepare("SELECT id,pinned FROM clips WHERE hash=?"); q.addBindValue(hash);
    if (q.exec() && q.next()) { QSqlQuery move(m_db); move.prepare("UPDATE clips SET created=?, text=?, type=?, image=? WHERE id=?"); move.addBindValue(QDateTime::currentDateTimeUtc().toString(Qt::ISODate)); move.addBindValue(textValue); move.addBindValue(type); move.addBindValue(imagePath); move.addBindValue(q.value(0)); move.exec(); load(); return; }
    q.prepare("INSERT INTO clips(text,pinned,created,type,hash,image) VALUES(?,0,?,?,?,?)"); q.addBindValue(textValue); q.addBindValue(QDateTime::currentDateTimeUtc().toString(Qt::ISODate)); q.addBindValue(type); q.addBindValue(hash); q.addBindValue(imagePath);
    if (!q.exec()) { const bool dbg = qEnvironmentVariable("CLIPBOARD_DEBUG") == QByteArrayLiteral("1"); if (dbg) qDebug().noquote() << "[clip] insert failed:" << q.lastError().text() << "hash=" << hash; }
    load();
}
void ClipboardModel::captureClipboard() {
    if (!m_clipboard) return;
    const auto *mime = m_clipboard->mimeData();
    const bool debugTag = qEnvironmentVariable("CLIPBOARD_DEBUG") == QByteArrayLiteral("1");
    if (debugTag) qDebug().noquote() << "[clip] dataChanged" << (mime ? mime->formats().join(',') : QStringLiteral("none"));
    if (!mime) return;

    const auto storeImage = [this, debugTag](const QImage &img, const char *source) -> bool {
        if (img.isNull()) return false;
        QByteArray bytes;
        QBuffer buffer(&bytes);
        buffer.open(QIODevice::WriteOnly);
        if (!img.save(&buffer, "PNG")) return false;
        const auto dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        const auto imgDir = dir + QStringLiteral("/images");
        QDir().mkpath(imgDir);
        const QString hash = QString::fromLatin1(QCryptographicHash::hash(bytes, QCryptographicHash::Sha256).toHex());
        const QString path = imgDir + QStringLiteral("/clip-") + hash + QStringLiteral(".png");
        if (!QFile::exists(path)) {
            QFile file(path);
            if (!file.open(QIODevice::WriteOnly) || file.write(bytes) != bytes.size()) return false;
            file.close();
        }
        if (debugTag) qDebug().noquote() << "[clip] image saved" << path << source;
        insertItem({}, QStringLiteral("Image"), path, hash);
        return true;
    };

    if (mime->hasImage()) { const QImage img = m_clipboard->image(); if (storeImage(img, "clipboard")) return; }
    if (mime->hasUrls()) {
        const auto urls = mime->urls();
        for (const auto &u : urls) {
            if (u.isLocalFile()) {
                if (debugTag) qDebug().noquote() << "[clip] local uri" << u.toString();
                if (storeImage(QImage(u.toLocalFile()), "local file")) return;
            }
        }
    }
    if (mime->hasText()) insertItem(mime->text(), mime->hasHtml() ? QStringLiteral("Rich text") : QStringLiteral("Text"));
}
void ClipboardModel::togglePinned(int row) { if (row < 0 || row >= m_items.size()) return; QSqlQuery q(m_db); q.prepare("UPDATE clips SET pinned=? WHERE id=?"); q.addBindValue(!m_items[row].pinned); q.addBindValue(m_items[row].id); q.exec(); load(); }
void ClipboardModel::clearUnpinned() {
    if (qEnvironmentVariable("CLIPBOARD_DEBUG") == QByteArrayLiteral("1"))
        qDebug().noquote() << "[clip] clearUnpinned invoked";
    QSqlQuery q(m_db); q.exec("DELETE FROM clips WHERE pinned=0"); load();
}
void ClipboardModel::pasteItem(int row) {
    if (row < 0 || row >= m_items.size() || !m_clipboard) return;
    const bool debugTag = qEnvironmentVariable("CLIPBOARD_DEBUG") == QByteArrayLiteral("1");
    if (debugTag) qDebug().noquote() << "[clip] pasteItem row=" << row << " id=" << m_items[row].id << " type=" << m_items[row].type;
    if (!m_items[row].imagePath.isEmpty()) {
        const QImage img(m_items[row].imagePath);
        if (!img.isNull()) m_clipboard->setImage(img);
    } else {
        m_clipboard->setText(m_items[row].text);
    }
    if (debugTag) qDebug().noquote() << "[clip] clipboard set, emitting itemSelected";
    emit itemSelected();
}
