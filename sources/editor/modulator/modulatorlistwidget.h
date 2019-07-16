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

#ifndef MODULATORLISTWIDGET_H
#define MODULATORLISTWIDGET_H

#include <QListWidget>
#include "basetypes.h"

class ModulatorListWidget : public QListWidget
{
    Q_OBJECT

public:
    ModulatorListWidget(QWidget * parent = nullptr);

    // Get all modulators that have been selected
    QList<ModulatorData> getSelectedModulators();

signals:
    void pasted();
    void copied();
    void deleted();

protected:
    void keyPressEvent(QKeyEvent * event);

private slots:
    void onSelectionChanged();
};

#endif // MODULATORLISTWIDGET_H
