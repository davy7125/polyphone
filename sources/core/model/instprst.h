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

#ifndef INSTPRST_H
#define INSTPRST_H

#include <QMap>
#include "basetypes.h"
#include "treeitem.h"
#include "division.h"
#include "indexedelementlist.h"
class Soundfont;

class InstPrst: public TreeItem // Common class for inst and prst
{
public:
    InstPrst(Soundfont * soundfont, int row, TreeItem * parent, EltID id);
    virtual ~InstPrst() override;

    void decrementRow() { _row--; }

    // Division operations
    int addDivision();
    Division * getGlobalDivision() { return _globalDivision; }
    Division * getDivision(int index);
    const IndexedElementList<Division *> & getDivisions() { return _divisions; }
    bool deleteDivision(int index);
    int indexOfId(int id) override;

    // Name, extra fields
    void setName(QString name);
    QString getName();
    void setExtraField(AttributeType champ, int value);
    int getExtraField(AttributeType champ);

    // TreeItem implementation
    int childCount() const override;
    TreeItem * child(int row) override;
    QString display() override;
    int row() override { return _row; }

private:
    Soundfont * _soundfont;
    IndexedElementList<Division *> _divisions;
    Division * _globalDivision;
    bool _hidden;
    int _row;
    QString _name;
    QMap<AttributeType, int> _extraFields; // Used for presets only
};

#endif // INSTPRST_H
