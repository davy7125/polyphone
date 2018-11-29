/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2018 Davy Triponney                                **
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

#ifndef SMPL_H
#define SMPL_H

#include "basetypes.h"
#include "sound.h"
#include "treeitem.h"
class Soundfont;

class Smpl: public TreeItem
{
public:
    Smpl(Soundfont * soundfont, TreeItem * parent, EltID id);
    virtual ~Smpl() {}

    void setName(QString name);
    QString getName() { return _name; }

    Sound _sound;
    quint16 _wSampleLink;
    SFSampleLink _sfSampleType;

    // TreeItem implementation
    int childCount() const override;
    TreeItem * child(int row) override;
    QString display() override;
    int row() override;
    int indexOfId(int id) override;

private:
    Soundfont * _soundfont;
    QString _name;
};

#endif // SMPL_H
