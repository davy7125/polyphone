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

#ifndef PAGEOVERVIEWSMPL_H
#define PAGEOVERVIEWSMPL_H

#include "pageoverview.h"

class PageOverviewSmpl : public PageOverview
{
    Q_OBJECT

public:
    PageOverviewSmpl(QWidget * parent = nullptr);

protected:
    QString getTitle();
    QStringList getHorizontalHeader();
    void prepare(EltID id);
    void getInformation(EltID id, QStringList &info, QStringList &order);

private:
    QString isUsed(EltID id);
    QString totalLength(EltID id);
    QString loopLength(EltID id);
    QString rootKey(EltID id);
    QString correction(EltID id);
    QString type(EltID id);
    QString link(EltID id);
    QString sampleRate(EltID id);

    QList<int> _usedSmpl;
    bool _orderMode;
};

#endif // PAGEOVERVIEWSMPL_H
