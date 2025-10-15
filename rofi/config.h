#ifndef CONFIG_H
#define CONFIG_H

#include <QDebug>
#include <QMutex>
#include "../common/common_config.h"

class Config : public CommonConfig {
    public:
      // Singleton access
      static Config& instance();

      // Delete copy constructor and assignment operator
      Config(const Config&) = delete;
      Config& operator=(const Config&) = delete;

      // Configuration properties (public for backward compatibility)
      bool duplicates = false;
      int kind = -1; // -1 => all;
      int numberEntries = 0; // infinite

      // Configurable display strings for common control characters
      QString tabDisplayString = QStringLiteral("⭾");
      QString newlineDisplayString = QStringLiteral("⏎");

      // Immutability control
      void freeze(); // Make configuration immutable after CLI parsing
      bool isFrozen() const; // Check if configuration is frozen
      void ensureNotFrozen(const QString& operation) const; // Throw if frozen

     //void load(const QString& path);
     void load();
     //void save(const QString& path);
     void save();

     /**
      * @brief Apply command line argument overrides to configuration
      */
     void applyArgOverrides();

     ~Config();

   private:
     Config() = default;
     bool frozen = false; // Tracks if configuration is immutable
     static QMutex configMutex; // Protects configuration loading and CLI parsing
 };

QDebug &operator<<(QDebug &out, const Config &c);

#endif // CONFIG_H
