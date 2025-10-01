#ifndef QLIPMON_H
#define QLIPMON_H

#include "database.h"
#include "database_entry.h"
#include "config.h"

#include <QObject>
#include <QApplication>
#include <QClipboard>
#include <QString>
#include <QStringList>

class QlipMon : public QObject, public database
{
    Q_OBJECT
    //Q_CLASSINFO("D-Bus Interface", QLIPMON_DBUS_FQDN)
    Q_PROPERTY(bool broadcast MEMBER _broadcast)

public:
    //explicit QlipMon(QObject *parent = nullptr);
    explicit QlipMon(const Config& config, QObject *parent = nullptr);

public slots:
    void setText(const QString &text, int mode=0);
    QString getLastText(int mode=0);
    QStringList getTextHistory(int mode=-1, bool duplicates=false);
    QList<database_entry> getHistory();
    void clearHistory();
    int getEntryCount();


signals:
    void updated(QString text, int mode);

private:
    QClipboard* clip;
    bool _broadcast;

private slots:
    void changed(QClipboard::Mode mode);

};


#endif // QLIPMON_H
