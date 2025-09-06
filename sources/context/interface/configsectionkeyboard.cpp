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

#include "configsectionkeyboard.h"
#include "ui_configsectionkeyboard.h"
#include "contextmanager.h"
#include "editkey.h"
#include <QTextStream>
#include <QFile>
#include "tuningprogrammanager.h"

ConfigSectionKeyboard::ConfigSectionKeyboard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigSectionKeyboard),
    _currentTuningBank(0)
{
    ui->setupUi(this);
    this->renameComboFirstC();
    connect(ContextManager::configuration(), SIGNAL(keyboardOctaveChanged()), this, SLOT(initializeFirstC()));

    // Style
    ui->labelSubTitle1->setStyleSheet("QLabel{margin: 20px 0;}");
    ui->labelSubTitle2->setStyleSheet("QLabel{margin: 20px 0;}");
    QFont font = this->font();
    font.setBold(true);
    ui->tableKeyboardMap->horizontalHeader()->setFont(font);
    ui->tableKeyboardMap->verticalHeader()->setFont(font);
    ui->tableKeyboardMap->verticalHeader()->setDefaultAlignment(Qt::AlignHCenter);
    ui->tableKeyboardMap->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableKeyboardMap->setStyleSheet(ContextManager::theme()->getTableTheme());

    ui->spinReferencePitch->setSuffix(" " + tr("Hz", "unit for Herz"));
    ui->pushDefaultReferencePitch->setIcon(ContextManager::theme()->getColoredSvg(":/icons/edit-undo.svg", QSize(14, 14), ThemeManager::BUTTON_TEXT));
    ui->pushDefaultTemperament->setIcon(ContextManager::theme()->getColoredSvg(":/icons/edit-undo.svg", QSize(14, 14), ThemeManager::BUTTON_TEXT));

    // Populate the table with all keys and all octaves
    for (int j = 0; j < ui->tableKeyboardMap->columnCount(); j++)
        for (int i = 0; i < ui->tableKeyboardMap->rowCount(); i++)
            ui->tableKeyboardMap->setCellWidget(i, j, new EditKey(i, static_cast<ConfManager::Key>(j)));

    // Temperament relative keys
    ui->comboTemperamentRelativeKey->blockSignals(true);
    for (int i = 0; i < 12; i++)
        ui->comboTemperamentRelativeKey->addItem(ContextManager::keyName()->getKeyName(i, true, false, false, true), QString::number(i));
    ui->comboTemperamentRelativeKey->blockSignals(false);
}

ConfigSectionKeyboard::~ConfigSectionKeyboard()
{
    delete ui;
    TuningProgramManager::kill();
}

void ConfigSectionKeyboard::initialize()
{
    // Name of the keys in the table
    for (quint32 i = 0; i <= 12; i++)
        ui->tableKeyboardMap->horizontalHeaderItem(static_cast<int>(i))->setText(
                    ContextManager::keyName()->getKeyName(i, true, false, false, true));

    // Populate the table
    int maxWidth = 50;
    for (int j = 0; j < ui->tableKeyboardMap->columnCount(); j++)
    {
        for (int i = 0; i < ui->tableKeyboardMap->rowCount(); i++)
        {
            EditKey* key = dynamic_cast<EditKey*>(ui->tableKeyboardMap->cellWidget(i, j));
            key->updateText();
            maxWidth = qMax(maxWidth, key->sizeHint().width());
        }
    }
    ui->tableKeyboardMap->horizontalHeader()->setDefaultSectionSize(maxWidth);

    // Octave configuration
    initializeFirstC();

    // Reference pitch
    ui->spinReferencePitch->blockSignals(true);
    ui->spinReferencePitch->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "reference_pitch", 4400).toDouble() / 10.0);
    ui->spinReferencePitch->blockSignals(false);

    // Load the temperaments
    updateTemperaments();
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

void ConfigSectionKeyboard::updateTemperaments()
{
    if (TuningProgramManager::getInstance()->hasBeenUpdated())
    {
        QMap<int, TuningProgram> * programs = TuningProgramManager::getInstance()->getAllPrograms();
        QStringList currentTemperamentConfig = ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "temperament", "").toString().split(",");

        // Add all temperaments
        ui->comboTemperament->blockSignals(true);
        for (auto i = programs->cbegin(), end = programs->cend(); i != end; ++i)
            ui->comboTemperament->addItem(i.value()._name, i.key());

        // Select the current one
        if (currentTemperamentConfig[0].isEmpty() || ui->comboTemperament->findText(currentTemperamentConfig[0]) == -1)
            ui->comboTemperament->setCurrentIndex(0); // Equal temperament
        else
            ui->comboTemperament->setCurrentText(currentTemperamentConfig[0]);
        ui->comboTemperament->blockSignals(false);

        // Relative key combobox
        ui->comboTemperamentRelativeKey->blockSignals(true);
        ui->comboTemperamentRelativeKey->setCurrentIndex(currentTemperamentConfig.size() == 14 ? currentTemperamentConfig[13].toInt() : 0);
        ui->comboTemperamentRelativeKey->setEnabled(ui->comboTemperament->currentIndex() != 0);
        ui->comboTemperamentRelativeKey->blockSignals(false);
    }
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

void ConfigSectionKeyboard::on_spinReferencePitch_valueChanged(double value)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "reference_pitch", static_cast<int>(value * 10.0 + 0.5));
}

void ConfigSectionKeyboard::on_pushDefaultReferencePitch_clicked()
{
    ui->spinReferencePitch->setValue(440.0);
}

void ConfigSectionKeyboard::on_comboTemperament_currentIndexChanged(int index)
{
    TuningProgram * program = TuningProgramManager::getInstance()->getTuningProgram(ui->comboTemperament->itemData(index).toInt());
    if (program == nullptr)
        return;

    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "temperament",
                                              program->getDescription() +
                                              "," + ui->comboTemperamentRelativeKey->currentData().toString());
    ui->comboTemperamentRelativeKey->setEnabled(index != 0);
}

void ConfigSectionKeyboard::on_comboTemperamentRelativeKey_currentIndexChanged(int index)
{
    TuningProgram * program = TuningProgramManager::getInstance()->getTuningProgram(ui->comboTemperament->currentData().toInt());
    if (program == nullptr)
        return;

    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "temperament",
                                              program->getDescription() +
                                              "," + ui->comboTemperamentRelativeKey->itemData(index).toString());
}

void ConfigSectionKeyboard::on_pushDefaultTemperament_clicked()
{
    ui->comboTemperament->setCurrentIndex(0);
    ui->comboTemperamentRelativeKey->setCurrentIndex(0);
}

bool ConfigSectionKeyboard::processRPNChanged(int channel, int parameter, int value, bool isRegistered, int trigger)
{
    if (!isRegistered)
        return false;

    // These configurations will be for all channels
    Q_UNUSED(channel)

    if (parameter == 1 && trigger >= 4)
    {
        // Master fine tuning: convert to Hz
        double pitchHz = 440;
        if (trigger == 4)
        {
            double semitones = static_cast<double>(value - 8192) / 8192.f;
            pitchHz = 440.0 * qPow(2.0, semitones / 12.0) + 0.5;
        }
        else if (trigger == 5)
            pitchHz = ui->spinReferencePitch->value() + 1;
        else if (trigger == 6)
            pitchHz = ui->spinReferencePitch->value() - 1;

        if (pitchHz < 390)
            pitchHz = 390;
        else if (pitchHz > 470)
            pitchHz = 470;
        ui->spinReferencePitch->setValue(pitchHz + 0.05);
    }
    else if (parameter == 2 && (trigger == 3 || trigger >= 5))
    {
        // Master coarse tuning => not implemented
    }
    else if (parameter == 3 && (trigger == 3 || trigger >= 5))
    {
        // Tuning program
        int tuningProgramIndex = 128 * _currentTuningBank + (value >> 7);
        int newIndex = -1;
        if (trigger == 5)
        {
            if (ui->comboTemperament->currentIndex() < ui->comboTemperament->count() - 1)
                newIndex = ui->comboTemperament->currentIndex() + 1;
        }
        else if (trigger == 6)
        {
            if (ui->comboTemperament->currentIndex() > 0)
                newIndex = ui->comboTemperament->currentIndex() - 1;
        }
        else
        {
            for (int i = 0; i < ui->comboTemperament->count(); i++)
            {
                if (ui->comboTemperament->itemData(i).toInt() == tuningProgramIndex)
                {
                    newIndex = i;
                    break;
                }
            }
        }

        if (newIndex != -1)
        {
            ui->comboTemperament->blockSignals(true);
            ui->comboTemperament->setCurrentIndex(newIndex);
            ui->comboTemperament->blockSignals(false);
            on_comboTemperament_currentIndexChanged(newIndex);
        }
    }
    else if (parameter == 4 && (trigger == 3 || trigger >= 5))
    {
        // Tuning bank
        if (trigger == 5)
            _currentTuningBank++;
        else if (trigger == 6)
            _currentTuningBank--;
        else
            _currentTuningBank = (value >> 7);

        if (_currentTuningBank < 0)
            _currentTuningBank = 0;
        else if (_currentTuningBank > 127)
            _currentTuningBank = 127;
    }
    else if (parameter == 5 && (trigger == 3 || trigger >= 5))
    {
        // Tuning relative key
        int key;
        if (trigger == 5)
            key = ui->comboTemperamentRelativeKey->currentIndex() + 1;
        else if (trigger == 6)
            key = ui->comboTemperamentRelativeKey->currentIndex() - 1;
        else
            key = (value >> 7);
        if (key < 0)
            key += 12;
        key %= 12;

        ui->comboTemperamentRelativeKey->setCurrentIndex(key);
    }

    return false;
}

