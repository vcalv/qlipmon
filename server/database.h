#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QClipboard>
#include <QList>
#include <QObject>
#include <memory>

#include "../common/database_entry.h"

class database {
  public:
    // Factory method for database initialization - only way to create database
    static database& createFromConfig();

    // Singleton access
    static database& instance();

    // Destructor
    ~database();

    // Delete copy constructor and assignment operator
    database(const database&) = delete;
    database& operator=(const database&) = delete;

    database_entry getLast();
    QList<database_entry> getUniqueEntries();
    QList<database_entry> getDuplicateEntries();
    QString getLast(QClipboard::Mode mode);
    void clearHistory();
    void save(QString text, QClipboard::Mode mode);

  private:
    // Private constructor - only called by factory method
    database(int numberEntries, bool useDiskDatabase, const QString& databasePath);

    // Static instance storage
    static std::unique_ptr<database> databaseInstance;

    int numberEntries;
    void __cleanup();
};

#endif // DATABASE_H
