#include "config.h"

#include <QFileInfo>
#include <QSettings>

extern "C" {
#include <rofi/helper.h>
}

// Static instance storage
QScopedPointer<const Config> Config::configInstance;

// Private constructor
Config::Config(bool duplicates, int kind, int numberEntries,
            QString tabDisplayString, QString newlineDisplayString)
    : duplicates(duplicates)
    , kind(kind)
    , numberEntries(numberEntries)
    , tabDisplayString(std::move(tabDisplayString))
    , newlineDisplayString(std::move(newlineDisplayString))
{
    qDebug() << "Rofi Config constructed with:"
            << "entries=" << numberEntries
            << "duplicates=" << duplicates
            << "kind=" << kind;
}

// Factory method for CLI construction
const Config& Config::createFromCLI() {
    // Load default values from file first
    bool duplicates = false;
    int kind = -1;
    int numberEntries = 0;
    QString tabDisplayString = QStringLiteral("⭾");
    QString newlineDisplayString = QStringLiteral("⏎");

    // Load from settings file
    QSettings* settings = CommonConfig::getSettings("qlipmon", "rofi");
    ensureConfigDirectory(settings->fileName());
    createDefaultConfig(settings);

    numberEntries = loadValue(settings, "entries", numberEntries);
    duplicates = loadValue(settings, "duplicates", duplicates);
    kind = loadValue(settings, "kind", kind);
    tabDisplayString = loadValue(settings, "tab_string", tabDisplayString);
    newlineDisplayString = loadValue(settings, "newline_string", newlineDisplayString);

    // Apply CLI overrides
    int originalNumberEntries = numberEntries;
    bool originalDuplicates = duplicates;
    int originalKind = kind;
    QString originalTabString = tabDisplayString;
    QString originalNewlineString = newlineDisplayString;

    // Helper function to apply integer argument override with logging
    auto applyIntOverride = [&](const char* argName, int& target, int original) {
        int value = 0;
        if (find_arg_int(argName, &value)) {
            target = value;
            if (target != original) {
                qInfo() << "Command line override:" << argName << "=" << target << "(was" << original << ")";
            }
        }
    };

    // Helper function to apply string argument override with logging
    auto applyStringOverride = [&](const char* argName, QString& target, const QString& original) {
        char *value = nullptr;
        if (find_arg_str(argName, &value)) {
            target = QString(value);
            g_free(value);
            if (target != original) {
                qInfo() << "Command line override:" << argName << "='" << target << "'(was'" << original << "')";
            }
        }
    };

    // Helper function to apply boolean argument override using string helper
    auto applyBoolOverride = [&](const char* argName, bool& target, bool original) {
        QString boolStr;
        applyStringOverride(argName, boolStr, original ? "true" : "false");
        if (!boolStr.isEmpty()) {
            target = boolStr.toLower() == "true";
        }
    };

    // Apply all argument overrides
    applyIntOverride("-qlipmon-max-items", numberEntries, originalNumberEntries);
    applyIntOverride("-qlipmon-kind", kind, originalKind);
    applyBoolOverride("-qlipmon-duplicates", duplicates, originalDuplicates);
    applyStringOverride("-qlipmon-tab-string", tabDisplayString, originalTabString);
    applyStringOverride("-qlipmon-newline-string", newlineDisplayString, originalNewlineString);

    // Create and store the immutable config instance using reset
    if (!configInstance) {
        configInstance.reset(
            new Config(duplicates, kind, numberEntries,
                       tabDisplayString, newlineDisplayString)
        );
    }

    qDebug() << "Rofi Config created from CLI arguments:" << *configInstance;
    return *configInstance;
}

QDebug &operator<<(QDebug &out, const Config &c){
    out<<"Config{ number:"<<c.numberEntries<<", duplicates: "<<c.duplicates<<", "
       <<" kind: "<<c.kind<<", tab_string: "<<c.tabDisplayString
       <<", newline_string: "<<c.newlineDisplayString<<"}";
    return out;
}

// Const singleton access
const Config& Config::instance() {
    if (!configInstance) {
        qFatal("Config not initialized! Call createFromCLI() first.");
    }
    return *configInstance;
}

// Save current configuration to file
void Config::save() const {
    QSettings* settings = CommonConfig::getSettings("qlipmon", "rofi");
    qDebug() << "Saving rofi config to" << settings->fileName();

    saveValue(settings, "entries", numberEntries);
    saveValue(settings, "duplicates", duplicates);
    saveValue(settings, "kind", kind);
    saveValue(settings, "tab_string", tabDisplayString);
    saveValue(settings, "newline_string", newlineDisplayString);

    settings->sync();
    qDebug() << "Rofi config saved";
}

