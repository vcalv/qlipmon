#ifndef DATABASE_ENTRY_H
#define DATABASE_ENTRY_H

#include <QString>
#include <QClipboard>
#include <QList>
#include <QDateTime>
#include <QObject>
class QDBusArgument;

#include <QDebug>

struct database_entry {
    quint64 id;
    QString text;
    QClipboard::Mode mode;
    QDateTime when;
};

QDebug& operator<<(QDebug& out, const database_entry& entry);
const QDBusArgument& operator>>(const QDBusArgument& argument, database_entry& e);

Q_DECLARE_METATYPE(database_entry);
Q_DECLARE_METATYPE(QList<database_entry>);

#endif // DATABASE_ENTRY_H