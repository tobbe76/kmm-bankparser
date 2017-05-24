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

#include "bankparser.h"
#include "logindialog.h"

bool PendingAccounJobs::isPendingJob() const
{
    return !jobList.isEmpty();
}

AccountJob PendingAccounJobs::getJob()
{
    return jobList.takeFirst();
}

void PendingAccounJobs::addJob(const AccountJob &job)
{
    jobList.append(job);
}


BankParser::BankParser()
{
    isParsing = false;
}

void BankParser::loginIfNeeded()
{
    if(!isLoggedIn())
    {
        LoginDialog ld;
        ld.loginBank(this);
        ld.setModal(true);
        ld.exec();
    }
}

void BankParser::accountFinished(MyMoneyStatement *s)
{
    emit accountFinishedSignal(s);
    if(pendingAccounJobs.isPendingJob())
    {
        qDebug() << "Run NEXT job" << isParsing;
        processAccount(pendingAccounJobs.getJob());
    }
    else
    {
        isParsing = false;
    }
}

void BankParser::addStatement(MyMoneyStatement *s, const QDate &date, const QString &payee, const QString &sum, const QString &memo, const quint16 &verif)
{
    MyMoneyStatement::Transaction t;

    t.m_datePosted = date;
    t.m_strPayee = payee;
    t.m_strMemo = memo;
    t.m_strBankID = QString("ID %1").arg(verif);
    t.m_amount = MyMoneyMoney(sum);

    s->m_listTransactions.append(t);
}

void BankParser::processAccount(KmmAccountInfo& accountInfo, const DateInterval &dateInterval)
{
    loginIfNeeded();
    AccountJob job(accountInfo, dateInterval);
    if(isParsing)
    {
        qDebug() << "Add pending job" << isParsing;
        pendingAccounJobs.addJob(job);
    }
    else
    {
        qDebug() << "Run job" << isParsing;
        isParsing = true;
        processAccount(job);
    }
}
