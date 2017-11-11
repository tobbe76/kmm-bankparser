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

SwedbankParser::SwedbankParser()
{
    qDebug() << "********************** construct ";
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
    loggedInOk = false;
}

bool SwedbankParser::login(QWebView* view)
{
  qDebug() << "SwedbankParser::login start";
  
  accountPage = new QWebPage(this);
  
  view->setPage(accountPage);

  connect(accountPage, SIGNAL(loadFinished(bool)), this, SLOT(login_loadFinished(bool)), Qt::QueuedConnection);
  connect(accountPage->mainFrame(), SIGNAL(urlChanged(const QUrl&)), this, SLOT(urlChanged(const QUrl&)), Qt::QueuedConnection);
  view->load(QUrl("https://demo.swedbank.se/app/privat/login"));

  loggedInOk = true;
  qDebug() << "SwedbankParser::login end";
  
  return true;
}

bool SwedbankParser::isLoggedIn()
{
  return loggedInOk;
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

void SwedbankParser::timeout()
{
    qDebug() << "TimeOut";
    timer->stop();
    loadFinished(true);
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

void SwedbankParser::epa()
{
    accountPage->mainFrame()->evaluateJavaScript
                            ("Sys.WebForms.PageRequestManager.getInstance().remove_endRequest(MainWindow.epa);");
    timer->start(100); //Todo change to poll with 100ms
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
    Q_UNUSED(accList);
    loginIfNeeded();
}

void SwedbankParser::parseAccountTables(bool ok)
{
    qDebug() << "SwedbankParser::parseAccountTables begin";

    const QString accountListParser = R"(
            var accLists = document.getElementsByClassName("_list _list--striped-reverse");
            for(var i = 0; i < accLists.length; i++) {
                var infoList = accList[i].getElementsByTagName("li");
                for(var j = 0; j < infoLists.length; j++) {
                    console.log(infoLists[j].innerText);
                }
            }
            var accArr = [];
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

    qDebug() << "SwedbankParser::parseAccountTables end";
}

void SwedbankParser::login_loadFinished(bool ok)
{
    qDebug() << "SwedbankParser::login_loadFinished begin" << accountPage->mainFrame()->url() << " " << ok;

    if(QUrl("https://demo.swedbank.se/app/privat/start-page") == accountPage->mainFrame()->url())
    {
        qDebug() << "SwedbankParser::isLoginFinished Done";
        connect(accountPage, SIGNAL(loadFinished(bool)), this, SLOT(login_loadFinished(bool)), Qt::QueuedConnection);
        disconnect(accountPage, SIGNAL(loadFinished(bool)), this, SLOT(login_loadFinished(bool)));
        loggedInOk = true;
        emit loginFinished(true);
        accountPage->mainFrame()->load(QUrl("https://demo.swedbank.se/app/privat/konton-och-kort"));
    }
    qDebug() << "SwedbankParser::login_loadFinished end";
}

void SwedbankParser::urlChanged(const QUrl & url) {
    qDebug() << "SwedbankParser::urlChanged " << url;
    if(QUrl("https://demo.swedbank.se/app/privat/start-page") == accountPage->mainFrame()->url())
    {
        qDebug() << "SwedbankParser::urlChanged Done";
        connect(accountPage, SIGNAL(loadFinished(bool)), this, SLOT(login_loadFinished(bool)), Qt::QueuedConnection);
        disconnect(accountPage, SIGNAL(loadFinished(bool)), this, SLOT(login_loadFinished(bool)));
        loggedInOk = true;
        emit loginFinished(true);
        accountPage->mainFrame()->load(QUrl("https://demo.swedbank.se/app/privat/konton-och-kort"));
    }
    qDebug() << "SwedbankParser::login_loadFinished end";
}
