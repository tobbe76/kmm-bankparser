/***************************************************************************
                          logindialog.h
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

#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QDebug>
#include "debugwebpage.h"

namespace Ui {
class BrowserLoginDialog;
}

class BrowserLoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BrowserLoginDialog(QWidget *parent = 0);
    ~BrowserLoginDialog();

    void openLoginDialog(DebugWebPage *webpage, QUrl url);
    void closeLoginDialog(void);
    bool isDialogOpen();

private slots:
    void on_BrowserLoginDialog_finished(int result);

private:
    Ui::BrowserLoginDialog *ui;
    bool isOpen;
};

#endif // LOGINDIALOG_H
