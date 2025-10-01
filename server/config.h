#ifndef CONFIG_H
#define CONFIG_H

#include <QDebug>
#include <QStandardPaths>

class Config{
  public:
    int numberEntries = 500;
    bool broadcast = true;
    bool dbus = true;
    bool useDiskDatabase = false;
    QString databasePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/qlipmon.db";

    //void load(const QString& path);
    void load();
    //void save(const QString& path);
    void save();

    void loadArgs(const QStringList &args);
    void loadArgs(int argc, char* argv[]);
};

QDebug &operator<<(QDebug &out, const Config &c);


#endif // CONFIG_H
