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
#include <QScrollBar>
#include "tuningprogrammanager.h"
#include "temperamentcombobox.h"
#include "temperamentsettings.h"

ConfigSectionKeyboard::ConfigSectionKeyboard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigSectionKeyboard),
    _currentTuningBank(0),
    _isSaving(false)
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

    ui->pushDefaultReferencePitch->setIcon(ContextManager::theme()->getColoredSvg(":/icons/edit-undo.svg", QSize(14, 14), ThemeManager::BUTTON_TEXT));
    ui->pushDefaultTemperament->setIcon(ContextManager::theme()->getColoredSvg(":/icons/edit-undo.svg", QSize(14, 14), ThemeManager::BUTTON_TEXT));

    // Populate the table with all keys and all octaves
    for (int j = 0; j < ui->tableKeyboardMap->columnCount(); j++)
        for (int i = 0; i < ui->tableKeyboardMap->rowCount(); i++)
            ui->tableKeyboardMap->setCellWidget(i, j, new EditKey(i, static_cast<ConfManager::Key>(j)));

    // Transposition, its label as wide as the one of the tonality so that the lists are aligned
    ui->comboTransposition->blockSignals(true);
    for (int semitones = -12; semitones <= 12; semitones++)
        ui->comboTransposition->addItem(TemperamentSettings::getTranspositionText(semitones), semitones);
    ui->comboTransposition->blockSignals(false);
    ui->comboTransposition->setToolTip(TemperamentSettings::getTranspositionToolTip());
    ui->labelTransposition->setToolTip(TemperamentSettings::getTranspositionToolTip());
    int labelWidth = qMax(ui->labelTransposition->sizeHint().width(), ui->labelTonality->sizeHint().width());
    ui->labelTransposition->setMinimumWidth(labelWidth);
    ui->labelTonality->setMinimumWidth(labelWidth);

    // Tonalities, named as configured
    ui->comboTemperamentRelativeKey->blockSignals(true);
    for (int i = 0; i < 12; i++)
        ui->comboTemperamentRelativeKey->addItem("", QString::number(i));
    ui->comboTemperamentRelativeKey->blockSignals(false);
    updateTonalityNames();
    connect(ContextManager::configuration(), SIGNAL(interfaceChanged()), this, SLOT(updateTonalityNames()));

    // Fine tunes of the temperament, editable, as high as 2 rows (as in the area of the virtual keyboard)
    ui->temperamentKeyboard->setFixedHeight(2 * ui->comboTemperament->sizeHint().height() +
                                            qMax(0, ui->gridLayout->verticalSpacing()));
    connect(ui->temperamentKeyboard, SIGNAL(edited()), this, SLOT(onTemperamentEdited()));
    connect(ui->temperamentKeyboard, SIGNAL(tonalityRequested(int)), this, SLOT(onTonalityRequested(int)));

    // The same settings can be changed in the area of the virtual keyboard
    connect(ContextManager::configuration(), SIGNAL(soundEngineConfigurationChanged()), this, SLOT(onSoundEngineConfigurationChanged()));
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
    adjustTableHeight();

    // Octave configuration
    initializeFirstC();

    // Reference pitch and temperament
    ui->referencePitchEditor->updateNames();
    ui->comboTemperament->fillTemperaments();
    onSoundEngineConfigurationChanged();
}

void ConfigSectionKeyboard::adjustTableHeight()
{
    // Height showing all rows: frame, header, rows and horizontal scrollbar
    // (the default section size is used since the last row may be stretched)
    QTableWidget * table = ui->tableKeyboardMap;
    QHeaderView * header = table->horizontalHeader();
    int height = 2 * table->frameWidth() +
                 qMax(header->minimumHeight(), header->sizeHint().height()) +
                 table->rowCount() * table->verticalHeader()->defaultSectionSize() +
                 table->horizontalScrollBar()->sizeHint().height();
    if (table->style()->styleHint(QStyle::SH_ScrollView_FrameOnlyAroundContents, nullptr, table))
        height += table->style()->pixelMetric(QStyle::PM_ScrollView_ScrollBarSpacing, nullptr, table);
    table->setFixedHeight(height);
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

void ConfigSectionKeyboard::onSoundEngineConfigurationChanged()
{
    // Changes made here are already shown
    if (_isSaving)
        return;

    ui->comboTransposition->blockSignals(true);
    ui->comboTransposition->setCurrentIndex(ui->comboTransposition->findData(TemperamentSettings::getTransposition()));
    ui->comboTransposition->blockSignals(false);

    _temperament.load();
    updateTemperamentWidgets();
}

void ConfigSectionKeyboard::updateTonalityNames()
{
    for (int i = 0; i < 12; i++)
        ui->comboTemperamentRelativeKey->setItemText(i, ContextManager::keyName()->getKeyName(static_cast<unsigned int>(i), true, false, false, true));
}

void ConfigSectionKeyboard::updateTemperamentWidgets()
{
    ui->comboTemperament->selectProgram(_temperament.getProgram());

    ui->comboTemperamentRelativeKey->blockSignals(true);
    ui->comboTemperamentRelativeKey->setCurrentIndex(_temperament.getTonality());
    ui->comboTemperamentRelativeKey->setEnabled(_temperament.isTonalityUsed());
    ui->labelTonality->setEnabled(_temperament.isTonalityUsed());
    ui->comboTemperamentRelativeKey->blockSignals(false);

    ui->checkPlayUnusedNotes->blockSignals(true);
    ui->checkPlayUnusedNotes->setChecked(TemperamentSettings::playsUnusedNotes());
    ui->checkPlayUnusedNotes->blockSignals(false);

    double fineTunes[12];
    bool used[12];
    _temperament.getTemperament(fineTunes, used);
    ui->temperamentKeyboard->setTemperament(fineTunes, used, _temperament.isTonalityUsed() ? _temperament.getTonality() : -1);
    ui->temperamentKeyboard->setReferencePitchKey(TemperamentSettings::getReferencePitchKey());
}

void ConfigSectionKeyboard::saveTemperament()
{
    _isSaving = true;
    _temperament.save();
    _isSaving = false;
    updateTemperamentWidgets();
}

int ConfigSectionKeyboard::lastProgramIndex() const
{
    // The custom temperament is at the end
    return ui->comboTemperament->findData(TemperamentSettings::CUSTOM) - 1;
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

void ConfigSectionKeyboard::on_pushDefaultReferencePitch_clicked()
{
    // Reference pitch and transposition (same row)
    ui->referencePitchEditor->reset();
    ui->comboTransposition->setCurrentIndex(ui->comboTransposition->findData(0));
}

void ConfigSectionKeyboard::on_comboTemperament_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    _temperament.selectProgram(ui->comboTemperament->getProgram());
    saveTemperament();
}

void ConfigSectionKeyboard::on_comboTemperamentRelativeKey_currentIndexChanged(int index)
{
    _temperament.setTonality(index);
    saveTemperament();
}

void ConfigSectionKeyboard::onTemperamentEdited()
{
    // The temperament shown, including the change, becomes the custom temperament
    _temperament.setTemperament(ui->temperamentKeyboard->getValues(), ui->temperamentKeyboard->getUsed());
    saveTemperament();
}

void ConfigSectionKeyboard::onTonalityRequested(int key)
{
    // Transposition, as with the list
    ui->comboTemperamentRelativeKey->setCurrentIndex(key);
}

void ConfigSectionKeyboard::on_comboTransposition_currentIndexChanged(int index)
{
    _isSaving = true;
    TemperamentSettings::setTransposition(ui->comboTransposition->itemData(index).toInt());
    _isSaving = false;
}

void ConfigSectionKeyboard::on_checkPlayUnusedNotes_toggled(bool checked)
{
    TemperamentSettings::setPlayUnusedNotes(checked);
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
            pitchHz = ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "reference_pitch", 4400).toDouble() / 10.0 + 1;
        else if (trigger == 6)
            pitchHz = ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "reference_pitch", 4400).toDouble() / 10.0 - 1;

        if (pitchHz < 390)
            pitchHz = 390;
        else if (pitchHz > 470)
            pitchHz = 470;
        ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "reference_pitch", qRound(10.0 * pitchHz + 0.5)); // A4
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
        int currentIndex = ui->comboTemperament->currentIndex();
        if (trigger == 5)
        {
            // The custom temperament is not reached
            if (currentIndex < lastProgramIndex())
                newIndex = currentIndex + 1;
        }
        else if (trigger == 6)
        {
            if (currentIndex > 0)
                newIndex = qMin(currentIndex - 1, lastProgramIndex());
        }
        else
        {
            for (int i = 0; i <= lastProgramIndex(); i++)
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

