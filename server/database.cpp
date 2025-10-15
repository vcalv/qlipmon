#include "database.h"
#include "config.h"
#include <QSqlDatabase>
#include <QtSql>
#include <QDateTime>
#include <QList>
#include <QClipboard>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>

#include <QDebug>

// Static instance storage
std::unique_ptr<database> database::databaseInstance;

// Static database connection storage to ensure proper cleanup order
static QString staticConnectionName;

static QSqlDatabase __database(){
     return QSqlDatabase::database();
 }

static bool __transaction(){
     QSqlDatabase db = __database();
     if (db.isValid() && db.isOpen()) {
         return db.transaction();
     }
     return false;
 }

static bool __commit(){
     QSqlDatabase db = __database();
     if (db.isValid() && db.isOpen()) {
         return db.commit();
     }
     return false;
 }

static bool __rollback(){
     QSqlDatabase db = __database();
     if (db.isValid() && db.isOpen()) {
         return db.rollback();
     }
     return false;
 }

static database_entry entryFromQuery(QSqlQuery& query){
    database_entry ret;
    ret.id = query.value(0).toULongLong();
    ret.text = query.value(1).toString();
    ret.mode = QClipboard::Mode(query.value(2).toInt());
    ret.when = QDateTime::fromMSecsSinceEpoch(query.value(3).toLongLong());

    return ret;
}

static QList<database_entry> entriesFromSQL(const QString &sql){
    QSqlQuery query;
    query.prepare(sql);

    if(!query.exec()){
        qWarning() << "SQL SELECT ERROR: " << query.lastError().text();
        qWarning() << "Failed query: " << sql;
        return QList<database_entry>();
    }

    QList<database_entry> ret;
    while(query.next()){
        ret.append(entryFromQuery(query));
    }

    return ret;
}

database::~database(){
     qDebug()<<"~database";
     qDebug()<<"Closing database";
     QSqlDatabase db = QSqlDatabase::database();

     if (db.isValid() && db.isOpen()) {
         db.close();

         // Verify database is actually closed
         if(db.isOpen()){
             qCritical() << "Database still open after close attempt!";
             qCritical() << "Database connection name: " << db.connectionName();
         }
     }
 }

 // Private constructor - only called by factory method
 database::database(int numberEntries, bool useDiskDatabase, const QString& databasePath)
     : numberEntries(numberEntries)
 {

    qDebug() << "Available QtSQL drivers:" << QSqlDatabase::drivers();
    const QString DRIVER("QSQLITE");
    QSqlDatabase db = QSqlDatabase::addDatabase(DRIVER);

    if (useDiskDatabase) {
        qDebug() << "Using disk database at path:" << databasePath;

        // Always ensure default data directory exists
        QString defaultDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir dataDir(defaultDataDir);
        if (!dataDir.exists()) {
            if (!dataDir.mkpath(".")) {
                qCritical() << "Failed to create data directory:" << defaultDataDir;
                return;
            }
            qDebug() << "Created data directory:" << defaultDataDir;
        }

        // For non-default paths, check if directory exists
        if (databasePath != defaultDataDir + "/qlipmon.db") {
            QFileInfo fileInfo(databasePath);
            QDir dir = fileInfo.dir();
            if (!dir.exists()) {
                qCritical() << "Database directory does not exist:" << dir.path();
                qCritical() << "Please create the directory or use the default path";
                return;
            }
        }

        db.setDatabaseName(databasePath);
    } else {
        qDebug() << "Using in-memory database";
        db.setDatabaseName(":memory:");
    }

    if(!db.open()){
        qCritical() << "Failed to open database: " << db.lastError().text();
        qCritical() << "Database name: " << databasePath;
        qCritical() << "Driver: " << DRIVER;
        return;
    }

    // Ensure foreign key constraints are enabled
    QSqlQuery enableFK;
    if(!enableFK.exec("PRAGMA foreign_keys = ON")){
        qWarning() << "Failed to enable foreign key constraints: " << enableFK.lastError().text();
    } else {
        qDebug() << "Foreign key constraints enabled";
    }

    // Check if tables already exist
    QSqlQuery checkTables;
    if(!checkTables.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='texts'")){
        qCritical() << "SQL CHECK TABLES ERROR: " << checkTables.lastError().text();
        return;
    }

    bool tablesExist = checkTables.next();

    if (!tablesExist) {
        qDebug() << "Creating database schema...";
        const QStringList DDLs ={
              "CREATE TABLE texts ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                 "text TEXT NOT NULL"
              ");",
             "CREATE TABLE pastes ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                 "text_id INTEGER REFERENCES texts(id),"
                 "mode INTEGER NOT NULL,"
                 "ts INTEGER DEFAULT NULL"
              ");",
             "CREATE UNIQUE INDEX idx_texts_text ON texts(text);"
        };

        for (const QString& DDL: DDLs){
              QSqlQuery query;
              if(!query.exec(DDL)){
                  qCritical() << "SQL CREATE ERROR: " << query.lastError().text();
                  qCritical() << "Failed DDL: " << DDL;
                  return;
              }
          }
        qDebug() << "Database schema created successfully";
    } else {
        qDebug() << "Database schema already exists, skipping creation";
    }

    qDebug() << "Database constructed with:"
             << "entries=" << numberEntries
             << "disk_db=" << useDiskDatabase
             << "db_path=" << databasePath;
}

// Factory method for database initialization using Config
database& database::createFromConfig() {
    const Config& config = Config::instance();

    // Create and store the database instance
    databaseInstance = std::unique_ptr<database>(
        new database(config.numberEntries, config.useDiskDatabase, config.databasePath)
    );

    qDebug() << "Database created from Config";
    return *databaseInstance;
}

// Singleton access
 database& database::instance() {
     // In normal usage, createFromConfig() should be called first
     // If databaseInstance is null, we need to handle this gracefully
     if (!databaseInstance) {
         qCritical() << "Database not initialized! This is a programming error.";
         qCritical() << "Call database::createFromConfig() before using database::instance()";
         // Force a crash in debug mode to catch this error early
         Q_ASSERT(databaseInstance != nullptr);
         // In release mode, provide a safe fallback (though this indicates a bug)
         static database fallbackDatabase(500, false, ":memory:");
         return fallbackDatabase;
     }
     return *databaseInstance;
 }

void database::save(QString text, QClipboard::Mode mode){
    qDebug()<<"save("<<text<<", "<<mode<<")";

    if(0 == text.size()){
        qDebug()<<"Ignore empty paste";
        return;
    }

    qulonglong text_id=0;

    __transaction();

    // Look up existing text_id
    QSqlQuery query_text_id;
    query_text_id.prepare("SELECT id FROM texts WHERE text=?");
    query_text_id.addBindValue(text);

    if(!query_text_id.exec()){
        qWarning() << "SQL SELECT text_id ERROR: " << query_text_id.lastError().text();
        __rollback();
        return;
    }

    if(query_text_id.next()){
        text_id = query_text_id.value(0).toULongLong();
        qDebug()<<"Found previously selected text "<<text_id;
    } else {
        // Insert new text record
        QSqlQuery query_insert_text;
        query_insert_text.prepare("INSERT INTO texts (text) VALUES(?)");
        query_insert_text.addBindValue(text);

        if(!query_insert_text.exec()){
            qWarning() << "SQL INSERT text ERROR: " << query_insert_text.lastError().text();
            __rollback();
            return;
        }

        // Get the inserted text_id using Qt's built-in method (works with SQLite)
        text_id = query_insert_text.lastInsertId().toULongLong();

        if(text_id == 0){
            qWarning() << "ERROR: Invalid text_id returned from INSERT";
            __rollback();
            return;
        }

        qDebug()<<"New text_id="<<text_id;
    }

    if( 0 == text_id ){
        qWarning()<<"ERROR: Problem finding/inserting text";
        __rollback();
        return;
    }

    qint64 now = QDateTime::currentMSecsSinceEpoch();

    // Insert paste record
    QSqlQuery query;
    query.prepare("INSERT INTO pastes (text_id, mode, ts) VALUES (:text_id, :mode, :ts)");
    query.bindValue(":text_id", text_id);
    query.bindValue(":mode", static_cast<int>(mode));
    query.bindValue(":ts", now);

    if(query.exec()){
        __commit();
        __cleanup();
    } else {
        qWarning() << "SQL INSERT paste ERROR: " << query.lastError().text();
        qWarning() << "Failed to insert paste record for text_id=" << text_id << ", mode=" << mode;
        __rollback();
    }
}

database_entry database::getLast(){
    QSqlQuery query;
    query.prepare("SELECT pastes.id as id, text, mode, ts FROM pastes JOIN texts ON (pastes.text_id=texts.id) ORDER BY pastes.id DESC LIMIT 1");

    if(!query.exec()){
        qWarning() << "SQL SELECT getLast ERROR: " << query.lastError().text();
        database_entry ret;
        ret.mode = QClipboard::Mode(-1);
        return ret;
    }

    database_entry ret;
    ret.mode = QClipboard::Mode(-1);

    if(query.first()){
        ret = entryFromQuery(query);
    }

    return ret;
}

QString database::getLast(QClipboard::Mode mode){
    qDebug()<<"getLast("<<mode<<")";

    QSqlQuery query;
    query.prepare("SELECT text FROM pastes JOIN texts ON (pastes.text_id=texts.id) WHERE mode=? ORDER BY pastes.id DESC LIMIT 1");
    query.addBindValue(static_cast<int>(mode));

    if(!query.exec()){
        qWarning() << "SQL SELECT getLast(mode) ERROR: " << query.lastError().text();
        return "";
    }

    if(query.first()){
        return query.value(0).toString();
    } else {
        return "";
    }
}

QList<database_entry> database::getDuplicateEntries(){
    return entriesFromSQL("SELECT pastes.id as id, text, mode, ts FROM pastes JOIN texts ON (pastes.text_id=texts.id) ORDER BY id DESC");
}

QList<database_entry> database::getUniqueEntries(){
    return entriesFromSQL(
        "SELECT pastes.id as id, text, mode, ts FROM pastes "
        "JOIN texts ON (pastes.text_id=texts.id) "
            "WHERE pastes.id IN ("
                "SELECT MAX(id) FROM pastes GROUP BY (text_id)"
            ")"
        "ORDER BY id DESC;"
    );
}


void database::clearHistory(){
    qDebug()<<"Clearing ALL history entries";
    __transaction();

    QSqlQuery query;
    if(query.exec("DELETE FROM pastes")){
        qlonglong deletedCount = query.numRowsAffected();
        __commit();
        qDebug()<<"Cleared all paste entries: "<<deletedCount<<" records removed";
    } else {
        qWarning() << "SQL DELETE ERROR in clearHistory: " << query.lastError().text();
        __rollback();
    }
}

void database::__cleanup(){
    qDebug()<<"Cleaning up for "<<numberEntries<<" entries";
    if( numberEntries < 0 ){
        qDebug()<<"Unlimited entries. Nothing to delete";
        return;
    }

    // Debug: Check how many pastes and texts we have
    QSqlQuery countQuery;
    countQuery.exec("SELECT COUNT(*) FROM pastes");
    int pasteCount = countQuery.next() ? countQuery.value(0).toInt() : 0;

    countQuery.exec("SELECT COUNT(*) FROM texts");
    int textCount = countQuery.next() ? countQuery.value(0).toInt() : 0;

    qDebug()<<"Database state: "<<pasteCount<<" pastes, "<<textCount<<" texts";

    __transaction();

    QSqlQuery query;
    query.prepare(R"(
        DELETE FROM texts
        WHERE id IN (
            SELECT DISTINCT text_id
            FROM pastes
            ORDER BY pastes.id DESC
            LIMIT -1 OFFSET ?
        )
    )");
    query.addBindValue(numberEntries);

    if(query.exec()){
        qlonglong deletedCount = query.numRowsAffected();
        qDebug()<<"Cleanup completed: deleted "<<deletedCount<<" orphaned text records";
        __commit();
    }else{
        qWarning() << "SQL DELETE ERROR in cleanup: " << query.lastError().text();
        qWarning() << "Failed query: " << query.lastQuery();
        __rollback();
    }
}
