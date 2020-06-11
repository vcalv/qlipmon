#include "config.h"

#include <QFileInfo>
#include <QSettings>

static QSettings getSettings(){
    //QSettings settings(path, QSettings::Format::NativeFormat);
    return QSettings("qlipmon", "rofi");
}

void Config::load(){
    QSettings settings = getSettings();
    qDebug()<<"loading settings from file "<<settings.fileName();
    numberEntries = settings.value("entries", numberEntries).toInt();
    duplicates = settings.value("duplicates", duplicates).toBool();
    kind = settings.value("kind", kind).toBool();
    qDebug()<<"loaded settings "<<*this;
}

void Config::save(){
    QSettings settings = getSettings();
    qDebug()<<"save settings"<< *this <<" to file "<<settings.fileName();
    settings.setValue("entries", numberEntries);
    settings.setValue("duplicates", duplicates);
    settings.setValue("kind", kind);

    qDebug()<<"Saving config to "<<settings.fileName();
    settings.sync();
    qDebug()<<"Saved";
}

Config::~Config(){
    QSettings settings = getSettings();
    const QString fileName = settings.fileName();
    QFileInfo finfo(fileName);

    if(finfo.exists() && finfo.isFile()){
        qDebug()<<fileName<<" exists so not saving config";
    }else{
        save();
    }
}

QDebug &operator<<(QDebug &out, const Config &c){
    out<<"Config{ number:"<<c.numberEntries<<", duplicates: "<<c.duplicates<<", "<<" kind: "<<c.kind<<"}";
    return out;
}
