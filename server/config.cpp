#include "config.h"
#include <QSettings>
#include <QCommandLineParser>
#include <QStandardPaths>
#include <QFileInfo>
#include <QDir>
#include <QFile>

// Static instance storage
QScopedPointer<const Config> Config::configInstance;

// Private constructor
Config::Config(int numberEntries, bool broadcast, bool dbus,
            bool useDiskDatabase, QString databasePath)
    : numberEntries(numberEntries)
    , broadcast(broadcast)
    , dbus(dbus)
    , useDiskDatabase(useDiskDatabase)
    , databasePath(std::move(databasePath))
{
    qDebug() << "Server Config constructed with:"
            << "entries=" << numberEntries
            << "broadcast=" << broadcast
            << "dbus=" << dbus
            << "disk_db=" << useDiskDatabase
            << "db_path=" << databasePath;
}

// Factory method for CLI construction
const Config& Config::createFromCLI(int argc, char* argv[]) {
    const QStringList args(argv, argv + argc);

    // Load default values from file first
    int numberEntries = 500;
    bool broadcast = true;
    bool dbus = true;
    bool useDiskDatabase = false;
    QString databasePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/qlipmon.db";

    // Load from settings file
    QSettings* settings = CommonConfig::getSettings("qlipmon", "server");
    ensureConfigDirectory(settings->fileName());
    createDefaultConfig(settings);

    numberEntries = loadValue(settings, "entries", numberEntries);
    broadcast = loadValue(settings, "broadcast", broadcast);
    dbus = loadValue(settings, "dbus", dbus);
    useDiskDatabase = loadValue(settings, "use_disk_database", useDiskDatabase);
    databasePath = loadValue(settings, "database_path", databasePath);

    // Parse CLI arguments
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
    qDebug()<<"Parsed CLI arguments: entries=" << numberEntries;

    if (parser.isSet(saveOption)) {
        // Save current parsed values to file
        QSettings* settings = CommonConfig::getSettings("qlipmon", "server");
        saveValue(settings, "entries", numberEntries);
        saveValue(settings, "broadcast", broadcast);
        saveValue(settings, "dbus", dbus);
        saveValue(settings, "use_disk_database", useDiskDatabase);
        saveValue(settings, "database_path", databasePath);
        settings->sync();
        qDebug() << "CLI values saved to config file";
    }

    // Create and store the immutable config instance using reset
    if (!configInstance) {
        configInstance.reset(
            new Config(numberEntries, broadcast, dbus, useDiskDatabase, databasePath)
        );
    }

    qDebug() << "Server Config created from CLI arguments:" << *configInstance;
    return *configInstance;
}

QDebug &operator<<(QDebug &out, const Config &c){
    out<<"Config{ entries:"<<c.numberEntries<<", broadcast: "<<c.broadcast<<", "<<" dbus: "<<c.dbus<<", disk_db: "<<c.useDiskDatabase<<", db_path: "<<c.databasePath<<"}";
    return out;
}

// Const singleton access
const Config& Config::instance() {
    if (!configInstance) {
        qFatal("Config not initialized! Call createFromCLI() first.");
    }
    return *configInstance;
}

// Save current configuration to file
void Config::save() const {
    QSettings* settings = CommonConfig::getSettings("qlipmon", "server");
    qDebug() << "Saving server config to" << settings->fileName();

    saveValue(settings, "entries", numberEntries);
    saveValue(settings, "broadcast", broadcast);
    saveValue(settings, "dbus", dbus);
    saveValue(settings, "use_disk_database", useDiskDatabase);
    saveValue(settings, "database_path", databasePath);

    settings->sync();

    // Verify file was created
    if (QFile::exists(settings->fileName())) {
        qDebug()<<"Config file created successfully:" << settings->fileName();
    } else {
        qWarning()<<"Failed to create config file:" << settings->fileName();
    }
}

