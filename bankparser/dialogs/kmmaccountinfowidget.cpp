/***************************************************************************
                          logindialog.cpp
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

#include "kmmaccountinfowidget.h"
#include "ui_kmmaccountinfowidget.h"
#include "parserfactory.h"

int KmmAccountModelLocal::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 6;
}

int KmmAccountModelLocal::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

void KmmAccountModelLocal::setAccountList(const KmmAccountInfo& acc)
{
    beginResetModel();
    accInfo = acc;
    endResetModel();
}

QVariant KmmAccountModelLocal::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if(index.column() == 0)
        {
            if(index.row() == 0)
                return "Name:";
            else if(index.row() == 1)
                return "Number:";
            else if(index.row() == 2)
                return "Institution:";
            else if(index.row() == 3)
                return "Id:";
            else if(index.row() == 4)
                return "Opened:";
            else if(index.row() == 5)
                return "Reconciled:";
        }

        if(index.column() == 1)
        {
            if(index.row() == 0)
                return accInfo.getName();
            else if(index.row() == 1)
                return accInfo.getNumber();
            else if(index.row() == 2)
                return accInfo.getInstitution();
            else if(index.row() == 3)
                return accInfo.getId();
            else if(index.row() == 4)
                return accInfo.getOpenedDateStr();
            else if(index.row() == 5)
                return accInfo.getReconciledDateStr();
        }

    }
    return QVariant();
}

QVariant KmmAccountModelLocal::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section);
    Q_UNUSED(orientation);

    if (role == Qt::SizeHintRole) {
        return QSize(0, 0);
    }
    return QVariant();
}

int KmmAccountModelRemote::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

int KmmAccountModelRemote::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

void KmmAccountModelRemote::setMappedBank(const QString &bank)
{
    beginResetModel();
    this->bank = bank;
    endResetModel();
    dataChanged(createIndex(0, 0), createIndex(0, 0));
    dataChanged(createIndex(1, 1), createIndex(1, 1));
}

void KmmAccountModelRemote::setMappedAccount(const QString &accountKey)
{
    this->accountKey = accountKey;
    dataChanged(createIndex(2, 1), createIndex(2, 1));
}

QVariant KmmAccountModelRemote::data(const QModelIndex &index, int role) const
{
    if(role == Qt::TextAlignmentRole) {
        if(index.row() == 0)
        {
            return Qt::AlignHCenter;
        }
    }

    if ( role == Qt::DecorationRole ) {
        if(index.row() == 0 && index.column() == 0) {
            return ParserFactory::getPixmap(bank);
        }
    }

    if (role == Qt::DisplayRole)
    {
        if(index.column() == 0)
        {
            if(index.row() == 0)
                return "";
            else if(index.row() == 1)
                return "MappedBank:";
            else if(index.row() == 2)
                return "MappedAccount:";
        }

        if(index.column() == 1)
        {
            if(index.row() == 0)
                return "";
            else if(index.row() == 1)
                return bank;
            else if(index.row() == 2)
                return accountKey;
        }

    }
    else if (role == Qt::SizeHintRole) {
        if(index.row() == 0)
            return QSize(100, 50);
        return QVariant();
     //   return QSize(100, 5);
    }

    return QVariant();
}

QVariant KmmAccountModelRemote::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section);
    Q_UNUSED(orientation);

    if (role == Qt::SizeHintRole) {
        return QSize(0, 0);
    }
    return QVariant();
}


KmmAccountInfoWidget::KmmAccountInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KmmAccountInfoWidget)
{
    ui->setupUi(this);
    tableModelLocal = new KmmAccountModelLocal();
    tableModelRemote = new KmmAccountModelRemote();
}

void KmmAccountInfoWidget::setAccountInfo(const KmmAccountInfo& info)
{
    ui->tableLocal->setModel(tableModelLocal);
    tableModelLocal->setAccountList(info);
    ui->tableLocal->resizeColumnsToContents();
    ui->tableLocal->resizeRowsToContents();
    ui->tableLocal->setMinimumSize(ui->tableLocal->horizontalHeader()->length() + 10, ui->tableLocal->verticalHeader()->length());

    ui->tableRemote->setModel(tableModelRemote);
    tableModelRemote->setMappedBank(info.getMappedBank());
    tableModelRemote->setMappedAccount(info.getMappedAccount());
    ui->tableRemote->resizeColumnsToContents();
    ui->tableRemote->resizeRowsToContents();
    ui->tableRemote->setMinimumSize(ui->tableRemote->horizontalHeader()->length() + 10, ui->tableRemote->verticalHeader()->length());

    ui->tableRemote->setSpan(0,0,1,2);
}

void KmmAccountInfoWidget::setMappedBank(const QString& bank)
{
    tableModelRemote->setMappedBank(bank);
}

void KmmAccountInfoWidget::setMappedAccount(const QString& acc)
{
    tableModelRemote->setMappedAccount(acc);
}

KmmAccountInfoWidget::~KmmAccountInfoWidget()
{
    delete ui;
    delete tableModelLocal;
    delete tableModelRemote;
}
