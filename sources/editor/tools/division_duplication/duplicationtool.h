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

#ifndef DUPLICATIONTOOL_H
#define DUPLICATIONTOOL_H

#include "soundfontmanager.h"
#include <QMap>

class DuplicationTool : QObject
{
    Q_OBJECT

public:
    /// Initialize the duplication tool with an instrument or a preset
    DuplicationTool(EltID id);

    /// Duplicate each division by key
    void duplicateByKey();

    /// Duplicate each division by velocity
    void duplicateByVelocity(QList<QPair<int, int> > velocities);

private:
    /// Duplicate a division
    void duplicateByKey(QPair<int, int> keyRange, QPair<int, int> velRange, EltID id);

    /// Duplicate a series of division sharing the same key range
    void duplicateByVelocity(QMap<QPair<int, int>, QList<EltID> > elts, QList<QPair<int, int> > velocities);

    /// Copy the attributes of idFrom to idTo
    void duplicateGenMod(EltID idFrom, EltID idTo);

    SoundfontManager * _sf2;
    EltID _id;
    QMap<QPair<int, int>, QMap<QPair<int, int>, QList<EltID> > > _elts;
    bool _isInst;

    static bool lessThan(QPair<int, int> elt1, QPair<int, int> elt2);
};

#endif // DUPLICATIONTOOL_H
