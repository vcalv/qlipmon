#ifndef CONFIG_H
#define CONFIG_H

#include <QDebug>
#include <QMutex>
#include <QStandardPaths>
#include "../common/common_config.h"

class Config : public CommonConfig {
   public:
     // Singleton access
     static Config& instance();

     // Delete copy constructor and assignment operator
     Config(const Config&) = delete;
     Config& operator=(const Config&) = delete;

     // Configuration properties (public for backward compatibility)
     int numberEntries = 500;
     bool broadcast = true;
     bool dbus = true;
     bool useDiskDatabase = false;
     QString databasePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/qlipmon.db";

     // Immutability control
     void freeze(); // Make configuration immutable after CLI parsing
     bool isFrozen() const; // Check if configuration is frozen
     void ensureNotFrozen(const QString& operation) const; // Throw if frozen

    //void load(const QString& path);
    void load();
    //void save(const QString& path);
    void save();

    void loadArgs(const QStringList &args);
    void loadArgs(int argc, char* argv[]);

    ~Config();

  private:
    Config() = default;
    bool frozen = false; // Tracks if configuration is immutable
    static QMutex configMutex; // Protects configuration loading and CLI parsing
};

QDebug &operator<<(QDebug &out, const Config &c);


#endif // CONFIG_H
