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

#include "smpl.h"
#include "soundfont.h"
#include "utils.h"

Smpl::Smpl(int row, TreeItem *parent, EltID id) : TreeItem(id, parent),
    _row(row)
{

}

int Smpl::childCount() const
{
    return 0;
}

TreeItem * Smpl::child(int row)
{
    Q_UNUSED(row)
    return nullptr;
}

QString Smpl::display()
{
    return _name.isEmpty() ? "..." : _name;
}

QString Smpl::sortText()
{
    return _nameSort;
}

void Smpl::setName(QString name)
{
    _name = name;
    _nameSort = Utils::removeAccents(_name).toLower();
    notifyRename();
}

int Smpl::indexOfId(int id)
{
    Q_UNUSED(id)
    return -1;
}
