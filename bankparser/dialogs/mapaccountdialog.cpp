/***************************************************************************
                          mapaccountdialog.cpp
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

#include "mapaccountdialog.h"
#include "ui_mapaccountdialog.h"
#include <QDebug>
#include "parserfactory.h"

int AccountListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return accList.size();
}

void AccountListModel::setAccountList(const QList<BankAccountInfo> &accs) {
    beginResetModel();
    accList = accs;
    endResetModel();
}

QString AccountListModel::getAccountKey(const QModelIndex &index) const
{
    return accList[index.row()].getKey();
}

QVariant AccountListModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        QString bepa;
        bepa = accList[index.row()].getName() + " " + accList[index.row()].getNumber();
        return bepa;
    }
    else
    {
        return QVariant();
    }
}

MapAccountDialog::MapAccountDialog(const KmmAccountInfo& kmmAccountInfo, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MapAccountDialog)
{
    ui->setupUi(this);
    ui->bankparserBox->addItem("Select Bank");
    ui->bankparserBox->addItems(ParserFactory::supportedParsers());
    ui->accInfoWidget->setAccountInfo(kmmAccountInfo);
    bank = kmmAccountInfo.getMappedBank();
    accountKey = kmmAccountInfo.getMappedAccount();

    accountListModel = new AccountListModel();
    ui->bankAccList->setModel(accountListModel);
    connect(ui->bankparserBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(bankparserBox_currentIndexChanged(QString)));
}

QString MapAccountDialog::getAccountNr() const
{
  return accountKey;
}

QString MapAccountDialog::getSelectedParser() const
{
  return bank;
}

MapAccountDialog::~MapAccountDialog()
{
    delete accountListModel;
    delete ui;
}

void MapAccountDialog::on_bankAccList_clicked(const QModelIndex &index)
{
    accountKey = accountListModel->getAccountKey(index);
    qDebug() << "on_bankAccList_clicked" << accountListModel->data(index, Qt::DisplayRole).toString();
    ui->accInfoWidget->setMappedAccount(accountListModel->data(index, Qt::DisplayRole).toString());
}

void MapAccountDialog::bankparserBox_currentIndexChanged(const QString &arg1)
{
    ui->accInfoWidget->setMappedBank(arg1);
    bank = arg1;

    BankParser* parser = ParserFactory::getParser(arg1);
    connect(parser, SIGNAL(accountListFinishedSignal(QList<BankAccountInfo>)), this, SLOT(accountListFinishedSlot(QList<BankAccountInfo>)));
    parser->getAccountList();
}

void MapAccountDialog::accountListFinishedSlot(QList<BankAccountInfo> accList)
{
    accountListModel->setAccountList(accList);
}
