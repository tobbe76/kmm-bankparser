/***************************************************************************
                          datesdialog.h
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

#ifndef DATESDIALOG_H
#define DATESDIALOG_H

#include <QDialog>
#include <QDebug>
#include "kmmaccountinfo.h"
#include <QDate>
#include "dateinterval.h"

namespace Ui {
class DatesDialog;
}

class DatesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DatesDialog(const KmmAccountInfo &kmmAccountInfo, QWidget *parent = 0);
    ~DatesDialog();

    DateInterval getDateInterval() const;
    bool getUseForAll() const;

private slots:
    void on_buttonBox_accepted();

    void on_useReconciled_toggled(bool checked);

    void on_buttonBox_rejected();

    void on_toDate_dateChanged(const QDate &date);

    void on_fromDate_dateChanged(const QDate &date);

private:
    void setDatesOk();
    Ui::DatesDialog *ui;
    QDate lastReconciled;
};

#endif
