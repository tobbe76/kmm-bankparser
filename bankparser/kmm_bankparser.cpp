/***************************************************************************
                          bankparserplugin.cpp
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

#include "kmm_bankparser.h"

// ----------------------------------------------------------------------------
// QT Includes

#include <QFile>
#include <QTextStream>
#include <QRadioButton>
#include <QSpinBox>
#include <QByteArray>
#include <QVector>
// ----------------------------------------------------------------------------
// KDE Includes

#include <KPluginFactory>
#include <KMessageBox>
#include <KActionCollection>
#include <mymoneyfile.h>

// ----------------------------------------------------------------------------
// Project Includes
#include "mymoneyaccount.h"
#include "mymoneykeyvaluecontainer.h"
#include "mymoneystatement.h"
#include "statementinterface.h"
#include "mapaccountdialog.h"
#include "bankparser.h"
#include "parserfactory.h"
#include "datesdialog.h"
#include "dateinterval.h"
#include "kmymoneyglobalsettings.h"

class Kmm_Bankparser::Private
{
public:
  QList<MyMoneyStatement> m_statementlist;
  bool useForAll;
  DateInterval dateInterval;
};

Kmm_Bankparser::Kmm_Bankparser(QObject *parent, const QVariantList&) :
    KMyMoneyPlugin::Plugin(parent, "KMyMoney BANKPARSER"),
    /*
     * the string in the line above must be the same as
     * X-KDE-PluginInfo-Name and the provider name assigned in
     * BankparserPlugin::onlineBankingSettings()
     */
    d(new Private)
{
  Q_INIT_RESOURCE(files);

  
  setComponentName("kmm_bankparser", "kmm_bankparser");
  setXMLFile("kmm_bankparser.rc");

  d->useForAll = false;
  // For ease announce that we have been loaded.
  qDebug("KMyMoney bankparser plugin loaded");
}

Kmm_Bankparser::~Kmm_Bankparser()
{
  delete d;
}

const MyMoneyAccount& Kmm_Bankparser::account(const QString& key, const QString& value) const
{
  qDebug() << "account " << key << " "  << value;
  return statementInterface()->account(key, value);
}

void Kmm_Bankparser::plug()
{
}

void Kmm_Bankparser::unplug()
{
}

void Kmm_Bankparser::configurationChanged()
{
}

void Kmm_Bankparser::protocols(QStringList& protocolList) const
{
  protocolList << "bankparser";
}

QWidget* Kmm_Bankparser::accountConfigTab(const MyMoneyAccount& acc, QString& name)
{
  Q_UNUSED(acc);
  name = "Online settings";
  return NULL;
}

void Kmm_Bankparser::injectExternalSettings(KMyMoneySettings* p)
{
}

MyMoneyKeyValueContainer Kmm_Bankparser::onlineBankingSettings(const MyMoneyKeyValueContainer& current)
{
  MyMoneyKeyValueContainer kvp(current);
  // keep the provider name in sync with the one found in kmm_sebimport.desktop
  kvp["provider"] = "KMyMoney BANKPARSER";
  return kvp;
}

bool Kmm_Bankparser::mapAccount(const MyMoneyAccount& acc, MyMoneyKeyValueContainer& settings)
{
  qDebug() << "Map account " << acc.number() << "name " << acc.name() << " ID " << acc.id();
  KmmAccountInfo accountInfo(acc.number(), acc.name(), acc.institutionId(), acc.id(),
                             acc.lastReconciliationDate(), acc.openingDate(),
                             acc.onlineBankingSettings().value("kmmseb-parser-ref"),
                             acc.onlineBankingSettings().value("kmmseb-acc-ref"));

  QPointer<MapAccountDialog> dlg = new MapAccountDialog(accountInfo);
  dlg->exec();
  settings.setValue("kmmseb-acc-ref", dlg->getAccountNr());
  settings.setValue("kmmseb-parser-ref", dlg->getSelectedParser());

  return true;
}

bool Kmm_Bankparser::updateAccount(const MyMoneyAccount& acc, bool moreAccounts)
{
  qDebug("Kmm_Bankparser::updateAccount");
  
  KmmAccountInfo account(acc.number(), acc.name(), acc.institutionId(), acc.id(), acc.lastReconciliationDate(),
                         acc.openingDate(),
                         acc.onlineBankingSettings().value("kmmseb-parser-ref"),
                         acc.onlineBankingSettings().value("kmmseb-acc-ref"));

  qDebug() << "UPDATE " << acc.onlineBankingSettings().value("kmmseb-parser-ref") << " " << acc.onlineBankingSettings().value("kmmseb-acc-ref");

  BankParser* bankparser = ParserFactory::getParser(account.getMappedBank());
  if(bankparser == NULL) {
    qCritical() << "No parser found for bank: " << account.getMappedBank();
    return false;
  }

  if(d->useForAll == false)
  {
      DatesDialog dates(account);
      int res = dates.exec();

      if(res == QDialog::Rejected) {
          return false;
      }

      d->dateInterval = dates.getDateInterval();
      d->useForAll = dates.getUseForAll();
  }

  connect(bankparser, SIGNAL(accountFinishedSignal(MyMoneyStatement*)),
          this, SLOT(accountFinished(MyMoneyStatement*)),
          Qt::UniqueConnection);

  bankparser->processAccount(account, d->dateInterval);

  if(!moreAccounts) {
    d->useForAll = false;
  }
  
  return true;
}

void Kmm_Bankparser::accountFinished(MyMoneyStatement* s)
{
  statementInterface()->import(*s);
  qDebug() << "accountFinished " << s->m_accountId;
  delete s;
}

K_PLUGIN_FACTORY_WITH_JSON(Kmm_bankparserFactory, "kmm_bankparser.json", registerPlugin<Kmm_Bankparser>();)

#include "kmm_bankparser.moc"
