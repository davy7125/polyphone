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

#ifndef PAGEOVERVIEWSMPL_H
#define PAGEOVERVIEWSMPL_H

#include "pageoverview.h"

class PageOverviewSmpl : public PageOverview
{
    Q_OBJECT

public:
    PageOverviewSmpl(QWidget * parent = nullptr);

    bool isSuitableFor(ElementType elementType) override
    {
        return elementType == elementRootSmpl;
    }

protected:
    QString getTitle() override;
    QStringList getHorizontalHeader() override;
    void prepare(EltID id) override;
    void getInformation(EltID id, QStringList &info, QStringList &order, QList<int> &status) override;

private:
    QString isUsed(EltID id, int &status);
    QString totalLength(EltID id);
    QString loopLength(EltID id, int &status);
    quint8 rootKey(EltID id, int &status);
    qint8 correction(EltID id, int &status);
    QString type(EltID id, int &status);
    QString link(int &status);
    QString sampleRate(EltID id);
    void findLinkedSample(EltID id);
    QString compression(EltID id, int &status);

    QList<int> _usedSmpl;
    EltID _linkedSampleId;
    int _linkedSampleStatus; // 0: none, 1: valid, 2: not valid
};

#endif // PAGEOVERVIEWSMPL_H
