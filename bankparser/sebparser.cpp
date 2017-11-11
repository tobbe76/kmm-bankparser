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
#include <QWebElement>
#include <QWebFrame>
#include <QWebView>
#include <QDebug>
#include <QTimer>
#include "debugwebpage.h"

SebParser::SebParser()
{
    loggedInOk = false;
    accountPage = new DebugWebPage(this);
}

bool SebParser::login(QWebView* view)
{
    qDebug() << "SebParser::login start";
  
    view->setPage(accountPage);

    connect(accountPage, SIGNAL(loadFinished(bool)), this, SLOT(login_loadFinished(bool)), Qt::QueuedConnection);
    view->load(QUrl("https://privat.ib.seb.se/wow/1000/1000/wow1020.aspx"));

    qDebug() << "SebParser::login end";

    connect(this, SIGNAL(statementExpandedSignal(void)), this, SLOT(statementExpanded(void)), Qt::QueuedConnection);
    connect(this, SIGNAL(newPageLoadedSignal(void)), this, SLOT(newPageLoaded(void)), Qt::QueuedConnection);

    return true;
}

bool SebParser::isLoggedIn()
{
    return loggedInOk;
}

void SebParser::processAccount(const AccountJob &accountJob)
{
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
}

SebParser::~SebParser()
{
   delete accountPage;
}

void SebParser::parseAccountTables()
{
    const QString accountListParser = R"(
            var accDiv = document.getElementById("IKPMaster_MainPlaceHolder_ucAccountTable_pnlAccounts");
            var tables = accDiv.getElementsByTagName("table");
            var accArr = [];
            for(var i = 0; i < tables.length; i++) {
                var tab = tables[i].tBodies[0];
                for(var j = 0; j < tab.rows.length; j++) {
                    var href = tab.rows[j].cells[0].getElementsByTagName("a")[0].href;
                    accArr.push({Link:href, Name:tab.rows[j].cells[0].innerText.trim(), Number:tab.rows[j].cells[1].innerText.trim()});
                }
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

void SebParser::expandStatement(int rowNr)
{
    qDebug() << "expandStatement START";

    accountPage->mainFrame()->addToJavaScriptWindowObject("SebParser", this);

    QString sebExpandTransaction = R"(
            var accDiv = document.getElementById("IKPMaster_MainPlaceHolder_ucAccountEvents_DataTable");
            var tables = accDiv.getElementsByTagName("table");
            var rows = tables[0].tBodies[0].rows;
            Sys.WebForms.PageRequestManager.getInstance().add_endRequest( SebParser.expandStatementEndRequest );
            var event = document.createEvent('Event');
            event.initEvent('click', true, true);
            rows[%1].dispatchEvent(event);
            null;)";

    QVariant res = accountPage->mainFrame()->evaluateJavaScript(sebExpandTransaction.arg(rowNr));

    qDebug() << "expandStatement DONE";
}

void SebParser::findStatementsToExpand(QWebFrame* view)
{
    qDebug() << "findStatementsToExpand START";

    QString sebExpandTransaction = R"(
            var accDiv = document.getElementById("IKPMaster_MainPlaceHolder_ucAccountEvents_DataTable");
            var tables = accDiv.getElementsByTagName("table");
            var transArr = [];
            var rows = tables[0].tBodies[0].rows;
            for(var i = 2; i < rows.length; i = i +2) {
                if(rows[i+1].cells[1].getElementsByClassName("key-value-list").length == 0) {
                    transArr.push(i);
                }
            }
            transArr;)";

    QVariant res = view->evaluateJavaScript(sebExpandTransaction);

    QList<QVariant> list = res.toList();

    for(int i = 0; i < list.size(); i++)
    {
        rowsToExpand.append(list[i].toInt());
    }

    if(rowsToExpand.isEmpty()) {
        parseStatements(accountPage->mainFrame());
    }
    else {
        expandStatement(rowsToExpand.takeFirst());
    }

    qDebug() << "findStatementsToExpand DONE";
}


void SebParser::parseStatements(QWebFrame* view)
{
    qDebug() << "parseStatements START";

    QString sebTransactionParser = R"(
            var accDiv = document.getElementById("IKPMaster_MainPlaceHolder_ucAccountEvents_DataTable");
            var tables = accDiv.getElementsByTagName("table");
            var transArr = [];
            var rows = tables[0].tBodies[0].rows;
            for(var i = 2; i < rows.length; i = i +2) {
                var trans = rows[i];
                var date = trans.cells[1].innerText;
                var name = trans.cells[2].innerText.split('/')[0];
                var sum  = trans.cells[4].innerText;
                if(sum.length == 0) {
                    sum = trans.cells[3].innerText;
                }
                var extra = "";
                var valList = rows[i+1].cells[1].getElementsByClassName("key-value-list");
                for(var j = 0; j < valList.length; j = j +1) {
                    extra += valList[j].innerText;
                }
                transArr.push({Name:name, Date:date, Sum:sum, Extra:extra});
            }
            transArr;)";


    QVariant res = view->evaluateJavaScript(sebTransactionParser);

    QList<QVariant> list = res.toList();

    for(int i = 0; i < list.size(); i++)
    {
        QMap<QString, QVariant> map = list[i].toMap();
        QDate date = QDate::fromString(map["Date"].toString(), "yyyy-MM-dd");

        if(dateInterval.isOlderThanInterval(date))
        {
            qDebug() << "Finished date " << date << "wanted interval " << dateInterval;
            accountFinished(s);
            return;
        }

        if(dateInterval.isWithinInterval(date)) {
            QString var = map["Name"].toString() + map["Date"].toString() + map["Sum"].toString() + map["Extra"].toString();
            QByteArray barr = var.toUtf8();
            quint16 crc1 = qChecksum(barr.data(), barr.length());
            addStatement(s, date, map["Name"].toString(), map["Sum"].toString(), map["Extra"].toString(), crc1);
        }
    }

    qDebug() << "MORE IS NEEDED";
    accountPage->mainFrame()->addToJavaScriptWindowObject("SebParser", this);

    const QString SebNextPage = R"(
            var isMore;
            var nextButton = document.getElementById("IKPMaster_MainPlaceHolder_ucAccountEvents_BTN_NEXT");
            if(nextButton) {
              Sys.WebForms.PageRequestManager.getInstance().add_endRequest( SebParser.newPageEndRequest );
              var event = document.createEvent('Event');
              event.initEvent('click', true, true);
              document.getElementById("IKPMaster_MainPlaceHolder_ucAccountEvents_BTN_NEXT").dispatchEvent(event);
              isMore = 1;
            }
            else {
              isMore = 0;
            }
            isMore;)";

    QVariant res2 = accountPage->mainFrame()->evaluateJavaScript(SebNextPage);
    qDebug() << "RES2 " << res2.typeName() << " val " <<  (int)res2.toDouble() << " " << res2.toDouble();
    if((int)res2.toDouble() == 0) {
        qDebug() << "Finished no more data ";
        accountFinished(s);
    }

    qDebug() << "parseStatements DONE";
}

void SebParser::expandStatementEndRequest()
{
    emit statementExpandedSignal();
}

void SebParser::statementExpanded(void)
{
    accountPage->mainFrame()->evaluateJavaScript
            ("Sys.WebForms.PageRequestManager.getInstance().remove_endRequest( SebParser.expandStatementEndRequest ); null;");
    if(rowsToExpand.isEmpty()) {
        parseStatements(accountPage->mainFrame());
    }
    else {
        expandStatement(rowsToExpand.takeFirst());
    }
}

void SebParser::newPageLoaded()
{
    accountPage->mainFrame()->evaluateJavaScript
                            ("Sys.WebForms.PageRequestManager.getInstance().remove_endRequest( SebParser.newPageEndRequest ); null;");
    findStatementsToExpand(accountPage->mainFrame());
}

void SebParser::newPageEndRequest()
{
    emit newPageLoadedSignal();
}

void SebParser::loadFinished(bool ok)
{
    if(ok)
    {
        qDebug() << "SebParser::loadFinished begin";

        QWebElement statements = accountPage->mainFrame()->findFirstElement("div[id=IKPMaster_MainPlaceHolder_ucAccountEvents_DataTable]");
        if(!statements.isNull())
        {
            accountPage->mainFrame()->addToJavaScriptWindowObject("SebParser", this);

            QString sebFilterDatesParser = R"(
                    Sys.WebForms.PageRequestManager.getInstance().add_endRequest( SebParser.newPageEndRequest );
                    var updSearch  = document.getElementById("IKPMaster_MainPlaceHolder_ucAccountEvents_UpdSearch");
                    var butts = updSearch.getElementsByClassName("m-button")
                    var newestDate = document.getElementById("IKPMaster_MainPlaceHolder_ucAccountEvents_T1");
                    var oldestDate = document.getElementById("IKPMaster_MainPlaceHolder_ucAccountEvents_T2");
                    newestDate.value = "%1";
                    oldestDate.value = "%2";
                    var event = document.createEvent('Event');
                    event.initEvent('click', true, true);
                    butts[2].dispatchEvent(event);
                    null;)";

            accountPage->mainFrame()->evaluateJavaScript
                    (sebFilterDatesParser.arg(dateInterval.getNewestDate().toString("yyyy-MM-dd"),
                                              dateInterval.getOldestDate().toString("yyyy-MM-dd")));
        }
    }
    else
    {
        qDebug() << "Failed to load page";
    }
    qDebug() << "SebParser::loadFinished end";
}

void SebParser::getAccountList(QList<BankAccountInfo> &accList)
{
    loginIfNeeded();
    accList.append(accountMap.values());
}

void SebParser::login_loadFinished(bool ok)
{
    if(ok)
    {
        qDebug() << "SebParser::login_loadFinished start " << accountPage->mainFrame()->url();
  
        QWebElement accountTables = accountPage->mainFrame()->findFirstElement("div[id=IKPMaster_MainPlaceHolder_ucAccountTable_pnlAccounts]");
        if(!accountTables.isNull()) {
            qDebug() << "SebParser::login_loadFinished Done";

            parseAccountTables();
            connect(accountPage, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)), Qt::QueuedConnection);
            disconnect(accountPage, SIGNAL(loadFinished(bool)), this, SLOT(login_loadFinished(bool)));
            loggedInOk = true;
            emit loginFinished(true);
        }

        qDebug() << "SebParser::login_loadFinished end";
    }
    else
    {
        qDebug() << "Failed to load page";
    }
}
