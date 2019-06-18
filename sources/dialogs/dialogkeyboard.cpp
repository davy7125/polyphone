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

DialogKeyboard::DialogKeyboard(QWidget *parent) :
    QDialog(parent, Qt::Tool | Qt::WindowCloseButtonHint),
    ui(new Ui::DialogKeyboard)
{
    ui->setupUi(this);

    // Style
    ui->frameBottom->setStyleSheet("QFrame{background-color:" +
                                   ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name() + ";color:" +
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
    connect(ContextManager::configuration(), SIGNAL(keyMapChanged()), ui->keyboard, SLOT(updateMapping()));

    // Connections for displaying the current note and velocity
    connect(ui->keyboard, SIGNAL(mouseOver(int, int)), this, SLOT(keyPlayed(int, int)));
    connect(ContextManager::midi(), SIGNAL(keyPlayed(int,int)), this, SLOT(keyPlayed(int, int)));
}

DialogKeyboard::~DialogKeyboard()
{
    delete ui;
}

void DialogKeyboard::keyPlayed(int key, int vel)
{
    if (key >= 0 && key <= 127 && vel > 0)
    {
        ui->labelKey->setText(ContextManager::keyName()->getKeyName(key));
        ui->labelVel->setText(QString::number(vel));
    }
    else
    {
        ui->labelKey->setText("-");
        ui->labelVel->setText("-");
    }
}

void DialogKeyboard::glow()
{
    ui->keyboard->setFocus();
    ui->keyboard->triggerGlowEffect();
}

void DialogKeyboard::hideEvent(QHideEvent * event)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "keyboardGeometry", this->saveGeometry());
    QDialog::hideEvent(event);
}

void DialogKeyboard::showEvent(QShowEvent * event)
{
    QDialog::showEvent(event);
    QByteArray geometry = ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "keyboardGeometry", QByteArray()).toByteArray();
    if (!geometry.isEmpty())
        this->restoreGeometry(geometry);
}

void DialogKeyboard::closeEvent(QCloseEvent * event)
{
    QDialog::closeEvent(event);
    EditorToolBar::updateKeyboardButtonsState(false);
}

void DialogKeyboard::on_comboType_currentIndexChanged(int index)
{
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

    int dialogWidth = (int)((double)(ui->keyboard->height()) * 0.92 * ui->keyboard->ratio());
    this->resize(dialogWidth, this->height());
}
