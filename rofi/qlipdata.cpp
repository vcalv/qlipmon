#include "qlipdata.h"

#include "config.h"
#include <QtDBus/QtDBus>


RofiData* QlipData::getEntries(){
    RofiData *ret = new RofiData;
    qDebug()<<"using singleton config";

    QlipMonInterface _interface(QLIPMON_DBUS_FQDN, QLIPMON_DBUS_PATH, QDBusConnection::sessionBus(), 0);
    QDBusReply reply = _interface.getTextHistory(Config::instance().kind, Config::instance().duplicates);

    if(reply.isValid()){
        ret->error = false;
        ret->errorString = "";
        ret->entries = reply.value();
        auto &entries = ret->entries;

        qDebug()<<"Got a list with "<<entries.size()<<" elements";
        if(Config::instance().numberEntries > 0 && entries.size() > Config::instance().numberEntries){
            entries.erase(entries.begin() + Config::instance().numberEntries, entries.end());
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
    _interface.setText(txt, Config::instance().kind);
}
