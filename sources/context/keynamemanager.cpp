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

#include "keynamemanager.h"
#include "confmanager.h"


KeyNameManager::KeyNameManager(ConfManager * configuration) :
    _configuration(configuration)
{
    _nameMiddleC = static_cast<NameMiddleC>(
                _configuration->getValue(ConfManager::SECTION_DISPLAY, "name_middle_c", 0).toInt());
}

void KeyNameManager::setMiddleKey(NameMiddleC name)
{
    _nameMiddleC = name;
    _configuration->setValue(ConfManager::SECTION_DISPLAY, "name_middle_c", static_cast<int>(_nameMiddleC));
}

QString KeyNameManager::getKeyName(unsigned int keyNum, bool forceTexte, bool with0, bool forceC4, bool noOctave)
{
    if (_nameMiddleC == MIDDLE_C_60 && !forceTexte)
    {
        if (with0)
        {
            if (keyNum < 10)
                return "00" + QString::number(keyNum);
            else if (keyNum < 100)
                return "0" + QString::number(keyNum);
            else
                return QString::number(keyNum);
        }
        else
            return QString::number(keyNum);
    }

    QString keyName;
    if (_nameMiddleC == MIDDLE_C_C3_WITH_FLATS || _nameMiddleC == MIDDLE_C_C4_WITH_FLATS || _nameMiddleC == MIDDLE_C_C5_WITH_FLATS)
    {
        switch (keyNum % 12)
        {
        case 0:  keyName = QObject::trUtf8("C", "key name");  break;
        case 1:  keyName = QObject::trUtf8("D♭", "key name"); break;
        case 2:  keyName = QObject::trUtf8("D", "key name");  break;
        case 3:  keyName = QObject::trUtf8("E♭", "key name"); break;
        case 4:  keyName = QObject::trUtf8("E", "key name");  break;
        case 5:  keyName = QObject::trUtf8("F", "key name");  break;
        case 6:  keyName = QObject::trUtf8("G♭", "key name"); break;
        case 7:  keyName = QObject::trUtf8("G", "key name");  break;
        case 8:  keyName = QObject::trUtf8("A♭", "key name"); break;
        case 9:  keyName = QObject::trUtf8("A", "key name");  break;
        case 10: keyName = QObject::trUtf8("B♭", "key name"); break;
        case 11: keyName = QObject::trUtf8("B", "key name");  break;
        }
    }
    else
    {
        switch (keyNum % 12)
        {
        case 0:  keyName = QObject::trUtf8("C", "key name");  break;
        case 1:  keyName = QObject::trUtf8("C♯", "key name"); break;
        case 2:  keyName = QObject::trUtf8("D", "key name");  break;
        case 3:  keyName = QObject::trUtf8("D♯", "key name"); break;
        case 4:  keyName = QObject::trUtf8("E", "key name");  break;
        case 5:  keyName = QObject::trUtf8("F", "key name");  break;
        case 6:  keyName = QObject::trUtf8("F♯", "key name"); break;
        case 7:  keyName = QObject::trUtf8("G", "key name");  break;
        case 8:  keyName = QObject::trUtf8("G♯", "key name"); break;
        case 9:  keyName = QObject::trUtf8("A", "key name");  break;
        case 10: keyName = QObject::trUtf8("A♯", "key name"); break;
        case 11: keyName = QObject::trUtf8("B", "key name");  break;
        }
    }

    if (!noOctave)
    {
        int numOctave = (keyNum) / 12 - 1;
        if ((_nameMiddleC == MIDDLE_C_C3_WITH_FLATS || _nameMiddleC == MIDDLE_C_C3_WITH_SHARPS) && !forceC4)
            numOctave -= 1;
        else if ((_nameMiddleC == MIDDLE_C_C5_WITH_FLATS || _nameMiddleC == MIDDLE_C_C5_WITH_SHARPS) && !forceC4)
            numOctave += 1;
        keyName += QString::number(numOctave);
    }

    return keyName;
}

int KeyNameManager::getKeyNum(QString keyName, bool forceC4)
{
    keyName = keyName.replace(",", ".").toLower();
    int note = qRound(keyName.toDouble());
    if (note == 0 && keyName != "0" && keyName.size() >= 2)
    {
        switch (keyName.at(0).unicode())
        {
        case 'c': note = 60; break;
        case 'd': note = 62; break;
        case 'e': note = 64; break;
        case 'f': note = 65; break;
        case 'g': note = 67; break;
        case 'a': case 'h': note = 69; break;
        case 'b': note = 71; break;
        default : return -1;
        }
        keyName = keyName.right(keyName.size() - 1);
        if (keyName.at(0).unicode() == '#' || keyName.at(0) == QString::fromUtf8("♯").at(0))
        {
            note++;
            keyName = keyName.right(keyName.size() - 1);
        }
        else if (keyName.at(0).unicode() == 'b' || keyName.at(0) == QString::fromUtf8("♭").at(0))
        {
            note --;
            keyName = keyName.right(keyName.size() - 1);
        }

        if (!keyName.isEmpty())
        {
            int octave = keyName.toInt();
            if ((octave == 0 && keyName != "0"))
                return -1;
            else
                note += (octave - 4) * 12;
        }

        if ((_nameMiddleC == MIDDLE_C_C3_WITH_FLATS || _nameMiddleC == MIDDLE_C_C3_WITH_SHARPS) && !forceC4)
            note += 12;
        else if ((_nameMiddleC == MIDDLE_C_C5_WITH_FLATS || _nameMiddleC == MIDDLE_C_C5_WITH_SHARPS) && !forceC4)
            note -= 12;
    }
    return note;
}
