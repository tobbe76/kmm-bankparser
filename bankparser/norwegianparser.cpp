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
#include <QWebElement>
#include <QWebFrame>
#include <QWebView>
#include <QDebug>
#include <QTimer>
#include "debugwebpage.h"

NorwegianParser::NorwegianParser()
{
    accountPage = new DebugWebPage(this);
    ld = new BrowserLoginDialog();
    connect(accountPage->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(attachObject()), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
    connect(this, SIGNAL(ajaxStopEventSignal(void)), this, SLOT(ajaxStopEventSlot(void)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
}

void NorwegianParser::getAccountList(QList<BankAccountInfo> &accList)
{
    accList.append(accountMap.values());
}

void NorwegianParser::loginIfNeeded(void)
{
    qDebug() << "NorwegianParser::loginIfNeeded begin";
  
    connect(accountPage, SIGNAL(loadFinished(bool)), this, SLOT(login_loadFinished(bool)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
    disconnect(accountPage, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));

    accountPage->mainFrame()->load(QUrl("https://www.banknorwegian.se/MinSida"));

    qDebug() << "NorwegianParser::loginIfNeeded end";
    return;
}

void NorwegianParser::processAccount(const AccountJob &accountJob)
{
    qDebug() << "NorwegianParser::processAccount begin " << accountJob.getAccountInfo().getMappedAccount();

    yearSelected = false;
    year = 0;

    s = new MyMoneyStatement();
    this->dateInterval = accountJob.getDateInterval();
    if(accountMap.contains(accountJob.getAccountInfo().getMappedAccount())) {
        s->m_accountId = accountJob.getAccountInfo().getId();
        qDebug() << "Load url " << accountMap.value(accountJob.getAccountInfo().getMappedAccount()).getUrl();
        accountPage->mainFrame()->load(accountMap.value(accountJob.getAccountInfo().getMappedAccount()).getUrl());
 //       attachObject();
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

bool NorwegianParser::parseStatements(QWebFrame* frame)
{
    qDebug() << "NorwegianParser::parseStatements begin";

    const QString norwegianTransactionParser = R"(
            var accDiv = document.getElementById("transactions");
            var tables = accDiv.getElementsByTagName("table");
            var accArr = [];
            var bodies = tables[0].tBodies;
            for(var i = 0; i < bodies.length; i++) {
              var trans = bodies[i];
              var date = trans.rows[0].cells[0].innerText;
              var name = trans.rows[0].cells[1].getElementsByTagName("div")[0].innerText;
              var type = trans.rows[0].cells[1].getElementsByTagName("div")[1].innerText;
              var sum  = trans.rows[0].cells[4].innerText;
              accArr.push({Name:name, Date:date, Sum:sum, Extra:type});
            }
            accArr;)";

    QVariant res = frame->evaluateJavaScript(norwegianTransactionParser);

    QList<QVariant> list = res.toList();
    for(int i = 0; i < list.size(); i++)
    {
        QMap<QString, QVariant> map = list[i].toMap();
        QDate date = QDate::fromString(map["Date"].toString(), "yyyy-MM-dd");

        if(dateInterval.isOlderThanInterval(date))
        {
            qDebug() << "NorwegianParser::parseStatements Finished date " << date << "wanted interval " << dateInterval;
            return true;
        }

        if(dateInterval.isWithinInterval(date)) {
            QString var = map["Name"].toString() + map["Date"].toString() + map["Sum"].toString() + map["Extra"].toString();
            QByteArray barr = var.toUtf8();
            quint16 crc1 = qChecksum(barr.data(), barr.length());
            addStatement(s, date, map["Name"].toString(), map["Sum"].toString(), map["Extra"].toString(), crc1);
        }
    }

    qDebug() << "NorwegianParser::parseStatements end";
    return false;
}

void NorwegianParser::ajaxStopEventSlot()
{
    qDebug() << "NorwegianParser::ajaxStopEventSlot begin";
  
    if(yearSelected == false)
    {
        year = 2017;
        selectYear(QString::number(year));
        yearSelected = true;
        return;
    }

    QWebElement statements = accountPage->mainFrame()->findFirstElement("div[id=transactions]");
    if(!statements.isNull())
    {
        if(parseStatements(accountPage->mainFrame())) {
            accountFinished(s);
        }
        else {
            year--;
            selectYear(QString::number(year));
        }
    }
    else
    {
        accountFinished(s);
    }

    qDebug() << "NorwegianParser::ajaxStopEventSlot end";
}

void NorwegianParser::attachObject()
{
    qDebug() << "NorwegianParser::attachObject begin";

    accountPage->mainFrame()->addToJavaScriptWindowObject("NorwegianParser", this);

    const QString ajaxStopEvent = "$( document ).ajaxStop( NorwegianParser.ajaxStopEventSignal ); null;";
    accountPage->mainFrame()->evaluateJavaScript(ajaxStopEvent);

    qDebug() << "NorwegianParser::attachObject done";
}

void NorwegianParser::selectYear(const QString &wantedYear)
{
    qDebug() << "NorwegianParser::selectYear begin " << wantedYear;

    const QString selectYear = R"(document.getElementsByName("YearList")[0].value = "%1";
                                  var event = document.createEvent('Event');
                                  event.initEvent('change', true, true);
                                  document.getElementsByName("YearList")[0].dispatchEvent(event);
                                  null;)";

    accountPage->mainFrame()->evaluateJavaScript(selectYear.arg(wantedYear));

    qDebug() << "NorwegianParser::selectYear end";
}

void NorwegianParser::loadFinished(bool ok)
{
    qDebug() << "NorwegianParser::loadFinished11 begin" << accountPage->mainFrame()->url() << " " << ok;
//    attachObject();
 //   ajaxStopEventSlot();
}

void NorwegianParser::loadFinished2(bool ok)
{
    qDebug() << "NorwegianParser::loadFinished22 begin" << accountPage->mainFrame()->url() << " " << ok;
//    attachObject();
 //   ajaxStopEventSlot();
}

void NorwegianParser::login_loadFinished(bool ok)
{
    qDebug() << "NorwegianParser::isLoginFinished begin" << accountPage->mainFrame()->url() << " " << ok;
  
    if(QUrl("https://www.banknorwegian.se/MinSida") == accountPage->mainFrame()->url())
    {
        qDebug() << "NorwegianParser::isLoginFinished Done";
        parseAccountTables();
        connect(accountPage, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
        connect(accountPage->mainFrame(), SIGNAL(loadFinished(bool)), this, SLOT(loadFinished2(bool)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));

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
