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

#ifndef LEDBUTTON_H
#define LEDBUTTON_H

#include <QPushButton>

// Checkable button displaying a led, lit when checked
class LedButton : public QPushButton
{
    Q_OBJECT

public:
    LedButton(QWidget * parent = nullptr);

protected:
    // The icon is also updated when the state is changed with the signals blocked
    void checkStateSet() override;
    void nextCheckState() override;
    void paintEvent(QPaintEvent * event) override;

private:
    void updateIcon();

    QPixmap _ledOn;
    QPixmap _ledOff;
};

#endif // LEDBUTTON_H
