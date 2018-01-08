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

#ifndef TESTPARSER_H
#define TESTPARSER_H

#include <QUrl>
#include <QMap>
#include <QDate>
#include <QFile>
#include <QTimer>
#include <QTextStream>
#include "bankparser.h"

class TestParser : public BankParser
{
    Q_OBJECT

public:
    TestParser();
    ~TestParser();
    void processAccount(const AccountJob& accountJob);
    void getAccountList(QList<BankAccountInfo> &accList);
    void loginIfNeeded(void);

 signals:
    void newPageLoadedSignal(void);
    void statementExpandedSignal(void);

public slots:
    void jsLoginCallback();

private slots:
    void processAccountTimout();

private:

    QMap<QString, BankAccountInfo> accountMap;
    MyMoneyStatement* s;
    QList<int> rowsToExpand;
    bool loggedInOk;
    bool isFirst;
};

#endif
