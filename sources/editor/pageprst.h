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

#ifndef PAGE_PRST_H
#define PAGE_PRST_H

#include <QWidget>
#include "pagetable.h"

namespace Ui
{
    class PagePrst;
}


class SpinBox;

class PagePrst : public PageTable
{
    Q_OBJECT

public:
    explicit PagePrst(QWidget *parent = nullptr);
    ~PagePrst() override;
    void spinUpDown(int steps, SpinBox *spin);

    // Display options
    QList<DisplayOption> getDisplayOptions(IdList selectedIds) override;

protected:
    bool updateInterface(QString editingSource, IdList selectedIds, int displayOption) override;
    void keyPlayedInternal2(int key, int velocity) override;

private slots:
    void setBank();
    void setPreset();

private:
    void setBank(quint16 desiredBank, int collisionResolution);
    bool isBankAvailable(quint16 wBank);
    QList<int> getUsedPresetsForBank(int sf2Index, quint16 wBank);

    void setPreset(quint16 desiredPreset, int collisionResolution);
    bool isPresetAvailable(quint16 wPreset);
    QList<int> getUsedBanksForPreset(int sf2Index, quint16 wPreset);

    Ui::PagePrst *ui;
};

// Classe TableWidget pour presets
class TableWidgetPrst : public TableWidget
{
    Q_OBJECT
public:
    TableWidgetPrst(QWidget *parent = nullptr);
    ~TableWidgetPrst();

    // Association champ - ligne
    AttributeType getChamp(int row);
    int getRow(quint16 champ);

private:
    QList<AttributeType> _fieldList;
};


// SpinBox
class SpinBox : public QSpinBox
{
    Q_OBJECT
public:
    SpinBox(QWidget *parent = nullptr) : QSpinBox(parent) {}
    void init(PagePrst *page) { this->page = page; }

public slots:
    virtual void stepBy(int steps) { this->page->spinUpDown(steps, this); }

private:
    PagePrst *page;
    QList<AttributeType> _rows;
};

#endif // PAGE_PRST_H
