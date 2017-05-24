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
#include <QWebElement>
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
    bool login(QWebView* view);
    bool isLoggedIn();
    void processAccount(const AccountJob& accountJob);
    void getAccountList(QList<BankAccountInfo> &accList);

 signals:
    void accountFinishedSignal(MyMoneyStatement* s);
    void loginFinished(bool result);
    
public slots:
    void endRequest();
  
private slots:
    void login_loadFinished(bool ok);
    void loadFinished(bool ok);

private:
    void parseAccountTables();
    bool parseStatements(QWebFrame *view);
    
    QWebPage* accountPage;
    QMap<QString, BankAccountInfo> accountMap;
    MyMoneyStatement* s;
    DateInterval dateInterval;
    bool loggedInOk;
};

#endif
