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
#include <QApplication>

DialogKeyboard::DialogKeyboard(QWidget *parent) :
    QDialog(parent, Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint),
    ui(new Ui::DialogKeyboard),
    _stateUpdate(true),
    _keyboardTop(0),
    _keepKeyboardTop(false)
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
    connect(this, SIGNAL(processKeyMainThread(int,int,int)), this, SLOT(onProcessKeyMainThread(int,int,int)));

    // Buttons toggling the controller and effect areas, styled as the stereo editing button
    QString styleSheet = "QPushButton{border: 1px solid " + ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() +
                         "; border-top: 0; border-right: 0; border-bottom: 0} QPushButton:hover {background-color:" +
                         ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND, ThemeManager::HOVERED).name() + "}";
    ui->pushControllers->setStyleSheet(styleSheet);
    ui->pushControllers->setIcon(getToggleIcon(":/icons/MIDI.svg"));
    ui->pushEffects->setStyleSheet(styleSheet);
    ui->pushEffects->setIcon(getToggleIcon(":/icons/sound.svg"));
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

    // The window follows the ratio of the keyboard: its height changes with its width, and conversely
    // Not possible if the size is imposed (maximized window), the keyboard being then centered
    QSize size = event->size();
    if (event->oldSize().isValid() && !(this->windowState() & (Qt::WindowMaximized | Qt::WindowFullScreen)) &&
        size.height() != getHeightForWidth(size.width()))
    {
        int width = size.width();
        if (width == event->oldSize().width())
            width = getWidthForHeight(size.height());
        this->resize(width, getHeightForWidth(width));
        return;
    }

    // The window is moved so that the keyboard stays at the same place on the screen when the area above it is shown
    // or hidden (not possible with Wayland)
    if (_keepKeyboardTop)
    {
        _keepKeyboardTop = false;
        int offset = ui->keyboard->y() - _keyboardTop;
        if (offset != 0)
            this->move(this->x(), this->y() - offset);
    }
}

int DialogKeyboard::getHeightForWidth(int width)
{
    // Height of the window when each element has its preferred size, the keyboard keeping its ratio
    return this->layout()->totalHeightForWidth(width);
}

int DialogKeyboard::getWidthForHeight(int height)
{
    // Largest width whose height doesn't exceed height (not less than the minimum width)
    int low = this->minimumWidth();
    int high = 16384;
    while (low < high)
    {
        int middle = (low + high + 1) / 2;
        if (getHeightForWidth(middle) <= height)
            low = middle;
        else
            high = middle - 1;
    }
    return low;
}

void DialogKeyboard::adjustHeight()
{
    // Same width (possibly increased by the minimum width of an area), the height following the content
    this->layout()->invalidate();
    this->layout()->activate();
    int width = qMax(this->width(), this->minimumWidth());
    int height = getHeightForWidth(width);
    if (width == this->width() && height == this->height())
        _keepKeyboardTop = false; // No resize event
    this->resize(width, height);
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

void DialogKeyboard::on_pushControllers_clicked()
{
    // Position of the keyboard to keep, the window growing or shrinking from its top
    _keyboardTop = ui->keyboard->y();
    _keepKeyboardTop = this->isVisible() && !(this->windowState() & (Qt::WindowMaximized | Qt::WindowFullScreen));

    // Toggle the visibility of the controller area
    bool isVisible = ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "control_area_expanded", false).toBool();
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "control_area_expanded", !isVisible);
    updateAreaVisibility();
}

void DialogKeyboard::on_pushEffects_clicked()
{
    // Toggle the visibility of the effect area
    bool isVisible = ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "effect_area_expanded", false).toBool();
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "effect_area_expanded", !isVisible);
    updateAreaVisibility();
}

void DialogKeyboard::updateAreaVisibility()
{
    // Initialize the expanded / collapsed state
    bool isVisible = ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "control_area_expanded", false).toBool();
    ui->controllerArea->setVisible(isVisible);
    ui->pushControllers->setChecked(isVisible);
    ui->pushControllers->setToolTip(isVisible ? tr("Hide the controller area") : tr("Show the controller area"));

    isVisible = ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "effect_area_expanded", false).toBool();
    ui->effectArea->setVisible(isVisible);
    ui->pushEffects->setChecked(isVisible);
    ui->pushEffects->setToolTip(isVisible ? tr("Hide the effects") : tr("Show the effects"));

    adjustHeight();
}

void DialogKeyboard::keyPressEvent(QKeyEvent * event)
{
    // Prevent the escape key from closing the window
    if (event->key() != Qt::Key_Escape)
          QDialog::keyPressEvent(event);
}

QIcon DialogKeyboard::getToggleIcon(QString svgPath)
{
    // Lit when checked, dimmed otherwise
    QIcon icon;
    icon.addPixmap(ContextManager::theme()->getColoredSvg(svgPath, QSize(16, 16), ThemeManager::HIGHLIGHTED_TEXT), QIcon::Normal, QIcon::On);
    QMap<QString, QString> replacement;
    replacement["currentColor"] = ThemeManager::mix(ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND),
                                                    ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT), 0.4).name();
    icon.addPixmap(ContextManager::theme()->getColoredSvg(svgPath, QSize(16, 16), replacement), QIcon::Normal, QIcon::Off);
    return icon;
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
