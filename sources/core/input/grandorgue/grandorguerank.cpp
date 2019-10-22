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

#include "grandorguerank.h"
#include "grandorguepipe.h"
#include "soundfontmanager.h"

GrandOrgueRank::GrandOrgueRank(QString rootDir) :
    _rootDir(rootDir)
{

}

GrandOrgueRank::~GrandOrgueRank()
{
    while (!_pipes.isEmpty())
        delete _pipes.take(_pipes.firstKey());
}

void GrandOrgueRank::processData(QString key, QString value)
{
    if (key.startsWith("pipe"))
    {
        if (key.length() < 7)
            return;

        // Extract the number of the pipe
        key = key.right(key.length() - 4);
        bool ok = false;
        int number = key.left(3).toInt(&ok);
        if (!ok || number < 0)
            return;

        // Property
        QString property = key.length() > 3 ? key.right(key.length() - 3) : "#";

        // Store data
        if (!_pipes.contains(number))
            _pipes[number] = new GrandOrguePipe(_rootDir);
        _pipes[number]->processData(property, value);
    }
    else
        _properties[key] = value;
}

void GrandOrgueRank::createInstrument(SoundfontManager * sm, EltID idSf2)
{
    // At least one valid pipe?
    bool ok = false;
    foreach (GrandOrguePipe * pipe, _pipes.values())
    {
        if (pipe->isValid())
        {
            ok = true;
            break;
        }
    }
    if (!ok)
        return;

    // New instrument
    EltID idInst = idSf2;
    idInst.typeElement = elementInst;
    idInst.indexElt = sm->add(idInst);

    // Name
    sm->set(idInst, champ_name, _properties.contains("name") ? _properties["name"] : "no name");

    // Associate samples

    //Name=Contrebasse 16'
    //Pipe001PitchTuning=-4
    //Pipe001Gain=3
    //Pipe003=.\om_flautat\flautat_03.wav
    //FirstAccessibleKeyMIDINoteNumber
    //FirstMidiNoteNumber
    //Pipe010AmplitudeLevel=130

    //Référence vers pipe existant:
    //Pipe002=REF:001:001:002
}
