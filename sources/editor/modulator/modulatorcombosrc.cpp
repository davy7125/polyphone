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

#include "modulatorcombosrc.h"
#include <QStandardItemModel>
#include "soundfontmanager.h"

ModulatorComboSrc::ModulatorComboSrc(QWidget * parent) : QComboBox(parent)
{
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
}

void ModulatorComboSrc::initialize(EltID id, bool source1)
{
    // Store the id and if this is the first source
    _id = id;
    _source1 = source1;

    // Populate
    this->blockSignals(true);
    this->addItem(getIndexName(0, false), QString::number(0));
    this->addItem(getIndexName(2, false), QString::number(2));
    this->addItem(getIndexName(3, false), QString::number(3));
    this->addItem(getIndexName(10, false), QString::number(10));
    this->addItem(getIndexName(13, false), QString::number(13));
    this->addItem(getIndexName(14, false), QString::number(14));
    this->addItem(getIndexName(16, false), QString::number(16));
    for (int i = 1; i < 6; i++)
        this->addItem(getIndexName(i, true), "CC#" + QString::number(i));
    for (int i = 7; i < 32; i++)
        this->addItem(getIndexName(i, true), "CC#" + QString::number(i));
    for (int i = 64; i < 98; i++)
        this->addItem(getIndexName(i, true), "CC#" + QString::number(i));
    for (int i = 102; i < 120; i++)
        this->addItem(getIndexName(i, true), "CC#" + QString::number(i));

    if (_source1)
    {
        // Add "Linked"
        this->insertSeparator(this->count());
        this->addItem("");
        setLink(false, trUtf8("Link"));
    }
    this->blockSignals(false);

    loadValue();
}

void ModulatorComboSrc::initialize(SFModulator mod)
{
    _id.typeElement = elementUnknown;

    // Just load one value (default modulator)
    this->addItem(getIndexName(mod.Index, mod.CC), mod.CC ? "CC#" + QString::number(mod.Index) : QString::number(mod.Index));
}

void ModulatorComboSrc::loadValue()
{
    this->blockSignals(true);

    // Select the value
    SoundfontManager * sm = SoundfontManager::getInstance();
    SFModulator sfModTmp = sm->get(_id, _source1 ? champ_sfModSrcOper : champ_sfModAmtSrcOper).sfModValue;

    if (sfModTmp.Index == 127 && sfModTmp.CC == 0 && _source1)
    {
        // Find linked modulators
        QList<int> linkMods = getAssociatedMods(_id);
        QString linkText;
        if (linkMods.empty())
            linkText = trUtf8("Link (invalid)");
        else
        {
            linkText = trUtf8("Modulator") + ":";
            for (int i = 0; i < linkMods.count(); i++)
                linkText += (i > 0 ? ", #" : " #") + QString::number(linkMods[i]);
        }
        setLink(true, linkText);

        this->setCurrentIndex(this->count() - 1);
    }
    else
    {
        // Select a source
        this->setCurrentText(getIndexName(sfModTmp.Index, sfModTmp.CC));
    }
    this->blockSignals(false);
}

QList<int> ModulatorComboSrc::getAssociatedMods(EltID id)
{
    QList<int> result;
    EltID id2 = _id;
    SoundfontManager * sm = SoundfontManager::getInstance();
    int modCount = 0;
    foreach (int j, sm->getSiblings(_id))
    {
        id2.indexMod = j;
        if (id2.indexMod != _id.indexMod)
            if (sm->get(id2, champ_sfModDestOper).wValue == 32768 + id.indexMod)
                result << modCount + 1;
        modCount++;
    }
    return result;
}

void ModulatorComboSrc::setLink(bool enabled, QString text)
{
    // Text
    this->setItemText(this->count() - 1, text);

    // Enabled?
    QStandardItemModel * model = qobject_cast<QStandardItemModel *>(this->model());
    QStandardItem * item = model->item(this->count() - 1);
    item->setFlags(enabled ? item->flags() | Qt::ItemIsEnabled : item->flags() & ~Qt::ItemIsEnabled);
}

int ModulatorComboSrc::getIndex()
{
    return this->itemData(this->currentIndex()).toString().split("#").last().toInt();
}

bool ModulatorComboSrc::isCC()
{
    return this->itemData(this->currentIndex()).toString().contains("#");
}

void ModulatorComboSrc::onCurrentIndexChanged(int index)
{
    if (_id.typeElement == elementUnknown)
        return;

    // New value
    bool isCC = this->isCC();
    index = this->getIndex();

    // Compare with the old value
    SoundfontManager * sm = SoundfontManager::getInstance();
    AttributeValue val = sm->get(_id, _source1 ? champ_sfModSrcOper : champ_sfModAmtSrcOper);
    if (val.sfModValue.Index != index || val.sfModValue.CC != isCC)
    {
        // If this was a link, change the destination of the targeting modulator(s) to 0
        if (val.sfModValue.Index == 127 && val.sfModValue.CC == 0)
        {
            foreach (int i, sm->getSiblings(_id))
            {
                if (i == _id.indexMod)
                    continue;
                EltID otherModId = _id;
                otherModId.indexMod = i;
                int destIndex = sm->get(otherModId, champ_sfModDestOper).wValue;
                if (destIndex == 32768 + _id.indexMod)
                {
                    AttributeValue val2;
                    val2.wValue = 0;
                    sm->set(otherModId, champ_sfModDestOper, val2);
                }
            }
        }

        val.sfModValue.Index = static_cast<quint8>(index);
        val.sfModValue.CC = isCC;
        sm->set(_id, _source1 ? champ_sfModSrcOper : champ_sfModAmtSrcOper, val);
        sm->endEditing("modulatorEditor");
    }
}

QString ModulatorComboSrc::getIndexName(quint16 iVal, bool CC)
{
    QString qStr = "";
    if (CC)
    {
        switch (iVal)
        {
        case 0: qStr = "Bank select"; break;
        case 1: qStr = "Modulation wheel"; break;
        case 2: qStr = "Breath controller"; break;
        case 4: qStr = "Foot pedal"; break;
        case 5: qStr = "Portamento time"; break;
        case 6: qStr = "Data entry"; break;
        case 7: qStr = "Main volume"; break;
        case 8: qStr = "Balance"; break;
        case 10: qStr = "Pan position"; break;
        case 11: qStr = "Expression pedal"; break;
        case 12: qStr = "Effect control 1"; break;
        case 13: qStr = "Effect control 2"; break;
        case 16: qStr = "General purpose 1"; break;
        case 17: qStr = "General purpose 2"; break;
        case 18: qStr = "General purpose 3"; break;
        case 19: qStr = "General purpose 4"; break;
        case 32: qStr = "Bank select (fine)"; break;
        case 33: qStr = "Modulation wheel (fine)"; break;
        case 34: qStr = "Breath controller (fine)"; break;
        case 36: qStr = "Foot pedal (fine)"; break;
        case 37: qStr = "Portamento time (fine)"; break;
        case 38: qStr = "Data entry (fine)"; break;
        case 39: qStr = "Main volume (fine)"; break;
        case 40: qStr = "Balance (fine)"; break;
        case 42: qStr = "Pan position (fine)"; break;
        case 43: qStr = "Expression pedal (fine)"; break;
        case 44: qStr = "Effect control 1 (fine)"; break;
        case 45: qStr = "Effect control 2 (fine)"; break;
        case 48: qStr = "General purpose 1 (fine)"; break;
        case 49: qStr = "General purpose 2 (fine)"; break;
        case 50: qStr = "General purpose 3 (fine)"; break;
        case 51: qStr = "General purpose 4 (fine)"; break;
        case 64: qStr = "Hold pedal (on/off)"; break;
        case 65: qStr = "Portamento (on/off)"; break;
        case 66: qStr = "Sostenuto (on/off)"; break;
        case 67: qStr = "Soft pedal (on/off)"; break;
        case 68: qStr = "Legato pedal (on/off)"; break;
        case 69: qStr = "Hold 2 pedal (on/off)"; break;
        case 70: qStr = "Sound controller 1 (variation)"; break;
        case 71: qStr = "Sound controller 2 (timbre)"; break;
        case 72: qStr = "Sound controller 3 (release time)"; break;
        case 73: qStr = "Sound controller 4 (attack time)"; break;
        case 74: qStr = "Sound controller 5 (brightness)"; break;
        case 75: qStr = "Sound controller 6"; break;
        case 76: qStr = "Sound controller 7"; break;
        case 77: qStr = "Sound controller 8"; break;
        case 78: qStr = "Sound controller 9"; break;
        case 79: qStr = "Sound controller 10"; break;
        case 80: qStr = "General purpose 5"; break;
        case 81: qStr = "General purpose 6"; break;
        case 82: qStr = "General purpose 7"; break;
        case 83: qStr = "General purpose 8"; break;
        case 84: qStr = "Portamento control"; break;
        case 91: qStr = "Effects 1 depth (reverb)"; break;
        case 92: qStr = "Effects 2 depth (tremolo)"; break;
        case 93: qStr = "Effects 3 depth (chorus)"; break;
        case 94: qStr = "Effects 4 depth (celeste)"; break;
        case 95: qStr = "Effects 5 depth (phaser)"; break;
        case 96: qStr = "Data increment"; break;
        case 97: qStr = "Data decrement"; break;
        case 98: qStr = "Non-registered parameter (fine)"; break;
        case 99: qStr = "Non-registered parameter"; break;
        case 100: qStr = "Registered parameter (fine)"; break;
        case 101: qStr = "Registered parameter"; break;
        case 120: qStr = "All sound off"; break;
        case 121: qStr = "All controller off"; break;
        case 122: qStr = "Local keyboard (on/off)"; break;
        case 123: qStr = "All notes off"; break;
        case 124: qStr = "Omni mode off"; break;
        case 125: qStr = "Omni mode on"; break;
        case 126: qStr = "Mono on (poly off)"; break;
        case 127: qStr = "Poly on (mono off)"; break;
        default: qStr = "Undefined"; break;
        }
        qStr = "CC-" + QString("%1").arg(iVal, 3, 10, QChar('0')) + " " + qStr;
    }
    else
    {
        switch (iVal)
        {
        case 0: qStr = "No controller, = 1"; break;
        case 2: qStr = "Note-On velocity"; break;
        case 3: qStr = "Note-On key number"; break;
        case 10: qStr = "Poly pressure"; break;
        case 13: qStr = "Channel pressure"; break;
        case 14: qStr = "Pitch wheel"; break;
        case 16: qStr = "Pitch wheel sensitivity"; break;
        case 127: qStr = "Link"; break;
        }
    }
    return qStr;
}
