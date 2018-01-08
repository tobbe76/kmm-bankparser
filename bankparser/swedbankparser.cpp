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
#include <QDebug>
#include <QTimer>
#include "debugwebpage.h"

SwedbankParser::SwedbankParser() :
    BankParser(":/files/js/swedbank.js")
{
    qDebug() << "SwedbankParser::SwedbankParser begin";

    loggedInOk = false;
    ld = new BrowserLoginDialog();

    qDebug() << "SwedbankParser::SwedbankParser end";

}

void SwedbankParser::loginIfNeeded(void)
{
  qDebug() << "SwedbankParser::login start";

  if(!loggedInOk) {

      ld->openLoginDialog(accountPage, QUrl("https://demo.swedbank.se/app/privat/login"));

      connect(accountPage, SIGNAL(urlChanged(const QUrl&)), this, SLOT(urlChanged(const QUrl&)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));

      loggedInOk = true;
  }
  else {
      emit loginFinished(true);
  }

  qDebug() << "SwedbankParser::login end";
  
  return;
}


void SwedbankParser::processAccount(const AccountJob &accountJob)
{
  s = new MyMoneyStatement();
  this->dateInterval = accountJob.getDateInterval();
  qDebug() << "SwedbankParser::processAccount " << accountJob.getAccountInfo().getMappedAccount();
  if(accountMap.contains(accountJob.getAccountInfo().getMappedAccount())) {

      BankAccountInfo account = accountMap.value(accountJob.getAccountInfo().getMappedAccount());
      accountPage->runJavaScript(QString("parseStatements(%1);").arg(account.getIndex()));

      s->m_accountId = accountJob.getAccountInfo().getId();
  }
  else {
    accountFinished(s);
  }
}

SwedbankParser::~SwedbankParser()
{
}

void SwedbankParser::parseStatements(const QVariant &res)
{
    QList<QVariant> list = res.toList();

    qDebug() << "SwedbankParser::parseStatements size: " << list.size();

    for(int i = 0; i < list.size(); i++)
    {
        QMap<QString, QVariant> map = list[i].toMap();
        QDate date = QDate::fromString(map["Date"].toString(), "yyyy-MM-dd");

        qDebug() << "name " << map["Name"].toString();
        addStatement(s, date, map["Name"].toString(), map["Sum"].toString(), "extra", 12);
    }
    accountFinished(s);

    return;
}

void SwedbankParser::getAccountList(QList<BankAccountInfo> &accList)
{
    accList.append(accountMap.values());
}

void SwedbankParser::parseAccountTables(const QVariant &res)
{
    qDebug() << "SwedbankParser::parseAccountTables begin";

    QList<QVariant> list = res.toList();
    for(int i = 0; i < list.size(); i++)
    {
        QMap<QString, QVariant> map = list[i].toMap();
        BankAccountInfo account;
        account.setName(map["Name"].toString());
        account.setNumber(map["Number"].toString());
        account.setKey(map["Number"].toString().replace(" ", ""));
        account.setIndex(map["LinkIndex"].toDouble());
        accountMap[account.getKey()] = account;
        qDebug() << "name " << map["Name"].toString() << "Key " << account.getKey() << " " << map["LinkIndex"].toDouble();
    }

    loggedInOk = true;
    emit loginFinished(true);
    ld->closeLoginDialog();

    qDebug() << "SwedbankParser::parseAccountTables end";
}

void SwedbankParser::urlChanged(const QUrl & url) {
    qDebug() << "SwedbankParser::urlChanged " << url;
    if(QUrl("https://demo.swedbank.se/app/privat/start-page") == accountPage->getUrl())
    {
        qDebug() << "SwedbankParser::urlChanged Done";

        accountPage->runJavaScript("parseAccounts()");
    }

    qDebug() << "SwedbankParser::urlChanged end";
}

