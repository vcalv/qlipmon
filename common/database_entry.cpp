#include "database_entry.h"
#include <QtDBus>
#include <QDebug>

/// Initializer
class Init {
  public:
    Init() {
        qDebug() << "Registering types";
        qRegisterMetaType<database_entry>("database_entry");
        qDBusRegisterMetaType<database_entry>();

        qRegisterMetaType<QList<database_entry>>("list_database_entry");
        qDBusRegisterMetaType<QList<database_entry>>();
        qDebug() << "Types registered";
    }
};
static Init _initTypes;

QDBusArgument& operator<<(QDBusArgument& argument, const database_entry& e) {
    argument.beginStructure();
    argument << e.id;
    argument << e.text;
    argument << e.mode;
    argument << e.when.toMSecsSinceEpoch();
    argument.endStructure();
    return argument;
}

const QDBusArgument& operator>>(const QDBusArgument& argument, database_entry& e) {
    qint64 when;
    int mode;

    argument.beginStructure();
    argument >> e.id;
    argument >> e.text;
    argument >> mode;
    argument >> when;
    argument.endStructure();

    e.mode = QClipboard::Mode(mode);
    e.when = QDateTime::fromMSecsSinceEpoch(when);
    return argument;
}

QDebug& operator<<(QDebug& out, const database_entry& e) {
    out << "database::entry{ id:" << e.id << ", text: " << e.text << ", " << " mode: " << e.mode
        << ", when: " << e.when << "}";
    return out;
}