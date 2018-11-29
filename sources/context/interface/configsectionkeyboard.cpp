/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2018 Davy Triponney                                **
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

ConfigSectionKeyboard::ConfigSectionKeyboard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigSectionKeyboard)
{
    ui->setupUi(this);
    connect(ui->tableKeyboardMap, SIGNAL(combinaisonChanged(int,int,QString)), this, SLOT(combinaisonChanged(int,int,QString)));
    this->renameComboDo();
}

ConfigSectionKeyboard::~ConfigSectionKeyboard()
{
    delete ui;
}

void ConfigSectionKeyboard::initialize()
{
    // Name of the keys in the table
    for (int i = 0; i <= 12; i++)
    {
        ui->tableKeyboardMap->horizontalHeaderItem(i)->setText(ContextManager::keyName()->getKeyName(i, true, false, false, true));
    }

    // Populate the table
    ui->tableKeyboardMap->blockSignals(true);
    ui->tableKeyboardMap->populate();
    ui->tableKeyboardMap->blockSignals(false);

    // Octave configuration
    ui->comboDo->blockSignals(true);
    int octaveMapping = ContextManager::configuration()->getValue(ConfManager::SECTION_MAP, "octave_offset", 3).toInt();
    if (octaveMapping >= ui->comboDo->count())
        octaveMapping = 3;
    else if (octaveMapping < 0)
        octaveMapping = 0;
    ui->comboDo->setCurrentIndex(octaveMapping);
    ui->comboDo->blockSignals(false);

    // Default velocity
    ui->spinDefaultVelocity->blockSignals(true);
    ui->spinDefaultVelocity->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_KEYBOARD, "velocity", 127).toInt());
    ui->spinDefaultVelocity->blockSignals(false);
}

void ConfigSectionKeyboard::renameComboDo()
{
    int nbElement = ui->comboDo->count();
    for (int i = 0; i < nbElement; i++)
        ui->comboDo->setItemText(i, ContextManager::keyName()->getKeyName(12 * i));
}

void ConfigSectionKeyboard::combinaisonChanged(int key, int numOctave, QString combinaison)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_MAP,
                                              "key_" + QString::number(numOctave) + "_" + QString::number(key),
                                              combinaison);
}

void ConfigSectionKeyboard::on_comboDo_currentIndexChanged(int index)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_MAP, "octave_offset", index);
}

void ConfigSectionKeyboard::on_spinDefaultVelocity_editingFinished()
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_KEYBOARD, "velocity", ui->spinDefaultVelocity->value());
}
