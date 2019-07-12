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

#include "configsectionkeyboard.h"
#include "ui_configsectionkeyboard.h"
#include "contextmanager.h"
#include "editkey.h"

ConfigSectionKeyboard::ConfigSectionKeyboard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigSectionKeyboard)
{
    ui->setupUi(this);
    this->renameComboFirstC();
    connect(ContextManager::configuration(), SIGNAL(keyboardOctaveChanged()), this, SLOT(initializeFirstC()));

    // Style
    QFont font = this->font();
    font.setBold(true);
    ui->tableKeyboardMap->horizontalHeader()->setFont(font);
    ui->tableKeyboardMap->verticalHeader()->setFont(font);

    // Populate the table with all keys and all octaves
    for (int j = 0; j < ui->tableKeyboardMap->columnCount(); j++)
        for (int i = 0; i < ui->tableKeyboardMap->rowCount(); i++)
            ui->tableKeyboardMap->setCellWidget(i, j, new EditKey(i, static_cast<ConfManager::Key>(j)));
}

ConfigSectionKeyboard::~ConfigSectionKeyboard()
{
    delete ui;
}

void ConfigSectionKeyboard::initialize()
{
    // Name of the keys in the table
    for (quint32 i = 0; i <= 12; i++)
    {
        ui->tableKeyboardMap->horizontalHeaderItem(static_cast<int>(i))->setText(
                    ContextManager::keyName()->getKeyName(i, true, false, false, true));
    }

    // Populate the table
    for (int j = 0; j < ui->tableKeyboardMap->columnCount(); j++)
        for (int i = 0; i < ui->tableKeyboardMap->rowCount(); i++)
            (dynamic_cast<EditKey*>(ui->tableKeyboardMap->cellWidget(i, j)))->updateText();

    // Octave configuration
    initializeFirstC();

    // Default velocity
    ui->spinDefaultVelocity->blockSignals(true);
    ui->spinDefaultVelocity->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_MIDI, "velocity", 127).toInt());
    ui->spinDefaultVelocity->blockSignals(false);

    // Default polyphonic aftertouch
    ui->spinBoxDefaultAfterTouch->blockSignals(true);
    ui->spinBoxDefaultAfterTouch->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_MIDI, "aftertouch", 127).toInt());
    ui->spinBoxDefaultAfterTouch->blockSignals(false);
}

void ConfigSectionKeyboard::initializeFirstC()
{
    ui->comboFirstC->blockSignals(true);
    int octaveMapping = ContextManager::configuration()->getValue(ConfManager::SECTION_KEYBOARD, "octave_offset", 3).toInt();
    if (octaveMapping >= ui->comboFirstC->count())
        octaveMapping = 3;
    else if (octaveMapping < 0)
        octaveMapping = 0;
    ui->comboFirstC->setCurrentIndex(octaveMapping);
    ui->comboFirstC->blockSignals(false);
}

void ConfigSectionKeyboard::renameComboFirstC()
{
    int nbElement = ui->comboFirstC->count();
    for (int i = 0; i < nbElement; i++)
        ui->comboFirstC->setItemText(i, ContextManager::keyName()->getKeyName(static_cast<unsigned int>(12 * i)));
}

void ConfigSectionKeyboard::on_comboFirstC_currentIndexChanged(int index)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_KEYBOARD, "octave_offset", index);
}

void ConfigSectionKeyboard::on_spinDefaultVelocity_editingFinished()
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_MIDI, "velocity", ui->spinDefaultVelocity->value());
}

void ConfigSectionKeyboard::on_spinBoxDefaultAfterTouch_editingFinished()
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_MIDI, "aftertouch", ui->spinBoxDefaultAfterTouch->value());
}
