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
#include <QWebElement>
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
    void ajaxStopEventSlot();
    void attachObject();

signals:
    void ajaxStopEventSignal(void);
    
private slots:
    void login_loadFinished(bool ok);
    void loadFinished(bool result);
    void loadFinished2(bool result);

private:
    void parseAccountTables();
    bool parseStatements(QWebFrame *frame);
    void selectYear(const QString &wantedYear);

    QMap<QString, BankAccountInfo> accountMap;
    int year;
    QWebPage* accountPage;
    MyMoneyStatement* s;
    DateInterval dateInterval;
    bool yearSelected;
};

#endif
