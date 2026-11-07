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
#include "bannermenu.h"
#include "dialogdecoration.h"
#include <QApplication>
#include <QCheckBox>

DialogKeyboard::DialogKeyboard(QWidget *parent) :
    QDialog(parent, Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint),
    ui(new Ui::DialogKeyboard),
    _stateUpdate(true)
{
    ui->setupUi(this);

    // Banner, with a flat list for the type of keyboard
    ThemeManager * theme = ContextManager::theme();
    ui->frameTop->setStyleSheet(theme->getBannerTheme() +
                                "QComboBox{combobox-popup:0;border:0;padding:0;background:transparent;color:" +
                                theme->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() + "}" +
                                "QComboBox:hover{color:" + theme->getColor(ThemeManager::HIGHLIGHTED_TEXT, ThemeManager::HOVERED).name() + "}" +
                                "QComboBox::drop-down{border:0;width:14px}" +
                                "QComboBox::down-arrow{image:url(:/icons/arrow_down.svg);height:12px;width:12px}");
    ui->toolButtonSettings->setIcon(theme->getBannerIcon(":/icons/menu.svg", 24));
    ui->layoutTop->setAlignment(ui->layoutInfo, Qt::AlignVCenter); // Height of the other banners, the content centered
    DialogDecoration::apply(this, ui->frameTop);

    // Bar between the effects and the controllers
    ui->lineAreas->setStyleSheet("QFrame{border:0;border-top:1px solid " + theme->getColor(ThemeManager::BORDER).name() + "}");

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
    connect(this, SIGNAL(processKeyMainThread(int,int,int)), this, SLOT(onProcessKeyMainThread(int,int,int)));

    // Menu of the banner showing or hiding the controllers and the effects
    ConfManager * configuration = ContextManager::configuration();
    BannerMenu * menu = new BannerMenu(ui->toolButtonSettings, this);
    QCheckBox * checkControllers = menu->addCheckBox(tr("MIDI controllers"), 0, 2);
    checkControllers->setChecked(configuration->getValue(ConfManager::SECTION_DISPLAY, "control_area_expanded", false).toBool());
    connect(checkControllers, SIGNAL(toggled(bool)), this, SLOT(onControllersToggled(bool)));
    QCheckBox * checkEffects = menu->addCheckBox(tr("Effects"), 1, 2);
    checkEffects->setChecked(configuration->getValue(ConfManager::SECTION_DISPLAY, "effect_area_expanded", false).toBool());
    connect(checkEffects, SIGNAL(toggled(bool)), this, SLOT(onEffectsToggled(bool)));
    updateAreaVisibility();

    // The keys played with the computer keyboard are captured whatever the widget having the focus in this window
    qApp->installEventFilter(this);
}

DialogKeyboard::~DialogKeyboard()
{
    ContextManager::midi()->removeListener(this);
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

void DialogKeyboard::showEvent(QShowEvent * event)
{
    QDialog::showEvent(event);
    if (_stateUpdate)
        EditorToolBar::updateKeyboardButtonsState(true);

    // Restore the geometry (not working with Wayland)
    QByteArray geometry = ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "keyboardGeometry", QByteArray()).toByteArray();
    if (!geometry.isEmpty())
        this->restoreGeometry(geometry);
}

void DialogKeyboard::hideEvent(QHideEvent * event)
{
    QDialog::hideEvent(event);
    if (_stateUpdate)
        EditorToolBar::updateKeyboardButtonsState(false);

    // Save the geometry
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "keyboardGeometry", this->saveGeometry());
}

void DialogKeyboard::resizeEvent(QResizeEvent * event)
{
    QDialog::resizeEvent(event);
    followKeyboardRatio();
}

void DialogKeyboard::followKeyboardRatio()
{
    // The window keeps the ratio of the keyboard: its height is defined by its width. It is a constraint and not a resize
    // because a window manager can ignore any resize while an edge of the window is dragged, whereas it follows the
    // constraints
    // Not possible if the size is imposed (maximized window), the keyboard being then centered
    if (this->windowState() & (Qt::WindowMaximized | Qt::WindowFullScreen))
    {
        this->setMinimumHeight(0);
        this->setMaximumHeight(QWIDGETSIZE_MAX);
    }
    else
        this->setFixedHeight(getHeightForWidth(this->width()));
}

void DialogKeyboard::changeEvent(QEvent * event)
{
    QDialog::changeEvent(event);

    // The release of a key wouldn't be received by the keyboard if another window has the focus:
    // all notes started by the computer keyboard are stopped as soon as the focus is lost
    if (event->type() == QEvent::ActivationChange && !this->isActiveWindow())
        ui->keyboard->releaseKeys();

    // The constraint on the height depends on the state of the window
    if (event->type() == QEvent::WindowStateChange)
        followKeyboardRatio();
}

int DialogKeyboard::getHeightForWidth(int width)
{
    // Height of the window when each element has its preferred size, the keyboard keeping its ratio
    return this->layout()->totalHeightForWidth(width);
}

void DialogKeyboard::adjustHeight()
{
    // Same width (possibly increased by the minimum width of an area), the height following the content
    this->layout()->invalidate();
    this->layout()->activate();
    followKeyboardRatio();
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

    adjustHeight();
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

void DialogKeyboard::onControllersToggled(bool checked)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "control_area_expanded", checked);
    updateAreaVisibility();
}

void DialogKeyboard::onEffectsToggled(bool checked)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "effect_area_expanded", checked);
    updateAreaVisibility();
}

void DialogKeyboard::updateAreaVisibility()
{
    int keyboardTop = ui->keyboard->y();
    bool controllersVisible = ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "control_area_expanded", false).toBool();
    bool effectsVisible = ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "effect_area_expanded", false).toBool();
    ui->controllerArea->setVisible(controllersVisible);
    ui->effectArea->setVisible(effectsVisible);
    ui->lineAreas->setVisible(controllersVisible && effectsVisible);
    adjustHeight();

    // The window grows or shrinks from its top so that the keyboard stays at the same place on the screen (not possible
    // with Wayland)
    if (this->isVisible() && !(this->windowState() & (Qt::WindowMaximized | Qt::WindowFullScreen)))
        this->move(this->x(), this->y() - (ui->keyboard->y() - keyboardTop));
}

void DialogKeyboard::keyPressEvent(QKeyEvent * event)
{
    // Prevent the escape key from closing the window
    if (event->key() != Qt::Key_Escape)
          QDialog::keyPressEvent(event);
}

bool DialogKeyboard::eventFilter(QObject * watched, QEvent * event)
{
    // Keys sent to another widget of this window (effect area, combobox, ...) but played by the keyboard
    if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease)
    {
        QWidget * widget = qobject_cast<QWidget *>(watched);
        if (widget != nullptr && widget->window() == this && widget != ui->keyboard && !ui->keyboard->isAncestorOf(widget) &&
            ui->keyboard->capturesKey(static_cast<QKeyEvent *>(event)))
        {
            QCoreApplication::sendEvent(ui->keyboard, event);
            return true;
        }
    }

    return QDialog::eventFilter(watched, event);
}

bool DialogKeyboard::processKey(int channel, int key, int vel)
{
    if (channel == -1)
        emit processKeyMainThread(channel, key, vel);
    return false;
}

void DialogKeyboard::onProcessKeyMainThread(int channel, int key, int vel)
{
    Q_UNUSED(channel)

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

PianoKeybdCustom * DialogKeyboard::getKeyboard()
{
    return ui->keyboard;
}
