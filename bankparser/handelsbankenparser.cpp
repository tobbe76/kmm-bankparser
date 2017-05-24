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
#include <QWebElement>
#include <QWebFrame>
#include <QWebView>
#include <QDebug>
#include <QTimer>
#include "debugwebpage.h"

HandelsbankenParser::HandelsbankenParser() : BankParser()
{
    loggedInOk = false;
    accountPage = new DebugWebPage(this);
}

bool HandelsbankenParser::login(QWebView* view)
{
    qDebug() << "HandelsbankenParser::login begin";

    view->setPage(accountPage);

    connect(accountPage, SIGNAL(loadFinished(bool)), this, SLOT(login_loadFinished(bool)), Qt::QueuedConnection);
    view->load(QUrl("https://ow.handelsbanken.se/bb/seok/idemstatic/MOVE/MOVE_index_privat.html"));

    qDebug() << "HandelsbankenParser::login end";

    return true;
}

bool HandelsbankenParser::isLoggedIn()
{
    return loggedInOk;
}

void HandelsbankenParser::processAccount(const AccountJob &accountJob)
{
    qDebug() << "HandelsbankenParser::processAccount begin";
    loginIfNeeded();
    s = new MyMoneyStatement();
    this->dateInterval = accountJob.getDateInterval();
    if(accountMap.contains(accountJob.getAccountInfo().getMappedAccount())) {
        s->m_accountId = accountJob.getAccountInfo().getId();
        qDebug() << "Load url " << accountMap.value(accountJob.getAccountInfo().getMappedAccount()).getUrl();
        accountPage->mainFrame()->load(accountMap.value(accountJob.getAccountInfo().getMappedAccount()).getUrl());
    }
    else {
        accountFinished(s);
    }
    qDebug() << "HandelsbankenParser::processAccount end";
}

HandelsbankenParser::~HandelsbankenParser()
{
    delete accountPage;
}

void HandelsbankenParser::parseAccountTables()
{
    const QString accountListParser = R"(
            var accDiv = document.getElementById("account_cards_m");
            var tables = accDiv.getElementsByTagName("table");
            var rows = tables[0].tBodies[0].rows;
            var accArr = [];
            for(var i = 0; i < rows.length; i++) {
                var href = rows[i].cells[0].getElementsByTagName("a")[0].href;
                var number = rows[i].cells[0].getElementsByTagName("span")[0].innerHTML.split('-')[1].trim();
                accArr.push({Link:href, Name:rows[i].cells[0].innerText.trim(), Number:number});
            }
            accArr;)";

    QVariant res = accountPage->mainFrame()->evaluateJavaScript(accountListParser);

    QList<QVariant> list = res.toList();
    for(int i = 0; i < list.size(); i++)
    {
        QMap<QString, QVariant> map = list[i].toMap();
        BankAccountInfo account;
        account.setUrl(QUrl(map["Link"].toString()));
        account.setName(map["Name"].toString());
        account.setNumber(map["Number"].toString());
        account.setKey(map["Number"].toString().replace(" ", ""));
        accountMap[account.getKey()] = account;
    }
}

bool HandelsbankenParser::parseStatements(QWebFrame* view)
{
    QString transactionParser = R"(
            var accDiv = document.getElementsByTagName("form");
            var transArr = [];
            for(var j = 0; j < accDiv.length; j++) {
                if("accounttransactions" == accDiv[j].getAttribute("name")) {
                    var tables = accDiv[j].getElementsByTagName("table");
                    var rows = tables[12].tBodies[0].rows;
                    for(var i = 2; i < rows.length; i++) {
                        var trans = rows[i];
                        var date = trans.cells[2].innerText;
                        var name = trans.cells[3].innerText;
                        var sum  = trans.cells[4].innerText;
                        var extra = "Empty";
                        transArr.push({Name:name, Date:date, Sum:sum, Extra:extra});
                    }
                }
            }
            transArr;)";


    QVariant res = view->evaluateJavaScript(transactionParser);

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
    return true;
}

void HandelsbankenParser::loadFinished(bool ok)
{
    if(ok)
    {
        qDebug() << "LOAD Finished mainwindow" << accountPage->mainFrame()->url();

        if(accountPage->currentFrame() == accountPage->mainFrame()->childFrames()[0])
        {
            if(parseStatements(accountPage->mainFrame()->childFrames()[0])) {
                accountFinished(s);
            }
        }
    }
    else
    {
        qDebug() << "Failed to load page";
    }
}

void HandelsbankenParser::getAccountList(QList<BankAccountInfo> &accList)
{
    loginIfNeeded();
    accList.append(accountMap.values());
}

void HandelsbankenParser::login_loadFinished(bool ok)
{
    if(ok)
    {
        qDebug() << "HandelsbankenParser::login_loadFinished start " << accountPage->mainFrame()->url();
  
        if(QUrl("https://ow.handelsbanken.se/bb/seok/idemstatic/MOVE/MOVE_index_privat.html") == accountPage->mainFrame()->url())
        {
            qDebug() << "HandelsbankenParser::login_loadFinished Done";
            parseAccountTables();
            connect(accountPage, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)), Qt::QueuedConnection);
            disconnect(accountPage, SIGNAL(loadFinished(bool)), this, SLOT(login_loadFinished(bool)));
            loggedInOk = true;
            emit loginFinished(true);
        }

        qDebug() << "HandelsbankenParser::login_loadFinished end";
    }
    else
    {
        qDebug() << "Failed to load page";
    }
}
