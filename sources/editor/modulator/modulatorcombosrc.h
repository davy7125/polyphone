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

#ifndef MODULATORCOMBOSRC_H
#define MODULATORCOMBOSRC_H

#include <QComboBox>
#include "basetypes.h"

class ModulatorComboSrc : public QComboBox
{
    Q_OBJECT

public:
    ModulatorComboSrc(QWidget* parent = nullptr);

    // Initialize the combobox
    void initialize(EltID id, bool source1);

    // Load value
    void loadValue();

    static QString getIndexName(quint16 iVal, bool CC);

private slots:
    void onCurrentIndexChanged(int index);

private:
    QList<int> getAssociatedMods(EltID id);
    void setLink(bool enabled, QString text);
    int getIndex();
    bool isCC();

    EltID _id;
    bool _source1;
};

#endif // MODULATORCOMBOSRC_H
