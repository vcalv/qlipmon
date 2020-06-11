#ifndef CONFIG_H
#define CONFIG_H

#include <QDebug>

class Config{
  public:
    int numberEntries = 500;
    bool broadcast = true;
    bool dbus = true;

    //void load(const QString& path);
    void load();
    //void save(const QString& path);
    void save();

    void loadArgs(const QStringList &args);
    void loadArgs(int argc, char* argv[]);
};

QDebug &operator<<(QDebug &out, const Config &c);


#endif // CONFIG_H
