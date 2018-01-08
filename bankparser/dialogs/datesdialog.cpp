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

void DatesDialog::setDatesOk() {
    QPalette palette;
    palette.setColor(QPalette::Text,Qt::black);
    ui->toDate->setPalette(palette);
    ui->fromDate->setPalette(palette);
    ui->fromDate->setToolTip("");
    ui->toDate->setToolTip("");
}

void DatesDialog::on_toDate_dateChanged(const QDate &date)
{
    Q_UNUSED(date);
    if(ui->fromDate->date() <= ui->toDate->date()) {
        setDatesOk();
    }
    else {
        QPalette palette;
        palette.setColor(QPalette::Text,Qt::red);
        ui->toDate->setPalette(palette);
        ui->toDate->setToolTip("Should be later than from date");
    }
}

void DatesDialog::on_fromDate_dateChanged(const QDate &date)
{
    Q_UNUSED(date);

    if(ui->fromDate->date() <= ui->toDate->date()) {
        setDatesOk();
    }
    else {
        QPalette palette;
        palette.setColor(QPalette::Text,Qt::red);
        ui->fromDate->setPalette(palette);
        ui->fromDate->setToolTip("Should be earlier than to date");
    }
}
