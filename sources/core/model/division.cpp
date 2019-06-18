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

#include "division.h"
#include "modulator.h"
#include "instprst.h"
#include "soundfont.h"
#include "smpl.h"

Division::Division(InstPrst * instPrst, Soundfont * soundfont, TreeItem * parent, EltID id) : TreeItem(id, parent),
    _instPrst(instPrst),
    _soundfont(soundfont),
    _mute(false)
{}

Division::~Division()
{
    for (int i = _modulators.indexCount() - 1; i >= 0; i--)
    {
        Modulator * elt = _modulators.atIndex(i);
        if (elt != nullptr)
            delete _modulators.takeAtIndex(i);
    }
}

bool Division::isSet(AttributeType champ)
{
    return _parameters.contains(champ);
}

AttributeValue Division::getGen(AttributeType champ)
{
    AttributeValue value;
    if (_parameters.contains(champ))
        value = _parameters[champ];
    else
        value.wValue = 0;
    return value;
}

void Division::setGen(AttributeType champ, AttributeValue value)
{
    _parameters[champ] = value;
    if (champ == champ_sampleID || champ == champ_instrument)
        notifyRename();
}

void Division::resetGen(AttributeType champ)
{
    _parameters.remove(champ);
}

int Division::addMod()
{
    return _modulators.add(new Modulator(_modulators.indexCount()));
}

Modulator * Division::getMod(int index)
{
    if (index < _modulators.indexCount())
        return _modulators.atIndex(index);
    return nullptr;
}

bool Division::deleteMod(int index)
{
    if (index < _modulators.indexCount())
    {
        delete _modulators.takeAtIndex(index);
        return true;
    }
    return false;
}

int Division::childCount() const
{
    return 0;
}

TreeItem * Division::child(int row)
{
    Q_UNUSED(row)
    return nullptr;
}

QString Division::display()
{
    QString display = "";

    // Take the name of the corresponding element
    ElementType type = this->getId().typeElement;
    if (type == elementInstSmpl && isSet(champ_sampleID))
    {
        Smpl * smpl = _soundfont->getSample(getGen(champ_sampleID).wValue);
        if (smpl != nullptr)
            display = smpl->display();
    }
    else if (type == elementPrstInst && isSet(champ_instrument))
    {
        InstPrst * inst = _soundfont->getInstrument(getGen(champ_instrument).wValue);
        if (inst != nullptr)
            display = inst->display();
    }

    return display.isEmpty() ? "..." : display;
}

int Division::row()
{
    // Shouldn't be used (parent->row() will never point on a division)
    return _instPrst->indexOfId(_id.indexElt);
}

int Division::indexOfId(int id)
{
    Q_UNUSED(id)
    return -1;
}
