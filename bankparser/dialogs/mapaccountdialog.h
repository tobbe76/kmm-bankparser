/***************************************************************************
                          mapaccountdialog.h
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

#ifndef MAPACCOUNTDIALOG_H
#define MAPACCOUNTDIALOG_H

#include <QDialog>
#include <QListWidgetItem>
#include <QAbstractListModel>
#include "bankparser.h"
#include "kmmaccountinfo.h"

class AccountListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    AccountListModel(QObject *parent = 0) : QAbstractListModel(parent) {}
    QString getAccountKey(const QModelIndex &index) const;
    void setAccountList(const QList<BankAccountInfo> &accs);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

private:
    QList<BankAccountInfo> accList;
};

namespace Ui {
class MapAccountDialog;
}

class MapAccountDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MapAccountDialog(const KmmAccountInfo &kmmAccountInfo, QWidget *parent = 0);
    ~MapAccountDialog();
    QString getAccountNr() const;
    QString getSelectedParser() const;

private slots:

    void on_bankAccList_clicked(const QModelIndex &index);

    void bankparserBox_currentIndexChanged(const QString &arg1);

private:
    Ui::MapAccountDialog *ui;
    QListWidgetItem* mappedAccountItem;
    AccountListModel* accountListModel;
    QString accountKey;
    QString bank;
};

#endif // MAPACCOUNTDIALOG_H
