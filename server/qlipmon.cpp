#include "qlipmon.h"
#include "config.h"
#include "qlipmon_adaptor.h"

#include <QObject>
#include <QtDebug>

#include <QGuiApplication>
#include <QString>
#include <QSettings>
#include <QtDBus/QDBusConnection>

QlipMon::QlipMon(const Config& config, QObject *parent): QObject(parent), db(config.numberEntries, config.useDiskDatabase, config.databasePath){
        clip = QGuiApplication::clipboard();
        QObject::connect(clip, &QClipboard::changed, this, &QlipMon::changed);

        if(config.dbus){
            dbusAdaptor = new QlipmonAdaptor(this);
            QDBusConnection connection = QDBusConnection::sessionBus();
            connection.registerObject(QLIPMON_DBUS_PATH, this);
            connection.registerService(QLIPMON_DBUS_FQDN);
        }else{
            qWarning()<<"No DBUS interface. Why even run this?";
            dbusAdaptor = nullptr;
        }

        setProperty("broadcast", config.broadcast);
}

void QlipMon::changed(QClipboard::Mode mode){
    QString text = clip->text(mode);

    if(_broadcast){
        qDebug()<<"broadcasting";
        emit updated(text, (int)(mode));
    }else{
        qDebug()<<"NOT broadcasting";
    }

    db.save(text, mode);
}

QString QlipMon::getLastText(int mode){
    return db.getLast(QClipboard::Mode(mode));
}

QStringList QlipMon::getTextHistory(int _mode, bool duplicates){
    const auto mode = QClipboard::Mode(_mode);
    QStringList ret;

    auto entries = duplicates ? db.getDuplicateEntries(): db.getUniqueEntries();

    for (const database_entry &entry : entries){
        if(_mode < 0 || entry.mode == mode){
            ret.append(entry.text);
        }
    }
    return ret;
}

QList<database_entry> QlipMon::getHistory(){
    return db.getUniqueEntries();
}

void QlipMon::setText(const QString &text, int mode){
    qDebug()<<"setText("<<text<<")";

    if( mode < 0 ){
        //set text to both clipboards
        clip->setText(text, QClipboard::Mode::Clipboard);
        clip->setText(text, QClipboard::Mode::Selection);
    }else{
        clip->setText(text, QClipboard::Mode(mode));
    }
}

void QlipMon::clearHistory(){
    qDebug()<<"clearHistory()";
    db.clearHistory();
}

int QlipMon::getEntryCount(){
    auto entries = db.getUniqueEntries();
    return entries.count();
}
