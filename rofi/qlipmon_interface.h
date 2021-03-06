/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -V -i ../server/database_entry.h --classname QlipMonInterface -p qlipmon_interface.h: ../qlipmon.xml
 *
 * qdbusxml2cpp is Copyright (C) 2020 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef QLIPMON_INTERFACE_H
#define QLIPMON_INTERFACE_H

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>
#include "../server/database_entry.h"

/*
 * Proxy class for interface QLIPMON_DBUS_FQDN
 */
class QlipMonInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return QLIPMON_DBUS_FQDN; }

public:
    QlipMonInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = nullptr);

    ~QlipMonInterface();

    Q_PROPERTY(bool broadcast READ broadcast WRITE setBroadcast)
    inline bool broadcast() const
    { return qvariant_cast< bool >(property("broadcast")); }
    inline void setBroadcast(bool value)
    { setProperty("broadcast", QVariant::fromValue(value)); }

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<QList<database_entry> > getHistory()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("getHistory"), argumentList);
    }

    inline QDBusPendingReply<QString> getLastText()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("getLastText"), argumentList);
    }

    inline QDBusPendingReply<QString> getLastText(int mode)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(mode);
        return asyncCallWithArgumentList(QStringLiteral("getLastText"), argumentList);
    }

    inline QDBusPendingReply<QStringList> getTextHistory()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("getTextHistory"), argumentList);
    }

    inline QDBusPendingReply<QStringList> getTextHistory(int mode)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(mode);
        return asyncCallWithArgumentList(QStringLiteral("getTextHistory"), argumentList);
    }

    inline QDBusPendingReply<QStringList> getTextHistory(int mode, bool duplicates)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(mode) << QVariant::fromValue(duplicates);
        return asyncCallWithArgumentList(QStringLiteral("getTextHistory"), argumentList);
    }

    inline QDBusPendingReply<> setText(const QString &text)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(text);
        return asyncCallWithArgumentList(QStringLiteral("setText"), argumentList);
    }

    inline QDBusPendingReply<> setText(const QString &text, int mode)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(text) << QVariant::fromValue(mode);
        return asyncCallWithArgumentList(QStringLiteral("setText"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void updated(const QString &text, int mode);
};

namespace DOMAIN {
  typedef ::QlipMonInterface DOMAIN;
}
#endif
