/***************************************************************************
                          bankparser.h
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

#ifndef BANKPARSER_H
#define BANKPARSER_H

#include <QUrl>
#include <QMap>
#include <QWebElement>
#include <QWebView>
#include <QFile>
#include <QTimer>
#include <QTextStream>
#include "kmymoneyplugin.h"
#include "logininterface.h"
#include "bankaccountinfo.h"
#include "kmmaccountinfo.h"
#include "dateinterval.h"

class AccountJob
{
public:
    AccountJob(KmmAccountInfo& accountInfo, DateInterval dateInterval) :
        AccountInfo(accountInfo), Interval(dateInterval) {};

    KmmAccountInfo getAccountInfo() const { return AccountInfo;};
    DateInterval getDateInterval() const { return Interval;};

private:
    KmmAccountInfo AccountInfo;
    DateInterval Interval;
};

class PendingAccounJobs
{
public:
    bool isPendingJob() const;
    AccountJob getJob();
    void addJob(const AccountJob& job);
private:
    QList<AccountJob> jobList;
};


class BankParser : public LoginInterface
{

public:
    BankParser();
    virtual ~BankParser() {};

    /* Return true if already logged in */
    virtual bool isLoggedIn() = 0;

    /* Parse the statements for all accounts */
    void processAccount(KmmAccountInfo& accountInfo, const DateInterval& dateInterval);

    virtual void getAccountList(QList<BankAccountInfo> &accList) = 0;

signals:
    virtual void accountFinishedSignal(MyMoneyStatement* s) = 0;

protected:
    void loginIfNeeded();
    void accountFinished(MyMoneyStatement* s);
    virtual void processAccount(const AccountJob& accountJob) = 0;
    void addStatement(MyMoneyStatement *s, const QDate& date, const QString& payee, const QString& sum, const QString& memo, const quint16& verif);
    bool isParsing;
    PendingAccounJobs pendingAccounJobs;
};

#endif
