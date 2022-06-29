/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2020 Davy Triponney                                **
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
        case 0:  keyName = QObject::tr("C", "key name");  break;
        case 1:  keyName = QObject::tr("D♭", "key name"); break;
        case 2:  keyName = QObject::tr("D", "key name");  break;
        case 3:  keyName = QObject::tr("E♭", "key name"); break;
        case 4:  keyName = QObject::tr("E", "key name");  break;
        case 5:  keyName = QObject::tr("F", "key name");  break;
        case 6:  keyName = QObject::tr("G♭", "key name"); break;
        case 7:  keyName = QObject::tr("G", "key name");  break;
        case 8:  keyName = QObject::tr("A♭", "key name"); break;
        case 9:  keyName = QObject::tr("A", "key name");  break;
        case 10: keyName = QObject::tr("B♭", "key name"); break;
        case 11: keyName = QObject::tr("B", "key name");  break;
        }
    }
    else
    {
        switch (keyNum % 12)
        {
        case 0:  keyName = QObject::tr("C", "key name");  break;
        case 1:  keyName = QObject::tr("C♯", "key name"); break;
        case 2:  keyName = QObject::tr("D", "key name");  break;
        case 3:  keyName = QObject::tr("D♯", "key name"); break;
        case 4:  keyName = QObject::tr("E", "key name");  break;
        case 5:  keyName = QObject::tr("F", "key name");  break;
        case 6:  keyName = QObject::tr("F♯", "key name"); break;
        case 7:  keyName = QObject::tr("G", "key name");  break;
        case 8:  keyName = QObject::tr("G♯", "key name"); break;
        case 9:  keyName = QObject::tr("A", "key name");  break;
        case 10: keyName = QObject::tr("A♯", "key name"); break;
        case 11: keyName = QObject::tr("B", "key name");  break;
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
    keyName = keyName
            .replace(",", ".")
            .replace("♯", "#")
            .replace("♭", "b")
            .toLower();
    bool isDouble;
    int note = qRound(keyName.toDouble(&isDouble));
    if (!isDouble && keyName.size() >= 2)
    {
        // Separate the keyname from the octave number
        int split = keyName.indexOf("-");
        if (split == -1)
            split = keyName.size() - 1;
        QString octaveStr = keyName.right(keyName.size() - split);
        keyName = keyName.left(split);

        // Which key?
        if (keyName == QObject::tr("C", "key name").toLower() || keyName == 'c')
            note = 60;
        else if (keyName == QObject::tr("C♯", "key name").toLower().replace("♯", "#") ||
                 keyName == QObject::tr("D♭", "key name").toLower().replace("♭", "b") ||
                 keyName == "c#" || keyName == "db")
            note = 61;
        else if (keyName == QObject::tr("D", "key name").toLower() || keyName == 'd')
            note = 62;
        else if (keyName == QObject::tr("D♯", "key name").toLower().replace("♯", "#") ||
                 keyName == QObject::tr("E♭", "key name").toLower().replace("♭", "b") ||
                 keyName == "d#" || keyName == "eb")
            note = 63;
        else if (keyName == QObject::tr("E", "key name").toLower() || keyName == 'e')
            note = 64;
        else if (keyName == QObject::tr("F", "key name").toLower() || keyName == 'f')
            note = 65;
        else if (keyName == QObject::tr("F♯", "key name").toLower().replace("♯", "#") ||
                 keyName == QObject::tr("G♭", "key name").toLower().replace("♭", "b") ||
                 keyName == "f#" || keyName == "gb")
            note = 66;
        else if (keyName == QObject::tr("G", "key name").toLower() || keyName == 'g')
            note = 67;
        else if (keyName == QObject::tr("G♯", "key name").toLower().replace("♯", "#") ||
                 keyName == QObject::tr("A♭", "key name").toLower().replace("♭", "b") ||
                 keyName == "g#" || keyName == "ab" || keyName == "hb")
            note = 68;
        else if (keyName == QObject::tr("A", "key name").toLower() || keyName == 'a' || keyName == 'h')
            note = 69;
        else if (keyName == QObject::tr("A♯", "key name").toLower().replace("♯", "#") ||
                 keyName == QObject::tr("B♭", "key name").toLower().replace("♭", "b") ||
                 keyName == "a#" || keyName == "h#" || keyName == "bb")
            note = 70;
        else if (keyName == QObject::tr("B", "key name").toLower() || keyName == 'b')
            note = 71;
        else
            return -1;

        // Include the octave
        if (!octaveStr.isEmpty())
        {
            int octave = octaveStr.toInt();
            if ((octave == 0 && octaveStr != "0"))
                return -1;
            else
                note += (octave - 4) * 12;
        }

        if ((_nameMiddleC == MIDDLE_C_C3_WITH_FLATS || _nameMiddleC == MIDDLE_C_C3_WITH_SHARPS) && !forceC4)
            note += 12;
        else if ((_nameMiddleC == MIDDLE_C_C5_WITH_FLATS || _nameMiddleC == MIDDLE_C_C5_WITH_SHARPS) && !forceC4)
            note -= 12;
    }
    return (note > 127 || note < -1) ? -1 : note;
}
