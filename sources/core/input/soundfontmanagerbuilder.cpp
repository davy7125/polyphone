/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2026 Davy Triponney                                **
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


#include "soundfontmanagerbuilder.h"
#include "soundfontmanager.h"
#include "sound.h"

SoundfontManagerBuilder::SoundfontManagerBuilder(SoundfontManager * sm, int sf2Index) :
    _sm(sm),
    _sf2Index(sf2Index)
{}

EltID SoundfontManagerBuilder::toEltId(const Id &id) const
{
    return EltID(static_cast<ElementType>(id.type), _sf2Index, id.element, id.zone, id.modulator);
}

int SoundfontManagerBuilder::add(const Id &id)
{
    if (id.type == TYPE_SF2)
    {
        _sf2Index = _sm->add(EltID(elementSf2));
        return _sf2Index;
    }
    return _sm->add(toEltId(id));
}

void SoundfontManagerBuilder::set(const Id &id, AttributeType champ, AttributeValue value)
{
    _sm->set(toEltId(id), champ, value);
}

void SoundfontManagerBuilder::setText(const Id &id, AttributeType champ, const std::string &text)
{
    _sm->set(toEltId(id), champ, QString::fromStdString(text));
}

bool SoundfontManagerBuilder::isSet(const Id &id, AttributeType champ)
{
    return _sm->isSet(toEltId(id), champ);
}

AttributeValue SoundfontManagerBuilder::get(const Id &id, AttributeType champ)
{
    return _sm->get(toEltId(id), champ);
}

std::string SoundfontManagerBuilder::getText(const Id &id, AttributeType champ)
{
    return _sm->getQstr(toEltId(id), champ).toStdString();
}

void SoundfontManagerBuilder::reset(const Id &id, AttributeType champ)
{
    _sm->reset(toEltId(id), champ);
}

void SoundfontManagerBuilder::setSoundProperty(const Id &id, AttributeType champ, AttributeValue value)
{
    _sm->getSound(toEltId(id))->set(champ, value);
}

std::vector<int> SoundfontManagerBuilder::getSiblings(const Id &id)
{
    EltID eltId = toEltId(id);
    QList<int> siblings = _sm->getSiblings(eltId);
    return std::vector<int>(siblings.begin(), siblings.end());
}

bool SoundfontManagerBuilder::isAvailable(int bank, int preset)
{
    return _sm->isAvailable(EltID(elementSf2, _sf2Index), static_cast<quint16>(bank), static_cast<quint16>(preset));
}

int SoundfontManagerBuilder::closestAvailablePreset(int bank, int preset)
{
    return _sm->closestAvailablePreset(EltID(elementSf2, _sf2Index), static_cast<quint16>(bank),
                                       static_cast<quint16>(preset));
}

void SoundfontManagerBuilder::simplify(const Id &id, AttributeType champ)
{
    _sm->simplify(toEltId(id), champ);
}
