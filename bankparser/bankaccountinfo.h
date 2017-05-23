/***************************************************************************
                          bankaccountinfo.h
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

#ifndef BANKACCOUNTINFO_H
#define BANKACCOUNTINFO_H

#include <QString>
#include <QDate>
#include <QUrl>

class BankAccountInfo
{
public:
    BankAccountInfo() {}
    BankAccountInfo(const QString name, const QString number, const QString key) :
        Name(name), Number(number), Key(key) {}

    QString getName() const {return Name;};
    QString getKey() const {return Key;};
    QString getNumber() const {return Number;};
    QUrl getUrl() const {return Url;};

    void setName(QString name) {Name = name;};
    void setNumber(QString number) {Number = number;};
    void setKey(QString key) {Key = key;};
    void setId(QString id) {Id = id;};
    void setUrl(QUrl url) {Url = url;};

  private:
    QString Name;
    QString Number;
    QString Id;
    QString Key;
    QString Description;
    QUrl    Url;
};

#endif
