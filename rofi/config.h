#ifndef CONFIG_H
#define CONFIG_H

#include <QDebug>

class Config{
  public:
    bool duplicates = false;
    int kind = -1; // -1 => all;
    int numberEntries = 0; // infinite

    //void load(const QString& path);
    void load();
    //void save(const QString& path);
    void save();

    ~Config();
};

QDebug &operator<<(QDebug &out, const Config &c);

#endif // CONFIG_H
