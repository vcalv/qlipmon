#include "qlipdata.h"

#include "config.h"


RofiData* QlipData::getEntries(){
    Config config;
    RofiData *ret = new RofiData;

    config.load();
    qDebug()<<"loaded config "<<config;

    QlipMonInterface _interface(QLIPMON_DBUS_FQDN, QLIPMON_DBUS_PATH, QDBusConnection::sessionBus(), 0);
    *ret = _interface.getTextHistory(config.kind, config.duplicates).value();

    qDebug()<<"Got a list with "<<ret->size()<<" elements";
    if(config.numberEntries > 0 && ret->size() > config.numberEntries){
        ret->erase(ret->begin() + config.numberEntries, ret->end());
    }

    return ret;
}

void QlipData::setText(const QString &txt){
    QlipMonInterface _interface(QLIPMON_DBUS_FQDN, QLIPMON_DBUS_PATH, QDBusConnection::sessionBus(), 0);
    Config config;
    config.load();

    _interface.setText(txt, config.kind);
}
