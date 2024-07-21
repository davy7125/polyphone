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

#ifndef GRANDORGUESTOP_H
#define GRANDORGUESTOP_H

#include <QMap>
#include "basetypes.h"
#include "grandorguerank.h"
class GrandOrgueDataThrough;
class GrandOrgueRankLink;
class SoundfontManager;

class GrandOrgueStop
{
public:
    GrandOrgueStop(QString rootDir, GrandOrgueDataThrough * godt, int id);
    ~GrandOrgueStop();

    void readData(QString key, QString value);
    bool isValid();
    void preProcess();
    void process(SoundfontManager * sm, int sf2Index, QMap<int, GrandOrgueRank *> &ranks, int presetId = -1);
    bool isTriggeredByThisSwitch(int switchNumber);

private:
    int getFirstPipeNumber();
    RangesType getDefaultKeyRange();

    QString _rootDir;
    GrandOrgueDataThrough * _godt;
    GrandOrgueRank _anonymousRank;
    QMap<QString, QString> _properties;
    QMap<int, GrandOrgueRankLink *> _rankLinks;
    QList<int> _switches;

    bool _writtenInSf2;
};

#endif // GRANDORGUESTOP_H
