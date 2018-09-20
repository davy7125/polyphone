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

#include "utils.h"
#include <QStringList>

QString Utils::s_diacriticLetters;
QStringList Utils::s_noDiacriticLetters;

int Utils::naturalOrder(const QString &a, const QString &b)
{
    QString str1 = a.toLower();
    QString str2 = b.toLower();

    while (!str1.isEmpty() && !str2.isEmpty())
    {
        // Find the number part
        int numberLength1 = 0, numberLength2 = 0;
        int number1 = getNumberPart(str1, numberLength1);
        int number2 = getNumberPart(str2, numberLength2);

        if (numberLength1 > 0 && numberLength2 > 0)
        {
            // Two numbers to compare
            if (number1 == number2)
            {
                // We skip the numbers and continue the reading
                str1 = str1.right(str1.length() - numberLength1);
                str2 = str2.right(str2.length() - numberLength2);
            }
            else
                return number1 - number2;
        }
        else if (numberLength1 == 0 && numberLength2 == 0)
        {
            // Two letters to compare
            if (str1[0] == str2[0])
            {
                // We skip the first letter and continue the reading
                str1 = str1.right(str1.length() - 1);
                str2 = str2.right(str2.length() - 1);
            }
            else
                return str1[0].toLatin1() - str2[0].toLatin1();
        }
        else
        {
            // One number for one string, one letter for the other
            return str1[0].toLatin1() - str2[0].toLatin1();
        }
    }

    if (!str1.isEmpty())
        return 1;
    if (!str2.isEmpty())
        return -1;

    return 0;
}

int Utils::getNumberPart(const QString &str, int &length)
{
    // Find all characters that are numbers
    length = 0;
    while (length < str.length() && str[length].isNumber())
        length++;
    if (length > 0)
        return str.left(length).toInt();
    return 0;
}

QString Utils::removeAccents(QString s)
{
    if (s_diacriticLetters.isEmpty())
    {
        s_diacriticLetters = QString::fromUtf8("ŠŒŽšœžŸ¥µÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝßàáâãäåæçèéêëìíîïðñòóôõöøùúûüýÿ");
        s_noDiacriticLetters << "S"<<"OE"<<"Z"<<"s"<<"oe"<<"z"<<"Y"<<"Y"<<"u"<<"A"<<"A"<<"A"<<"A"<<"A"<<"A"<<"AE"<<"C"<<"E"
                            <<"E"<<"E"<<"E"<<"I"<<"I"<<"I"<<"I"<<"D"<<"N"<<"O"<<"O"<<"O"<<"O"<<"O"<<"O"<<"U"<<"U"<<"U"<<"U"
                           <<"Y"<<"s"<<"a"<<"a"<<"a"<<"a"<<"a"<<"a"<<"ae"<<"c"<<"e"<<"e"<<"e"<<"e"<<"i"<<"i"<<"i"<<"i"<<"o"
                          <<"n"<<"o"<<"o"<<"o"<<"o"<<"o"<<"o"<<"u"<<"u"<<"u"<<"u"<<"y"<<"y";
    }

    QString output = "";
    for (int i = 0; i < s.length(); i++)
    {
        QChar c = s[i];
        int dIndex = s_diacriticLetters.indexOf(c);
        if (dIndex < 0) {
            output.append(c);
        } else {
            QString replacement = s_noDiacriticLetters[dIndex];
            output.append(replacement);
        }
    }

    return output;
}
