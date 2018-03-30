/***************************************************************************
                          bankparserplugin.h
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

#ifndef BANKPARSERPLUGIN_H
#define BANKPARSERPLUGIN_H

// ----------------------------------------------------------------------------
// KDE Includes

// ----------------------------------------------------------------------------
// QT Includes

#include <QList>
#include <QStringList>

// ----------------------------------------------------------------------------
// Project Includes

#include "kmymoneyplugin.h"
#include "bankparser.h"
#include "mymoneyaccount.h"
#include "mymoneykeyvaluecontainer.h"
/**
@author Thorbjorn Larsson
*/
class Kmm_Bankparser : public KMyMoneyPlugin::Plugin, public KMyMoneyPlugin::OnlinePlugin
{
  Q_OBJECT
  Q_INTERFACES(KMyMoneyPlugin::OnlinePlugin)
    
public:
  explicit Kmm_Bankparser(QObject *parent, const QVariantList&);

  ~Kmm_Bankparser();

  /**
    * Returns a pointer to a widget that will be added as tab to
    * the account edit dialog. @sa KNewAccountDlg. The data of the
    * current account is passed as const reference @a acc. @a name references
    * a QString that will receive the name of the tab to be shown in the dialog.
    */
  QWidget* accountConfigTab(const MyMoneyAccount& acc, QString& name) override;

  /**
    * Retrieves the online banking settings and updates the password in the KDE wallet.
    * The caller has the choice to pass a MyMoneyKeyValueContainer with the @a current
    * settings. Only those are modified that are used by the plugin.
    */
  MyMoneyKeyValueContainer onlineBankingSettings(const MyMoneyKeyValueContainer& current) override;

  void protocols(QStringList& protocolList) const override;

  bool mapAccount(const MyMoneyAccount& acc, MyMoneyKeyValueContainer& settings) override;
  bool updateAccount(const MyMoneyAccount& acc, bool moreAccounts) override;
  void plug() override;
  void unplug() override;
  void configurationChanged() override;
  void injectExternalSettings(KMyMoneySettings* p) override;
protected slots:
  void accountFinished(MyMoneyStatement* s);
  
protected:
  void createActions();

private:
  void addAccount(QString id, QString nr, QDate lastRecDate, BankParser *parser);
  /// \internal d-pointer class.
  class Private;
  /// \internal d-pointer instance.
  Private* const d;
};

#endif
