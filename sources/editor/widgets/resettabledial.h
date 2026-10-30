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

// Dial edited by dragging the mouse, as in audio applications, whose default value is restored with a double click
// It is drawn with the theme colors whatever the style, so that it remains visible with a dark theme
class ResettableDial : public QDial
{
    Q_OBJECT

public:
    ResettableDial(QWidget * parent = nullptr);

    // Value restored with a double click (0 by default)
    void setDefaultValue(int defaultValue) { _defaultValue = defaultValue; }

protected:
    void mousePressEvent(QMouseEvent * event) override;
    void mouseMoveEvent(QMouseEvent * event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;
    void mouseDoubleClickEvent(QMouseEvent * event) override;
    void paintEvent(QPaintEvent * event) override;

private:
    int _defaultValue;
    double _dragValue;
    QPointF _lastPosition;
};

#endif // RESETTABLEDIAL_H
