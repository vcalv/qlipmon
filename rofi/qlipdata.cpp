#include "qlipdata.h"

#include "config.h"


RofiData* QlipData::getEntries(){
    Config config;
    RofiData *ret = new RofiData;

    config.load();
    qDebug()<<"loaded config "<<config;

    QlipMonInterface _interface(QLIPMON_DBUS_FQDN, QLIPMON_DBUS_PATH, QDBusConnection::sessionBus(), 0);
    QDBusReply reply = _interface.getTextHistory(config.kind, config.duplicates);

    if(reply.isValid()){
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
    QlipMonInterface _interface(QLIPMON_DBUS_FQDN, QLIPMON_DBUS_PATH, QDBusConnection::sessionBus(), 0);
    Config config;
    config.load();

    _interface.setText(txt, config.kind);
}
