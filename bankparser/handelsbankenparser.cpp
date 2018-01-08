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

#include "handelsbankenparser.h"
#include <QDebug>
#include <QTimer>
#include "debugwebpage.h"

HandelsbankenParser::HandelsbankenParser() :
    BankParser(":/files/js/handelsbanken.js")
{
    loggedInOk = false;
    ld = new BrowserLoginDialog();
}

void HandelsbankenParser::loginIfNeeded(void)
{
    qDebug() << "HandelsbankenParser::login begin";

    if(!loggedInOk) {
        nextCommand = "parseAccounts();";
        ld->openLoginDialog(accountPage, QUrl("https://ow.handelsbanken.se/bb/seok/idemstatic/MOVE/MOVE_index_privat.html"));
        connect(accountPage, SIGNAL(loadFinished(bool)), this, SLOT(login_loadFinished(bool)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
    }
    else {
        emit loginFinished(true);
    }

    qDebug() << "HandelsbankenParser::login end";

    return;
}

void HandelsbankenParser::processAccount(const AccountJob &accountJob)
{
    qDebug() << "HandelsbankenParser::processAccount begin";
    s = new MyMoneyStatement();
    this->dateInterval = accountJob.getDateInterval();
    if(accountMap.contains(accountJob.getAccountInfo().getMappedAccount())) {
        s->m_accountId = accountJob.getAccountInfo().getId();
        qDebug() << "Load url " << accountMap.value(accountJob.getAccountInfo().getMappedAccount()).getUrl();
        nextCommand = "parseStatements();";
        accountPage->loadPage(accountMap.value(accountJob.getAccountInfo().getMappedAccount()).getUrl());
    }
    else {
        accountFinished(s);
    }
    qDebug() << "HandelsbankenParser::processAccount end";
}

HandelsbankenParser::~HandelsbankenParser()
{
}

void HandelsbankenParser::parseAccountTables(const QVariant &res)
{
    QList<QVariant> list = res.toList();
    for(int i = 0; i < list.size(); i++)
    {
        QMap<QString, QVariant> map = list[i].toMap();
        BankAccountInfo account;
        account.setUrl(QUrl(map["Link"].toString()));
        account.setName(map["Name"].toString());
        account.setNumber(map["Number"].toString());
        account.setKey(map["Number"].toString().replace(" ", ""));
        qDebug() << "Account " << account.getName() << " " << account.getUrl() << " " << account.getKey();
        accountMap[account.getKey()] = account;
    }

    loggedInOk = true;
    emit loginFinished(true);
    ld->closeLoginDialog();
}

void HandelsbankenParser::parseStatements(const QVariant &res)
{
    QList<QVariant> list = res.toList();
    for(int i = 0; i < list.size(); i++)
    {
        QMap<QString, QVariant> map = list[i].toMap();
        QDate date = QDate::fromString(map["Date"].toString(), "yyyy-MM-dd");

        QString var = map["Name"].toString() + map["Date"].toString() + map["Sum"].toString() + map["Extra"].toString();
        QByteArray barr = var.toUtf8();
        quint16 crc1 = qChecksum(barr.data(), barr.length());
        addStatement(s, date, map["Name"].toString(), map["Sum"].toString(), "memo", crc1);
    }
    accountFinished(s);

    return;
}

void HandelsbankenParser::getAccountList(QList<BankAccountInfo> &accList)
{
    accList.append(accountMap.values());
}

void HandelsbankenParser::login_loadFinished(bool ok)
{
    if(ok)
    {
        qDebug() << "HandelsbankenParser::login_loadFinished start " << accountPage->getUrl();
  
        if(QUrl("https://ow.handelsbanken.se/bb/seok/idemstatic/MOVE/MOVE_index_privat.html") == accountPage->getUrl())
        {
            qDebug() << "HandelsbankenParser::login_loadFinished Done";
            disconnect(accountPage, SIGNAL(loadFinished(bool)), this, SLOT(login_loadFinished(bool)));
        }

        qDebug() << "HandelsbankenParser::login_loadFinished end";
    }
    else
    {
        qDebug() << "Failed to load page";
    }
}

void HandelsbankenParser::webChannelInitialized()
{
    qDebug() << "HandelsbankenParser::webChannelInitialized begin";

    accountPage->runJavaScript(nextCommand);

    qDebug() << "HandelsbankenParser::webChannelInitialized end";
}
