/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "pageoverviewsmpl.h"
#include "config.h"

PageOverviewSmpl::PageOverviewSmpl(QWidget * parent) : PageOverview(PAGE_SMPL, elementSmpl, parent) {}

QString PageOverviewSmpl::getTitle()
{
    return trUtf8("Liste des échantillons");
}

QStringList PageOverviewSmpl::getHorizontalHeader()
{
    QStringList hHeader;
    hHeader << trUtf8("Utilisé")
            << trUtf8("Durée\ntotale")
            << trUtf8("Durée de\nla boucle")
            << trUtf8("Note\nde base")
            << trUtf8("Correction")
            << trUtf8("Type\nd'échantillon")
            << trUtf8("Échantillon\nlié");
    return hHeader;
}

void PageOverviewSmpl::prepare(EltID id)
{
    // List of used samples
    _usedSmpl.clear();

    id.typeElement = elementInst;
    int nbElt = _sf2->count(id);
    for (int i = 0; i < nbElt; i++)
    {
        id.indexElt = i;
        if (!_sf2->get(id, champ_hidden).bValue)
        {
            EltID idSubElt = id;
            idSubElt.typeElement = elementInstSmpl;
            int nbSubElt = _sf2->count(idSubElt);
            for (int j = 0; j < nbSubElt; j++)
            {
                idSubElt.indexElt2 = j;
                if (!_sf2->get(idSubElt, champ_hidden).bValue)
                    _usedSmpl << _sf2->get(idSubElt, champ_sampleID).wValue;
            }
        }
    }
}

// Called for each smpl
QStringList PageOverviewSmpl::getInformation(EltID id)
{
    QStringList info;
    info << isUsed(id)
         << totalLength(id)
         << loopLength(id)
         << rootKey(id)
         << correction(id)
         << type(id)
         << link(id);
    return info;
}

QString PageOverviewSmpl::isUsed(EltID id)
{
    return _usedSmpl.contains(id.indexElt) ? trUtf8("oui") : trUtf8("non");
}

QString PageOverviewSmpl::totalLength(EltID id)
{
    quint32 length = _sf2->get(id, champ_dwLength).dwValue;
    quint32 sampleRate = _sf2->get(id, champ_dwSampleRate).dwValue;
    return QString::number((double)length / sampleRate, 'f', 3) + trUtf8("s", "unit for seconds");
}

QString PageOverviewSmpl::loopLength(EltID id)
{
    quint32 startLoop = _sf2->get(id, champ_dwStartLoop).dwValue;
    quint32 endLoop = _sf2->get(id, champ_dwEndLoop).dwValue;
    quint32 sampleRate = _sf2->get(id, champ_dwSampleRate).dwValue;
    if (startLoop != endLoop)
        return QString::number((double)(endLoop - startLoop) / sampleRate, 'f', 3) + trUtf8("s", "unit for seconds");
    else
        return "-";
}

QString PageOverviewSmpl::rootKey(EltID id)
{
    return Config::getInstance()->getKeyName(_sf2->get(id, champ_byOriginalPitch).bValue);
}

QString PageOverviewSmpl::correction(EltID id)
{
    return QString::number(_sf2->get(id, champ_chPitchCorrection).cValue);
}

QString PageOverviewSmpl::type(EltID id)
{
    QString type;

    switch (_sf2->get(id, champ_sfSampleType).sfLinkValue)
    {
    case linkInvalid:
        type = trUtf8("Lien invalide");
        break;
    case monoSample: case RomMonoSample:
        type = trUtf8("Mono", "opposite to stereo");
        break;
    case rightSample: case RomRightSample:
        type = trUtf8("Stéréo droit");
        break;
    case leftSample: case RomLeftSample:
        type = trUtf8("Stéréo gauche");
        break;
    case linkedSample: case RomLinkedSample:
        type = trUtf8("Stéréo non défini");
        break;
    }

    return type;
}

QString PageOverviewSmpl::link(EltID id)
{
    SFSampleLink type = _sf2->get(id, champ_sfSampleType).sfLinkValue;

    if (type == monoSample || type == RomMonoSample)
        return "-";
    else
    {
        EltID id2 = id;
        id2.indexElt = _sf2->get(id, champ_wSampleLink).wValue;
        if (_sf2->isValide(id2))
            return _sf2->getQstr(id2, champ_name);
        else
            return trUtf8("non valide");
    }
}
