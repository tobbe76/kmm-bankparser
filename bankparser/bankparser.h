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
#include <QFile>
#include <QTimer>
#include <QIcon>
#include <QTextStream>
#include "mymoneystatement.h"
#include "kmymoneyplugin.h"
#include "bankaccountinfo.h"
#include "kmmaccountinfo.h"
#include "dateinterval.h"
#include "logindialog.h"
#include "debugwebpage.h"
#ifdef USE_WEBENGINE
#include <QWebChannel>
#endif

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


class BankParser : public QObject
{
    Q_OBJECT

public:
    BankParser();
    BankParser(QString javaScriptFile);

    virtual ~BankParser() {};

    /* Parse the statements for all accounts */
    void processAccount(KmmAccountInfo& accountInfo, const DateInterval& dateInterval);
    void getAccountList();

signals:
    void accountListFinishedSignal(QList<BankAccountInfo> accList);
    void accountFinishedSignal(MyMoneyStatement* s);
    void loginFinished(bool result);

public slots:
    void loginFinishedSlot(bool result);
    virtual void webChannelInitialized();
#ifdef USE_WEBKIT
    void attachObject();
    void loadFinished(bool res);
#endif
protected:
    void accountFinished(MyMoneyStatement* s);
    virtual void processAccount(const AccountJob& accountJob) = 0;
    virtual void getAccountList(QList<BankAccountInfo> &accList) = 0;
    virtual void loginIfNeeded() = 0;
    void addStatement(MyMoneyStatement *s, const QDate& date, const QString& payee, const QString& sum, const QString& memo, const quint16& verif);
    BrowserLoginDialog *ld;
    DebugWebPage* accountPage;
#ifdef USE_WEBENGINE
    QWebChannel* channel_;
#endif

private:
    QString readFile(QString fileName);
    bool isParsing;
    PendingAccounJobs pendingAccounJobs;
    int waitingOp;
    QString script;
};

#endif
