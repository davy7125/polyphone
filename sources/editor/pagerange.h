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

#ifndef PAGERANGE_H
#define PAGERANGE_H

#include "page.h"

namespace Ui {
class PageRange;
}

class PageRange : public Page
{
    Q_OBJECT

public:
    explicit PageRange(QWidget *parent = nullptr);
    ~PageRange();

    bool isSuitableFor(ElementType elementType) override
    {
        return elementType == elementInst || elementType == elementInstSmpl ||
                elementType == elementPrst || elementType == elementPrstInst;
    }

    QString getLabel() override { return tr("Ranges"); }
    QString getIconName() override { return ":/icons/range.svg"; }

protected:
    void updateInterface(QString editingSource) override;
    bool keyPlayedInternal(int key, int velocity) override;

private:
    Ui::PageRange *ui;
    IdList _currentParentIds;
};

#endif // PAGERANGE_H
