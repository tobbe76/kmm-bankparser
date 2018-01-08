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

#include "sebparser.h"
#include <QDebug>
#include <QTimer>
#include "debugwebpage.h"

SebParser::SebParser() :
    BankParser(":/files/js/seb.js")
{
    ld = new BrowserLoginDialog();
}

void SebParser::loginIfNeeded(void)
{
    qDebug() << "SebParser::loginIfNeeded start";

    nextCommand = "isLoggedIn();";
    accountPage->loadPage(QUrl("https://privat.ib.seb.se/wow/1000/1100/wown1100.aspx"));

    qDebug() << "SebParser::loginIfNeeded end";
}

void SebParser::processAccount(const AccountJob &accountJob)
{
    s = new MyMoneyStatement();
    this->dateInterval = accountJob.getDateInterval();

    if(accountMap.contains(accountJob.getAccountInfo().getMappedAccount())) {
        s->m_accountId = accountJob.getAccountInfo().getId();
        nextCommand = QString("parseStatements(\"%1\", \"%2\");").arg(dateInterval.getNewestDate().toString("yyyy-MM-dd"),
                                                                      dateInterval.getOldestDate().toString("yyyy-MM-dd"));
        qDebug() << "Load url " << accountMap.value(accountJob.getAccountInfo().getMappedAccount()).getUrl();
        accountPage->loadPage(accountMap.value(accountJob.getAccountInfo().getMappedAccount()).getUrl());
    }
    else {
        accountFinished(s);
    }
}

SebParser::~SebParser()
{
}

void SebParser::parseAccountsResponse(const QVariantList &res)
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

void SebParser::parseStatementsResponse(const QVariantList &res)
{
    qDebug() << "SebParser::parseStatementsResponse begin";

    QList<QVariant> list = res;

    bool isFirst = true;
    for(int i = 0; i < list.size(); i++)
    {
        QMap<QString, QVariant> map = list[i].toMap();
        QDate date = QDate::fromString(map["Date"].toString(), "yyyy-MM-dd");

        if(dateInterval.isWithinInterval(date)) {
            if(isFirst) {
                closingBalance = map["Balance"].toString();
                isFirst = false;
            }
            QString var = map["Name"].toString() + map["Date"].toString() + map["Sum"].toString() + map["Extra"].toString();
            QByteArray barr = var.toUtf8();
            quint16 crc1 = qChecksum(barr.data(), barr.length());
            addStatement(s, date, map["Name"].toString(), map["Sum"].toString(), map["Extra"].toString(), crc1);
        }
    }

    if(!s->m_listTransactions.isEmpty()) {
        s->m_dateBegin = s->m_listTransactions.last().m_datePosted;
        s->m_dateEnd = s->m_listTransactions.first().m_datePosted;
        s->m_closingBalance = MyMoneyMoney(closingBalance);
    }
    accountFinished(s);

    qDebug() << "SebParser::parseStatementsResponse end";
}

void SebParser::getAccountList(QList<BankAccountInfo> &accList)
{
    accList.append(accountMap.values());
}

void SebParser::isLoggedInResponse(const QVariant &res) {

    qDebug() << "SebParser::isLoggedInResponse start " << accountPage->getUrl();

    qDebug() << "type " << res.typeName();
    if(res.toBool()) {
        qDebug() << "SebParser::isLoggedInResponse Logged in ok";
        accountPage->runJavaScript("parseAccounts();");
        ld->closeLoginDialog();
    }
    else {
        if(!ld->isDialogOpen()) {
            ld->openLoginDialog(accountPage, QUrl("https://privat.ib.seb.se/wow/1000/1000/wow1020.aspx"));
        }
    }
    qDebug() << "SebParser::isLoggedInResponse end";

}

void SebParser::webChannelInitialized()
{
    qDebug() << "SebParser::webChannelInitialized start " << accountPage->getUrl();

    accountPage->runJavaScript(nextCommand);

    qDebug() << "SebParser::webChannelInitialized end";
}
