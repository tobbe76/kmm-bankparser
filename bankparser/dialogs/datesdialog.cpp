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

#include "datesdialog.h"
#include "ui_datesdialog.h"
#include <qwebelement.h>
#include <qwebframe.h>

DatesDialog::DatesDialog(const KmmAccountInfo &kmmAccountInfo, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DatesDialog)
{
    ui->setupUi(this);

    ui->widget->setAccountInfo(kmmAccountInfo);
    lastReconciled = kmmAccountInfo.getReconciledDate();
    ui->fromDate->setDate(lastReconciled);
    ui->toDate->setDate(QDate::currentDate());
}


DatesDialog::~DatesDialog()
{
    delete ui;
}

void DatesDialog::on_buttonBox_accepted()
{
    accept();
}

DateInterval DatesDialog::getDateInterval() const
{
    return DateInterval(ui->fromDate->date(), ui->toDate->date());
}

bool DatesDialog::getUseForAll() const
{
    return ui->useForAllAccounts->isChecked();
}

void DatesDialog::on_useReconciled_toggled(bool checked)
{
    ui->fromDate->setDisabled(checked);
    if(checked)
    {
        ui->fromDate->setDate(lastReconciled.addDays(1));
    }
}

void DatesDialog::on_buttonBox_rejected()
{
    reject();
}
