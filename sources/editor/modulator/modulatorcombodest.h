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

#ifndef MODULATORCOMBODEST_H
#define MODULATORCOMBODEST_H

#include <QComboBox>
#include "basetypes.h"

class ModulatorComboDest : public QComboBox
{
    Q_OBJECT

public:
    ModulatorComboDest(QWidget* parent = nullptr);

    // Initialize the combobox
    void initialize(EltID id);
    void initialize(quint16 index);

    // Load value
    void loadValue();

    // Get the attribute
    AttributeType getCurrentAttribute();

    // Keep a minimum width of 50px, regardless the content of the ComboBox
    QSize sizeHint() const override        { return minimumSizeHint(); }
    QSize minimumSizeHint() const override { return QSize(50, QComboBox::minimumSizeHint().height()); }

private slots:
    void onCurrentIndexChanged(int index);

private:
    void selectAttribute(AttributeType attribute);
    void selectIndex(int index);
    int getCurrentIndex();

    QList<AttributeType> _destIndex;
    EltID _id;
    QList<int> _listIndex;
};

#endif // MODULATORCOMBODEST_H
