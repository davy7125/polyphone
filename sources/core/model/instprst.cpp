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

#include "instprst.h"
#include "soundfont.h"
#include "utils.h"

InstPrst::InstPrst(Soundfont * soundfont, int row, TreeItem * parent, EltID id) : TreeItem(id, parent),
    _soundfont(soundfont),
    _globalDivision(new Division(nullptr, _soundfont, nullptr, EltID())),
    _row(row)
{
}

InstPrst::~InstPrst()
{
    for (int i = _divisions.indexCount() - 1; i >= 0; i--)
    {
        Division * elt = _divisions.atIndex(i);
        if (elt != nullptr)
        {
            elt->notifyDeletion(false);
            delete _divisions.takeAtIndex(i);
        }
    }

    delete _globalDivision;
}

int InstPrst::addDivision()
{
    EltID childId = this->getId();
    childId.typeElement = (this->getId().typeElement == elementPrst ? elementPrstInst : elementInstSmpl);
    childId.indexElt2 = _divisions.indexCount();

    int index = _divisions.add(new Division(this, _soundfont, this, childId));
    _divisions.atIndex(index)->notifyCreated();
    return index;
}

Division * InstPrst::getDivision(int index)
{
    if (index < _divisions.indexCount())
        return _divisions.atIndex(index);
    return nullptr;
}

bool InstPrst::deleteDivision(int index)
{
    if (index < _divisions.indexCount())
    {
        Division * elt = _divisions.atIndex(index);
        if (elt != nullptr)
        {
            elt->notifyDeletion();
            delete _divisions.takeAtIndex(index);
            return true;
        }
    }

    return false;
}

int InstPrst::indexOfId(int id)
{
    return _divisions.positionOfIndex(id);
}

int InstPrst::childCount() const
{
    return _divisions.positionCount();
}

TreeItem * InstPrst::child(int row)
{
    return _divisions.atPosition(row);
}

QString InstPrst::display()
{
    QString display = "";
    if (_extraFields.contains(champ_wBank) && _extraFields.contains(champ_wPreset))
        display = QString("%1:%2 ").arg(_extraFields[champ_wBank], 3, 10, QChar('0')).arg(_extraFields[champ_wPreset], 3, 10, QChar('0'));
    display += (_name.isEmpty() ? "..." : _name);
    return display;
}

QString InstPrst::sortText()
{
    QString sortText = "";
    if (_extraFields.contains(champ_wBank) && _extraFields.contains(champ_wPreset))
        sortText = QString("%1:%2 ").arg(_extraFields[champ_wBank], 3, 10, QChar('0')).arg(_extraFields[champ_wPreset], 3, 10, QChar('0'));
    sortText += _nameSort;
    return sortText;
}

void InstPrst::setName(QString name)
{
    _name = name;
    _nameSort = Utils::removeAccents(_name).toLower();
    notifyRename();
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
