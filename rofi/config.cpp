#include "config.h"

#include <QFileInfo>
#include <QSettings>


void Config::load(){
    QSettings* settings = CommonConfig::getSettings("qlipmon", "rofi");

    // Ensure config directory exists and create default config if needed
    ensureConfigDirectory(settings->fileName());
    createDefaultConfig(settings);

    qDebug()<<"loading settings from file "<<settings->fileName();
    numberEntries = loadValue(settings, "entries", numberEntries);
    duplicates = loadValue(settings, "duplicates", duplicates);
    kind = loadValue(settings, "kind", kind);
    tabDisplayString = loadValue(settings, "tab_string", tabDisplayString);
    newlineDisplayString = loadValue(settings, "newline_string", newlineDisplayString);
    qDebug()<<"loaded settings "<<*this;
}

void Config::save(){
    QSettings* settings = CommonConfig::getSettings("qlipmon", "rofi");
    qDebug()<<"save settings"<< *this <<" to file "<<settings->fileName();
    saveValue(settings, "entries", numberEntries);
    saveValue(settings, "duplicates", duplicates);
    saveValue(settings, "kind", kind);
    saveValue(settings, "tab_string", tabDisplayString);
    saveValue(settings, "newline_string", newlineDisplayString);

    qDebug()<<"Saving config to "<<settings->fileName();
    settings->sync();
    qDebug()<<"Saved";
}

Config::~Config(){
    QSettings* settings = CommonConfig::getSettings("qlipmon", "rofi");
    const QString fileName = settings->fileName();
    QFileInfo finfo(fileName);

    if(finfo.exists() && finfo.isFile()){
        qDebug()<<fileName<<" exists so not saving config";
    }else{
        save();
    }
}

QDebug &operator<<(QDebug &out, const Config &c){
    out<<"Config{ number:"<<c.numberEntries<<", duplicates: "<<c.duplicates<<", "
       <<" kind: "<<c.kind<<", tab_string: "<<c.tabDisplayString
       <<", newline_string: "<<c.newlineDisplayString<<"}";
    return out;
}
