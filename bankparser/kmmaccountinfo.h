/***************************************************************************
                          kmmaccountinfo.h
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

#ifndef KMMACCOUNTINFO_H
#define KMMACCOUNTINFO_H

#include <QString>
#include <QDate>

class KmmAccountInfo
{

public:
    KmmAccountInfo() {}
    KmmAccountInfo(const QString &name, const QString &number, const QString &institution, const QString &id, const QDate &reconciled, const QDate &opened, const QString &mappedBank, const QString &mappedAccount) :
        Name(name), Number(number), Institution(institution), Id(id), ReconciledDate(reconciled), OpenedDate(opened), MappedBank(mappedBank), MappedAccount(mappedAccount) {}
  
    QString getName() const {return Name;};
    QString getNumber() const {return Number;};
    QString getInstitution() const {return Institution;};
    QString getId() const {return Id;};
    QString getReconciledDateStr() const {return ReconciledDate.toString("yyyy-MM-dd");};
    QString getOpenedDateStr() const {return OpenedDate.toString("yyyy-MM-dd");};
    QDate getReconciledDate() const {return ReconciledDate;};

    QString getMappedBank() const {return MappedBank;};
    QString getMappedAccount() const {return MappedAccount;};

    void setMappedBank(const QString &bank) { MappedBank = bank;};
    void setMappedAccount(const QString &acc) { MappedAccount = acc;};

private:
    QString Name;
    QString Number;
    QString Institution;
    QString Id;
    QDate   ReconciledDate;
    QDate   OpenedDate;

    QString MappedBank;
    QString MappedAccount;
};

#endif
