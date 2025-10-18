#ifndef CONFIG_H
#define CONFIG_H

#include <QDebug>
#include <QMutex>
#include <QScopedPointer>
#include "../common/common_config.h"

class Config : public CommonConfig {
  public:
    // Factory method for CLI construction - only way to create config
    static const Config& createFromCLI(int argc, char* argv[]);

    // Const singleton access
    static const Config& instance();

    // Delete copy constructor and assignment operator
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    // Immutable configuration properties
    const int numberEntries;
    const bool broadcast;
    const bool dbus;
    const bool useDiskDatabase;
    const QString databasePath;

    // void load(const QString& path);
    void save() const;

  private:
    // Private constructor - only called by factory method
    Config(int numberEntries, bool broadcast, bool dbus, bool useDiskDatabase,
           QString databasePath);

    // Static instance storage
    static QScopedPointer<const Config> configInstance;
};

QDebug& operator<<(QDebug& out, const Config& c);

#endif // CONFIG_H
