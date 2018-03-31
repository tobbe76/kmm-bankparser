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

#include "logindialog.h"
#include "ui_logindialog.h"
#include "debugwebpage.h"

BrowserLoginDialog::BrowserLoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BrowserLoginDialog)
{
    ui->setupUi(this);
    isOpen = false;
}

void BrowserLoginDialog::openLoginDialog(DebugWebPage *webpage, QUrl url)
{
    isOpen = true;
    webpage->setUrl(QUrl("about:blank"));
    ui->webView->setPage(webpage);
    ui->webView->load(url);
    setModal(true);
    open();
}

BrowserLoginDialog::~BrowserLoginDialog()
{
    delete ui;
}

void BrowserLoginDialog::closeLoginDialog(void)
{
    done(0);
}

bool BrowserLoginDialog::isDialogOpen(void)
{
    return isOpen;
}

void BrowserLoginDialog::on_BrowserLoginDialog_finished(int result)
{
    Q_UNUSED(result);
    isOpen = false;
}
