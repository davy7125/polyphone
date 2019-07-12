/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program. If not, see http://www.gnu.org/licenses/.    **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include "eltid.h"
class SoundfontManager;

class Utils
{
public:
    /// Return the natural comparison between two strings
    /// -1 if a should be before b, 0 if equals, 1 is a should be after b
    /// This is case insensitive
    static int naturalOrder(QString a, QString b);
    static int sortDivisions(EltID id1, EltID id2, int sortType);

    /// Remove all accents
    /// Can be useful for sorting strings
    static QString removeAccents(QString s);

    /// Encrypt / decrypt data
    static QString rsaEncrypt(QString input);
    static QString rsaDecrypt(QString input);

    /// Conversion functions
    static void prepareConversionTables(); // Call it once before concave(..) or convex(..)
    static double concave(double value);
    static double convex(double value);

private:
    static int getNumberPart(const QString &str, int &length);
    static int compareKey(SoundfontManager *sm, EltID idDiv1, EltID idDiv2);
    static int compareVelocity(SoundfontManager *sm, EltID idDiv1, EltID idDiv2);
    static int compareName(SoundfontManager *sm, EltID idDiv1, EltID idDiv2);

    static QString s_diacriticLetters;
    static QStringList s_noDiacriticLetters;

    static double s_concaveTable[128];
    static double s_convexTable[128];
};

#endif // UTILS_H
