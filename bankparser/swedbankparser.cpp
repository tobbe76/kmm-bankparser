/***************************************************************************
                          swedbankparser.cpp
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

#include "swedbankparser.h"
#include <QWebElement>
#include <QWebFrame>
#include <QWebView>
#include <QDebug>
#include <QTimer>
#include "debugwebpage.h"

SwedbankParser::SwedbankParser()
{
    loggedInOk = false;
    accountPage = new DebugWebPage(this);
    ld = new BrowserLoginDialog();
}

void SwedbankParser::loginIfNeeded(void)
{
  qDebug() << "SwedbankParser::login start";

  if(!loggedInOk) {

      ld->openLoginDialog(accountPage, QUrl("https://demo.swedbank.se/app/privat/login"));

      connect(accountPage, SIGNAL(loadFinished(bool)), this, SLOT(login_loadFinished(bool)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
      connect(accountPage->mainFrame(), SIGNAL(urlChanged(const QUrl&)), this, SLOT(urlChanged(const QUrl&)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));

      loggedInOk = true;
  }
  else {
      emit loginFinished(true);
  }

  qDebug() << "SwedbankParser::login end";
  
  return;
}

void SwedbankParser::accountFinished()
{
  emit accountFinishedSignal(s);
}

void SwedbankParser::processAccount(const AccountJob &accountJob)
{
  s = new MyMoneyStatement();
  this->dateInterval = accountJob.getDateInterval();
  if(accountList.contains(accountJob.getAccountInfo().getMappedAccount())) {
    s->m_accountId = accountJob.getAccountInfo().getId();    
    qDebug() << "Load url " << accountList.value(accountJob.getAccountInfo().getMappedAccount());
    accountPage->mainFrame()->load(accountList.value(accountJob.getAccountInfo().getMappedAccount()));
  }
  else {
    accountFinished();
  }
}

SwedbankParser::~SwedbankParser()
{
}

void SwedbankParser::writeQifStatement(QDate date, QString payee, QString sum, QString memo, quint16 verif)
{
  MyMoneyStatement::Transaction t;
  
  t.m_datePosted = date;
  t.m_strPayee = payee;
  t.m_strMemo = memo;
  t.m_strBankID = QString("ID %1").arg(verif);
  t.m_amount = MyMoneyMoney(sum);

  s->m_listTransactions.append(t);
}

bool SwedbankParser::parseStatements(QWebElement s)
{
    Q_UNUSED(s);
    return false;
}

void SwedbankParser::accountsLoadedReq()
{
    accountPage->mainFrame()->evaluateJavaScript
                            ("Sys.WebForms.PageRequestManager.getInstance().remove_endRequest(SwedbankParser.accountsLoadedReq);");
    qDebug() << "HOHO accountsLoadedReq";
}

void SwedbankParser::loadFinished(bool arg1)
{
    Q_UNUSED(arg1);

    qDebug() << "LOAD Finished mainwindow";

    QWebElement statements = accountPage->mainFrame()->findFirstElement("div[id=IKPMaster_MainPlaceHolder_ucAccountEvents_DataTable]");
    if(!statements.isNull())
    {
        qDebug() << "parseStatements state: ";
        if(parseStatements(statements)) {
            accountFinished();
        }
        else {
            QWebElement next = accountPage->mainFrame()->findFirstElement("div.pagination");
            QWebElement link = next.findFirst("a[id=IKPMaster_MainPlaceHolder_ucAccountEvents_BTN_NEXT]");
            QString href = link.attribute("href");
            qDebug() << "MORE IS NEEDED " << href;
            accountPage->mainFrame()->addToJavaScriptWindowObject("MainWindow", this);
            QVariant res = accountPage->mainFrame()->evaluateJavaScript
                        ("Sys.WebForms.PageRequestManager.getInstance().add_endRequest(MainWindow.epa);");
            link.evaluateJavaScript("this.click()"); //load more statements same account
        }
    }
}

void SwedbankParser::getAccountList(QList<BankAccountInfo> &accList)
{
    accList.append(accountMap.values());
}

void SwedbankParser::parseAccountTables()
{
    qDebug() << "SwedbankParser::parseAccountTables begin";

    const QString accountListParser = R"(
            var accArr = [];
            var accLists = document.getElementsByClassName("_list _list--striped-reverse");
            for(var i = 0; i < accLists.length; i++) {
                var infoLists = accLists[i].getElementsByTagName("li");
                for(var j = 0; j < infoLists.length; j++) {
                    var nameEl = infoLists[j].getElementsByTagName("sw-label");
                    var nrEl = infoLists[j].getElementsByTagName("sw-label-detail");
                    if(nrEl.length > 0 && nameEl.length > 0) {
                      accArr.push({Name:nameEl[0].innerText, Number:nrEl[0].innerText});
                    }
                }
            }
            accArr;)";

    QVariant res = accountPage->mainFrame()->evaluateJavaScript(accountListParser);

    QList<QVariant> list = res.toList();
    for(int i = 0; i < list.size(); i++)
    {
        QMap<QString, QVariant> map = list[i].toMap();
        BankAccountInfo account;
   //     account.setUrl("Empty");
        account.setName(map["Name"].toString());
        account.setNumber(map["Number"].toString());
        account.setKey(map["Number"].toString().replace(" ", ""));
        accountMap[account.getKey()] = account;
    }

    loggedInOk = true;
    emit loginFinished(true);
    ld->closeLoginDialog();

    qDebug() << "SwedbankParser::parseAccountTables end";
}

void SwedbankParser::login_loadFinished(bool ok)
{
    qDebug() << "SwedbankParser::login_loadFinished begin" << accountPage->mainFrame()->url() << " " << ok;

    qDebug() << "SwedbankParser::login_loadFinished end";
}

void SwedbankParser::urlChanged(const QUrl & url) {
    qDebug() << "SwedbankParser::urlChanged " << url;
    if(QUrl("https://demo.swedbank.se/app/privat/start-page") == accountPage->mainFrame()->url())
    {
        qDebug() << "SwedbankParser::urlChanged Done";
        disconnect(accountPage, SIGNAL(loadFinished(bool)), this, SLOT(login_loadFinished(bool)));

        accountPage->mainFrame()->addToJavaScriptWindowObject("SwedbankParser", this);

        QString SwedbankGetAccounts = R"(
         var sidebar = document.getElementById("sidebarArea");
         var butts = sidebar.getElementsByTagName("a");
         var event = document.createEvent('Event');
         event.initEvent('click', true, true);
         butts[1].dispatchEvent(event);)";

        accountPage->mainFrame()->evaluateJavaScript(SwedbankGetAccounts);
    }
    else if(QUrl("https://demo.swedbank.se/app/privat/konton-och-kort") == accountPage->mainFrame()->url())
    {
        QTimer::singleShot(100, this, SLOT(parseAccountTables()));
    }
    qDebug() << "SwedbankParser::urlChanged end";
}
