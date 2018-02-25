/***************************************************************************
                          norwegianparser.cpp
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

#include "norwegianparser.h"
#include <QDebug>
#include <QTimer>
#include "debugwebpage.h"

NorwegianParser::NorwegianParser() :
    BankParser(":/files/js/norwegian.js")
{
    ld = new BrowserLoginDialog();
}

void NorwegianParser::getAccountList(QList<BankAccountInfo> &accList)
{
    accList.append(accountMap.values());
}

void NorwegianParser::loginIfNeeded(void)
{
    qDebug() << "NorwegianParser::loginIfNeeded begin";
  
    connect(accountPage, SIGNAL(loadFinished(bool)), this, SLOT(login_loadFinished(bool)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
    nextCommand = "";
    accountPage->loadPage(QUrl("https://www.banknorwegian.se/MinSida"));

    qDebug() << "NorwegianParser::loginIfNeeded end";
    return;
}

void NorwegianParser::processAccount(const AccountJob &accountJob)
{
    qDebug() << "NorwegianParser::processAccount begin " << accountJob.getAccountInfo().getMappedAccount();

    s = new MyMoneyStatement();
    this->dateInterval = accountJob.getDateInterval();
    if(accountMap.contains(accountJob.getAccountInfo().getMappedAccount())) {
        s->m_accountId = accountJob.getAccountInfo().getId();
        nextCommand = QString("parseStatements(%1, %2);").arg(dateInterval.getNewestDate().toString("yyyy"),
                                                              dateInterval.getOldestDate().toString("yyyy"));
        qDebug() << "Load url " << accountMap.value(accountJob.getAccountInfo().getMappedAccount()).getUrl();
        accountPage->loadPage(accountMap.value(accountJob.getAccountInfo().getMappedAccount()).getUrl());
    }
    else {
        accountFinished(s);
    }

    qDebug() << "NorwegianParser::processAccount end";
}


NorwegianParser::~NorwegianParser()
{
    delete accountPage;
}

void NorwegianParser::parseAccountTables()
{
    BankAccountInfo account;
    account.setUrl(QUrl("https://www.banknorwegian.se/MinSida/Creditcard/Transactions"));
    account.setName("Norwegian Card");
    account.setNumber("12345678");
    account.setKey("12345678");
    accountMap[account.getKey()] = account;
}

void NorwegianParser::parseStatementsResponse(const QVariantList &res)
{
    qDebug() << "NorwegianParser::parseStatements begin";

    QList<QVariant> list = res;
    for(int i = 0; i < list.size(); i++)
    {
        QMap<QString, QVariant> map = list[i].toMap();
        QDate date = QDate::fromString(map["Date"].toString(), "yyyy-MM-dd");

        if(dateInterval.isWithinInterval(date)) {
            QString var = map["Name"].toString() + map["Date"].toString() + map["Sum"].toString() + map["Extra"].toString();
            QByteArray barr = var.toUtf8();
            quint16 crc1 = qChecksum(barr.data(), barr.length());
            addStatement(s, date, map["Name"].toString(), map["Sum"].toString(), map["Extra"].toString(), crc1);
        }
    }
    accountFinished(s);

    qDebug() << "NorwegianParser::parseStatements end";
}

void NorwegianParser::login_loadFinished(bool ok)
{
    qDebug() << "NorwegianParser::isLoginFinished begin" << accountPage->getUrl() << " " << ok;
  
    if(QUrl("https://www.banknorwegian.se/MinSida") == accountPage->getUrl())
    {
        qDebug() << "NorwegianParser::isLoginFinished Done";
        parseAccountTables();

        disconnect(accountPage, SIGNAL(loadFinished(bool)), this, SLOT(login_loadFinished(bool)));
        emit loginFinished(true);
        ld->closeLoginDialog();
    }
    else {
        if(!ld->isDialogOpen()) {
            ld->openLoginDialog(accountPage, QUrl("https://www.banknorwegian.se/Login"));
        }
    }

    qDebug() << "NorwegianParser::isLoginFinished end";
}

void NorwegianParser::webChannelInitialized()
{
    qDebug() << "NorwegianParser::webChannelInitialized begin";

    if(nextCommand.length() > 0) {
        accountPage->runJavaScript(nextCommand);
    }

    qDebug() << "NorwegianParser::webChannelInitialized end";
}
