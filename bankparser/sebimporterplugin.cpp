/***************************************************************************
                          sebimporterplugin.cpp
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

#include "sebimporterplugin.h"

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
#include <KDebug>
#include <KFile>
#include <KUrl>
#include <KAction>
#include <KMessageBox>
#include <KActionCollection>
#include <KWallet/Wallet>
#include <mymoneyfile.h>

// ----------------------------------------------------------------------------
// Project Includes

#include "ui_importoption.h"
#include "mapaccountdialog.h"
#include "bankparser.h"
#include "parserfactory.h"
#include "datesdialog.h"
#include "dateinterval.h"

K_PLUGIN_FACTORY(SebImportFactory, registerPlugin<SebImporterPlugin>();)
K_EXPORT_PLUGIN(SebImportFactory("kmm_sebimport"))

using KWallet::Wallet;

class SebImporterPlugin::Private
{
public:
  QList<MyMoneyStatement> m_statementlist;
  bool useForAll;
  DateInterval dateInterval;
};

SebImporterPlugin::SebImporterPlugin(QObject *parent, const QVariantList&) :
    KMyMoneyPlugin::Plugin(parent, "KMyMoney SEB"),
    /*
     * the string in the line above must be the same as
     * X-KDE-PluginInfo-Name and the provider name assigned in
     * SebImporterPlugin::onlineBankingSettings()
     */
    KMyMoneyPlugin::OnlinePlugin(),
    d(new Private)
{
  setComponentData(SebImportFactory::componentData());
  setXMLFile("kmm_sebimport.rc");

  d->useForAll = false;
  // For ease announce that we have been loaded.
  qDebug("KMyMoney sebimport plugin loaded");
}

SebImporterPlugin::~SebImporterPlugin()
{
  qDebug("Destructor called ");
  delete d;
}

const MyMoneyAccount& SebImporterPlugin::account(const QString& key, const QString& value) const
{
  qDebug() << "account " << key << " "  << value;
  return statementInterface()->account(key, value);
}

void SebImporterPlugin::protocols(QStringList& protocolList) const
{
  protocolList.clear();
  protocolList << "WWW";
}

QWidget* SebImporterPlugin::accountConfigTab(const MyMoneyAccount& acc, QString& name)
{
  Q_UNUSED(acc);
  name = i18n("Online settings");
  return NULL;
}

MyMoneyKeyValueContainer SebImporterPlugin::onlineBankingSettings(const MyMoneyKeyValueContainer& current)
{
  qDebug() << "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM onlineBankingSettings";
  MyMoneyKeyValueContainer kvp(current);
  // keep the provider name in sync with the one found in kmm_sebimport.desktop
  kvp["provider"] = "KMyMoney SEB";
  return kvp;
}

bool SebImporterPlugin::mapAccount(const MyMoneyAccount& acc, MyMoneyKeyValueContainer& settings)
{
  Q_UNUSED(acc);

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

bool SebImporterPlugin::updateAccount(const MyMoneyAccount& acc, bool moreAccounts)
{
  Q_UNUSED(moreAccounts);

  qDebug("SebImporterPlugin::updateAccount");
  
  KmmAccountInfo account(acc.number(), acc.name(), acc.institutionId(), acc.id(), acc.lastReconciliationDate(),
                         acc.openingDate(),
                         acc.onlineBankingSettings().value("kmmseb-parser-ref"),
                         acc.onlineBankingSettings().value("kmmseb-acc-ref"));

  qDebug() << "UPDATE " << acc.onlineBankingSettings().value("kmmseb-parser-ref") << " " << acc.onlineBankingSettings().value("kmmseb-acc-ref");

  BankParser* bankparser = ParserFactory::getParser(account.getMappedBank());
  if(bankparser == NULL) {
    qDebug() << "EPA ERROR NULL";
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

  return true;
}

void SebImporterPlugin::accountFinished(MyMoneyStatement* s)
{
  statementInterface()->import(*s);
  qDebug() << "accountFinished " << s->m_accountId;
  delete s;
  
}
