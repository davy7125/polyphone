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

#ifndef PIANOKEYBDCUSTOM_H
#define PIANOKEYBDCUSTOM_H

#include "pianokeybd.h"
#include <QKeyEvent>


class PianoKeybdCustom : public PianoKeybd
{
    Q_OBJECT

public:
    PianoKeybdCustom(QWidget * parent);

    // Add a range and a root key if rootKey != -1
    void addRangeAndRootKey(int rootKey, int noteMin, int noteMax);

    // Remove all customization
    void clearCustomization();

    // Add a current range around a key
    void addCurrentRange(int note, int noteMin, int noteMax);

public slots:
    // Remove a current range around a key
    void removeCurrentRange(int num);

private slots:
    void setKey(int num, int vel);

private:
    void updateRanges();

    QList<int> _currentRange;
    QList<int> _rootKeys;
    QMap<int, QList<int> > _mapPressed;

    static QColor COLOR_PRESSED;
    static QColor COLOR_BLACK_ENABLED;
    static QColor COLOR_WHITE_ENABLED;
    static QColor COLOR_BLACK_DISABLED;
    static QColor COLOR_WHITE_DISABLED;
    static QColor COLOR_BLACK_RANGE;
    static QColor COLOR_WHITE_RANGE;
};

#endif // PIANOKEYBDCUSTOM_H
