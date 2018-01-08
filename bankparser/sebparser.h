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

#ifndef SEBPARSER_H
#define SEBPARSER_H

#include <QUrl>
#include <QMap>
#include <QDate>
#include <QFile>
#include <QTimer>
#include <QTextStream>
#include "bankparser.h"

class SebParser : public BankParser
{
    Q_OBJECT

public:
    SebParser();
    ~SebParser();
    void processAccount(const AccountJob& accountJob);
    void getAccountList(QList<BankAccountInfo> &accList);
    void loginIfNeeded(void);
      
public slots:
    void webChannelInitialized();
    void isLoggedInResponse(const QVariant &res);
    void parseAccountsResponse(const QVariantList &res);
    void parseStatementsResponse(const QVariantList &res);

private:

    QMap<QString, BankAccountInfo> accountMap;
    MyMoneyStatement* s;
    DateInterval dateInterval;
    QString closingBalance;
    QString nextCommand;
};

#endif
