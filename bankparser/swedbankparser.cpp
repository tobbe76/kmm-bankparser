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

void SwedbankParser::parseAccountTables(QWebElement accountTables)
{
    QUrl baseUrl = accountPage->mainFrame()->baseUrl();

    QWebElementCollection account = accountTables.findAll("tbody tr");
    foreach (QWebElement e, account) {
        QWebElementCollection em = e.findAll("td");
        QWebElement link = em[0].findFirst("a");
        QUrl relativeUrl(link.attribute("href"));
        QUrl absoluteUrl = baseUrl.resolved(relativeUrl);
	QString number = em[1].toPlainText().trimmed().simplified().replace(" ", "");

	accountList[number] = absoluteUrl;
    }
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
