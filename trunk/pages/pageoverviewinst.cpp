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

#include "pageoverviewinst.h"
#include "config.h"

PageOverviewInst::PageOverviewInst(QWidget * parent) : PageOverview(PAGE_INST, elementInst, parent) {}

QString PageOverviewInst::getTitle()
{
    return trUtf8("Liste des instruments");
}

QStringList PageOverviewInst::getHorizontalHeader()
{
    QStringList hHeader;
    hHeader << trUtf8("Utilisé")
            << trUtf8("Nombre d'échantillons")
            << trUtf8("Nombre de paramètres")
            << trUtf8("Nombre de modulateurs")
            << trUtf8("Étendue totale (note)")
            << trUtf8("Étendue totale (vélocité)")
            << trUtf8("Atténuation")
            << trUtf8("Lecture en boucle")
            << trUtf8("Chorus")
            << trUtf8("Reverbération");
    return hHeader;
}

void PageOverviewInst::prepare(EltID id)
{
    // List of used instruments
    _usedInst.clear();

    id.typeElement = elementPrst;
    int nbElt = sf2->count(id);
    for (int i = 0; i < nbElt; i++)
    {
        id.indexElt = i;
        if (!sf2->get(id, champ_hidden).bValue)
        {
            EltID idSubElt = id;
            idSubElt.typeElement = elementPrstInst;
            int nbSubElt = sf2->count(idSubElt);
            for (int j = 0; j < nbSubElt; j++)
            {
                idSubElt.indexElt2 = j;
                if (!sf2->get(idSubElt, champ_hidden).bValue)
                    _usedInst << sf2->get(idSubElt, champ_instrument).wValue;
            }
        }
    }
}

// Called for each instrument
QStringList PageOverviewInst::getInformation(EltID id)
{
    QStringList info;
    info << isUsed(id)
         << getSampleNumber(id)
         << getParameterNumber(id)
         << getModulatorNumber(id)
         << getKeyRange(id)
         << getVelocityRange(id)
         << getAttenuation(id)
         << getLoop(id)
         << getChorus(id)
         << getReverb(id);
    return info;
}

QString PageOverviewInst::isUsed(EltID id)
{
    return _usedInst.contains(id.indexElt) ? trUtf8("oui") : trUtf8("non");
}

QString PageOverviewInst::getSampleNumber(EltID id)
{
    id.typeElement = elementInstSmpl;
    return QString::number(sf2->count(id, false));
}

QString PageOverviewInst::getParameterNumber(EltID id)
{
    int count = 0;

    // Parameters for the global division
    id.typeElement = elementInstGen;
    count += sf2->count(id);

    // Parameters for the sample divisions
    id.typeElement = elementInstSmpl;
    int nbElt = sf2->count(id);
    for (int i = 0; i < nbElt; i++)
    {
        id.indexElt2 = i;
        if (!sf2->get(id, champ_hidden).bValue)
        {
            EltID idGen = id;
            idGen.typeElement = elementInstSmplGen;
            count += sf2->count(idGen);
        }
    }

    return QString::number(count);
}

QString PageOverviewInst::getModulatorNumber(EltID id)
{
    int count = 0;

    // Modulators for the global division
    id.typeElement = elementInstMod;
    count += sf2->count(id);

    // Modulators for the sample divisions
    id.typeElement = elementInstSmpl;
    int nbElt = sf2->count(id);
    for (int i = 0; i < nbElt; i++)
    {
        id.indexElt2 = i;
        if (!sf2->get(id, champ_hidden).bValue)
        {
            EltID idMod = id;
            idMod.typeElement = elementInstSmplMod;
            count += sf2->count(idMod);
        }
    }

    return QString::number(count);
}

QString PageOverviewInst::getKeyRange(EltID id)
{
    // Global keyrange
    int globalMin = 0;
    int globalMax = 127;
    if (sf2->isSet(id, champ_keyRange))
    {
        rangesType range = sf2->get(id, champ_keyRange).rValue;
        globalMin = range.byLo;
        globalMax = range.byHi;
    }

    // Keyrange per division
    int min = 127;
    int max = 0;
    id.typeElement = elementInstSmpl;
    int nbElt = sf2->count(id);
    for (int i = 0; i < nbElt; i++)
    {
        id.indexElt2 = i;
        if (!sf2->get(id, champ_hidden).bValue)
        {
            if (sf2->isSet(id, champ_keyRange))
            {
                rangesType range = sf2->get(id, champ_keyRange).rValue;
                min = qMin(min, (int)range.byLo);
                max = qMax(max, (int)range.byHi);
            }
            else
            {
                min = qMin(min, globalMin);
                max = qMax(max, globalMax);
            }
        }
    }

    if (min > max)
        return "?";
    else if (min == max)
        return Config::getInstance()->getKeyName(min);
    else
        return Config::getInstance()->getKeyName(min) + " - " + Config::getInstance()->getKeyName(max);
}

QString PageOverviewInst::getVelocityRange(EltID id)
{
    // Global velocity range
    int globalMin = 0;
    int globalMax = 127;
    if (sf2->isSet(id, champ_velocity))
    {
        rangesType range = sf2->get(id, champ_velocity).rValue;
        globalMin = range.byLo;
        globalMax = range.byHi;
    }

    // Velocity range per division
    int min = 127;
    int max = 0;
    id.typeElement = elementInstSmpl;
    int nbElt = sf2->count(id);
    for (int i = 0; i < nbElt; i++)
    {
        id.indexElt2 = i;
        if (!sf2->get(id, champ_hidden).bValue)
        {
            if (sf2->isSet(id, champ_velocity))
            {
                rangesType range = sf2->get(id, champ_velocity).rValue;
                min = qMin(min, (int)range.byLo);
                max = qMax(max, (int)range.byHi);
            }
            else
            {
                min = qMin(min, globalMin);
                max = qMax(max, globalMax);
            }
        }
    }

    if (min > max)
        return "?";
    else if (min == max)
        return QString::number(min);
    else
        return QString::number(min) + " - " + QString::number(max);
}

QString PageOverviewInst::getAttenuation(EltID id)
{
    return getRange(id, champ_initialAttenuation);
}

QString PageOverviewInst::getLoop(EltID id)
{
    // Global value
    int globalValue = 0;
    if (sf2->isSet(id, champ_sampleModes))
        globalValue = sf2->get(id, champ_sampleModes).wValue;

    // Attenuation per division
    QList<int> modes;
    id.typeElement = elementInstSmpl;
    int nbElt = sf2->count(id);
    for (int i = 0; i < nbElt; i++)
    {
        id.indexElt2 = i;
        if (!sf2->get(id, champ_hidden).bValue)
        {
            int value = globalValue;
            if (sf2->isSet(id, champ_sampleModes))
                value = sf2->get(id, champ_sampleModes).wValue;

            if (!modes.contains(value))
                modes << value;
        }
    }

    if (modes.count() == 0)
        return "?";
    else if (modes.count() == 1)
    {
        switch (modes[0])
        {
        case 0:
            return trUtf8("non");
        case 1:
            return trUtf8("oui");
        case 3:
            return trUtf8("oui, jusqu'à la fin");
        }

        return "?";
    }
    else
        return trUtf8("plusieurs modes");
}

QString PageOverviewInst::getChorus(EltID id)
{
    return getRange(id, champ_chorusEffectsSend);
}

QString PageOverviewInst::getReverb(EltID id)
{
    return getRange(id, champ_reverbEffectsSend);
}
