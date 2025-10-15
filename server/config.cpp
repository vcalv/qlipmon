#include "config.h"
#include <QSettings>
#include <QCommandLineParser>
#include <QStandardPaths>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <stdexcept>

// Static singleton instance and mutex
Config& Config::instance() {
    static Config instance;
    return instance;
}

QMutex Config::configMutex;

Config::~Config() {
    qDebug() << "Server Config singleton destroyed";
}


void Config::loadArgs(const QStringList &args){
        ensureNotFrozen("load CLI arguments");
        QMutexLocker locker(&configMutex);
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

        QCommandLineOption diskDatabaseOption(QStringList() << "disk-database");
        diskDatabaseOption.setDescription("use disk database instead of memory");
        diskDatabaseOption.setValueName("true/false");
        diskDatabaseOption.setDefaultValue("false");
        parser.addOption(diskDatabaseOption);

        QCommandLineOption databasePathOption(QStringList() << "database-path");
        databasePathOption.setDescription("database file path (when using disk database)");
        databasePathOption.setValueName("path");
        databasePathOption.setDefaultValue(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/qlipmon.db");
        parser.addOption(databasePathOption);

        QCommandLineOption saveOption(QStringList() << "s" << "save config");
        saveOption.setDescription("save command line values in config file");
        parser.addOption(saveOption);

        parser.process(args);

        const long _history_number = parser.value(historyNumberOption).toLong();

        if (parser.isSet(broadcastOption))
            broadcast = parser.value(broadcastOption).toLower() == "true";

        if (parser.isSet(dbusOption))
            dbus = parser.value(dbusOption).toLower() == "true";

        if (parser.isSet(diskDatabaseOption))
            useDiskDatabase = parser.value(diskDatabaseOption).toLower() == "true";

        if (parser.isSet(databasePathOption))
            databasePath = parser.value(databasePathOption);

        numberEntries = _history_number;
        qDebug()<<"Config after command line parsing = "<<*this;

        if (parser.isSet(saveOption))
            save();

        // Configuration is now complete and immutable
        freeze();
}

void Config::loadArgs(int argc, char* argv[]){
    const QStringList args(argv, argv + argc);
    loadArgs(args);
}

void Config::load(){
    ensureNotFrozen("load configuration from file");
    QMutexLocker locker(&configMutex);

    QSettings* settings = CommonConfig::getSettings("qlipmon", "server");

    // Ensure config directory exists and create default config if needed
    ensureConfigDirectory(settings->fileName());
    createDefaultConfig(settings);

    qDebug()<<"loading settings from file "<<settings->fileName();

    numberEntries = loadValue(settings, "entries", numberEntries);
    broadcast = loadValue(settings, "broadcast", broadcast);
    dbus = loadValue(settings, "dbus", dbus);
    useDiskDatabase = loadValue(settings, "use_disk_database", useDiskDatabase);
    databasePath = loadValue(settings, "database_path", databasePath);
    qDebug()<<"loaded settings "<<*this;
}

void Config::save(){
    QSettings* settings = CommonConfig::getSettings("qlipmon", "server");
    qDebug()<<"save settings"<< *this <<" to file "<<settings->fileName();

    saveValue(settings, "entries", numberEntries);
    saveValue(settings, "broadcast", broadcast);
    saveValue(settings, "dbus", dbus);
    saveValue(settings, "use_disk_database", useDiskDatabase);
    saveValue(settings, "database_path", databasePath);

    qDebug()<<"Saving config to "<<settings->fileName();
    settings->sync();

    // Verify file was created
    if (QFile::exists(settings->fileName())) {
        qDebug()<<"Config file created successfully:" << settings->fileName();
    } else {
        qWarning()<<"Failed to create config file:" << settings->fileName();
    }
}

QDebug &operator<<(QDebug &out, const Config &c){
    out<<"Config{ entries:"<<c.numberEntries<<", broadcast: "<<c.broadcast<<", "<<" dbus: "<<c.dbus<<", disk_db: "<<c.useDiskDatabase<<", db_path: "<<c.databasePath<<"}";
    return out;
}

void Config::freeze() {
    if (!frozen) {
        frozen = true;
        qDebug() << "Configuration frozen - now immutable";
    }
}

bool Config::isFrozen() const {
    return frozen;
}

void Config::ensureNotFrozen(const QString& operation) const {
    if (frozen) {
        qCritical() << "Configuration is frozen - cannot " << operation;
        throw std::runtime_error(qPrintable("Configuration is immutable after CLI parsing. Cannot " + operation));
    }
}
