/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
**                2014      Andrea Celani                                 **
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

#include "modulatorcombodest.h"
#include "soundfontmanager.h"

ModulatorComboDest::ModulatorComboDest(QWidget * parent) : QComboBox(parent)
{
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
}

void ModulatorComboDest::initialize(EltID id)
{
    // Store the id
    _id = id;

    // Combobox related to an instrument modulator?
    bool isInst = (id.typeElement == elementInstMod || id.typeElement == elementInstSmplMod);

    // List of all possible attributes
    if (isInst)
        _destIndex << champ_startAddrsOffset
                   << champ_startAddrsCoarseOffset
                   << champ_endAddrsOffset
                   << champ_endAddrsCoarseOffset
                   << champ_startloopAddrsOffset
                   << champ_startloopAddrsCoarseOffset
                   << champ_endloopAddrsOffset
                   << champ_endloopAddrsCoarseOffset
                   << champ_overridingRootKey;
    _destIndex << champ_fineTune
               << champ_coarseTune
               << champ_scaleTuning
               << champ_initialFilterFc
               << champ_initialFilterQ
               << champ_pan
               << champ_chorusEffectsSend
               << champ_reverbEffectsSend;
    if (isInst)
        _destIndex << champ_keynum
                   << champ_exclusiveClass;
    _destIndex << champ_initialAttenuation
               << champ_delayVolEnv
               << champ_attackVolEnv
               << champ_holdVolEnv
               << champ_decayVolEnv
               << champ_sustainVolEnv
               << champ_releaseVolEnv
               << champ_keynumToVolEnvHold
               << champ_keynumToVolEnvDecay;
    if (isInst)
        _destIndex << champ_velocity
                   << champ_sampleModes;
    _destIndex << champ_delayModEnv
               << champ_attackModEnv
               << champ_holdModEnv
               << champ_decayModEnv
               << champ_sustainModEnv
               << champ_releaseModEnv
               << champ_modEnvToPitch
               << champ_modEnvToFilterFc
               << champ_keynumToModEnvHold
               << champ_keynumToModEnvDecay
               << champ_delayModLFO
               << champ_freqModLFO
               << champ_modLfoToPitch
               << champ_modLfoToVolume
               << champ_modLfoToFilterFc
               << champ_delayVibLFO
               << champ_freqVibLFO
               << champ_vibLfoToPitch;

    // Populate with the attribute list
    this->blockSignals(true);
    for (int i = 0; i < _destIndex.count(); i++)
        this->addItem(Attribute::getDescription(_destIndex[i], !isInst));
    this->blockSignals(false);

    loadValue();
}

void ModulatorComboDest::loadValue()
{
    this->blockSignals(true);

    // Remove modulators
    while (this->count() > _destIndex.count())
        this->removeItem(this->count() - 1);
    _listIndex.clear();

    // Add the other modulators
    SoundfontManager * sm = SoundfontManager::getInstance();
    bool separatorInserted = false;
    foreach (int modNumber, sm->getSiblings(_id))
    {
        _listIndex << modNumber;
        if (modNumber != _id.indexMod)
        {
            if (!separatorInserted)
            {
                this->insertSeparator(this->count());
                separatorInserted = true;
            }
            this->addItem(trUtf8("Modulator") + ": #" + QString::number(_listIndex.count()));
        }
    }

    // Select the value
    AttributeValue genValTmp = sm->get(_id, champ_sfModDestOper);
    if (genValTmp.wValue >= 32768) // Link to another modulator?
        this->selectIndex(_listIndex.indexOf(genValTmp.wValue - 32768));
    else
        this->selectAttribute(genValTmp.sfGenValue);

    this->blockSignals(false);
}

void ModulatorComboDest::selectAttribute(AttributeType attribute)
{
    if (_destIndex.indexOf(attribute) != -1)
        this->setCurrentIndex(_destIndex.indexOf(attribute));
}

AttributeType ModulatorComboDest::getCurrentAttribute()
{
    if (this->currentIndex() < _destIndex.count())
        return _destIndex[this->currentIndex()];
    return champ_unknown;
}

void ModulatorComboDest::selectIndex(int index)
{
    int iVal = this->findText(trUtf8("Modulator") + ": #" + QString::number(index + 1));
    if (iVal != -1)
        this->setCurrentIndex(iVal);
    else
        this->setCurrentIndex(0);
}

int ModulatorComboDest::getCurrentIndex()
{
    if (this->currentIndex() > _destIndex.count())
    {
        QStringList split = this->currentText().split('#');
        if (split.count() == 2)
        {
            bool ok = false;
            int result = split[1].toInt(&ok);
            if (ok)
                return result - 1;
        }
    }

    return -1;
}

void ModulatorComboDest::onCurrentIndexChanged(int index)
{
    // New index
    if (getCurrentAttribute() != champ_unknown)
        index = (int)getCurrentAttribute();
    else
    {
        int newIndex = getCurrentIndex();
        if (newIndex >= 0 && newIndex < _listIndex.count())
            index = _listIndex[newIndex] + 32768;
        else
        {
            qDebug() << "pas bon";
            return;
        }
    }

    // Compare with the old value
    AttributeValue val;
    SoundfontManager * sm = SoundfontManager::getInstance();
    val.sfGenValue = sm->get(_id, champ_sfModDestOper).sfGenValue;
    if (val.wValue != index)
    {
        // If this is a link, change the source 1 of the targeted modulator to "link"
        if (index >= 32768 && sm->getSiblings(_id).contains(index - 32768))
        {
            EltID otherModId = _id;
            otherModId.indexMod = index - 32768;
            AttributeValue val2;
            val2.sfModValue = sm->get(otherModId, champ_sfModSrcOper).sfModValue;
            val2.sfModValue.Index = 127;
            val2.sfModValue.CC = 0;
            sm->set(otherModId, champ_sfModSrcOper, val2);
        }

        val.sfGenValue = (AttributeType)index;
        sm->set(_id, champ_sfModDestOper, val);
        sm->endEditing("modulatorEditor");
    }
}
