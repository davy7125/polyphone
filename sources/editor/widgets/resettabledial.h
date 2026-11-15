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

#ifndef RESETTABLEDIAL_H
#define RESETTABLEDIAL_H

#include <QDial>
class QLabel;
class QTimer;

// Dial edited by dragging the mouse, as in audio applications, whose default value is restored with a double click
// It is drawn with the theme colors whatever the style, so that it remains visible with a dark theme
// While it is adjusted, a bubble can display its value below its name (tooltip), since a tooltip is closed by Qt as soon
// as the mouse is clicked or the wheel is used
class ResettableDial : public QDial
{
    Q_OBJECT

public:
    ResettableDial(QWidget * parent = nullptr);
    ~ResettableDial() override;

    // Value restored with a double click (0 by default)
    void setDefaultValue(int defaultValue) { _defaultValue = defaultValue; }

    // Bubble displaying the value while adjusting the dial (disabled by default)
    void setValueBubbleEnabled(bool enabled);

protected:
    bool event(QEvent * event) override;
    void mousePressEvent(QMouseEvent * event) override;
    void mouseMoveEvent(QMouseEvent * event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;
    void mouseDoubleClickEvent(QMouseEvent * event) override;
    void wheelEvent(QWheelEvent * event) override;
    void keyPressEvent(QKeyEvent * event) override;
    void leaveEvent(QEvent * event) override;
    void hideEvent(QHideEvent * event) override;
    void paintEvent(QPaintEvent * event) override;

private:
    // While dragging, the bubble remains until the mouse is released, plus a delay
    // Otherwise (wheel, keys, double click), it is hidden after a delay or as soon as the mouse leaves the dial
    void showValueBubble(bool dragging);
    void hideValueBubble();

    int _defaultValue;
    double _dragValue;
    QPointF _lastPosition;
    QLabel * _valueBubble; // Created when it is first displayed
    QTimer * _bubbleTimer;
    bool _valueBubbleEnabled;
    bool _bubbleFromDrag;

    // Only one bubble at a time, the bubbles of adjacent dials would overlap
    static ResettableDial * s_dialWithBubble;
};

#endif // RESETTABLEDIAL_H
