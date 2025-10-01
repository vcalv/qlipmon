#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QClipboard>
#include <QList>
#include <QObject>

#include "database_entry.h"


class database
{
public:

    database(const int numberEntries=500, bool useDiskDatabase=false, const QString& databasePath="qlipmon.db");
    ~database();

    database_entry getLast();
    QList<database_entry> getUniqueEntries();
    QList<database_entry> getDuplicateEntries();
    QString getLast(QClipboard::Mode mode);


protected:
    void _save(QString text, QClipboard::Mode mode);

private:
    int numberEntries;
    void __cleanup();

};

#endif // DATABASE_H
