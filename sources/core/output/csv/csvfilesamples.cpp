/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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

#include "csvfilesamples.h"
#include "soundfontmanager.h"
#include "soundfonts.h"
#include "soundfont.h"
#include "smpl.h"

CsvFileSamples::CsvFileSamples(QString filePath) : AbstractCsvFile(filePath) {}

QList<QString> CsvFileSamples::getHeaders()
{
    QList<QString> headers;
    headers << "id"
            << "name"
            << "sample type"
            << "sample link id"
            << "pitch"
            << "pitch correction (cents)"
            << "length"
            << "start loop"
            << "end loop"
            << "sample rate"
            << "bits per sample";
    return headers;
}

bool CsvFileSamples::writeSamples(SoundfontManager * sm, EltID idSf2)
{
    QVectorIterator<Smpl*> i(sm->getSoundfonts()->getSoundfont(idSf2.indexSf2)->getSamples().values());
    while (i.hasNext())
    {
        Smpl * elt = i.next();
        if (elt->isHidden())
            continue;

        QStringList elements;
        elements << QString::number(elt->getId().indexElt);
        elements << elt->getName().replace(",", "_").toUtf8();
        switch (elt->_sfSampleType)
        {
        case linkInvalid: elements << "invalid"; break;
        case monoSample: elements << "mono"; break;
        case rightSample: elements << "right"; break;
        case leftSample: elements << "left"; break;
        case linkedSample: elements << "linked"; break;
        case RomMonoSample: elements << "mono (ROM)"; break;
        case RomRightSample: elements << "right (ROM)"; break;
        case RomLeftSample: elements << "left (ROM)"; break;
        case RomLinkedSample: elements << "linked (ROM)"; break;
        default:
            elements << QString::number(elt->_sfSampleType);
            break;
        }
        elements << QString::number(elt->_wSampleLink);
        elements << QString::number(elt->_sound.getUInt32(champ_byOriginalPitch));
        elements << QString::number(elt->_sound.getInt32(champ_chPitchCorrection));
        elements << QString::number(elt->_sound.getUInt32(champ_dwLength));
        elements << QString::number(elt->_sound.getUInt32(champ_dwStartLoop));
        elements << QString::number(elt->_sound.getUInt32(champ_dwEndLoop));
        elements << QString::number(elt->_sound.getUInt32(champ_dwSampleRate));
        elements << QString::number(elt->_sound.getUInt32(champ_bpsFile));

        if (!this->writeLine(elements))
            return false;
    }

    return true;
}
