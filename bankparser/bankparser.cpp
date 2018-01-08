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
#ifdef USE_WEBENGINE
#include <QWebEngineScriptCollection>
#else
#include <QWebFrame>
#endif

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

QString BankParser::readFile(QString fileName) {
    QFile apiFile(fileName);
    if(!apiFile.open(QIODevice::ReadOnly))
        qDebug()<<"Couldn't load fileName: " << fileName;
    QString str = QString::fromLatin1(apiFile.readAll());
    apiFile.close();

    return str;
}

BankParser::BankParser()
{
    qDebug() << "BankParser::BankParser begin";

    isParsing = false;
    waitingOp = 0;
    connect(this, SIGNAL(loginFinished(bool)), this, SLOT(loginFinishedSlot(bool)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));

    accountPage = new DebugWebPage(this);

#ifdef USE_WEBENGINE
    channel_ = new QWebChannel(accountPage);
    //attach it to the QWebEnginePage
    accountPage->setWebChannel(channel_);

    QString qwebchannel = readFile(":/qtwebchannel/qwebchannel.js");
    QString common = readFile(":/files/js/common.js");

    qwebchannel.append(common);

    qwebchannel.append("console.log(\"QWEBCHANNEL LOADED\");");


    QWebEngineScriptCollection &scc = accountPage->scripts();
    QWebEngineScript sc;
    sc.setSourceCode(qwebchannel);
    sc.setWorldId(0);
    scc.insert(sc);

    channel_->registerObject(QStringLiteral("jshelper"), this);
#else
    script = "function bpClick(el) {"
            "var event = document.createEvent('Event');"
            "event.initEvent('click', true, true);"
            "el.dispatchEvent(event);"
            "}";
    connect(accountPage->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(attachObject()), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
    connect(accountPage->mainFrame(), SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));


#endif

    qDebug() << "BankParser::BankParser begin";

}

BankParser::BankParser(QString javaScriptFile) :
    BankParser()
{
    QString js = readFile(javaScriptFile);
#ifdef USE_WEBENGINE

    QWebEngineScriptCollection &scc = accountPage->scripts();
    QWebEngineScript sc;
    sc.setSourceCode(js);
    sc.setWorldId(0);
    scc.insert(sc);
#else
    script += js;
#endif
}

void BankParser::accountFinished(MyMoneyStatement *s)
{
    qDebug("EMIT accountFinishedSignal");
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
    qDebug() << "BankParser::processAccount begin";
    AccountJob job(accountInfo, dateInterval);
    pendingAccounJobs.addJob(job);
    if(waitingOp == 0)
    {
        waitingOp = 2;
        loginIfNeeded();
    }
    else {
        qDebug() << "Login already in progress";
    }
}

void BankParser::getAccountList()
{
    waitingOp = 1;
    loginIfNeeded();
}

void BankParser::loginFinishedSlot(bool result) {
    Q_UNUSED(result);
    qDebug() << "BankParser::loginFinishedSlot BEGIN";
    if(waitingOp == 1)
    {
        QList<BankAccountInfo> list;
        getAccountList(list);
        emit accountListFinishedSignal(list);
    }
    else if(waitingOp == 2)
    {
        if(!isParsing && pendingAccounJobs.isPendingJob())
        {
            qDebug() << "Run First job" << isParsing;
            isParsing = true;
            processAccount(pendingAccounJobs.getJob());
        }
    }
    waitingOp = 0;
    qDebug() << "BankParser::loginFinishedSlot END";

}

void BankParser::webChannelInitialized()
{
    qDebug() << "BankParser::webChannelInitialized EMPTY";
}
#ifdef USE_WEBKIT
void BankParser::attachObject() {
    qDebug() << "BankParser::attachObject begin";

    accountPage->mainFrame()->addToJavaScriptWindowObject("jshelper", this);
    accountPage->runJavaScript(script);
    qDebug() << "BankParser::attachObject end";
}
void BankParser::loadFinished(bool res) {
    Q_UNUSED(res);
    qDebug() << "BankParser::loadFinished begin" << accountPage->mainFrame()->url();
    webChannelInitialized();
    qDebug() << "BankParser::loadFinished end";
}
#endif
