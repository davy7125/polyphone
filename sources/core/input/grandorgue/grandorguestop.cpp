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

#include "grandorguestop.h"
#include "grandorguepipe.h"

GrandOrgueStop::GrandOrgueStop(QString rootDir) :
    _rootDir(rootDir)
{

}

GrandOrgueStop::~GrandOrgueStop()
{
    while (!_pipes.isEmpty())
        delete _pipes.take(_pipes.firstKey());
}

void GrandOrgueStop::processData(QString key, QString value)
{
    if (key.startsWith("rank"))
    {
        if (key.length() < 7)
            return;

        // Extract the number of the rank
        key = key.right(key.length() - 4);
        bool ok = false;
        int number = key.left(3).toInt(&ok);
        if (!ok || number < 0)
            return;

        // Property
        QString property = key.length() > 3 ? key.right(key.length() - 3) : "#";

        // Store data
        _rankProperties[number][property] = value;
    }
    else if (key.startsWith("pipe"))
    {
        if (key.length() < 7)
            return;

        // Extract the number of the pipe
        key = key.right(key.length() - 4);
        bool ok = false;
        int number = key.left(3).toInt(&ok);
        if (!ok || number < 0)
            return;

        // Property
        QString property = key.length() > 3 ? key.right(key.length() - 3) : "#";

        // Store data
        if (!_pipes.contains(number))
            _pipes[number] = new GrandOrguePipe(_rootDir);
        _pipes[number]->processData(property, value);
    }
    else
        _properties[key] = value;
}

void GrandOrgueStop::createPreset(SoundfontManager * sm, EltID idSf2, QMap<int, GrandOrgueRank*> * ranks)
{

}
