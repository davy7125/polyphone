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

#include "temperamentarea.h"
#include "contextmanager.h"
#include "temperamentcombobox.h"
#include "temperamentkeyboard.h"
#include "referencepitcheditor.h"
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QToolButton>
#include <QGridLayout>

TemperamentArea::TemperamentArea(QWidget * parent) : QWidget(parent),
    _isSaving(false),
    _areDetailsShown(ContextManager::configuration()->getValue(
        ConfManager::SECTION_DISPLAY, "temperament_area_details", false).toBool())
{
    // Reference pitch: note = frequency, and transposition
    _referencePitchEditor = new ReferencePitchEditor();
    _comboTransposition = new QComboBox();
    _comboTransposition->setToolTip(TemperamentSettings::getTranspositionToolTip());
    for (int semitones = -12; semitones <= 12; semitones++)
        _comboTransposition->addItem(TemperamentSettings::getTranspositionText(semitones), semitones);

    // Temperaments and scales shown on demand
    _buttonDetails = new QToolButton();
    _buttonDetails->setAutoRaise(true);
    _buttonDetails->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    // Temperament and tonality
    _labelTemperament = new QLabel(tr("Temperament"));
    _comboTemperament = new TemperamentComboBox();
    _comboTemperament->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    _comboTemperament->setMinimumContentsLength(18);
    _comboTemperament->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    _labelTonality = new QLabel(tr("Tonality"));
    _comboTonality = new QComboBox();
    for (int key = 0; key < 12; key++)
        _comboTonality->addItem("");
    updateNames();

    // Notes not used, the box on the right
    _checkPlayUnusedNotes = new QCheckBox(tr("Also play the unused notes"));
    _checkPlayUnusedNotes->setLayoutDirection(Qt::RightToLeft);

    // 12 notes
    _keyboard = new TemperamentKeyboard();

    // 3 rows: reference pitch (note, "=", frequency) and transposition, temperament as wide as them, tonality below
    // the note with the notes not used aligned on the right. On the right, the button showing the temperaments and
    // scales and below it the 12 notes, as high as 2 rows
    QGridLayout * grid = new QGridLayout(this);
    grid->setContentsMargins(6, 6, 6, 6);
    grid->setHorizontalSpacing(6);
    grid->setVerticalSpacing(4);
    grid->addWidget(new QLabel(tr("Reference pitch")), 0, 0);
    _referencePitchEditor->setParent(this);
    _referencePitchEditor->moveToGrid(grid, 0, 1);
    QLabel * labelTransposition = new QLabel(tr("Transposition"));
    labelTransposition->setToolTip(_comboTransposition->toolTip());
    grid->addWidget(labelTransposition, 0, 4);
    grid->addWidget(_comboTransposition, 0, 5);
    grid->addWidget(_labelTemperament, 1, 0);
    grid->addWidget(_comboTemperament, 1, 1, 1, 5);
    grid->addWidget(_labelTonality, 2, 0);
    grid->addWidget(_comboTonality, 2, 1);
    grid->addWidget(_checkPlayUnusedNotes, 2, 2, 1, 4, Qt::AlignRight | Qt::AlignAbsolute); // Not mirrored (box on the right)
    QWidget * separation = new QWidget(); // Empty column: twice the spacing before the right part
    separation->setFixedSize(0, 0);
    grid->addWidget(separation, 0, 6);
    grid->addWidget(_buttonDetails, 0, 7, Qt::AlignLeft);
    grid->addWidget(_keyboard, 1, 7, 2, 1);
    grid->setColumnStretch(1, 1); // Note and frequency sharing the width
    grid->setColumnStretch(3, 1);

    // Changes
    _comboTemperament->fillTemperaments();
    updateValues();
    connect(_comboTemperament, SIGNAL(currentIndexChanged(int)), this, SLOT(onTemperamentChanged(int)));
    connect(_comboTonality, SIGNAL(currentIndexChanged(int)), this, SLOT(onTonalityChanged(int)));
    connect(_comboTransposition, SIGNAL(currentIndexChanged(int)), this, SLOT(onTranspositionChanged(int)));
    connect(_keyboard, SIGNAL(edited()), this, SLOT(onKeyboardEdited()));
    connect(_keyboard, SIGNAL(tonalityRequested(int)), this, SLOT(onTonalityRequested(int)));
    connect(_checkPlayUnusedNotes, SIGNAL(toggled(bool)), this, SLOT(onPlayUnusedNotesToggled(bool)));
    connect(_buttonDetails, SIGNAL(clicked()), this, SLOT(onDetailsClicked()));

    // The same settings can be changed in the keyboard settings
    connect(ContextManager::configuration(), SIGNAL(soundEngineConfigurationChanged()), this, SLOT(updateValues()));
    connect(ContextManager::configuration(), SIGNAL(interfaceChanged()), this, SLOT(updateNames()));
}

void TemperamentArea::updateNames()
{
    // Sharps or flats as configured
    for (int key = 0; key < 12; key++)
        _comboTonality->setItemText(key, ContextManager::keyName()->getKeyName(static_cast<unsigned int>(key), true, false, false, true));
}

void TemperamentArea::updateValues()
{
    // Changes made here are already shown
    if (_isSaving)
        return;

    _checkPlayUnusedNotes->blockSignals(true);
    _checkPlayUnusedNotes->setChecked(TemperamentSettings::playsUnusedNotes());
    _checkPlayUnusedNotes->blockSignals(false);

    _comboTransposition->blockSignals(true);
    _comboTransposition->setCurrentIndex(_comboTransposition->findData(TemperamentSettings::getTransposition()));
    _comboTransposition->blockSignals(false);

    _temperament.load();
    updateTemperamentWidgets();
}

void TemperamentArea::updateTemperamentWidgets()
{
    double fineTunes[12];
    bool used[12];
    _temperament.getTemperament(fineTunes, used);
    _keyboard->setTemperament(fineTunes, used, _temperament.isTonalityUsed() ? _temperament.getTonality() : -1);
    _keyboard->setReferencePitchKey(TemperamentSettings::getReferencePitchKey());

    _comboTemperament->selectProgram(_temperament.getProgram());

    _comboTonality->blockSignals(true);
    _comboTonality->setCurrentIndex(_temperament.getTonality());
    _comboTonality->setEnabled(_temperament.isTonalityUsed());
    _labelTonality->setEnabled(_temperament.isTonalityUsed());
    _comboTonality->blockSignals(false);

    // Possibly the temperament in use shown by the button
    updateDetails();
}

void TemperamentArea::updateDetails()
{
    // Temperaments and scales shown or hidden
    bool isShown = _areDetailsShown;
    for (QWidget * widget : std::initializer_list<QWidget *> {
             _labelTemperament, _comboTemperament, _labelTonality, _comboTonality, _checkPlayUnusedNotes, _keyboard })
        widget->setVisible(isShown);

    // Once hidden, the button names the temperament or scale in use so that it is not forgotten
    QString text = tr("Temperaments and scales");
    if (!isShown && _temperament.getProgram() != 0)
    {
        text = _comboTemperament->currentText();
        if (_temperament.isTonalityUsed())
            text += " (" + _comboTonality->currentText() + ")";
    }
    _buttonDetails->setText(text);
    _buttonDetails->setToolTip(isShown ? tr("Hide the temperaments and scales") :
                                         tr("Show the temperaments and scales (microtonal tuning)"));
    _buttonDetails->setIcon(ContextManager::theme()->getColoredSvg(
        isShown ? ":/icons/arrow_down.svg" : ":/icons/arrow_right.svg", QSize(12, 12), ThemeManager::WINDOW_TEXT));
}

void TemperamentArea::onDetailsClicked()
{
    _areDetailsShown = !_areDetailsShown;
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "temperament_area_details", _areDetailsShown);
    updateDetails();

    // New height known now (not when the layout requests are processed) for the window following the content
    this->layout()->invalidate();
    this->layout()->activate();
    this->updateGeometry();
    emit detailsToggled();
}

void TemperamentArea::saveTemperament()
{
    _isSaving = true;
    _temperament.save();
    _isSaving = false;
    updateTemperamentWidgets();
}

void TemperamentArea::onTemperamentChanged(int index)
{
    Q_UNUSED(index)
    _temperament.selectProgram(_comboTemperament->getProgram());
    saveTemperament();
}

void TemperamentArea::onTonalityChanged(int index)
{
    _temperament.setTonality(index);
    saveTemperament();
}

void TemperamentArea::onKeyboardEdited()
{
    // The temperament shown, including the change, becomes the custom temperament
    _temperament.setTemperament(_keyboard->getValues(), _keyboard->getUsed());
    saveTemperament();
}

void TemperamentArea::onTonalityRequested(int key)
{
    // Transposition, as with the list
    _comboTonality->setCurrentIndex(key);
}

void TemperamentArea::onTranspositionChanged(int index)
{
    _isSaving = true;
    TemperamentSettings::setTransposition(_comboTransposition->itemData(index).toInt());
    _isSaving = false;
}

void TemperamentArea::onPlayUnusedNotesToggled(bool checked)
{
    _isSaving = true;
    TemperamentSettings::setPlayUnusedNotes(checked);
    _isSaving = false;
}
