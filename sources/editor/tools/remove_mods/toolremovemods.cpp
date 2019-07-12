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

#include "toolremovemods.h"
#include "soundfontmanager.h"

ToolRemoveMods::ToolRemoveMods() :
    AbstractToolIterating(
        QList<ElementType>() << elementInst << elementPrst << elementSf2, nullptr, nullptr)
{

}

void ToolRemoveMods::beforeProcess(IdList ids)
{
    _deletionType = DeletionGlobal;
    if (!ids.empty())
    {
        switch (ids[0].typeElement)
        {
        case elementInst: case elementInstSmpl:
            _deletionType = DeletionForInstrument;
            break;
        case elementPrst: case elementPrstInst:
            _deletionType = DeletionForPreset;
            break;
        default:
            break;
        }
    }
    _count = 0;
}

void ToolRemoveMods::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    Q_UNUSED(parameters)

    switch (_deletionType)
    {
    case DeletionGlobal: {
        id.typeElement = elementInst;
        EltID idInst(elementInst, id.indexSf2);
        foreach (int i, sm->getSiblings(idInst))
            clearModInst(sm, EltID(elementInst, id.indexSf2, i));
        EltID idPrst(elementPrst, id.indexSf2);
        foreach (int i, sm->getSiblings(idPrst))
            clearModPrst(sm, EltID(elementPrst, id.indexSf2, i));
    } break;
    case DeletionForInstrument:
        id.typeElement = elementInst;
        clearModInst(sm, id);
        break;
    case DeletionForPreset:
        id.typeElement = elementPrst;
        clearModPrst(sm, id);
        break;
    }
}

void ToolRemoveMods::clearModInst(SoundfontManager *sm, EltID idInst)
{
    // Mods in the global division
    clearMod(sm, EltID(elementInstMod, idInst.indexSf2, idInst.indexElt));

    // Mods in each division linked to an element
    EltID idInstSmpl(elementInstSmpl, idInst.indexSf2, idInst.indexElt);
    foreach (int i, sm->getSiblings(idInstSmpl))
        clearMod(sm, EltID(elementInstSmplMod, idInst.indexSf2, idInst.indexElt, i));
}

void ToolRemoveMods::clearModPrst(SoundfontManager *sm, EltID idPrst)
{
    // Mods in the global division
    clearMod(sm, EltID(elementPrstMod, idPrst.indexSf2, idPrst.indexElt));

    // Mods in each division linked to an element
    EltID idPrstInst(elementPrstInst, idPrst.indexSf2, idPrst.indexElt);
    foreach (int i, sm->getSiblings(idPrstInst))
        clearMod(sm, EltID(elementPrstInstMod, idPrst.indexSf2, idPrst.indexElt, i));
}

void ToolRemoveMods::clearMod(SoundfontManager * sm, EltID idMod)
{
    foreach (int i, sm->getSiblings(idMod))
    {
        idMod.indexMod = i;
        sm->remove(idMod);
        _count++;
    }
}

QString ToolRemoveMods::getConfirmation()
{
    if (_count == 1)
        return trUtf8("1 modulator has been deleted.");
    else if (_count > 1)
        return trUtf8("%1 modulators have been deleted.").arg(QString::number(_count));
    return "";
}

QString ToolRemoveMods::getWarning()
{
    if (_count == 0)
        return trUtf8("The selection contains no modulators.");
    return "";
}
