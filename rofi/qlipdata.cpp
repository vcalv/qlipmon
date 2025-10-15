#include "qlipdata.h"

#include "config.h"
#include <QtDBus/QtDBus>


RofiData* QlipData::getEntries(){
    RofiData *ret = new RofiData;
    qDebug()<<"using singleton config";

    const Config& config = Config::instance();
    QlipMonInterface _interface(QLIPMON_DBUS_FQDN, QLIPMON_DBUS_PATH, QDBusConnection::sessionBus(), 0);
    QDBusReply reply = _interface.getTextHistory(config.kind, config.duplicates);

    if(reply.isValid()){
        ret->error = false;
        ret->errorString = "";
        ret->entries = reply.value();
        auto &entries = ret->entries;

        qDebug()<<"Got a list with "<<entries.size()<<" elements";
        if(config.numberEntries > 0 && entries.size() > config.numberEntries){
            entries.erase(entries.begin() + config.numberEntries, entries.end());
        }
    }else{
        qWarning()<<"Error getting clipboard data: "<<reply.error();
        ret->error = true;
        ret->errorString = reply.error().message();
    }

    return ret;
}

void QlipData::setText(const QString &txt){
    const Config& config = Config::instance();
    QlipMonInterface _interface(QLIPMON_DBUS_FQDN, QLIPMON_DBUS_PATH, QDBusConnection::sessionBus(), 0);
    _interface.setText(txt, config.kind);
}
