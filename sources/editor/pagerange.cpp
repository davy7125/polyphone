/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2020 Davy Triponney                                **
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

#include "pagerange.h"
#include "ui_pagerange.h"

PageRange::PageRange(QWidget *parent) :
    Page(parent, "rangeEditor"),
    ui(new Ui::PageRange)
{
    ui->setupUi(this);
    connect(ui->rangeEditor, SIGNAL(divisionsSelected(IdList)), this, SIGNAL(selectedIdsChanged(IdList)));
}

PageRange::~PageRange()
{
    delete ui;
}

void PageRange::updateInterface(QString editingSource)
{
    // Check if the new parents are the same
    IdList parentIds = _currentIds.getSelectedIds(elementInst);
    if (parentIds.empty())
        parentIds = _currentIds.getSelectedIds(elementPrst);
    bool sameElement = true;
    if (parentIds.count() == _currentParentIds.count())
    {
        for (int i = 0; i < parentIds.count(); i++)
        {
            if (parentIds[i] != _currentParentIds[i])
            {
                sameElement = false;
                break;
            }
        }
    }
    else
        sameElement = false;
    bool justSelection = (sameElement && editingSource == "command:selection");

    _currentParentIds = parentIds;

    ui->rangeEditor->display(_currentIds, justSelection);
}

void PageRange::keyPlayedInternal(int key, int velocity)
{
    ui->rangeEditor->playKey(key, velocity);
}

