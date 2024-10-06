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

#include "footerprst.h"
#include "ui_footerprst.h"
#include "soundfontmanager.h"

FooterPrst::FooterPrst(QWidget *parent) :
    AbstractFooter(parent),
    ui(new Ui::FooterPrst)
{
    ui->setupUi(this);

    // Initialization of spinBoxes
    ui->spinBank->init(this);
    ui->spinPreset->init(this);
    ui->spinBank->setStyleSheet("QSpinBox{margin-top: 1px;}");
    ui->spinPreset->setStyleSheet("QSpinBox{margin-top: 1px;}");
}

FooterPrst::~FooterPrst()
{
    delete ui;
}

void FooterPrst::updateInterface()
{
    _initializing = true;
    _currentParentIds = _currentIds.getSelectedIds(elementPrst);

    // Bank and preset values
    SoundfontManager * sf2 = SoundfontManager::getInstance();
    if (_currentParentIds.count() == 1)
    {
        ui->spinBank->setEnabled(true);
        ui->spinPreset->setEnabled(true);

        EltID id = _currentParentIds.first();
        ui->spinBank->setValue(sf2->get(id, champ_wBank).wValue);
        ui->spinPreset->setValue(sf2->get(id, champ_wPreset).wValue);
        ui->labelPercussion->setVisible(sf2->get(id, champ_wBank).wValue == 128);
    }
    else
    {
        // Check if the bank is the same in the selection
        bool sameNumber = true;
        int tmp = -1;
        foreach (EltID id, _currentParentIds)
        {
            int number = sf2->get(id, champ_wBank).wValue;
            if (tmp == -1)
                tmp = number;
            else if (tmp != number)
            {
                sameNumber = false;
                break;
            }
        }
        ui->spinBank->setEnabled(sameNumber);
        if (sameNumber)
            ui->spinBank->setValue(tmp);

        // Check if the preset is the same in the selection
        sameNumber = true;
        tmp = -1;
        foreach (EltID id, _currentParentIds)
        {
            int number = sf2->get(id, champ_wPreset).wValue;
            if (tmp == -1)
                tmp = number;
            else if (tmp != number)
            {
                sameNumber = false;
                break;
            }
        }
        ui->spinPreset->setEnabled(sameNumber);
        if (sameNumber)
            ui->spinPreset->setValue(tmp);

        // Hide the percussion label
        ui->labelPercussion->hide();
    }
    _initializing = false;
}

void FooterPrst::spinUpDown(int steps, SpinBox *spin)
{
    if (_initializing || _currentParentIds.empty() || steps == 0)
        return;

    _initializing = true;
    if (spin == ui->spinBank)
        this->setBank(ui->spinBank->value(), steps > 0 ? 1 : -1);
    else
        this->setPreset(ui->spinPreset->value(), steps > 0 ? 1 : -1);
    _initializing = false;
}

void FooterPrst::setBank()
{
    if (_initializing)
        return;
    _initializing = true;
    this->setBank(ui->spinBank->value(), 0);
    _initializing = false;
}

void FooterPrst::setPreset()
{
    if (_initializing)
        return;
    _initializing = true;
    this->setPreset(ui->spinPreset->value(), 0);
    _initializing = false;
}

QList<int> FooterPrst::getUsedPresetsForBank(int sf2Index, quint16 wBank)
{
    QList<int> ret;
    EltID id(elementPrst, sf2Index);
    SoundfontManager * sf2 = SoundfontManager::getInstance();
    foreach (int i, sf2->getSiblings(id))
    {
        id.indexElt = i;
        if (_currentParentIds.contains(id))
            continue;
        if (sf2->get(id, champ_wBank).wValue == wBank)
            ret << sf2->get(id, champ_wPreset).wValue;
    }
    return ret;
}

void FooterPrst::setBank(quint16 currentValue, int upDown)
{
    if (_currentParentIds.empty())
        return;

    // Desired bank
    quint16 desiredValue = currentValue;
    if (upDown < 0 && desiredValue > 0)
        desiredValue--;
    if (upDown > 0 && desiredValue < 128)
        desiredValue++;

    // Find the bank to set, according to the collision resolution method
    while (!isBankAvailable(desiredValue))
    {
        bool nothingFound = false;
        if (upDown == 0)
            nothingFound = true;
        else if (upDown > 0)
        {
            if (desiredValue >= 128) // Max is 128
                nothingFound = true;
            else
                desiredValue++;
        }
        else if (upDown < 0)
        {
            if (desiredValue == 0)
                nothingFound = true;
            else
                desiredValue--;
        }

        if (nothingFound)
        {
            desiredValue = currentValue;
            break;
        }
    }

    // Update the GUI
    ui->spinBank->setValue(desiredValue);
    ui->labelPercussion->setVisible(ui->spinBank->value() == 128);
}

bool FooterPrst::isBankAvailable(quint16 wBank)
{
    // Current sf2
    if (_currentParentIds.empty())
        return false;
    int indexSf2 = _currentParentIds[0].indexSf2;

    // Is it possible to change the bank of all current ids?
    QList<int> usePresets = getUsedPresetsForBank(indexSf2, wBank);
    SoundfontManager * sf2 = SoundfontManager::getInstance();
    foreach (EltID id, _currentParentIds)
        if (usePresets.contains(sf2->get(id, champ_wPreset).wValue))
            return false;

    return true;
}

QList<int> FooterPrst::getUsedBanksForPreset(int sf2Index, quint16 wPreset)
{
    QList<int> ret;
    EltID id(elementPrst, sf2Index);
    SoundfontManager * sf2 = SoundfontManager::getInstance();
    foreach (int i, sf2->getSiblings(id))
    {
        id.indexElt = i;
        if (_currentParentIds.contains(id))
            continue;
        if (sf2->get(id, champ_wPreset).wValue == wPreset)
            ret << sf2->get(id, champ_wBank).wValue;
    }
    return ret;
}

void FooterPrst::setPreset(quint16 currentValue, int upDown)
{
    if (_currentParentIds.empty())
        return;

    // Desired preset
    quint16 desiredValue = currentValue;
    if (upDown < 0 && desiredValue > 0)
        desiredValue--;
    if (upDown > 0 && desiredValue < 127)
        desiredValue++;

    // Find the preset to set, according to the collision resolution method
    while (!isPresetAvailable(desiredValue))
    {
        bool nothingFound = false;
        if (upDown == 0)
            nothingFound = true;
        else if (upDown > 0)
        {
            if (desiredValue == 127) // Max is 127
                nothingFound = true;
            else
                desiredValue++;
        }
        else if (upDown < 0)
        {
            if (desiredValue == 0)
                nothingFound = true;
            else
                desiredValue--;
        }

        if (nothingFound)
        {
            desiredValue = currentValue;
            break;
        }
    }

    // Update the GUI
    ui->spinPreset->setValue(desiredValue);
}

bool FooterPrst::isPresetAvailable(quint16 wPreset)
{
    // Current sf2
    if (_currentParentIds.empty())
        return false;
    int indexSf2 = _currentParentIds[0].indexSf2;

    // Is it possible to change the preset of all current ids?
    QList<int> useBanks = getUsedBanksForPreset(indexSf2, wPreset);
    SoundfontManager * sf2 = SoundfontManager::getInstance();
    foreach (EltID id, _currentParentIds)
        if (useBanks.contains(sf2->get(id, champ_wBank).wValue))
            return false;

    return true;
}

void FooterPrst::save(SpinBox * spin)
{
    SoundfontManager * sf2 = SoundfontManager::getInstance();
    if (spin == ui->spinBank)
    {
        // Has it changed?
        quint16 previousBank = sf2->get(_currentParentIds[0], champ_wBank).wValue;
        quint16 desiredBank = ui->spinBank->value();
        if (previousBank != desiredBank)
        {
            // Save the bank if it is available
            if (isBankAvailable(desiredBank))
            {
                AttributeValue v;
                v.wValue = desiredBank;
                foreach (EltID id, _currentParentIds)
                    sf2->set(id, champ_wBank, v);
                sf2->endEditing("footerPrst");
            }
            else
            {
                // Back to the previous bank
                ui->spinBank->setValue(previousBank);
            }
        }
    }
    else
    {
        // Has it changed?
        quint16 previousPreset = sf2->get(_currentParentIds[0], champ_wPreset).wValue;
        quint16 desiredPreset = ui->spinPreset->value();
        if (previousPreset != desiredPreset)
        {
            // Save the preset if it is available
            if (isPresetAvailable(desiredPreset))
            {
                AttributeValue v;
                v.wValue = desiredPreset;
                foreach (EltID id, _currentParentIds)
                    sf2->set(id, champ_wPreset, v);
                sf2->endEditing("footerPrst");
            }
            else
            {
                // Back to the previous preset
                ui->spinPreset->setValue(previousPreset);
            }
        }
    }
}

SpinBox::SpinBox(QWidget * parent) : QSpinBox(parent)
{
    connect(this, SIGNAL(editingFinished()), this, SLOT(onEditingFinished()));
}

void SpinBox::stepBy(int steps)
{
    _footer->spinUpDown(steps, this);
}

void SpinBox::onEditingFinished()
{
    _footer->spinUpDown(0, this);
    _footer->save(this);
}
