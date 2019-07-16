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

#ifndef MODULATORCOMBOCURVE_H
#define MODULATORCOMBOCURVE_H

#include <QComboBox>
#include "basetypes.h"

// Special combobox displaying a list of curves used for the modulators
class ModulatorComboCurve : public QComboBox
{
    Q_OBJECT

public:
    explicit ModulatorComboCurve(QWidget* parent = nullptr);
    ~ModulatorComboCurve();
    bool eventFilter(QObject* object, QEvent* event);

    void initialize(EltID id, bool source1);
    void initialize(SFModulator mod);
    void loadValue();

    // Get information about the evolution
    QString getEvolution();
    bool isBipolar() { return _isBipolar; }
    bool isDescending() { return _isDescending; }
    quint8 getType() { return _type; }

private:
    void valueSelected(int row, int column);

    EltID _id;
    bool _source1;
    quint8 _type;
    bool _isBipolar;
    bool _isDescending;

    static const QString s_rightArrow;
};

#endif // MODULATORCOMBOCURVE_H
