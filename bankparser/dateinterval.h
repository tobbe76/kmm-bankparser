/***************************************************************************
                          dateinterval.h
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

#ifndef DATEINTERVAL_H
#define DATEINTERVAL_H

#include <QDate>
#include <QDebug>

class DateInterval
{
public:
    DateInterval() {};
    DateInterval(const QDate oldestDate, const QDate newestDate) :
        OldestDate(oldestDate), NewestDate(newestDate) {};

    bool isWithinInterval(QDate date) const;
    bool isOlderThenInterval(QDate date) const;

    QDate getOldestDate() const { return OldestDate;};
    QDate getNewestDate() const { return NewestDate;};

private:
    QDate OldestDate;
    QDate NewestDate;

    friend QDebug operator<<(QDebug, const DateInterval &);
};

QDebug operator<<(QDebug, const DateInterval &);

#endif
