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

#include "directorydisplaymenu.h"
#include "ui_directorydisplaymenu.h"
#include "contextmanager.h"
#include <QMenu>
#include <QMouseEvent>
#include <QWidgetAction>
#include "menulabel.h"

DirectoryDisplayMenu::DirectoryDisplayMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DirectoryDisplayMenu)
{
    ui->setupUi(this);

    // Style
    _textColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name();
    _backgroundColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name();
    _hoveredBackgroundColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT, ThemeManager::HOVERED).name();
    ui->labelDisplay->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/eye.svg", QSize(16, 16), ThemeManager::HIGHLIGHTED_BACKGROUND));
    this->setStyle(false);

    // Icons
    _emptyIcon.addPixmap(QPixmap(24, 24), QIcon::Normal);
    _emptyIcon.addPixmap(QPixmap(24, 24), QIcon::Active);
    _checkIcon.addPixmap(ContextManager::theme()->getColoredSvg(":/icons/check.svg", QSize(24, 24), ThemeManager::LIST_TEXT), QIcon::Normal);
    _checkIcon.addPixmap(ContextManager::theme()->getColoredSvg(":/icons/check.svg", QSize(24, 24), ThemeManager::HIGHLIGHTED_TEXT), QIcon::Active);

    // Add a menu to the button
    QMenu * menu = new QMenu(this);
    menu->installEventFilter(this);
    connect(menu, SIGNAL(aboutToHide()), this, SLOT(onMenuClosed()));
    menu->setStyleSheet(ContextManager::theme()->getMenuTheme());

    // QWidgetAction * wAction = new QWidgetAction(menu);
    // wAction->setDefaultWidget(new MenuLabel(tr("Samples")));
    // menu->addAction(wAction);

    // int displayOptions = ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "directory_browser_display_options", 0x1FFF).toInt();
    // QAction * action = menu->addAction(tr("sampling rate (Hz)"));
    // action->setData(displayOptions & 0x0001);
    // action->setIcon(action->data().toBool() ? _checkIcon : _emptyIcon);
    // action = menu->addAction(tr("total duration (s)"));
    // action->setData(displayOptions & 0x0002);
    // action->setIcon(action->data().toBool() ? _checkIcon : _emptyIcon);
    // action = menu->addAction(tr("loop duration (s)"));
    // action->setData(displayOptions & 0x0004);
    // action->setIcon(action->data().toBool() ? _checkIcon : _emptyIcon);

    // wAction = new QWidgetAction(menu);
    // wAction->setDefaultWidget(new MenuLabel(tr("Instruments")));
    // menu->addAction(wAction);

    // action = menu->addAction(tr("number of divisions"));
    // action->setData(displayOptions & 0x0008);
    // action->setIcon(action->data().toBool() ? _checkIcon : _emptyIcon);
    // action = menu->addAction(tr("number of distinct key ranges"));
    // action->setData(displayOptions & 0x0010);
    // action->setIcon(action->data().toBool() ? _checkIcon : _emptyIcon);
    // action = menu->addAction(tr("number of distinct velocity ranges"));
    // action->setData(displayOptions & 0x0020);
    // action->setIcon(action->data().toBool() ? _checkIcon : _emptyIcon);
    // action = menu->addAction(tr("number of parameters"));
    // action->setData(displayOptions & 0x0040);
    // action->setIcon(action->data().toBool() ? _checkIcon : _emptyIcon);
    // action = menu->addAction(tr("number of modulators"));
    // action->setData(displayOptions & 0x0080);
    // action->setIcon(action->data().toBool() ? _checkIcon : _emptyIcon);

    // wAction = new QWidgetAction(menu);
    // wAction->setDefaultWidget(new MenuLabel(tr("Presets")));
    // menu->addAction(wAction);

    // action = menu->addAction(tr("number of divisions"));
    // action->setData(displayOptions & 0x0100);
    // action->setIcon(action->data().toBool() ? _checkIcon : _emptyIcon);
    // action = menu->addAction(tr("number of distinct key ranges"));
    // action->setData(displayOptions & 0x0200);
    // action->setIcon(action->data().toBool() ? _checkIcon : _emptyIcon);
    // action = menu->addAction(tr("number of distinct velocity ranges"));
    // action->setData(displayOptions & 0x0400);
    // action->setIcon(action->data().toBool() ? _checkIcon : _emptyIcon);
    // action = menu->addAction(tr("number of parameters"));
    // action->setData(displayOptions & 0x0800);
    // action->setIcon(action->data().toBool() ? _checkIcon : _emptyIcon);
    // action = menu->addAction(tr("number of modulators"));
    // action->setData(displayOptions & 0x1000);
    // action->setIcon(action->data().toBool() ? _checkIcon : _emptyIcon);

    ui->pushButton->setMenu(menu);
}

DirectoryDisplayMenu::~DirectoryDisplayMenu()
{
    delete ui;
}

int DirectoryDisplayMenu::displayOptions()
{
    int displayOptions = 0;
    QList<QAction *> actions = ui->pushButton->menu()->actions();
    int actionIndex = 0;
    for (int i = 0; i < actions.count(); i++)
    {
        if (i == 0 || i == 4 || i == 10)
            continue; // Separators
        if (actions[i]->data().toBool())
            displayOptions += (0x0001 << actionIndex);
        actionIndex++;
    }
    return displayOptions;
}

#if QT_VERSION >= 0x060000
void DirectoryDisplayMenu::enterEvent(QEnterEvent *event)
#else
void DirectoryDisplayMenu::enterEvent(QEvent *event)
#endif
{
    setStyle(true);
    QWidget::enterEvent(event);
}

void DirectoryDisplayMenu::leaveEvent(QEvent *event)
{
    setStyle(false);
    QWidget::leaveEvent(event);
}

void DirectoryDisplayMenu::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        ui->pushButton->menu()->exec();
    QWidget::mousePressEvent(event);
}

void DirectoryDisplayMenu::onMenuClosed()
{
    setStyle(false);
}

void DirectoryDisplayMenu::setStyle(bool hovered)
{
    ui->pushButton->setStyleSheet("QPushButton{border: 0; padding:0 5px 0 5px;color:" + _textColor + ";background-color:" +
                                  (hovered ? _hoveredBackgroundColor : _backgroundColor) + ";border-top-right-radius:2px;border-bottom-right-radius:2px}" +
                                  "QPushButton::menu-indicator{width:0px;image:url(:/misc/transparent.png)}");
    ui->labelDisplay->setStyleSheet("QLabel{border-top-left-radius:2px;border-bottom-left-radius:2px;padding: 0 5px;background-color:" +
                                 (hovered ? _hoveredBackgroundColor : _backgroundColor) + "}");
}

bool DirectoryDisplayMenu::eventFilter(QObject *obj, QEvent *event)
{
    QMenu * menu = qobject_cast<QMenu *>(obj);
    if (!menu)
        return QWidget::eventFilter(obj, event);

    if (event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
        QAction * action = menu->actionAt(mouseEvent->pos());
        if (action)
        {
            action->setData(!action->data().toBool());
            action->setIcon(action->data().toBool() ? _checkIcon : _emptyIcon);
            int currentOptions = displayOptions();
            emit displayOptionsChanged(currentOptions);
            ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "directory_browser_display_options", currentOptions);
            return true; // consume the event and don't close the menu
        }
    }

    return QWidget::eventFilter(obj, event);
}