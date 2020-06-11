#include "config.h"
#include <QSettings>
#include <QCommandLineParser>

static QSettings getSettings(){
    //QSettings settings(path, QSettings::Format::NativeFormat);
    return QSettings("qlipmon", "server");
}

void Config::loadArgs(const QStringList &args){
        load(); // load values from config

        QCommandLineParser parser;
        parser.setApplicationDescription("Monitors clipboard history");
        parser.addHelpOption();
        parser.addVersionOption();

        QCommandLineOption historyNumberOption(QStringList() << "n" << "limit-history");
        historyNumberOption.setDescription(QString("maximum number of elements saved (0 is no history, negative is infinite, default is ")+QString::number(numberEntries)+").");
        historyNumberOption.setValueName("number of elements");
        historyNumberOption.setDefaultValue(QString::number(numberEntries));
        parser.addOption(historyNumberOption);

        QCommandLineOption dbusOption(QStringList() << "d" << "dbus");
        dbusOption.setDescription("enable dbus");
        dbusOption.setValueName("true/false");
        dbusOption.setDefaultValue("true");
        parser.addOption(dbusOption);

        QCommandLineOption broadcastOption(QStringList() << "b" << "broadcast");
        broadcastOption.setDescription("broadcast new selection via dbus even");
        broadcastOption.setValueName("true/false");
        broadcastOption.setDefaultValue("false");
        parser.addOption(broadcastOption);

        QCommandLineOption saveOption(QStringList() << "s" << "save config");
        saveOption.setDescription("save command line values in config file");
        parser.addOption(saveOption);

        parser.process(args);

        const long _history_number = parser.value(historyNumberOption).toLong();

        if (parser.isSet(broadcastOption))
            broadcast = parser.value(broadcastOption).toLower() == "true";

        if (parser.isSet(dbusOption))
            dbus = parser.value(dbusOption).toLower() == "true";

        numberEntries = _history_number;
        qDebug()<<"Config after command line parsing = "<<*this;

        if (parser.isSet(saveOption))
            save();
}

void Config::loadArgs(int argc, char* argv[]){
    const QStringList args(argv, argv + argc);
    loadArgs(args);
}

void Config::load(){
    QSettings settings = getSettings();
    qDebug()<<"loading settings from file "<<settings.fileName();
    numberEntries = settings.value("entries", numberEntries).toInt();
    broadcast = settings.value("broadcast", broadcast).toBool();
    dbus = settings.value("dbus", dbus).toBool();
    qDebug()<<"loaded settings "<<*this;
}

void Config::save(){
    QSettings settings = getSettings();
    qDebug()<<"save settings"<< *this <<" to file "<<settings.fileName();
    settings.setValue("entries", numberEntries);
    settings.setValue("broadcast", broadcast);
    settings.setValue("dbus", dbus);

    qDebug()<<"Saving config to "<<settings.fileName();
    settings.sync();
    qDebug()<<"Saved";
}

QDebug &operator<<(QDebug &out, const Config &c){
    out<<"Config{ entries:"<<c.numberEntries<<", broadcast: "<<c.broadcast<<", "<<" dbus: "<<c.dbus<<"}";
    return out;
}
