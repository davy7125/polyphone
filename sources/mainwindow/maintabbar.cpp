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
    _currentItemInMotion(-1),
    _indexOfTabToClose(-1)
{
    setMouseTracking(true);
}

void MainTabBar::addWidget(QWidget * widget, QString iconName, QString label, bool isColored, bool isFirst)
{
    MainTabBarElement * tab = new MainTabBarElement(widget, iconName, isColored, isFirst);
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

void MainTabBar::moveItemTo(const QPoint &pos)
{

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
    // Reset state
    _indexOfTabToClose = -1;
    _currentItemInMotion = itemAt(event->pos());

    if (_currentItemInMotion != -1)
    {
        // Click on the close button?
        if (_tabs[_currentItemInMotion]->getCloseButtonPath().contains(event->pos()))
        {
            // We are about to close the tab if the mouse stays in the button
            _indexOfTabToClose = _currentItemInMotion;
        }
        else
        {
            // Select a new tab immediatly
            emit(widgetClicked(_tabs[_currentItemInMotion]->getWidget()));
        }
    }

    QWidget::mousePressEvent(event);
}

void MainTabBar::mouseMoveEvent(QMouseEvent *event)
{
    bool hoverUpdate = false;
    for (int i = 0; i < _tabs.count(); i++)
        hoverUpdate |= _tabs[i]->mouseMoved(event->pos());
    if (hoverUpdate)
        this->repaint();

    // this->setCursor(Qt::ArrowCursor);
    // Qt::ClosedHandCursor

    QWidget::mouseMoveEvent(event);
}

void MainTabBar::mouseReleaseEvent(QMouseEvent *event)
{
    if (_indexOfTabToClose != -1 && _tabs[_indexOfTabToClose]->getCloseButtonPath().contains(event->pos()))
    {
        // Select and close the tab
        emit(widgetClicked(_tabs[_indexOfTabToClose]->getWidget()));
        emit(closeClicked(_tabs[_indexOfTabToClose]->getWidget()));
    }

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

    painter.setPen(QColor(0, 100, 100));

    int x = -1; // Left border removed
    for (int i = 0; i < _tabs.size(); i++)
    {
        MainTabBarElement * tab = _tabs[i];
        x += tab->draw(painter, x, this->height());
    }

    QWidget::paintEvent(event);
}
