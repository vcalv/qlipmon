#ifndef QLIPDATA_H
#define QLIPDATA_H

#include <QStringList>

extern "C"{
#include <rofi/mode.h>
}

#include "qlipmon_interface.h"

class RofiData{
public:
    QStringList entries;
    bool error;
    QString errorString;
};

class QlipData
{
public:
    static RofiData* getEntries();
    static void setText(const QString &txt);
};

#endif // QLIPDATA_H
