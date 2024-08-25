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

#include "dialogkeyboard.h"
#include "ui_dialogkeyboard.h"
#include "contextmanager.h"
#include "editortoolbar.h"

DialogKeyboard::DialogKeyboard(QWidget *parent) :
    QDialog(parent, Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint),
    ui(new Ui::DialogKeyboard)
{
    ui->setupUi(this);

    // Style
    ui->frameBottom->setStyleSheet("QFrame#frameBottom{background-color:" +
                                   ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name() + ";}QLabel{color:" +
                                   ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() + "}");

    // Initialize the keyboard
    int type = ContextManager::configuration()->getValue(ConfManager::SECTION_KEYBOARD, "type", 1).toInt();
    if (type < 1 || type > 4)
        type = 1;
    switch (type)
    {
    case 1:
        ui->comboType->setCurrentIndex(0);
        break;
    case 2:
        ui->comboType->setCurrentIndex(1);
        break;
    case 3:
        ui->comboType->setCurrentIndex(3);
        break;
    case 4:
        ui->comboType->setCurrentIndex(2);
        break;
    }
    ContextManager::midi()->addListener(this, 0);
    connect(ContextManager::configuration(), SIGNAL(keyMapChanged()), ui->keyboard, SLOT(updateMapping()));

    // Connections for displaying the current note and velocity
    connect(ui->keyboard, SIGNAL(mouseOver(int,int)), this, SLOT(onMouseHover(int,int)));

    // Visibility of the control area
    updateControlAreaVisibility();

    // Restore the geometry
    QByteArray geometry = ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "keyboardGeometry", QByteArray()).toByteArray();
    if (!geometry.isEmpty())
        this->restoreGeometry(geometry);
}

DialogKeyboard::~DialogKeyboard()
{
    // Save the keyboard state
    ContextManager::midi()->removeListener(this);
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "keyboard_height", ui->keyboard->sizeHint().height());
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "keyboardGeometry", this->saveGeometry());
    delete ui;
}

void DialogKeyboard::onMouseHover(int key, int vel)
{
    if (_triggeredKeys.empty())
    {
        if (key >= 0 && key <= 127 && vel > 0)
        {
            ui->labelKey->setText(ContextManager::keyName()->getKeyName(static_cast<unsigned int>(key)));
            ui->labelVel->setText(QString::number(vel));
        }
        else
        {
            ui->labelKey->setText("-");
            ui->labelVel->setText("-");
        }
        ui->labelAftertouch->setText("-");
    }
}

void DialogKeyboard::glow()
{
    ui->keyboard->setFocus();
    ui->keyboard->triggerGlowEffect();
}

void DialogKeyboard::closeEvent(QCloseEvent * event)
{
    QDialog::closeEvent(event);
    EditorToolBar::updateKeyboardButtonsState(false);
}

void DialogKeyboard::on_comboType_currentIndexChanged(int index)
{
    // Save the keyboard height
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "keyboard_height", ui->keyboard->sizeHint().height());

    switch (index)
    {
    case 0:
        // 5 octaves
        ui->keyboard->set(PianoKeybd::PROPERTY_KEY_MIN, 36);
        ui->keyboard->set(PianoKeybd::PROPERTY_KEY_NUMBER, 61);
        ContextManager::configuration()->setValue(ConfManager::SECTION_KEYBOARD, "type", 1);
        break;
    case 1:
        // 6 octaves
        ui->keyboard->set(PianoKeybd::PROPERTY_KEY_MIN, 36);
        ui->keyboard->set(PianoKeybd::PROPERTY_KEY_NUMBER, 73);
        ContextManager::configuration()->setValue(ConfManager::SECTION_KEYBOARD, "type", 2);
        break;
    case 2:
        // 88 keys
        ui->keyboard->set(PianoKeybd::PROPERTY_KEY_MIN, 21);
        ui->keyboard->set(PianoKeybd::PROPERTY_KEY_NUMBER, 88);
        ContextManager::configuration()->setValue(ConfManager::SECTION_KEYBOARD, "type", 4);
        break;
    case 3:
        // 128 keys
        ui->keyboard->set(PianoKeybd::PROPERTY_KEY_MIN, 0);
        ui->keyboard->set(PianoKeybd::PROPERTY_KEY_NUMBER, 128);
        ContextManager::configuration()->setValue(ConfManager::SECTION_KEYBOARD, "type", 3);
        break;
    }

    this->adjustSize();
}

void DialogKeyboard::displayKeyInfo()
{
    if (_triggeredKeys.empty())
    {
        // Reset the key information
        ui->labelKey->setText("-");
        ui->labelVel->setText("-");
        ui->labelAftertouch->setText("-");
    }
    else
    {
        // Display the last key in the list
        ui->labelKey->setText(ContextManager::keyName()->getKeyName(static_cast<unsigned int>(_triggeredKeys.last().first)));
        ui->labelVel->setText(QString::number(_triggeredKeys.last().second.first));
        if (_triggeredKeys.last().second.second >= 0)
            ui->labelAftertouch->setText(QString::number(_triggeredKeys.last().second.second));
        else
            ui->labelAftertouch->setText("-");
    }
}

void DialogKeyboard::on_pushExpand_clicked()
{
    // Toggle the visibility of the modulator area
    bool isVisible = ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "control_area_expanded", false).toBool();
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "control_area_expanded", !isVisible);
    updateControlAreaVisibility();
}

void DialogKeyboard::updateControlAreaVisibility()
{
    // Save the keyboard height
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "keyboard_height", ui->keyboard->sizeHint().height());

    // Initialize the expanded / collapsed state
    if (ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "control_area_expanded", false).toBool())
    {
        ui->controllerArea->show();
        ui->pushExpand->setIcon(ContextManager::theme()->getColoredSvg(":/icons/arrow_up.svg", QSize(16, 16), ThemeManager::HIGHLIGHTED_TEXT));
        ui->pushExpand->setToolTip(tr("Hide the controller area"));
    }
    else
    {
        ui->controllerArea->hide();
        ui->pushExpand->setIcon(ContextManager::theme()->getColoredSvg(":/icons/arrow_down.svg", QSize(16, 16), ThemeManager::HIGHLIGHTED_TEXT));
        ui->pushExpand->setToolTip(tr("Show the controller area"));
    }

    this->adjustSize();
}

void DialogKeyboard::keyPressEvent(QKeyEvent * event)
{
    // Prevent the escape key from closing the window
    if (event->key() != Qt::Key_Escape)
          QDialog::keyPressEvent(event);
}

bool DialogKeyboard::processKey(int channel, int key, int vel)
{
    if (channel != -1)
        return false;

    if (key >= 0 && key <= 127)
    {
        if (vel > 0)
        {
            // Add a key in the list to display
            _triggeredKeys << QPair<int, QPair<int, int> >(key, QPair<int, int>(vel, vel));

            // Update the keyboard
            ui->keyboard->inputNoteOn(key, vel);
        }
        else
        {
            // Remove a key from the list
            for (int i = _triggeredKeys.count() - 1; i >= 0; i--)
                if (_triggeredKeys[i].first == key)
                    _triggeredKeys.removeAt(i);

            // Update the keyboard
            ui->keyboard->inputNoteOff(key);
            ui->keyboard->removeCurrentRange(key);
        }
    }

    displayKeyInfo();
    return false;
}

bool DialogKeyboard::processPolyPressureChanged(int channel, int key, int pressure)
{
    if (channel != -1 || _triggeredKeys.empty())
        return false;

    // Change the pressure of an existing key
    for (int i = 0; i < _triggeredKeys.count(); i++)
        if (_triggeredKeys[i].first == key)
            _triggeredKeys[i].second.second = pressure;

    if (_triggeredKeys.last().first == key)
        displayKeyInfo();

    return false;
}

bool DialogKeyboard::processMonoPressureChanged(int channel, int value)
{
    Q_UNUSED(channel)
    ui->controllerArea->updateMonoPressure(value);
    return false;
}

bool DialogKeyboard::processControllerChanged(int channel, int num, int value)
{
    if (channel != -1)
        return false;

    ui->controllerArea->updateController(num, value);
    return false;
}

bool DialogKeyboard::processBendChanged(int channel, float value)
{
    if (channel != -1)
        return false;

    ui->controllerArea->updateBend(value, channel != -1);
    return false;
}

bool DialogKeyboard::processBendSensitivityChanged(int channel, float semitones)
{
    if (channel != -1)
        return false;

    ui->controllerArea->updateBendSensitivity(semitones);
    return false;
}

PianoKeybdCustom * DialogKeyboard::getKeyboard()
{
    return ui->keyboard;
}
