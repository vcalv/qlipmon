#ifndef CONFIG_H
#define CONFIG_H

#include <QDebug>
#include "../common/common_config.h"

class Config : public CommonConfig {
   public:
     bool duplicates = false;
     int kind = -1; // -1 => all;
     int numberEntries = 0; // infinite

     // Configurable display strings for common control characters
     QString tabDisplayString = QStringLiteral("⭾");
     QString newlineDisplayString = QStringLiteral("⏎");

     //void load(const QString& path);
     void load();
     //void save(const QString& path);
     void save();

     /**
      * @brief Apply command line argument overrides to configuration
      */
     void applyArgOverrides();

     ~Config();
 };

QDebug &operator<<(QDebug &out, const Config &c);

#endif // CONFIG_H
