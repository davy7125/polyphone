/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2018 Davy Triponney                                **
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

#include "instprst.h"
#include "soundfont.h"

InstPrst::InstPrst(Soundfont * soundfont, TreeItem * parent, EltID id) : TreeItem(id, parent),
    _soundfont(soundfont),
    _globalDivision(new Division(NULL, NULL, EltID())),
    _divisionCounter(0)
{
}

InstPrst::~InstPrst()
{
    QList<int> keys = _divisions.keys();
    foreach (int key, keys)
    {
        _divisions[key]->notifyDeletion(false);
        delete _divisions.take(key);
    }
    delete _globalDivision;
}

int InstPrst::addDivision()
{
    EltID childId = this->getId();
    childId.typeElement = (this->getId().typeElement == elementPrst ? elementPrstInst : elementInstSmpl);
    childId.indexElt2 = _divisionCounter;

    _divisions[_divisionCounter] = new Division(this, this, childId);
    _divisions[_divisionCounter]->notifyCreated();
    return _divisionCounter++;
}

Division * InstPrst::getDivision(int index)
{
    if (_divisions.contains(index))
        return _divisions[index];
    return nullptr;
}

bool InstPrst::deleteDivision(int index)
{
    if (_divisions.contains(index))
    {
        _divisions[index]->notifyDeletion();
        delete _divisions.take(index);
        return true;
    }
    return false;
}

int InstPrst::indexOfId(int id)
{
    return _divisions.keys().indexOf(id);
}

int InstPrst::childCount() const
{
    return _divisions.count();
}

TreeItem * InstPrst::child(int row)
{
    return *std::next(_divisions.begin(), row);
}

QString InstPrst::display()
{
    QString display = "";
    if (_extraFields.contains(champ_wBank) && _extraFields.contains(champ_wPreset))
        display = QString("%1:%2 ").arg(_extraFields[champ_wBank], 3, 10, QChar('0')).arg(_extraFields[champ_wPreset], 3, 10, QChar('0'));
    display += (_name.isEmpty() ? "..." : _name);
    return display;
}

int InstPrst::row()
{
    // Preset?
    if (this->getId().typeElement == elementPrst)
        return _soundfont->indexOfPreset(this);
    return _soundfont->indexOfInstrument(this);
}

void InstPrst::setName(QString name)
{
    _name = name;
    notifyRename();
}

QString InstPrst::getName()
{
    return _name;
}

void InstPrst::setExtraField(AttributeType champ, int value)
{
    _extraFields[champ] = value;
    if (champ == champ_wPreset || champ == champ_wBank)
        notifyRename();
}

int InstPrst::getExtraField(AttributeType champ)
{
    return _extraFields.contains(champ) ? _extraFields[champ] : 0;
}
