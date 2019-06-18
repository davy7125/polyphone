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

#include "soundfonts.h"
#include "soundfont.h"

Soundfonts::Soundfonts() :
    _soundfontCounter(0)
{

}

Soundfonts::~Soundfonts()
{
    QList<int> keys = _soundfonts.keys();
    foreach (int key, keys)
        delete _soundfonts.take(key);
}

int Soundfonts::addSoundfont()
{
    _soundfonts[_soundfontCounter] = new Soundfont(EltID(elementSf2, _soundfontCounter, -1, -1, -1));
    return _soundfontCounter++;
}

Soundfont * Soundfonts::getSoundfont(int index)
{
    if (_soundfonts.contains(index))
        return _soundfonts[index];
    return nullptr;
}

bool Soundfonts::deleteSoundfont(int index)
{
    if (_soundfonts.contains(index))
    {
        delete _soundfonts.take(index);
        return true;
    }
    return false;
}

int Soundfonts::indexOf(Soundfont * soundfont)
{
    return _soundfonts.values().indexOf(soundfont);
}

QAbstractItemModel * Soundfonts::getModel(int indexSf2)
{
    if (_soundfonts.contains(indexSf2))
        return _soundfonts[indexSf2]->getModel();
    return nullptr;
}
