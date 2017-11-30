/***************************************************************************
                          sebparser.cpp
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

#include "testparser.h"
#include <QWebElement>
#include <QWebFrame>
#include <QWebView>
#include <QDebug>
#include <QTimer>
#include "debugwebpage.h"

TestParser::TestParser()
{
    loggedInOk = false;
    accountPage = new DebugWebPage(this);
    ld = new BrowserLoginDialog();
    connect(this, SIGNAL(jsLoginCallbackSignal(void)), this, SLOT(jsLoginCallbackSlot(void)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));

}

void TestParser::loginIfNeeded(void)
{
    qDebug() << "TestParser::login start";

    if(!loggedInOk) {

        ld->openLoginDialog(accountPage, QUrl("qrc:///html/testLogin.html"));
        connect(accountPage, SIGNAL(loadFinished(bool)), this, SLOT(login_loadFinished(bool)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
    }
    else {
        emit loginFinished(true);
    }

    qDebug() << "TestParser::login end";

    return;
}

TestParser::~TestParser()
{
   delete accountPage;
}

void TestParser::processAccountTimout()
{
    qDebug() << "TestParser::processAccountTimout begin";
    addStatement(s, QDate::currentDate(), "ICA", "200", "EXTRA", 123);
    addStatement(s, QDate::currentDate(), "STATOIL", "250", "EXTRA", 124);
    addStatement(s, QDate::currentDate(), "coop", "244", "EXTRA", 125);
    addStatement(s, QDate::currentDate(), "ikea", "204", "EXTRA", 126);

    accountFinished(s);
    qDebug() << "TestParser::processAccountTimout end";
}

void TestParser::processAccount(const AccountJob &accountJob)
{
    qDebug() << "TestParser::processAccount begin";
    s = new MyMoneyStatement();
    QTimer::singleShot(3000, this, SLOT(processAccountTimout()));
    qDebug() << "TestParser::processAccount end";
}

void TestParser::getAccountList(QList<BankAccountInfo> &accList)
{
    qDebug() << "TestParser::getAccountList begin";
    accList.append(BankAccountInfo("TestKonto1", "500.1111", "500.1111"));
    accList.append(BankAccountInfo("TestKonto2", "500.2222", "500.2222"));
    accList.append(BankAccountInfo("TestKonto3", "500.3333", "500.3333"));
    accList.append(BankAccountInfo("TestKonto4", "500.4444", "500.4444"));
    qDebug() << "TestParser::getAccountList end";
}

void TestParser::login_loadFinished(bool ok)
{
    qDebug() << "TestParser::login_loadFinished begin";

    accountPage->mainFrame()->addToJavaScriptWindowObject("TestParser", this);

    qDebug() << "TestParser::login_loadFinished end";
}

void TestParser::jsLoginCallbackSlot()
{
    qDebug() << "TestParser::jsLoginCallback begin";
    loggedInOk = true;
    emit loginFinished(true);
    ld->closeLoginDialog();
    disconnect(accountPage, SIGNAL(loadFinished(bool)), this, SLOT(login_loadFinished(bool)));

    qDebug() << "TestParser::jsLoginCallback end";
}
