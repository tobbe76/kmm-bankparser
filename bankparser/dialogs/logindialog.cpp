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
#include <qwebelement.h>
#include <qwebframe.h>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
}

void LoginDialog::loginBank(LoginInterface *bank)
{
    currentBank = bank;
    connect(currentBank, SIGNAL(loginFinished(bool)), this, SLOT(loginFinished(bool)), Qt::QueuedConnection);
    bank->login(ui->webView);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::loginFinished(bool result)
{
  qDebug() << "LoginDialog::loginFinished start result " << result;

  disconnect(currentBank, SIGNAL(loginFinished(bool)), this, SLOT(loginFinished(bool)));

  done(0);

  qDebug() << "LoginDialog::loginFinished end";
}
