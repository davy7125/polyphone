/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
**                2014      Andrea Celani                                 **
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

#ifndef MODULATORCELL_H
#define MODULATORCELL_H

#include <QWidget>
#include "basetypes.h"
class SoundfontManager;

namespace Ui {
class ModulatorCell;
}

class ModulatorCell : public QWidget
{
    Q_OBJECT

public:
    explicit ModulatorCell(EltID id, QWidget *parent = nullptr);
    ~ModulatorCell();

    AttributeType getTargetAttribute();

protected:
    void paintEvent(QPaintEvent* event);

private:
    Ui::ModulatorCell *ui;
    QColor _computationAreaColor;
    QColor _labelColor;
    EltID _id;
    SoundfontManager * _sm;
};

#endif // MODULATORCELL_H
