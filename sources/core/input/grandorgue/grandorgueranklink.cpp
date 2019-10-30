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

#include "grandorgueranklink.h"
#include "grandorguedatathrough.h"
#include "soundfontmanager.h"
#include <QDebug>

GrandOrgueRankLink::GrandOrgueRankLink(GrandOrgueDataThrough *godt) :
    _godt(godt),
    _rankId(-1),
    _firstKey(36),
    _firstPipeIndex(1)
{

}

void GrandOrgueRankLink::readData(QString key, QString value)
{
    if (key == "#")
    {
        bool ok = false;
        _rankId = value.toInt(&ok);
        if (!ok)
        {
            qDebug() << "couldn't read rank link id:" << value;
            _rankId = -1;
        }
    }
    else
        _properties[key] = value;
}

void GrandOrgueRankLink::preProcess(int firstKey)
{
    _firstKey = firstKey;
}

RangesType GrandOrgueRankLink::getKeyRange()
{
    RangesType keyRange;
    keyRange.byLo = 0;
    keyRange.byHi = 127;

    if (_properties.contains("firstaccessiblekeynumber") &&
            _properties.contains("pipecount"))
    {
        bool ok = false;
        int val1 = _properties["firstaccessiblekeynumber"].toInt(&ok);
        if (!ok)
            return keyRange;
        int val2 = _properties["pipecount"].toInt(&ok);
        if (!ok)
            return keyRange;
        val1 += _firstKey - 1;
        val2 += val1 - 1;
        if (val1 >= 0 && val2 >= 0 && val1 < 128 && val2 < 128)
        {
            keyRange.byLo = static_cast<quint8>(val1);
            keyRange.byHi = static_cast<quint8>(val2);
        }
    }

    return keyRange;
}

int GrandOrgueRankLink::getFirstPipeIndex()
{
    int result = 1;

    if (_properties.contains("firstpipenumber"))
    {
        bool ok = false;
        result = _properties["firstpipenumber"].toInt(&ok);
        if (!ok)
            qDebug() << "couldn't read first pipe number of a link: " << _properties["firstpipenumber"];
    }

    return result;
}
