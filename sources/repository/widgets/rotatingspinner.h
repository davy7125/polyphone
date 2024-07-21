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

#ifndef ROTATINGSPINNER_H
#define ROTATINGSPINNER_H

#include <QWidget>
#include "thememanager.h"
class QPropertyAnimation;


class RotatingSpinner : public QWidget
{
    Q_OBJECT

public:
    explicit RotatingSpinner(QWidget *parent = nullptr);
    ~RotatingSpinner();

    // Set the color type (default is window text color)
    void setColorType(ThemeManager::ColorType colorType) { _colorType = colorType; }

    // Start or restart the animation
    void startAnimation();

protected:
    void paintEvent(QPaintEvent * event);

private:
    QPropertyAnimation * _animation;
    QPixmap _pixmap;
    QSize _size;
    ThemeManager::ColorType _colorType;
};

#endif // ROTATINGSPINNER_H
