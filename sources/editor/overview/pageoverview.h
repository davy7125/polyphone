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

#ifndef PAGEOVERVIEW_H
#define PAGEOVERVIEW_H

#include <QWidget>
#include "page.h"

namespace Ui {
class PageOverview;
}

class PageOverview : public Page
{
    Q_OBJECT

public:
    PageOverview(TypePage typePage, ElementType typeElement, QWidget *parent = nullptr);
    ~PageOverview() override;

protected:
    // Update the interface
    bool updateInterface(QString editingSource, IdList selectedIds, int displayOption) override;

    // Refresh things after a page is shown
    void onShow() override;

    virtual QString getTitle() = 0;
    virtual QStringList getHorizontalHeader() = 0;
    virtual void prepare(EltID id) = 0;
    virtual void getInformation(EltID id, QStringList &info, QStringList &order) = 0;

    QString getRange(bool orderMode, EltID id, AttributeType champ);

private slots:
    void on_table_cellDoubleClicked(int row, int column);

private:
    Ui::PageOverview *ui;
    ElementType _typeElement;
    QString fillInformation(EltID id, int row);
};

#endif // PAGEOVERVIEW_H
