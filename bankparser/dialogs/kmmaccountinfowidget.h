/***************************************************************************
                          kmmaccountinfowidget.h
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

#ifndef KMMACCOUNTINFOWIDGET_H
#define KMMACCOUNTINFOWIDGET_H

#include <QWidget>
#include <QDebug>
#include "kmmaccountinfo.h"
#include <QAbstractTableModel>

class KmmAccountTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    KmmAccountTableModel(QObject *parent = 0) : QAbstractTableModel(parent) {}
    void setAccountList(const KmmAccountInfo& acc);
    void setMappedBank(const QString& bank);
    void setMappedAccount(const QString &accountKey);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

private:
    KmmAccountInfo accInfo;
};


namespace Ui {
class KmmAccountInfoWidget;
}

class KmmAccountInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KmmAccountInfoWidget(QWidget *parent = 0);
    ~KmmAccountInfoWidget();
    void setAccountInfo(const KmmAccountInfo& info);

    void setMappedBank(const QString &bank);
    void setMappedAccount(const QString &acc);


private:
    Ui::KmmAccountInfoWidget *ui;
    KmmAccountTableModel* tableModel;
};

#endif
