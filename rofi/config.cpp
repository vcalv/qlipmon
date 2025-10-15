#include "config.h"

#include <QFileInfo>
#include <QSettings>

extern "C" {
#include <rofi/helper.h>
}


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

void Config::applyArgOverrides() {
    // Store original values for logging
    int originalNumberEntries = numberEntries;
    bool originalDuplicates = duplicates;
    int originalKind = kind;
    QString originalTabString = tabDisplayString;
    QString originalNewlineString = newlineDisplayString;

    // Helper function to apply integer argument override with logging
    auto applyIntOverride = [&](const char* argName, int& target, int original) {
        int value = 0;
        if (find_arg_int(argName, &value)) {
            target = value;
            if (target != original) {
                qInfo() << "Command line override:" << argName << "=" << target << "(was" << original << ")";
            }
        }
    };

    // Helper function to apply string argument override with logging
    auto applyStringOverride = [&](const char* argName, QString& target, const QString& original) {
        char *value = nullptr;
        if (find_arg_str(argName, &value)) {
            target = QString(value);
            g_free(value);
            if (target != original) {
                qInfo() << "Command line override:" << argName << "='" << target << "'(was'" << original << "')";
            }
        }
    };

    // Helper function to apply boolean argument override using string helper
    auto applyBoolOverride = [&](const char* argName, bool& target, bool original) {
        QString boolStr;
        applyStringOverride(argName, boolStr, original ? "true" : "false");
        if (!boolStr.isEmpty()) {
            target = boolStr.toLower() == "true";
        }
    };

    // Apply all argument overrides using helper functions
    applyIntOverride("-qlipmon-max-items", numberEntries, originalNumberEntries);
    applyIntOverride("-qlipmon-kind", kind, originalKind);
    applyBoolOverride("-qlipmon-duplicates", duplicates, originalDuplicates);
    applyStringOverride("-qlipmon-tab-string", tabDisplayString, originalTabString);
    applyStringOverride("-qlipmon-newline-string", newlineDisplayString, originalNewlineString);
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
