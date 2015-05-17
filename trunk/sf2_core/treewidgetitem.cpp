/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
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
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "treewidgetitem.h"

bool TreeWidgetItem::operator < (const QTreeWidgetItem &other)const
{
    int column = treeWidget()->sortColumn();

    QString s1 = this->text(column).toLower();
    QString s2 = other.text(column).toLower();

    // ignore common prefix..
    int i = 0;
    while ((i < s1.length()) && (i < s2.length()) && (s1.at(i).toLower() == s2.at(i).toLower()))
        ++i;
    ++i;

    // something left to compare?
    if ((i < s1.length()) && (i < s2.length()))
    {
        // get number prefix from position i - doesnt matter from which string
        int k = i-1;
        // If not number return native comparator
        if(!s1.at(k).isNumber() || !s2.at(k).isNumber())
        {
            //Two next lines
            //E.g. 1_... < 12_...
            if(s1.at(k).isNumber())
                return false;
            if(s2.at(k).isNumber())
                return true;
            return QString::compare(s1, s2, Qt::CaseSensitive) < 0;
        }
        QString n = "";
        k--;
        while ((k >= 0) && (s1.at(k).isNumber()))
        {
            n = s1.at(k)+n;
            --k;
        }
        // get relevant/signficant number string for s1
        k = i-1;
        QString n1 = "";
        while ((k < s1.length()) && (s1.at(k).isNumber()))
        {
            n1 += s1.at(k);
            ++k;
        }

        // get relevant/signficant number string for s2
        // Decrease by
        k = i-1;
        QString n2 = "";
        while ((k < s2.length()) && (s2.at(k).isNumber()))
        {
            n2 += s2.at(k);
            ++k;
        }

        // got two numbers to compare?
        if (!n1.isEmpty() && !n2.isEmpty())
            return (n+n1).toInt() < (n+n2).toInt();
        else
        {
            // not a number has to win over a number.. number could have ended earlier... same prefix..
            if (!n1.isEmpty())
                return false;
            if (!n2.isEmpty())
                return true;
            return s1.at(i) < s2.at(i);
        }
    }
    else
    {
        // shortest string wins
        return s1.length() < s2.length();
    }
}
