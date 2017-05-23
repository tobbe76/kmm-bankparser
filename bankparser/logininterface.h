/***************************************************************************
                          logininterface.h
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

#ifndef LOGININTERFACE_H
#define LOGININTERFACE_H

#include <QUrl>
#include <QWebView>

class LoginInterface : public QObject
{

 public:
  LoginInterface() {};
  virtual ~LoginInterface() {};
  
  /* Called from the login dialog, should create a QWebPage and add it to the view */
  virtual bool login(QWebView* view) = 0;

signals:
  virtual void loginFinished(bool result) = 0;
};

#endif
