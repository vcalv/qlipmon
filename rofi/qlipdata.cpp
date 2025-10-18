#include "qlipdata.h"

#include "config.h"
#include "qlogging.h"
#include <QtDBus/QtDBus>
#include <QProcess>
#include <QProcessEnvironment>
#include <QDebug>
#include <QDir>

RofiData* QlipData::getEntries() {
    RofiData* ret = new RofiData;
    qDebug() << "using singleton config";

    // Initialize selection tracking
    ret->entrySelected = false;
    ret->selectedEntryText = QString();
    ret->selectedEntryKind = -1;

    const Config& config = Config::instance();
    QlipMonInterface _interface(QLIPMON_DBUS_FQDN, QLIPMON_DBUS_PATH, QDBusConnection::sessionBus(),
                                0);
    QDBusReply reply = _interface.getTextHistory(config.kind, config.duplicates);

    if (reply.isValid()) {
        ret->error = false;
        ret->errorString = "";
        ret->entries = reply.value();
        auto& entries = ret->entries;

        qDebug() << "Got a list with " << entries.size() << " elements";
        if (config.numberEntries > 0 && entries.size() > config.numberEntries) {
            entries.erase(entries.begin() + config.numberEntries, entries.end());
        }
    } else {
        qWarning() << "Error getting clipboard data: " << reply.error();
        ret->error = true;
        ret->errorString = reply.error().message();
    }

    return ret;
}

/**
 * @brief Executes the configured paste command with selected text and kind (fire-and-forget)
 *
 * Uses environment variables instead of string interpolation:
 * - QLIPMON_TEXT = selected text
 * - QLIPMON_KIND = kind (clipboard type as string)
 *
 * The user can reference these in their paste command like: echo $QLIPMON_TEXT
 */
void RofiData::executePasteCommand() {
    if (!entrySelected || selectedEntryText.isEmpty()) {
        qDebug() << "No selected entry. No command to run";
        return;
    }

    const Config& config = Config::instance();
    if (config.pasteCommand.isEmpty()) {
        qDebug() << "No paste command to run";
        return;
    }

    // Set up environment variables for the paste command
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("QLIPMON_TEXT", selectedEntryText);
    env.insert("QLIPMON_KIND", QString::number(selectedEntryKind));

    // Execute paste command with environment variables (fire-and-forget)
    qDebug() << "Running paste command" << config.pasteCommand;
    QProcess process;
    process.setProcessEnvironment(env);
    process.setWorkingDirectory(QDir::currentPath());
    process.setProgram("/bin/bash");
    process.setArguments(QStringList{"-c", config.pasteCommand});
    process.startDetached();
}

RofiData::~RofiData() {
    if (entrySelected && !selectedEntryText.isEmpty()) {
        qDebug() << "Plugin session summary: User selected clipboard entry of kind"
                 << selectedEntryKind << "with text:" << selectedEntryText;
    } else {
        qDebug() << "Plugin session summary: No entry was selected by user";
    }
}

void QlipData::setText(const QString& txt) {
    const Config& config = Config::instance();
    QlipMonInterface _interface(QLIPMON_DBUS_FQDN, QLIPMON_DBUS_PATH, QDBusConnection::sessionBus(),
                                0);
    _interface.setText(txt, config.kind);
}
