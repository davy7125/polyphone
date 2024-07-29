/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef UTILS_H
#define UTILS_H

#include <QString>

class Utils
{
public:
    static void prepareStaticVariables();

    /// Return the natural comparison between two strings
    /// -1 if a should be before b, 0 if equals, 1 is a should be after b
    /// This is case insensitive
    static int naturalOrder(QString a, QString b);

    /// Remove all accents
    /// Can be useful for sorting strings
    static QString removeAccents(QString s);

    /// Return the first letters in common from a list of string
    static QString commonPart(QStringList list);

    /// Encrypt / decrypt data
    static QString rsaEncrypt(QString input);

    /// Conversion functions
    static qint8 round8(double value);
    static qint16 round16(double value);
    static qint32 round32(double value);
    static qint32 floatToInt24(float f);
    static float int24ToFloat(qint32 i);

    static QString fixFilePath(QString filePath);

    static bool isValidUtf8(QByteArray data);

private:
    static int getNumberPart(const QString &str, int &length);
    static QString removeSuffix(QString txt);

    static QString s_diacriticLetters;
    static QStringList s_noDiacriticLetters;
    static float s_floatConversionCoef;
};

#endif // UTILS_H
