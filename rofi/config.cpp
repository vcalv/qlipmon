#include "config.h"

#include <QFileInfo>
#include <QSettings>

static QSettings* getSettings(){
    static QSettings* settings = nullptr;
    if (!settings) {
        settings = new QSettings(QSettings::Format::IniFormat, QSettings::Scope::UserScope, "qlipmon", "rofi");
        qDebug() << "Rofi config file path:" << settings->fileName();
    }
    return settings;
}

void Config::load(){
    QSettings* settings = getSettings();
    qDebug()<<"loading settings from file "<<settings->fileName();
    numberEntries = settings->value("entries", numberEntries).toInt();
    duplicates = settings->value("duplicates", duplicates).toBool();
    kind = settings->value("kind", kind).toInt();
    qDebug()<<"loaded settings "<<*this;
}

void Config::save(){
    QSettings* settings = getSettings();
    qDebug()<<"save settings"<< *this <<" to file "<<settings->fileName();
    settings->setValue("entries", numberEntries);
    settings->setValue("duplicates", duplicates);
    settings->setValue("kind", kind);

    qDebug()<<"Saving config to "<<settings->fileName();
    settings->sync();
    qDebug()<<"Saved";
}

Config::~Config(){
    QSettings* settings = getSettings();
    const QString fileName = settings->fileName();
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
