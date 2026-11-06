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


#include "bannermenu.h"
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QScreen>
#include <QToolButton>
#include <QWidgetAction>
#include "contextmanager.h"

BannerMenu::BannerMenu(QToolButton * button, QWidget * parent) :
    QMenu(parent),
    _button(button)
{
    ThemeManager * theme = ContextManager::theme();
    this->setStyleSheet(theme->getMenuTheme());

    QWidget * widget = new QWidget();
    widget->setAttribute(Qt::WA_NoMousePropagation); // A click on a label does not close the menu
    widget->setStyleSheet("QLabel,QCheckBox{color:" + theme->getColor(ThemeManager::LIST_TEXT).name() + "}");
    _grid = new QGridLayout(widget);
    _grid->setContentsMargins(15, 5, 15, 5);
    _grid->setHorizontalSpacing(10);
    _grid->setVerticalSpacing(10);

    QWidgetAction * action = new QWidgetAction(this);
    action->setDefaultWidget(widget);
    this->addAction(action);

    button->setMenu(this);
}

QCheckBox * BannerMenu::addCheckBox(QString text, int row, int columnCount)
{
    QLabel * label = new QLabel(text);
    label->installEventFilter(this);
    QCheckBox * checkBox = new QCheckBox();
    checkBox->setFocusPolicy(Qt::NoFocus);
    _grid->addWidget(label, row, 0, 1, columnCount - 1);
    _grid->addWidget(checkBox, row, columnCount - 1, Qt::AlignRight);
    _checkBoxes[label] = checkBox;
    return checkBox;
}

void BannerMenu::showEvent(QShowEvent * event)
{
    QMenu::showEvent(event);

    // Aligned on the right side of the button (a right-to-left button would have its arrow on the left side), within
    // the screen
    QPoint position = _button->mapToGlobal(QPoint(_button->width() - this->width(), _button->height()));
    if (this->screen() != nullptr)
        position.setX(qMax(position.x(), this->screen()->availableGeometry().left()));
    this->move(position.x(), this->y());
}

bool BannerMenu::eventFilter(QObject * watched, QEvent * event)
{
    if (_checkBoxes.contains(watched) && event->type() == QEvent::MouseButtonRelease &&
        static_cast<QMouseEvent *>(event)->button() == Qt::LeftButton)
    {
        _checkBoxes[watched]->toggle();
        return true;
    }
    return QMenu::eventFilter(watched, event);
}
