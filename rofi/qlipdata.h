#ifndef QLIPDATA_H
#define QLIPDATA_H

#include <QStringList>

extern "C" {
#include <rofi/mode.h>
}

#include "qlipmon_interface.h"
#include "config.h"

class RofiData {
  public:
    QStringList entries;
    bool error;
    QString errorString;
    // Config is now accessed via singleton pattern - no need to store pointer
};

class QlipData {
  public:
    static RofiData* getEntries();
    static void setText(const QString& txt);
};

#endif // QLIPDATA_H
