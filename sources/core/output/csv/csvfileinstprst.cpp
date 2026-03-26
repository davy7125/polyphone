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

#include "csvfileinstprst.h"
#include "soundfontmanager.h"
#include "soundfonts.h"
#include "soundfont.h"
#include "instprst.h"
#include "division.h"

CsvFileInstPrst::CsvFileInstPrst(QString filePath, bool isPrst, bool rawValues) : AbstractCsvFile(filePath),
    _isPrst(isPrst),
    _rawValues(rawValues)
{}

QList<QString> CsvFileInstPrst::getHeaders()
{
    QList<QString> headers;
    headers << "id"
            << (_isPrst ? "instrument id" : "sample id")
            << "key range"
            << "velocity range"
            << "attenuation"
            << "pan";
    if (!_isPrst)
    {
        headers << "loop playback"
                << "root key";
    }
    headers << "coarse tuning"
            << "fine tuning"
            << "scale tuning"
            << "filter cutoff"
            << "filter resonance"
            << "vol env delay"
            << "vol env attack"
            << "vol env hold"
            << "vol env decay"
            << "vol env sustain"
            << "vol env release"
            << "key → vol env hold"
            << "key → vol env decay"
            << "mod env delay"
            << "mod env attack"
            << "mod env hold"
            << "mod env decay"
            << "mod env sustain"
            << "mod env release"
            << "mod env → pitch"
            << "mod env → filter"
            << "key → mod env hold"
            << "key → mod env decay"
            << "mod LFO delay"
            << "mod LFO freq"
            << "mod LFO → pitch"
            << "mod LFO → filter"
            << "mod LFO → volume"
            << "vib LFO delay"
            << "vib LFO freq"
            << "vib LFO → pitch";
    if (!_isPrst)
        headers << "exclusive class";
    headers << "chorus"
            << "reverb";
    if (!_isPrst)
    {
        headers << "fixed key"
                << "fixed velocity"
                << "sample start offset"
                << "sample start coarse offset"
                << "sample end offset"
                << "sample end coarse offset"
                << "loop start offset"
                << "loop start coarse offset"
                << "loop end offset"
                << "loop end coarse offset";
    }

    return headers;
}

bool CsvFileInstPrst::write(SoundfontManager * sm, EltID idElt)
{
    // Global division
    InstPrst * elt = _isPrst ?
        sm->getSoundfonts()->getSoundfont(idElt.indexSf2)->getPreset(idElt.indexElt) :
        sm->getSoundfonts()->getSoundfont(idElt.indexSf2)->getInstrument(idElt.indexElt);
    Division * globalDiv = elt->getGlobalDivision();
    _globalAttributes = globalDiv->getAttributeValues();
    _globalAttributesSet = globalDiv->getAttributeSet();

    // Browse all divisions linked to a sample or an instrument
    QVectorIterator<Division*> i(elt->getDivisions().values());
    while (i.hasNext())
    {
        Division * div = i.next();
        if (div->isHidden())
            continue;
        _attributes = div->getAttributeValues();
        _attributesSet = div->getAttributeSet();

        QStringList elements;
        elements << QString::number(div->getId().indexElt2);
        elements << QString::number((_isPrst ? _attributes[champ_instrument] : _attributes[champ_sampleID]).wValue);

        elements << getRValue(champ_keyRange);
        elements << getRValue(champ_velRange);
        elements << getShValue(champ_initialAttenuation);
        elements << getShValue(champ_pan);

        if (!_isPrst)
        {
            AttributeValue * val = getAttributeValue(champ_sampleModes);
            switch (val->wValue)
            {
            case 0: elements << "no loop"; break;
            case 1: elements << "loop"; break;
            case 2: elements << "end only"; break;
            case 3: elements << "loop + end"; break;
            default: elements << QString::number(val->wValue); break;
            }

            elements << getShValue(champ_overridingRootKey);
        }

        elements << getShValue(champ_coarseTune);
        elements << getShValue(champ_fineTune);
        elements << getShValue(champ_scaleTuning);
        elements << getShValue(champ_initialFilterFc);
        elements << getShValue(champ_initialFilterQ);

        elements << getShValue(champ_delayVolEnv);
        elements << getShValue(champ_attackVolEnv);
        elements << getShValue(champ_holdVolEnv);
        elements << getShValue(champ_decayVolEnv);
        elements << getShValue(champ_sustainVolEnv);
        elements << getShValue(champ_releaseVolEnv);
        elements << getShValue(champ_keynumToVolEnvHold);
        elements << getShValue(champ_keynumToVolEnvDecay);

        elements << getShValue(champ_delayModEnv);
        elements << getShValue(champ_attackModEnv);
        elements << getShValue(champ_holdModEnv);
        elements << getShValue(champ_decayModEnv);
        elements << getShValue(champ_sustainModEnv);
        elements << getShValue(champ_releaseModEnv);
        elements << getShValue(champ_modEnvToPitch);
        elements << getShValue(champ_modEnvToFilterFc);
        elements << getShValue(champ_keynumToModEnvHold);
        elements << getShValue(champ_keynumToModEnvDecay);

        elements << getShValue(champ_delayModLFO);
        elements << getShValue(champ_freqModLFO);
        elements << getShValue(champ_modLfoToPitch);
        elements << getShValue(champ_modLfoToFilterFc);
        elements << getShValue(champ_modLfoToVolume);

        elements << getShValue(champ_delayVibLFO);
        elements << getShValue(champ_freqVibLFO);
        elements << getShValue(champ_vibLfoToPitch);

        if (!_isPrst)
            elements << getWValue(champ_exclusiveClass);

        elements << getShValue(champ_chorusEffectsSend);
        elements << getShValue(champ_reverbEffectsSend);

        if (!_isPrst)
        {
            elements << getShValue(champ_keynum);
            elements << getShValue(champ_velocity);

            elements << getShValue(champ_startAddrsOffset);
            elements << getShValue(champ_startAddrsCoarseOffset);
            elements << getShValue(champ_endAddrsOffset);
            elements << getShValue(champ_endAddrsCoarseOffset);

            elements << getShValue(champ_startloopAddrsOffset);
            elements << getShValue(champ_startloopAddrsCoarseOffset);
            elements << getShValue(champ_endloopAddrsOffset);
            elements << getShValue(champ_endloopAddrsCoarseOffset);
        }

        if (!this->writeLine(elements))
            return false;
    }

    return true;
}

QString CsvFileInstPrst::getRValue(AttributeType type)
{
    AttributeValue * value = getAttributeValue(type);
    return QString("%1-%2").arg(value->rValue.byLo).arg(value->rValue.byHi);
}

QString CsvFileInstPrst::getShValue(AttributeType type)
{
    AttributeValue * value = getAttributeValue(type);
    return _rawValues ?
               QString::number(value->shValue) :
               QString::number(Attribute::toRealValue(type, _isPrst, *value));
}

QString CsvFileInstPrst::getWValue(AttributeType type)
{
    AttributeValue * value = getAttributeValue(type);
    return _rawValues ?
               QString::number(value->wValue) :
               QString::number(Attribute::toRealValue(type, _isPrst, *value));
}

AttributeValue * CsvFileInstPrst::getAttributeValue(AttributeType type)
{
    if (_attributesSet[type])
        return &_attributes[type];

    if (_globalAttributesSet[type])
        return &_globalAttributes[type];

    _defaultValue = Attribute::getDefaultStoredValue(type, _isPrst);
    return &_defaultValue;
}
