#include "qlipmon.h"
#include "config.h"
#include "database.h"
#include "qlipmon_adaptor.h"

#include <QObject>
#include <QtDebug>

#include <QGuiApplication>
#include <QString>
#include <QtDBus/QDBusConnection>

QlipMon::QlipMon(QObject* parent) : QObject(parent) {
    // Get configuration from singleton
    const Config& config = Config::instance();

    // Initialize the database singleton using the config
    database::createFromConfig();
    clip = QGuiApplication::clipboard();
    QObject::connect(clip, &QClipboard::changed, this, &QlipMon::changed);

    if (config.dbus) {
        dbusAdaptor = new QlipmonAdaptor(this);
        QDBusConnection connection = QDBusConnection::sessionBus();
        connection.registerObject(QLIPMON_DBUS_PATH, this);

        if (!connection.registerService(QLIPMON_DBUS_FQDN)) {
            qFatal(
                "Failed to register D-Bus service '%s'. Another instance may already be running.",
                QLIPMON_DBUS_FQDN);
        }

        qInfo() << "D-Bus service registered successfully:" << QLIPMON_DBUS_FQDN;
    } else {
        qWarning() << "No DBUS interface. Why even run this?";
        dbusAdaptor = nullptr;
    }

    setProperty("broadcast", config.broadcast);
}

void QlipMon::changed(QClipboard::Mode mode) {
    QString text = clip->text(mode);

    if (_broadcast) {
        qDebug() << "broadcasting";
        emit updated(text, (int)(mode));
    } else {
        qDebug() << "NOT broadcasting";
    }

    database::instance().save(text, mode);
}

QString QlipMon::getLastText(int mode) {
    return database::instance().getLast(QClipboard::Mode(mode));
}

QStringList QlipMon::getTextHistory(int _mode, bool duplicates) {
    const auto mode = QClipboard::Mode(_mode);
    QStringList ret;

    auto entries = duplicates ? database::instance().getDuplicateEntries()
                              : database::instance().getUniqueEntries();

    for (const database_entry& entry : entries) {
        if (_mode < 0 || entry.mode == mode) {
            ret.append(entry.text);
        }
    }
    return ret;
}

QList<database_entry> QlipMon::getHistory() { return database::instance().getUniqueEntries(); }

void QlipMon::setText(const QString& text, int mode) {
    qDebug() << "setText(" << text << ")";

    if (mode < 0) {
        // set text to both clipboards
        clip->setText(text, QClipboard::Mode::Clipboard);
        clip->setText(text, QClipboard::Mode::Selection);
    } else {
        clip->setText(text, QClipboard::Mode(mode));
    }
}

void QlipMon::clearHistory() {
    qDebug() << "clearHistory()";
    database::instance().clearHistory();
}

int QlipMon::getEntryCount() {
    auto entries = database::instance().getUniqueEntries();
    return entries.count();
}
