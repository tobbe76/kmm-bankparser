/***************************************************************************
                          norwegianparser.h
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

#ifndef NORWEGIANPARSER_H
#define NORWEGIANPARSER_H

#include <QUrl>
#include <QMap>
#include <QFile>
#include <QTimer>
#include <QTextStream>
#include "kmymoneyplugin.h"
#include "bankparser.h"

class NorwegianParser : public BankParser
{
    Q_OBJECT

public:
    NorwegianParser();
    ~NorwegianParser();
    void processAccount(const AccountJob& accountJob);
    void getAccountList(QList<BankAccountInfo> &accList);
    void loginIfNeeded(void);

public slots:
    void parseAccountsResponse(const QVariantList &res);
    void parseStatementsResponse(const QVariantList &res);
    void webChannelInitialized();
    
private slots:
    void login_loadFinished(bool ok);

private:
    void parseAccountTables();

    QMap<QString, BankAccountInfo> accountMap;
    MyMoneyStatement* s;
    DateInterval dateInterval;
    QString nextCommand;

};

#endif
