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

#ifndef RESETTABLESLIDER_H
#define RESETTABLESLIDER_H

#include <QSlider>

// Slider whose default value is restored with a double click
class ResettableSlider : public QSlider
{
    Q_OBJECT

public:
    ResettableSlider(QWidget * parent = nullptr);

    // Value restored with a double click (0 by default)
    void setDefaultValue(int defaultValue) { _defaultValue = defaultValue; }

protected:
    void mouseDoubleClickEvent(QMouseEvent * event) override;

private:
    int _defaultValue;
};

#endif // RESETTABLESLIDER_H
