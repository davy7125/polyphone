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

#ifndef PAGEOVERVIEWINST_H
#define PAGEOVERVIEWINST_H

#include "pageoverview.h"

class PageOverviewInst : public PageOverview
{
    Q_OBJECT

public:
    PageOverviewInst(QWidget * parent = nullptr);

    bool isSuitableFor(ElementType elementType) override
    {
        return elementType == elementRootInst;
    }

protected:
    QString getTitle() override;
    QStringList getHorizontalHeader() override;
    void prepare(EltID id) override;
    void getInformation(EltID id, QStringList &info, QStringList &order, QList<int> &status) override;

private:
    QString isUsed(EltID id, int &status);
    QString getSampleNumber(EltID id);
    QString getParameterNumber(EltID id);
    QString getModulatorNumber(EltID id);
    QString getKeyRange(EltID id, bool orderMode);
    QString getVelocityRange(EltID id, bool orderMode);
    QString getAttenuation(EltID id, bool orderMode);
    QString getLoop(EltID id);
    QString getChorus(EltID id, bool orderMode);
    QString getReverb(EltID id, bool orderMode);

    QList<int> _usedInst;
};

#endif // PAGEOVERVIEWINST_H
