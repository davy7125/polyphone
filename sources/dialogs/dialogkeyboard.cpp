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

#include "dialogkeyboard.h"
#include "ui_dialogkeyboard.h"
#include "contextmanager.h"
#include "editortoolbar.h"

// Note: cannot be Qt::Tool instead of Qt::Dialog because keys must be catched before the QActions of the QMainWindow
DialogKeyboard::DialogKeyboard(QWidget *parent) :
    QDialog(parent, Qt::Dialog | Qt::WindowCloseButtonHint),
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
    ContextManager::midi()->setKeyboard(ui->keyboard);
    ContextManager::midi()->setControllerArea(ui->controllerArea);
    connect(ContextManager::configuration(), SIGNAL(keyMapChanged()), ui->keyboard, SLOT(updateMapping()));

    // Connections for displaying the current note, velocity and aftertouch
    connect(ui->keyboard, SIGNAL(mouseOver(int, int)), this, SLOT(onMouseHover(int,int)));
    connect(ui->keyboard, SIGNAL(polyPressureChanged(int, int)), this, SLOT(polyPressureChanged(int,int)));
    connect(ContextManager::midi(), SIGNAL(keyPlayed(int,int)), this, SLOT(keyPlayed(int, int)));
    connect(ContextManager::midi(), SIGNAL(polyPressureChanged(int,int)), this, SLOT(polyPressureChanged(int, int)));

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
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "keyboard_height", ui->keyboard->sizeHint().height());
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "keyboardGeometry", this->saveGeometry());
    delete ui;
}

void DialogKeyboard::keyPlayed(int key, int vel)
{
    if (key >= 0 && key <= 127)
    {
        if (vel == 0)
        {
            // Remove a key from the list
            for (int i = _triggeredKeys.count() - 1; i >= 0; i--)
                if (_triggeredKeys[i].first == key)
                    _triggeredKeys.removeAt(i);
        }
        else
        {
            // Add a key in the list to display
            _triggeredKeys << QPair<int, QPair<int, int> >(key, QPair<int, int>(vel, vel));
        }
    }

    displayKeyInfo();
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

void DialogKeyboard::polyPressureChanged(int key, int pressure)
{
    if (_triggeredKeys.empty())
        return;

    // Change the pressure of an existing key
    for (int i = 0; i < _triggeredKeys.count(); i++)
        if (_triggeredKeys[i].first == key)
            _triggeredKeys[i].second.second = pressure;

    if (_triggeredKeys.last().first == key)
        displayKeyInfo();
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

    this->resizeWindow();
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
        ui->pushExpand->setToolTip(trUtf8("Hide the controller area"));
    }
    else
    {
        ui->controllerArea->hide();
        ui->pushExpand->setIcon(ContextManager::theme()->getColoredSvg(":/icons/arrow_down.svg", QSize(16, 16), ThemeManager::HIGHLIGHTED_TEXT));
        ui->pushExpand->setToolTip(trUtf8("Show the controller area"));
    }

    this->resizeWindow();
}

void DialogKeyboard::resizeWindow()
{
    // Resize the window so that the keyboard has the same height
    int keyboardHeight = ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "keyboard_height", 60).toInt();

    // Keyboard corresponding width
    int keyboardWidth = static_cast<int>(ui->keyboard->ratio() * keyboardHeight);

    // Size of the window
    int margin = ui->topLayout->contentsMargins().left(); // Vertical and horizontal are same
    int windowWidth = keyboardWidth + 2 * margin;
    int windowHeight = ui->frameBottom->height() + 2 * margin + keyboardHeight;
    if (ui->controllerArea->isVisible())
        windowHeight += ui->controllerArea->height(); // Vertical margin is included

    // Resize the window
    this->adjustSize();
    this->resize(windowWidth, windowHeight + 3); // Adding a small offset (3) seems to be necessary...
}
