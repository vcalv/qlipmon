#ifndef QLIPDATA_H
#define QLIPDATA_H

#include <QStringList>

extern "C"{
#include <rofi/mode.h>
}

#include "qlipmon_interface.h"
#include "config.h"

class RofiData{
public:
    QStringList entries;
    bool error;
    QString errorString;
    Config* config;  // Store pointer to configured config to avoid repeated loading
};

class QlipData
{
public:
    static RofiData* getEntries();
    static RofiData* getEntries(Config* config);
    static void setText(const QString &txt);
    static void setText(const QString &txt, Config* config);
};

#endif // QLIPDATA_H
