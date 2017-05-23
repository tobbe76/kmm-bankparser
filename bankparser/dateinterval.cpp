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
#include "dateinterval.h"

bool DateInterval::isWithinInterval(QDate date) const
{
    return ((OldestDate <= date) && (date <= NewestDate));
}

bool DateInterval::isOlderThenInterval(QDate date) const
{
    return (date < OldestDate);
}

QDebug operator<<(QDebug q, const DateInterval & d)
{
    q << d.getOldestDate() << "---" << d.getNewestDate();
    return q;
}
