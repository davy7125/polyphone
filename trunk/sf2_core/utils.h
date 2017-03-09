/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2017 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://polyphone-soundfonts.com                      **
**             Date: 19.09.2016                                           **
***************************************************************************/

#ifndef UTILS_H
#define UTILS_H

#include <QString>

class Utils
{
public:
    /// Return the natural comparison between two strings
    /// -1 if a should be before b, 0 if equals, 1 is a should be after b
    /// This is case insensitive
    static int naturalOrder(const QString &a, const QString &b);

private:
    static int getNumberPart(const QString &str, int &length);
};

#endif // UTILS_H
