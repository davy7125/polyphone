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

#ifndef MAINTABBARELEMENT_H
#define MAINTABBARELEMENT_H

#include <QWidget>
#include <QPainter>
#include <QPainterPath>

class MainTabBarElement: QObject
{
    Q_OBJECT

public:
    MainTabBarElement(QWidget * widget, QString iconName, bool isColored, bool closable);

    void setLabel(QString label);
    void setTooltip(QString toolTip);
    void setIsEnabled(bool isEnabled);

    QWidget * getWidget() { return _widget; }
    QString getToolTip() {  return _toolTip; }
    QPainterPath getPath() { return _tabPath; }
    QPainterPath getCloseButtonPath() { return _closeButtonPath; }

    // Draw a tab at position (x, 0) with the specified height
    // The width of the tab is returned
    int draw(QPainter &painter, int x, int height);

    // Notify that the mouse position changed
    // Return true if a repaint is needed (close button hovered for instance)
    bool mouseMoved(QPoint pos);

    static int tabHeight();

signals:
    void closeClicked();

private:
    static const int TAB_ICON_SIZE;
    static const int TAB_CLOSE_ICON_PADDING;
    static const int CORNER_RADIUS;
    static const int MARGIN;

    // Configuration
    QWidget * _widget;
    QString _iconName;
    bool _isFirst;
    QString _label;
    QString _toolTip;
    bool _isEnabled;

    // Colors, gradients and icons
    QColor _highlightColor;
    QColor _borderColor;
    QColor _textColor;
    QColor _backgroundColor;
    QColor _closeButtonBackgroundColor;
    QLinearGradient _gradient;
    QPixmap _icon;
    QPixmap _closeIcon;
    QColor _textColorEnabled;
    QColor _backgroundColorEnabled;
    QColor _closeButtonBackgroundColorEnabled;
    QLinearGradient _gradientEnabled;
    QPixmap _iconEnabled;
    QPixmap _closeIconEnabled;

    // Tab and close button shapes
    QPainterPath _tabPath;
    QPainterPath _closeButtonPath;

    // State of the close button
    bool _closeButtonHovered;
};

#endif // MAINTABBARELEMENT_H
