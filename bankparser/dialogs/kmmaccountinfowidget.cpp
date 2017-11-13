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
#include <qwebelement.h>
#include <qwebframe.h>

int KmmAccountTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 10;
}

int KmmAccountTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

void KmmAccountTableModel::setAccountList(const KmmAccountInfo& acc)
{
    beginResetModel();
    accInfo = acc;
    endResetModel();
}

void KmmAccountTableModel::setMappedBank(const QString &bank)
{
    accInfo.setMappedBank(bank);
    dataChanged(createIndex(7, 1), createIndex(7, 1));
}

void KmmAccountTableModel::setMappedAccount(const QString &accountKey)
{
    accInfo.setMappedAccount(accountKey);
    dataChanged(createIndex(8, 1), createIndex(8, 1));
}

QVariant KmmAccountTableModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::TextAlignmentRole) {
        if(index.row() == 6)
        {
            return Qt::AlignHCenter;
        }
    }


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
            else if(index.row() == 6)
                return "";
            else if(index.row() == 7)
                return "MappedBank:";
            else if(index.row() == 8)
                return "MappedAccount:";
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
            else if(index.row() == 6)
                return "";
            else if(index.row() == 7)
                return accInfo.getMappedBank();
            else if(index.row() == 8)
                return accInfo.getMappedAccount();
        }

    }
    else if (role == Qt::SizeHintRole) {
        return QVariant();
     //   return QSize(100, 5);
    }
    else
    {
        return QVariant();
    }
    return QVariant();
}

QVariant KmmAccountTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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
    tableModel = new KmmAccountTableModel();
}

void KmmAccountInfoWidget::setAccountInfo(const KmmAccountInfo& info)
{
    ui->tableView->setModel(tableModel);
    tableModel->setAccountList(info);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
    ui->tableView->setMinimumSize(ui->tableView->horizontalHeader()->length() + 10, ui->tableView->verticalHeader()->length());
}

void KmmAccountInfoWidget::setMappedBank(const QString& bank)
{
    tableModel->setMappedBank(bank);
}

void KmmAccountInfoWidget::setMappedAccount(const QString& acc)
{
    tableModel->setMappedAccount(acc);
}

KmmAccountInfoWidget::~KmmAccountInfoWidget()
{
    delete ui;
    delete tableModel;
}
