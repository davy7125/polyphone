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

#include "maintabbarelement.h"
#include "contextmanager.h"

const int MainTabBarElement::TAB_ICON_SIZE = 24;
const int MainTabBarElement::TAB_CLOSE_ICON_PADDING = 5;
const int MainTabBarElement::CORNER_RADIUS = 3;
const int MainTabBarElement::MARGIN = 5;

int MainTabBarElement::tabHeight()
{
    return 4 * MARGIN + TAB_ICON_SIZE;
}

MainTabBarElement::MainTabBarElement(QWidget * widget, QString iconName, bool isColored, bool isFirst) :
    _widget(widget),
    _iconName(iconName),
    _isFirst(isFirst),
    _isEnabled(_isFirst),
    _gradient(QPointF(0, 0), QPointF(0, 1)),
    _gradientEnabled(QPointF(0, 0), QPointF(0, 1)),
    _closeButtonHovered(false)
{
    // Colors
    _highlightColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);
    _borderColor = ThemeManager::mix(
        ContextManager::theme()->getColor(ThemeManager::WINDOW_BACKGROUND),
        ContextManager::theme()->getColor(ThemeManager::WINDOW_TEXT),
        ContextManager::theme()->isDark(ThemeManager::WINDOW_BACKGROUND, ThemeManager::WINDOW_TEXT) ? 0.2 : 0.7);
    _textColor = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT);
    _backgroundColor = ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND);
    _closeButtonBackgroundColor = ThemeManager::mix(
        ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND),
        ContextManager::theme()->getColor(ThemeManager::LIST_TEXT),
        0.2);
    _textColorEnabled =
        isColored ?
            ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT) :
            ContextManager::theme()->getColor(ThemeManager::WINDOW_TEXT);
    _backgroundColorEnabled =
        isColored ?
            ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND) :
            ContextManager::theme()->getColor(ThemeManager::WINDOW_BACKGROUND);
    if (isColored)
    {
        _closeButtonBackgroundColorEnabled = ThemeManager::mix(
            ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND),
            ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT),
            0.2);
    }
    else
    {
        _closeButtonBackgroundColorEnabled = ThemeManager::mix(
            ContextManager::theme()->getColor(ThemeManager::WINDOW_BACKGROUND),
            ContextManager::theme()->getColor(ThemeManager::WINDOW_TEXT),
            0.2);
    }


    // Icons
    QMap<QString, QString> replacement;
    replacement["currentColor"] = _textColor.name();
    replacement["secondColor"] = _backgroundColor.name();
    _icon = ContextManager::theme()->getColoredSvg(iconName, QSize(TAB_ICON_SIZE, TAB_ICON_SIZE), replacement);

    if (!_isFirst)
    {
        _closeIcon = ContextManager::theme()->getColoredSvg(
            ":/icons/close.svg",
            QSize(TAB_ICON_SIZE - 2 * TAB_CLOSE_ICON_PADDING, TAB_ICON_SIZE - 2 * TAB_CLOSE_ICON_PADDING),
            replacement);
    }

    replacement["currentColor"] = _textColorEnabled.name();
    replacement["secondColor"] = _backgroundColorEnabled.name();
    _iconEnabled = ContextManager::theme()->getColoredSvg(iconName, QSize(TAB_ICON_SIZE, TAB_ICON_SIZE), replacement);

    if (!_isFirst)
    {
        _closeIconEnabled = ContextManager::theme()->getColoredSvg(
            ":/icons/close.svg",
            QSize(TAB_ICON_SIZE - 2 * TAB_CLOSE_ICON_PADDING, TAB_ICON_SIZE - 2 * TAB_CLOSE_ICON_PADDING),
            replacement);
    }

    // Gradients
    _gradient.setCoordinateMode(QGradient::ObjectMode);
    _gradient.setColorAt(0, _backgroundColor);
    _gradient.setColorAt(0.85, _backgroundColor);
    _gradient.setColorAt(0.93, _backgroundColor.darker(110));
    _gradient.setColorAt(1, _backgroundColor.darker(150));

    _gradientEnabled.setCoordinateMode(QGradient::ObjectMode);
    _gradientEnabled.setColorAt(0, _highlightColor);
    _gradientEnabled.setColorAt(0.13, _highlightColor);
    _gradientEnabled.setColorAt(0.18, _backgroundColorEnabled);
}

void MainTabBarElement::setLabel(QString label)
{
    int maxLabelSize = 30;
    _label = (label.size() > maxLabelSize ? label.left(maxLabelSize - 3) + "..." : label);
}


void MainTabBarElement::setTooltip(QString toolTip)
{
    _toolTip = toolTip;
}

void MainTabBarElement::setIsEnabled(bool isEnabled)
{
    _isEnabled = isEnabled;
}

int MainTabBarElement::draw(QPainter &painter, int x, int height)
{
    // Full width of the tab
    int textWidth = 0;
    int fullWidth = 4 * MARGIN + _icon.width();
    if (!_label.isEmpty())
    {
        textWidth = _label.isEmpty() ? 0 : painter.boundingRect(0, 0, 0, 0, 0, _label).width();
        fullWidth += 2 * MARGIN + textWidth;
    }
    if (!_isFirst)
        fullWidth += _closeIcon.width() + 2 * TAB_CLOSE_ICON_PADDING;

    // Draw the background with the border
    painter.setPen(_borderColor);
    painter.setBrush(_isEnabled ? QBrush(_gradientEnabled) : QBrush(_gradient));
    _tabPath.clear();
    _tabPath.moveTo(x, height); // Left border
    _tabPath.lineTo(x, CORNER_RADIUS + (_isEnabled ? 1 : 2) * MARGIN);
    _tabPath.arcTo(x, (_isEnabled ? 1 : 2) * MARGIN, 2 * CORNER_RADIUS, 2 * CORNER_RADIUS, 180, -90.0); // Top border
    _tabPath.arcTo(x + fullWidth - 2 * CORNER_RADIUS, (_isEnabled ? 1 : 2) * MARGIN, 2 * CORNER_RADIUS, 2 * CORNER_RADIUS, 90, -90.0);
    _tabPath.lineTo(x + fullWidth, height); // Right border
    _tabPath.lineTo(x, height); // Bottom
    painter.drawPath(_tabPath);

    if (_isEnabled)
    {
        // Bottom border hidden
        painter.setPen(QPen(_backgroundColorEnabled, 2));
        painter.drawLine(x, height, x + fullWidth, height);
    }
    else
    {
        // Bottom border visible
        painter.setPen(QPen(_borderColor, 1));
        painter.drawLine(x, height, x + fullWidth, height);
    }

    // Draw the icon
    painter.drawPixmap(x + 2 * MARGIN,
                       2 * MARGIN + (height - 2 * MARGIN) / 2 - _icon.height() / 2,
                       _isEnabled ? _iconEnabled : _icon);

    // Write the label
    painter.setPen(_isEnabled ? _textColorEnabled : _textColor);
    painter.drawText(x + 4 * MARGIN + _icon.width(), 2 * MARGIN,
                     textWidth, height - 2 * MARGIN, Qt::AlignLeft | Qt::AlignVCenter,
                     _label);

    // Possibly draw the close button
    if (!_isFirst)
    {
        // Shape of the close button
        _closeButtonPath.clear();
        _closeButtonPath.addRoundedRect(
            x + fullWidth - MARGIN - _closeIcon.width() - 2 * TAB_CLOSE_ICON_PADDING,
            2 * MARGIN + (height - 2 * MARGIN) / 2 - _closeIcon.height() / 2 - TAB_CLOSE_ICON_PADDING,
            _closeIcon.width() + 2 * TAB_CLOSE_ICON_PADDING,
            _closeIcon.height() + 2 * TAB_CLOSE_ICON_PADDING,
            CORNER_RADIUS, CORNER_RADIUS);

        if (_closeButtonHovered)
        {
            painter.setBrush(_isEnabled ? _closeButtonBackgroundColorEnabled : _closeButtonBackgroundColor);
            painter.setPen(Qt::transparent);
            painter.drawPath(_closeButtonPath);
        }

        painter.drawPixmap(x + fullWidth - MARGIN - _closeIcon.width() - TAB_CLOSE_ICON_PADDING,
                           2 * MARGIN + (height - 2 * MARGIN) / 2 - _closeIcon.height() / 2,
                           _isEnabled ? _closeIconEnabled : _closeIcon);
    }

    return fullWidth;
}

bool MainTabBarElement::mouseMoved(QPoint pos)
{
    bool closeButtonHovered = _closeButtonPath.contains(pos);
    if (closeButtonHovered != _closeButtonHovered)
    {
        _closeButtonHovered = closeButtonHovered;
        return true;
    }

    return false;
}
