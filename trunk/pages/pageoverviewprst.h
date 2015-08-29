/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef PAGEOVERVIEWPRST_H
#define PAGEOVERVIEWPRST_H

#include "pageoverview.h"

class PageOverviewPrst : public PageOverview
{
    Q_OBJECT

public:
    PageOverviewPrst(QWidget * parent = NULL);

protected:
    QString getTitle();
    QStringList getHorizontalHeader();
    void prepare(EltID id) { Q_UNUSED(id) }
    QStringList getInformation(EltID id);

private:
    QString getBankAndPreset(EltID id);
    QString getSampleNumber(EltID id);
    QString getParameterNumber(EltID id);
    QString getModulatorNumber(EltID id);
    QString getKeyRange(EltID id);
    QString getVelocityRange(EltID id);
    QString getAttenuation(EltID id);
    QString getChorus(EltID id);
    QString getReverb(EltID id);
};

#endif // PAGEOVERVIEWPRST_H
