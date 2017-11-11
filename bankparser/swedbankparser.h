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
#include <QWebElement>
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
    bool login(QWebView* view);
    bool isLoggedIn();
    void processAccount(const AccountJob& accountJob);
    void getAccountList(QList<BankAccountInfo> &accList);

 signals:
    void loginFinished(bool result);
    void accountFinishedSignal(MyMoneyStatement* s);
    
public slots:
    void epa();
  
private slots:
    void loadFinished(bool arg1);
    void timeout();
    void login_loadFinished(bool ok);
    void parseAccountTables(bool ok);
    void urlChanged(const QUrl & url);

private:
    void writeQifStatement(QDate date, QString payee, QString sum, QString memo, quint16 verif);
    bool parseStatements(QWebElement s);
    void accountFinished();

    QTimer *timer;
    QWebPage* accountPage;
    QMap<QString, QUrl> accountList;
    QMap<QString, BankAccountInfo> accountMap;
    MyMoneyStatement* s;
    DateInterval dateInterval;
    bool loggedInOk;
};

#endif
