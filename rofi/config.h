#ifndef CONFIG_H
#define CONFIG_H

#include <QDebug>
#include <QString>
#include <QMutex>
#include <QScopedPointer>
#include "../common/common_config.h"

class Config : public CommonConfig {
  public:
    // Factory method for CLI construction - only way to create config
    static const Config& createFromCLI();

    // Const singleton access
    static const Config& instance();

    // Delete copy constructor and assignment operator
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    // Immutable configuration properties
    const bool duplicates;
    const int kind;          // -1 => all;
    const int numberEntries; // infinite

    // Configurable display strings for common control characters
    const QString tabDisplayString;
    const QString newlineDisplayString;

    // Custom paste command (optional)
    const QString pasteCommand;

    // Custom icon
    const QString icon;

    // void load(const QString& path);
    void save() const;

  private:
    // Private constructor - only called by factory method
    Config(bool duplicates, int kind, int numberEntries, QString tabDisplayString,
           QString newlineDisplayString, QString pasteCommand, QString icon);

    // Static instance storage
    static QScopedPointer<const Config> configInstance;
};

QDebug& operator<<(QDebug& out, const Config& c);

#endif // CONFIG_H
