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

#include "maintabbar.h"
#include <QMouseEvent>
#include <QPaintEvent>
#include <QStylePainter>
#include <QStyleOptionTab>
#include <QToolTip>
#include "maintabbarelement.h"

MainTabBar::MainTabBar(QWidget *parent) : QWidget(parent),
    _clickedItemIndex(-1),
    _clickedInCloseButton(false)
{
    setMouseTracking(true);
}

void MainTabBar::addWidget(QWidget * widget, QString iconName, QString label, bool isColored)
{
    MainTabBarElement * tab = new MainTabBarElement(widget, iconName, isColored);
    tab->setLabel(label);
    _tabs.append(tab);

    this->repaint();
}

void MainTabBar::removeWidget(QWidget * widget)
{
    for (int i = _tabs.count() - 1; i >= 0; i--)
        if (_tabs[i]->getWidget() == widget)
            _tabs.removeAt(i);

    // Repaint will be done later when another widget will be enabled
}

void MainTabBar::setWidgetLabel(QWidget * widget, const QString &label)
{
    for (int i = 0; i < _tabs.size(); i++)
    {
        if (_tabs[i]->getWidget() == widget)
        {
            _tabs[i]->setLabel(label);
            this->repaint();
            break;
        }
    }
}

void MainTabBar::setWidgetToolTip(QWidget * widget, const QString &tip)
{
    for (int i = 0; i < _tabs.size(); i++)
    {
        if (_tabs[i]->getWidget() == widget)
        {
            _tabs[i]->setTooltip(tip);
            break;
        }
    }
}

void MainTabBar::currentWidgetChanged(QWidget * widget)
{
    for (int i = 0; i < _tabs.count(); i++)
        _tabs[i]->setIsEnabled(_tabs[i]->getWidget() == widget);
    this->repaint();
}

int MainTabBar::itemAt(const QPoint &pos)
{
    for (int i = 0; i < _tabs.count(); i++)
    {
        MainTabBarElement * tab = _tabs[i];
        if (tab->getPath().contains(pos))
            return i;
    }
    return -1;
}

bool MainTabBar::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip)
    {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
        int index = itemAt(helpEvent->pos());
        if (index != -1) {
            QToolTip::showText(helpEvent->globalPos(), _tabs[index]->getToolTip());
        } else {
            QToolTip::hideText();
            event->ignore();
        }

        return true;
    }
    return QWidget::event(event);
}

void MainTabBar::mousePressEvent(QMouseEvent *event)
{
    // Save state
    _clickedPosX = event->pos().x();
    _clickedInCloseButton = false;
    _clickedItemIndex = itemAt(event->pos());
    _xShift = 0;

    if (_clickedItemIndex != -1)
    {
        // Click on the close button?
        if (_tabs[_clickedItemIndex]->getCloseButtonPath().contains(event->pos()))
        {
            // We are about to close the tab if the mouse stays in the button
            _clickedInCloseButton = true;
        }
        else
        {
            // Select a new tab immediatly
            emit(widgetClicked(_tabs[_clickedItemIndex]->getWidget()));
        }
    }

    QWidget::mousePressEvent(event);
}

void MainTabBar::mouseMoveEvent(QMouseEvent *event)
{
    if (_clickedItemIndex == -1)
    {
        // Possibly hover a close button
        bool hoverUpdate = false;
        for (int i = 0; i < _tabs.count(); i++)
            hoverUpdate |= _tabs[i]->mouseMoved(event->pos());
        if (hoverUpdate)
            this->repaint();
    }
    else if (!_clickedInCloseButton)
    {
        this->setCursor(Qt::ClosedHandCursor);
        _xShift = event->pos().x() - _clickedPosX;
        repaint();
    }

    QWidget::mouseMoveEvent(event);
}

void MainTabBar::mouseReleaseEvent(QMouseEvent *event)
{
    if (_clickedInCloseButton && _clickedItemIndex != -1 && _tabs[_clickedItemIndex]->getCloseButtonPath().contains(event->pos()))
    {
        // Select and close the tab
        emit(widgetClicked(_tabs[_clickedItemIndex]->getWidget()));
        emit(closeClicked(_tabs[_clickedItemIndex]->getWidget()));
    }
    else if (_clickedItemIndex != -1 && !_clickedInCloseButton && _xShift != 0)
    {
        // Possibly reorder the tabs, taking into account their current paths
        QVector<MainTabBarElement *> reorederedTabs;
        bool itemPlaced = false;
        for (int i = 0; i < _tabs.count(); i++)
        {
            if (i == _clickedItemIndex)
                continue;
            if (!itemPlaced && _tabs[_clickedItemIndex]->getXstart() + _tabs[_clickedItemIndex]->getCurrentShift() < _tabs[i]->getXstart() + _tabs[i]->getCurrentShift())
            {
                reorederedTabs.append(_tabs[_clickedItemIndex]);
                itemPlaced = true;
            }
            reorederedTabs.append(_tabs[i]);
        }
        if (!itemPlaced)
            reorederedTabs.append(_tabs[_clickedItemIndex]);
        _tabs = reorederedTabs;
    }

    // Reset state
    _clickedInCloseButton = false;
    _clickedItemIndex = -1;
    this->setCursor(Qt::ArrowCursor);

    this->repaint();
    QWidget::mouseReleaseEvent(event);
}

void MainTabBar::resizeEvent(QResizeEvent *event)
{
    // int margin = style()->pixelMetric(QStyle::PM_DefaultTopLevelMargin);
    // int x = width() - margin;
    // int y = height() - margin;

    // y = updateButtonGeometry(newCircleButton, x, y);
    // y = updateButtonGeometry(newSquareButton, x, y);
    // updateButtonGeometry(newTriangleButton, x, y);
}

QSize MainTabBar::minimumSizeHint() const
{
    return QSize(0, MainTabBarElement::tabHeight());
}

void MainTabBar::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    // First compute all tab positions
    int x = 0;
    for (int i = 0; i < _tabs.size(); i++)
        x += _tabs[i]->computePosition(painter, x);

    if (_clickedItemIndex != -1 && !_clickedInCloseButton)
    {
        for (int i = 0; i < _tabs.size(); i++)
        {
            if (i < _clickedItemIndex)
            {
                // The tab is possibly sent to the right if its center is after the start of the moving tab
                if (_tabs[i]->getXstart() + _tabs[i]->getWidth() / 2 > _tabs[_clickedItemIndex]->getXstart() + _xShift)
                    _tabs[i]->draw(painter, _tabs[_clickedItemIndex]->getWidth(), this->height());
                else
                    _tabs[i]->draw(painter, 0, this->height());
            }
            else if (i > _clickedItemIndex)
            {
                // The tab is possibly sent to the left if its center is before the end of the moving tab
                if (_tabs[i]->getXstart() + _tabs[i]->getWidth() / 2 < _tabs[_clickedItemIndex]->getXstart() + _tabs[_clickedItemIndex]->getWidth() + _xShift)
                    _tabs[i]->draw(painter, -_tabs[_clickedItemIndex]->getWidth(), this->height());
                else
                    _tabs[i]->draw(painter, 0, this->height());
            }
        }

        // Finally draw the moving tab inside the limits
        if (_tabs[_clickedItemIndex]->getXstart() + _xShift < 0)
            _tabs[_clickedItemIndex]->draw(painter, -_tabs[_clickedItemIndex]->getXstart(), this->height());
        else if (_tabs[_clickedItemIndex]->getXstart() + _tabs[_clickedItemIndex]->getWidth() + _xShift > _tabs[_tabs.length() - 1]->getXstart() + _tabs[_tabs.length() - 1]->getWidth())
            _tabs[_clickedItemIndex]->draw(
                painter,
                _tabs[_tabs.length() - 1]->getXstart() + _tabs[_tabs.length() - 1]->getWidth() - _tabs[_clickedItemIndex]->getXstart() - _tabs[_clickedItemIndex]->getWidth(),
                this->height());
        else
            _tabs[_clickedItemIndex]->draw(painter, _xShift, this->height());
    }
    else
    {
        // Regular situation
        for (int i = 0; i < _tabs.size(); i++)
            _tabs[i]->draw(painter, 0, this->height());
    }

    QWidget::paintEvent(event);
}

QVector<QPair<int, MainTabBarElement *>> MainTabBar::reorder(QVector<QPair<int, MainTabBarElement *>> &tabs, int itemIndex, int shift)
{
    // Left and right positions of the item to move


}
