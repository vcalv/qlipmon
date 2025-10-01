#include "database.h"
#include <QSqlDatabase>
#include <QtSql>
#include <QDateTime>
#include <QList>
#include <QClipboard>
#include <QMutex>
#include <QMutexLocker>
#include <QAtomicInt>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>

#include <QDebug>

static QMutex __mtx;
static QAtomicInt __count(0);

/*
TODO
limit number of entries or space
deal with duplicate entries better
TTS ?
*/


static QSqlDatabase __database(){
    return QSqlDatabase::database();
}

static bool __transaction(){
    return __database().transaction();
}

static bool __commit(){
    return __database().commit();
}

static bool __rollback(){
    return __database().rollback();
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
    QSqlQuery query(sql);
    query.exec();

    if(!query.isActive())
        qWarning() << "SQL SELECT ERROR: " << query.lastError().text();

    QList<database_entry> ret;

    while(query.next()){
        ret.append(entryFromQuery(query));
    }

    return ret;

}

database::~database(){
    qDebug()<<"~database";
    QMutexLocker locker(&__mtx);

    if(0==--__count){
        qDebug()<<"Closing database";
        QSqlDatabase db = __database();
        db.close();

        if(db.open()){
            qCritical() << db.lastError();
            return;
        }
    }

}

database::database(const int _numberEntries, bool useDiskDatabase, const QString& databasePath){
    numberEntries = _numberEntries;
    QMutexLocker locker(&__mtx);

    if (__count){
        ++__count;
        qDebug()<<"Database already initalized [count = "<<__count<<"]";;
        return;
    }

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
        qCritical() << db.lastError();
        return;
    }

    // Check if tables already exist
    QSqlQuery checkTables("SELECT name FROM sqlite_master WHERE type='table' AND name='texts'");
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
                "text_id INTEGER REFERENCES texts(id) ON DELETE CASCADE,"
                "mode INTEGER NOT NULL,"
                "ts INTEGER DEFAULT NULL"
             ");",
            "CREATE UNIQUE INDEX idx_texts_text ON texts(text);"
        };

        for (const QString& DDL: DDLs){
            QSqlQuery query(DDL);
            if(!query.isActive()){
                qWarning() << "SQL CREATE ERROR: " << query.lastError().text();
                return;
            }
        }
        qDebug() << "Database schema created successfully";
    } else {
        qDebug() << "Database schema already exists, skipping creation";
    }

    ++__count;;

}

void database::_save(QString text, QClipboard::Mode mode){
    qDebug()<<"save("<<text<<", "<<mode<<")";

    if(0 == text.size()){
        qDebug()<<"Ignore empty paste";
        return;
    }

    qulonglong text_id=0;

    __transaction();

    QSqlQuery query_text_id;
    query_text_id.prepare("SELECT id FROM texts WHERE text=?");
    query_text_id.addBindValue(text);
    query_text_id.exec();

    if(!query_text_id.isActive()){
        qWarning() << "SQL SELECT text_id ERROR: " << query_text_id.lastError().text();
        __rollback();
        return;
    }

    if(query_text_id.next()){
        text_id = query_text_id.value(0).toULongLong();
        qDebug()<<"Found previously selected text "<<text_id;
    }else{
        QSqlQuery query_insert_text;
        query_insert_text.prepare("INSERT INTO texts (TEXT) VALUES(?);");
        query_insert_text.addBindValue(text);
        query_insert_text.exec();


        if(!query_insert_text.isActive()){
            qWarning() << "SQL INSERT text ERROR: " << query_insert_text.lastError().text();
            __rollback();
            return;
        }
        query_insert_text.clear();
        query_insert_text.exec("SELECT last_insert_rowid();");
        if(query_insert_text.next()){
            text_id = query_insert_text.value(0).toULongLong();
            qDebug()<<"New text_id="<<text_id;
        }else{
            qWarning()<<"ERROR: Can't find text_id of INSERT";
            __rollback();
            return;
        }
    }

    if( 0 == text_id ){
        qWarning()<<"ERROR: Problem finding/inserting text";
        __rollback();
        return;
    }

    qint64 now = QDateTime::currentMSecsSinceEpoch();

    QSqlQuery query;
    query.prepare("INSERT INTO pastes (text_id, mode, ts) VALUES (:text_id, :mode, :ts)");
    query.bindValue(":text_id", text_id);
    query.bindValue(":mode", int(mode));
    query.bindValue(":ts", now);
    query.exec();

    if(query.isActive()){
        __commit();
        __cleanup();
    }else{
        qWarning() << "SQL INSERT ERROR: " << query.lastError().text();
        __rollback();
    }
}

database_entry database::getLast(){
    QSqlQuery query("SELECT pastes.id as id, text, mode, ts FROM pastes JOIN texts ON (pastes.text_id=texts.id) ORDER BY pastes.id DESC LIMIT 1");
    query.exec();

    database_entry ret;
    ret.mode = QClipboard::Mode(-1);

    if(!query.isActive()){
        qWarning() << "SQL SELECT ERROR: " << query.lastError().text();
        return ret;
    }

    if(query.first()){
        ret = entryFromQuery(query);
    }

    return ret;
}

QString database::getLast(QClipboard::Mode mode){
    qDebug()<<"getLast("<<mode<<")";

    QSqlQuery query("SELECT text FROM pastes JOIN texts ON (pastes.text_id=texts.id) WHERE mode=? ORDER BY pastes.id DESC LIMIT 1");
    query.addBindValue(int(mode));
    query.exec();

    if(!query.isActive())
        qWarning() << "SQL SELECT ERROR: " << query.lastError().text();

    if(query.first()){
        return query.value(0).toString();
    }else{
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


void database::__cleanup(){
    qDebug()<<"Cleaning up for "<<numberEntries<<" entries";
    if( numberEntries < 0 ){
        qDebug()<<"Nothing to delete";
        return;
    }

    __transaction();

    QSqlQuery query(
        "DELETE FROM texts where id IN ("
            "SELECT DISTINCT text_id FROM pastes ORDER BY pastes.id DESC LIMIT ?,-1"
        ")");
    query.addBindValue(numberEntries);
    query.exec();

    if(query.isActive()){
        __commit();
    }else{
        qWarning() << "SQL DELETE ERROR: " << query.lastError().text();
        __rollback();
    }
}
