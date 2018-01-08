/***************************************************************************
                          sebparser.h
                             -------------------
    begin                : Sat Jan 01 2016
    copyright            : (C) 2016 by Thorbjorn Larsson
    email                : Thorbjorn Larsson <larsson074@gmail.com>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SWEDBANKPARSER_H
#define SWEDBANKPARSER_H

#include <QUrl>
#include <QMap>
#include <QFile>
#include <QTimer>
#include <QTextStream>
#include "logindialog.h"
#include "kmymoneyplugin.h"
#include "bankparser.h"

class SwedbankParser : public BankParser
{
    Q_OBJECT

public:
    SwedbankParser();
    ~SwedbankParser();
    void processAccount(const AccountJob& accountJob);
    void getAccountList(QList<BankAccountInfo> &accList);
    void loginIfNeeded(void);
    
public slots:
    void parseAccountTables(const QVariant &res);
    void parseStatements(const QVariant &res);

private slots:
    void urlChanged(const QUrl & url);

private:

    QMap<QString, BankAccountInfo> accountMap;
    MyMoneyStatement* s;
    DateInterval dateInterval;
    bool loggedInOk;
};

#endif
