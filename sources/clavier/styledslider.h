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

#ifndef PITCHWHEELSLIDER_H
#define PITCHWHEELSLIDER_H

#include <QSlider>
#include <QTimer>

class StyledSlider : public QSlider
{
    Q_OBJECT

public:
    StyledSlider(QWidget * parent = nullptr);

    // Configuration of the slider
    void setColorFromMiddle(bool fromMiddle) { _fromMiddle = fromMiddle; }

    // If set, the handle will be back to the specified position
    void setBackToValue(int middleValue) { _middleValue = middleValue; }

protected:
    void paintEvent(QPaintEvent *event);

private slots:
    void onTimeout();
    void onValueChanged(int value);
    void onSliderPressed() { _pressed = true; }
    void onSliderReleased() { _pressed = false; onValueChanged(this->value()); }

private:
    QColor _highlightColor;
    QColor _backgroundColor;
    bool _fromMiddle;
    int _middleValue;
    bool _pressed;
    bool _fast;

    QTimer _timer;
};

#endif // PITCHWHEELSLIDER_H
