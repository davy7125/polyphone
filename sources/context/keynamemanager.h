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

#ifndef KEYNAMEMANAGER_H
#define KEYNAMEMANAGER_H

#include <QString>
class ConfManager;

class KeyNameManager
{
public:
    enum NameMiddleC
    {
        MIDDLE_C_60,
        MIDDLE_C_C3_WITH_SHARPS,
        MIDDLE_C_C3_WITH_FLATS,
        MIDDLE_C_C4_WITH_SHARPS,
        MIDDLE_C_C4_WITH_FLATS,
        MIDDLE_C_C5_WITH_SHARPS,
        MIDDLE_C_C5_WITH_FLATS
    };

    KeyNameManager(ConfManager * configuration);

    /// Set the name of the middle key C
    void setMiddleKey(NameMiddleC name);

    /// Get the name of the middle key C
    NameMiddleC getNameMiddleC() { return _nameMiddleC; }

    /// Get a key name
    /// only with text if forceTexte is true
    /// with additional 0 (for example key 002 instead of key 2 if with0 is true)
    /// considering C4 as the middle C if forceC4 is true
    /// otherwise take the way the user configured it with NameMiddleC
    QString getKeyName(unsigned int keyNum, bool forceTexte = false, bool with0 = false,
                       bool forceC4 = false, bool noOctave = false);

    /// Get the number of the key based on a text
    /// considering C4 as the middle C if forceC4 is true
    /// otherwise take the NameMiddleC
    int getKeyNum(QString keyName, bool forceC4 = false);

private:
    ConfManager * _configuration;
    NameMiddleC _nameMiddleC;
};

#endif // KEYNAMEMANAGER_H
