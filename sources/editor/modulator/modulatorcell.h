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
    // Initialization based on an existing id
    ModulatorCell(EltID id, QWidget *parent = nullptr);

    // Initialization based on a default modulator
    ModulatorCell(ModulatorData modulatorData, QWidget * parent = nullptr);

    ~ModulatorCell();

    AttributeType getTargetAttribute();
    EltID getID() { return _id; }

    // Inform the cell that it's been selected (the colors will change)
    void setSelected(bool isSelected);

    void setOverridenBy(int otherModulator);
    void setOverridingDefault();

    // Get the modulator as configured in the cell
    ModulatorData getModulatorData();

protected:
    void paintEvent(QPaintEvent* event);

private slots:
    void on_spinAmount_editingFinished();
    void on_comboTransform_currentIndexChanged(int index);
    void onOutputChanged(int dummy);

private:
    void initializeStyle();
    void updateLabelModNumber();

    Ui::ModulatorCell *ui;
    QColor _computationAreaColor, _computationAreaColorSelected;
    QColor _labelColor, _labelColorSelected;
    bool _isSelected;
    QFont _fontHint;
    EltID _id;
    bool _isPrst;
    SoundfontManager * _sm;
    QString _intRange;
    bool _overridenBy;

    static const QString s_doubleArrow;
};

#endif // MODULATORCELL_H
