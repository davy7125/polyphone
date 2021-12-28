/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2020 Davy Triponney                                **
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

    // Combobox related to a preset modulator?
    bool isPrst = (id.typeElement == elementPrstMod || id.typeElement == elementPrstInstMod);

    // List of all possible attributes
    _destIndex = Attribute::getAttributeListForMod(isPrst);

    // Populate with the attribute list
    this->blockSignals(true);
    for (int i = 0; i < _destIndex.count(); i++)
        this->addItem(Attribute::getDescription(_destIndex[i], isPrst), _destIndex[i]);
    this->blockSignals(false);

    loadValue();
}

void ModulatorComboDest::initialize(quint16 index)
{
    _id.typeElement = elementUnknown;

    // Add just one value (default modulator)
    AttributeType currentAttribute = static_cast<AttributeType>(index);
    _destIndex << currentAttribute;
    this->addItem(Attribute::getDescription(currentAttribute, false), currentAttribute);
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
            this->addItem(tr("Modulator") + ": #" + QString::number(_listIndex.count()));
        }
    }

    // Select the value
    AttributeValue genValTmp = sm->get(_id, champ_sfModDestOper);
    if (genValTmp.wValue >= 32768) // Link to another modulator?
        this->selectIndex(_listIndex.indexOf(genValTmp.wValue - 32768));
    else
        this->selectAttribute(static_cast<AttributeType>(genValTmp.wValue));

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
        return static_cast<AttributeType>(this->currentData().toInt());
    return champ_unknown;
}

void ModulatorComboDest::selectIndex(int index)
{
    int iVal = this->findText(tr("Modulator") + ": #" + QString::number(index + 1));
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
    if (_id.typeElement == elementUnknown)
        return;

    // New index
    if (getCurrentAttribute() != champ_unknown)
        index = static_cast<int>(getCurrentAttribute());
    else
    {
        int newIndex = getCurrentIndex();
        if (newIndex >= 0 && newIndex < _listIndex.count())
            index = _listIndex[newIndex] + 32768;
        else
        {
            qDebug() << "not good";
            return;
        }
    }

    // Compare with the old value
    SoundfontManager * sm = SoundfontManager::getInstance();
    AttributeValue val = sm->get(_id, champ_sfModDestOper);
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

        // If this is not a link anymore, possibly change source 1 of the target modulator to "1"
        // if there is no other input
        if (val.wValue >= 32768 && index < 32768)
        {
            // Check that no other modulators are feeding the modulator that received the current output
            EltID otherModId = _id;
            bool stillLinked = false;
            foreach (int modId, sm->getSiblings(_id))
            {
                otherModId.indexMod = modId;
                if (otherModId.indexMod != _id.indexMod && sm->get(otherModId, champ_sfModDestOper).wValue == val.wValue)
                {
                    stillLinked = true;
                    break;
                }
            }
            if (!stillLinked)
            {
                otherModId.indexMod = val.wValue - 32768;
                AttributeValue val2;
                val2.sfModValue = sm->get(otherModId, champ_sfModSrcOper).sfModValue;
                val2.sfModValue.Index = 0;
                val2.sfModValue.CC = 0;
                sm->set(otherModId, champ_sfModSrcOper, val2);
            }
        }

        val.wValue = static_cast<quint16>(index);
        sm->set(_id, champ_sfModDestOper, val);
        sm->endEditing("modulatorEditor");
    }
}
