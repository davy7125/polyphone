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

#ifndef DIVISION_H
#define DIVISION_H

#include "basetypes.h"
#include <QMap>
#include "treeitem.h"
#include "indexedelementlist.h"
class Modulator;
class InstPrst;
class Soundfont;

class Division: public TreeItem
{
public:
    Division(InstPrst * instPrst, Soundfont * soundfont, TreeItem * parent, EltID id);
    virtual ~Division() override;

    // Operations on parameters
    bool isSet(AttributeType champ);
    void setGen(AttributeType champ, AttributeValue value);
    void resetGen(AttributeType champ);
    AttributeValue getGen(AttributeType champ);
    const QMap<AttributeType, AttributeValue> & getGens() { return _parameters; }

    void setMute(bool mute) { _mute = mute; }
    bool isMute() { return _mute; }

    // Operations on modulators
    int addMod();
    Modulator * getMod(int index);
    const IndexedElementList<Modulator *> & getMods() { return _modulators; }
    bool deleteMod(int index);

    // TreeItem implementation
    int childCount() const override;
    TreeItem * child(int row) override;
    QString display() override;
    int row() override;
    int indexOfId(int id) override;

private:
    InstPrst * _instPrst;
    Soundfont * _soundfont;
    IndexedElementList<Modulator *> _modulators;
    QMap<AttributeType, AttributeValue> _parameters;
    bool _mute;
};

#endif // DIVISION_H
