#ifndef COMMON_CONFIG_H
#define COMMON_CONFIG_H

#include <QSettings>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QFile>

/**
 * @brief Base class for configuration management
 *
 * Provides common functionality for loading and saving configuration
 * settings using QSettings with INI format.
 */
class CommonConfig
{
public:
    /**
     * @brief Get or create QSettings instance for this configuration
     * @param organization The organization name for QSettings
     * @param application The application name for QSettings
     * @return QSettings instance
     */
    static QSettings* getSettings(const QString& organization, const QString& application) {
        static QSettings* settings = nullptr;
        if (!settings) {
            settings = new QSettings(QSettings::Format::IniFormat, QSettings::Scope::UserScope, organization, application);
            qDebug() << "Config file path:" << settings->fileName();
        }
        return settings;
    }

    /**
     * @brief Ensure configuration directory exists
     * @param settingsPath Path to the settings file
     */
    static void ensureConfigDirectory(const QString& settingsPath) {
        QFileInfo fileInfo(settingsPath);
        QDir configDir = fileInfo.dir();
        if (!configDir.exists()) {
            configDir.mkpath(".");
            qDebug() << "Created config directory:" << configDir.path();
        }
    }

    /**
     * @brief Create default configuration file if it doesn't exist
     * @param settings QSettings instance
     */
    static void createDefaultConfig(QSettings* settings) {
        if (!QFile::exists(settings->fileName())) {
            qDebug() << "Config file doesn't exist, creating with defaults";
            // save() method will be called by derived classes
        }
    }

    /**
     * @brief Save configuration value with directory creation
     * @param settings QSettings instance
     * @param key Configuration key
     * @param value Configuration value
     */
    template<typename T>
    static void saveValue(QSettings* settings, const QString& key, const T& value) {
        ensureConfigDirectory(settings->fileName());
        settings->setValue(key, value);
    }

    /**
     * @brief Load configuration value with default fallback
     * @param settings QSettings instance
     * @param key Configuration key
     * @param defaultValue Default value if key doesn't exist
     * @return Loaded or default value
     */
    template<typename T>
    static T loadValue(QSettings* settings, const QString& key, const T& defaultValue) {
        return settings->value(key, defaultValue).template value<T>();
    }
};

#endif // COMMON_CONFIG_H