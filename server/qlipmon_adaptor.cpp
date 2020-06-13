/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -V -c QlipmonAdaptor -i qlipmon_adaptor.h -a :qlipmon_adaptor.cpp ../qlipmon.xml
 *
 * qdbusxml2cpp is Copyright (C) 2020 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include "qlipmon_adaptor.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class QlipmonAdaptor
 */

QlipmonAdaptor::QlipmonAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

QlipmonAdaptor::~QlipmonAdaptor()
{
    // destructor
}

bool QlipmonAdaptor::broadcast() const
{
    // get the value of property broadcast
    return qvariant_cast< bool >(parent()->property("broadcast"));
}

void QlipmonAdaptor::setBroadcast(bool value)
{
    // set the value of property broadcast
    parent()->setProperty("broadcast", QVariant::fromValue(value));
}

QList<database_entry> QlipmonAdaptor::getHistory()
{
    // handle method call DOMAIN.DOMAIN.getHistory
    QList<database_entry> out0;
    QMetaObject::invokeMethod(parent(), "getHistory", Q_RETURN_ARG(QList<database_entry>, out0));
    return out0;
}

QString QlipmonAdaptor::getLastText()
{
    // handle method call DOMAIN.DOMAIN.getLastText
    QString out0;
    QMetaObject::invokeMethod(parent(), "getLastText", Q_RETURN_ARG(QString, out0));
    return out0;
}

QString QlipmonAdaptor::getLastText(int mode)
{
    // handle method call DOMAIN.DOMAIN.getLastText
    QString out0;
    QMetaObject::invokeMethod(parent(), "getLastText", Q_RETURN_ARG(QString, out0), Q_ARG(int, mode));
    return out0;
}

QStringList QlipmonAdaptor::getTextHistory()
{
    // handle method call DOMAIN.DOMAIN.getTextHistory
    QStringList out0;
    QMetaObject::invokeMethod(parent(), "getTextHistory", Q_RETURN_ARG(QStringList, out0));
    return out0;
}

QStringList QlipmonAdaptor::getTextHistory(int mode)
{
    // handle method call DOMAIN.DOMAIN.getTextHistory
    QStringList out0;
    QMetaObject::invokeMethod(parent(), "getTextHistory", Q_RETURN_ARG(QStringList, out0), Q_ARG(int, mode));
    return out0;
}

QStringList QlipmonAdaptor::getTextHistory(int mode, bool duplicates)
{
    // handle method call DOMAIN.DOMAIN.getTextHistory
    QStringList out0;
    QMetaObject::invokeMethod(parent(), "getTextHistory", Q_RETURN_ARG(QStringList, out0), Q_ARG(int, mode), Q_ARG(bool, duplicates));
    return out0;
}

void QlipmonAdaptor::setText(const QString &text)
{
    // handle method call DOMAIN.DOMAIN.setText
    QMetaObject::invokeMethod(parent(), "setText", Q_ARG(QString, text));
}

void QlipmonAdaptor::setText(const QString &text, int mode)
{
    // handle method call DOMAIN.DOMAIN.setText
    QMetaObject::invokeMethod(parent(), "setText", Q_ARG(QString, text), Q_ARG(int, mode));
}
