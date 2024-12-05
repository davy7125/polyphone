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

#include "maintabbar.h"
#include <QMouseEvent>
#include <QPaintEvent>
#include <QStylePainter>
#include <QStyleOptionTab>
#include <QToolTip>
#include "maintabbarelement.h"
#include "contextmanager.h"

const int MainTabBar::TAB_MIN_WIDTH = 50;
const int MainTabBar::ARROW_WIDTH = 20;
const int MainTabBar::ARROW_INNER_MARGIN = 5;

MainTabBar::MainTabBar(QWidget *parent) : QWidget(parent),
    _clickedItemIndex(-1),
    _clickedInCloseButton(false),
    _firstTabDisplayed(0)
{
    setMouseTracking(true);

    // Icons
    _leftArrowIcon = ContextManager::theme()->getColoredSvg(
        ":/icons/arrow_left.svg", QSize(ARROW_WIDTH - ARROW_INNER_MARGIN, 1.5 * ARROW_WIDTH),
        ContextManager::theme()->isDark(ThemeManager::WINDOW_BACKGROUND, ThemeManager::WINDOW_TEXT) ?
            ThemeManager::WINDOW_TEXT : ThemeManager::WINDOW_BACKGROUND);
    _rightArrowIcon = ContextManager::theme()->getColoredSvg(
        ":/icons/arrow_right.svg", QSize(ARROW_WIDTH - ARROW_INNER_MARGIN, 1.5 * ARROW_WIDTH),             ContextManager::theme()->isDark(ThemeManager::WINDOW_BACKGROUND, ThemeManager::WINDOW_TEXT) ?
            ThemeManager::WINDOW_TEXT : ThemeManager::WINDOW_BACKGROUND);
}

void MainTabBar::addWidget(QWidget * widget, QString iconName, QString label, bool isColored)
{
    MainTabBarElement * tab = new MainTabBarElement(widget, iconName, isColored);
    tab->setLabel(label);
    _tabs.append(tab);
    _firstTabDisplayed = -1; // Go to the last position

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
    for (int i = 0; i < _tabs.count(); i++)
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
    for (int i = 0; i < _tabs.count(); i++)
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
    if (_withArrows)
    {
        if (pos.x() <= ARROW_WIDTH || pos.x() >= this->width() - ARROW_WIDTH)
            return -1;
    }

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
        if (index != -1)
        {
            QToolTip::showText(helpEvent->globalPos(), _tabs[index]->getToolTip());
        }
        else
        {
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
        // Middle click or click on the close button?
        if (event->button() == Qt::MouseButton::MiddleButton ||
            (_tabs[_clickedItemIndex]->isCloseButtonDisplayed() && _tabs[_clickedItemIndex]->getCloseButtonPath().contains(event->pos())))
        {
            // We are about to close the tab if the mouse stays in the button
            _clickedInCloseButton = true;
        }
        else
        {
            // Select a new tab immediatly
            emit widgetClicked(_tabs[_clickedItemIndex]->getWidget());
        }
    }
    else if (!_withArrows || (event->pos().x() > ARROW_WIDTH && event->pos().x() < this->width() - ARROW_WIDTH))
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

        QWidget::mouseMoveEvent(event);
    }
    else if (!_clickedInCloseButton)
    {
        this->setCursor(Qt::ClosedHandCursor);
        _xShift = event->pos().x() - _clickedPosX;
        repaint();
    }
}

void MainTabBar::mouseReleaseEvent(QMouseEvent *event)
{
    if (_withArrows && _clickedPosX <= ARROW_WIDTH && event->pos().x() <= ARROW_WIDTH)
    {
        goLeft();
    }
    else if (_withArrows && _clickedPosX >= this->width() - ARROW_WIDTH && event->pos().x() >= this->width() - ARROW_WIDTH)
    {
        goRight();
    }
    if (_clickedInCloseButton && _clickedItemIndex != -1 && (
            _tabs[_clickedItemIndex]->getCloseButtonPath().contains(event->pos()) ||
            (event->button() == Qt::MouseButton::MiddleButton && _clickedItemIndex == itemAt(event->pos()))))
    {
        // Select and close the tab
        emit widgetClicked(_tabs[_clickedItemIndex]->getWidget());
        emit closeClicked(_tabs[_clickedItemIndex]->getWidget());
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

    // Update the state of the tabs
    for (int i = 0; i < _tabs.count(); i++)
        _tabs[i]->mouseMoved(event->pos());

    this->repaint();
    QWidget::mouseReleaseEvent(event);
}

void MainTabBar::wheelEvent(QWheelEvent *event)
{
    if (_clickedItemIndex == -1 && !_clickedInCloseButton)
    {
        if (event->angleDelta().ry() > 0)
            goLeft();
        else if (event->angleDelta().ry() < 0)
            goRight();
    }
}

void MainTabBar::goLeft()
{
    if (_firstTabDisplayed > 0)
    {
        _firstTabDisplayed--;
        this->repaint();
    }
    else if (_firstTabDisplayed == -1 && _tabs.count() > 0)
    {
        _firstTabDisplayed = _tabs.count() - 1;
        int sumWidth = _tabs[_firstTabDisplayed]->getWidth();
        while (_firstTabDisplayed > 0 && sumWidth < this->width() - 2 * ARROW_WIDTH)
            sumWidth += _tabs[--_firstTabDisplayed]->getWidth();
        this->repaint();
    }
}

void MainTabBar::goRight()
{
    if (_firstTabDisplayed != -1 && _firstTabDisplayed < _tabs.count())
    {
        _firstTabDisplayed++;
        this->repaint();
    }
}

QSize MainTabBar::minimumSizeHint() const
{
    return QSize(0, MainTabBarElement::tabHeight());
}

void MainTabBar::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    // First compute all tab widths and get the offset
    QVector<int> widths(_tabs.count());
    for (int i = 0; i < _tabs.count(); i++)
        widths[i] = _tabs[i]->computeFullWidth(painter);
    int xOffset = adaptWidths(widths, this->width());

    if (_withArrows)
    {
        // Left / right buttons
        painter.drawPixmap(ARROW_INNER_MARGIN, 0.5 * (this->height() - _leftArrowIcon.height()), _leftArrowIcon);
        painter.drawPixmap(this->width() - _rightArrowIcon.width() - ARROW_INNER_MARGIN, 0.5 * (this->height() - _rightArrowIcon.height()), _rightArrowIcon);

        // Clipping
        painter.setClipRect(QRect(ARROW_WIDTH, 0, this->width()  - 2 * ARROW_WIDTH, this->height()));
    }

    int x = 0;
    for (int i = 0; i < _tabs.count(); i++)
    {
        _tabs[i]->setLimit(x - xOffset + (_withArrows ? ARROW_WIDTH : 0), widths[i]);
        x += widths[i];
    }

    if (_clickedItemIndex != -1 && !_clickedInCloseButton)
    {
        for (int i = 0; i < _tabs.count(); i++)
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
        for (int i = 0; i < _tabs.count(); i++)
            _tabs[i]->draw(painter, 0, this->height());
    }

    QWidget::paintEvent(event);
}

int MainTabBar::adaptWidths(QVector<int> &widths, int maxWidth)
{
    int sumWidth, firstBiggest, secondBiggest, firstBiggestCount;
    while ((sumWidth = sum(widths)) > maxWidth)
    {
        // Reduce the biggest widths but not less than the second biggest widths
        getTheTwoBiggestWidths(widths, firstBiggest, secondBiggest, firstBiggestCount);

        // Number of pixels to remove, globally and for each biggest tabs
        int delta = sumWidth - maxWidth;
        int deltaPerBiggestWidth = delta / firstBiggestCount;
        int deltaDelta = delta - deltaPerBiggestWidth * firstBiggestCount;
        if (firstBiggest - deltaPerBiggestWidth < secondBiggest)
            deltaPerBiggestWidth = firstBiggest - secondBiggest;

        // Resize the biggest widths
        for (int i = 0; i < widths.count(); i++)
        {
            if (widths[i] == firstBiggest)
            {
                widths[i] -= deltaPerBiggestWidth;
                if (deltaDelta)
                {
                    deltaDelta--;
                    widths[i] -= 1;
                }
            }
        }
    }

    // Minimum width
    int delta = 0;
    for (int i = 0; i < widths.count(); i++)
    {
        if (widths[i] < TAB_MIN_WIDTH)
        {
            delta += TAB_MIN_WIDTH - widths[i];
            widths[i] = TAB_MIN_WIDTH;
        }
    }
    if (delta > 0)
    {
        _withArrows = true;
        delta += 2 * ARROW_WIDTH;
    }
    else
        _withArrows = false;

    if (delta == 0)
        _firstTabDisplayed = 0;
    else if (_firstTabDisplayed == 0)
        delta = 0;
    else if (_firstTabDisplayed > 0)
    {
        // Maybe more can be displayed
        int rightWidth = 0;
        for (int i = _firstTabDisplayed; i < widths.count(); i++)
            rightWidth += widths[i];
        if (rightWidth < maxWidth - 2 * ARROW_WIDTH)
            _firstTabDisplayed = -1;
        else
        {
            // Otherwise the sum of the hidden column widths is returrned
            delta = 0;
            for (int i = 0; i < _firstTabDisplayed; i++)
                delta += widths[i];
        }
    }

    return delta;
}

int MainTabBar::sum(QVector<int> &v)
{
    int sum = 0;
    for (int i = 0; i < v.count(); i++)
        sum += v[i];
    return sum;
}

void MainTabBar::getTheTwoBiggestWidths(QVector<int> &v, int &firstBiggest, int &secondBiggest, int &firstBiggestCount)
{
    firstBiggest = 0;
    secondBiggest = 0;
    firstBiggestCount = 1;
    for (int i = 0; i < v.count(); i++)
    {
        if (v[i] > firstBiggest)
        {
            secondBiggest = firstBiggest;
            firstBiggest = v[i];
            firstBiggestCount = 1;
        }
        else if (v[i] == firstBiggest)
        {
            firstBiggestCount++;
        }
        else if (v[i] > secondBiggest)
            secondBiggest = v[i];
    }
}

QWidget * MainTabBar::getNextWidget()
{
    for (int i = 0; i < _tabs.count(); i++)
        if (_tabs[i]->isEnabled())
            return i < _tabs.count() - 1 ? _tabs[i + 1]->getWidget() : _tabs[0]->getWidget();

    return _tabs.count() > 0 ? _tabs[0]->getWidget() : nullptr;
}

QWidget * MainTabBar::getPreviousWidget()
{
    for (int i = 0; i < _tabs.count(); i++)
        if (_tabs[i]->isEnabled())
            return i > 0 ? _tabs[i - 1]->getWidget() : _tabs[_tabs.count() - 1]->getWidget();

    return _tabs.count() > 0 ? _tabs[_tabs.count() - 1]->getWidget() : nullptr;
}
