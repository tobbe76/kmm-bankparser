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
        nextCommand = QString("parseStatements();");
        QString oldest = dateInterval.getOldestDate().toString("yyyy-MM-dd");
        QString newest = dateInterval.getNewestDate().toString("yyyy-MM-dd");

        QString baseUrl = "https://www.banknorwegian.se/MyPage2/Transaction/GetTransactionsFromTo";
        QString account = "?accountNo=" + accountJob.getAccountInfo().getMappedAccount();
        QString o1      = "&getLastDays=false&fromLastEOC=false";
        QString from    = "&dateFrom=" + oldest;
        QString to      = "&dateTo=" + newest;
        QString o2      = "&coreDown=false";
        QUrl url = QUrl(baseUrl + account + o1 + from + to + o2);
        qDebug() << "Load url " << url;
        accountPage->loadPage(url);
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

void NorwegianParser::parseAccountsResponse(const QVariantList &res)
{
    QList<QVariant> list = res;
    for(int i = 0; i < list.size(); i++)
    {
        QMap<QString, QVariant> map = list[i].toMap();
        BankAccountInfo account;
        account.setUrl(QUrl(map["Link"].toString()));
        account.setName(map["Name"].toString());
        account.setNumber(map["Number"].toString());
        account.setKey(map["Number"].toString().replace(" ", ""));
        qDebug() << "Account " << account.getName() << " " << account.getKey();
        accountMap[account.getKey()] = account;
    }

    emit loginFinished(true);
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
        nextCommand = "parseAccounts();";
        accountPage->loadPage(QUrl("https://www.banknorwegian.se/MinSida/Creditcard/Transactions"));

        disconnect(accountPage, SIGNAL(loadFinished(bool)), this, SLOT(login_loadFinished(bool)));
        ld->closeLoginDialog();
    }
    else {
        if(!ld->isDialogOpen()) {
            ld->openLoginDialog(accountPage, QUrl("https://www.banknorwegian.se/Login"));
        }
    }

    qDebug() << "NorwegianParser::isLoginFinished end";
}

//https://www.banknorwegian.se/MyPage2/Transaction/GetTransactionsFromTo?accountNo=10292846001&getLastDays=false&fromLastEOC=false&dateFrom=2018-02-01&dateTo=2019-03-24&coreDown=false

void NorwegianParser::webChannelInitialized()
{
    qDebug() << "NorwegianParser::webChannelInitialized begin";

    if(nextCommand.length() > 0) {
        accountPage->runJavaScript(nextCommand);
    }

    qDebug() << "NorwegianParser::webChannelInitialized end";
}
