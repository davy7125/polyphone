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

#ifndef FOOTERPRST_H
#define FOOTERPRST_H

#include "abstractfooter.h"
#include <QSpinBox>

namespace Ui {
class FooterPrst;
}

class SpinBox;

class FooterPrst : public AbstractFooter
{
    Q_OBJECT

public:
    explicit FooterPrst(QWidget *parent = nullptr);
    ~FooterPrst();

    void updateInterface() override;
    void spinUpDown(int steps, SpinBox *spin);

private slots:
    void setBank();
    void setPreset();

private:
    QList<int> getUsedPresetsForBank(int sf2Index, quint16 wBank);
    void setBank(quint16 desiredBank, int collisionResolution);
    bool isBankAvailable(quint16 wBank);

    QList<int> getUsedBanksForPreset(int sf2Index, quint16 wPreset);
    void setPreset(quint16 desiredPreset, int collisionResolution);
    bool isPresetAvailable(quint16 wPreset);

    IdList _currentParentIds;
    bool _initializing;
    Ui::FooterPrst *ui;
};

// SpinBox
class SpinBox : public QSpinBox
{
    Q_OBJECT
public:
    SpinBox(QWidget *parent = nullptr) : QSpinBox(parent) {}
    void init(FooterPrst * footer) { _footer = footer; }

public slots:
    virtual void stepBy(int steps) { _footer->spinUpDown(steps, this); }

private:
    FooterPrst * _footer;
    QList<AttributeType> _rows;
};

#endif // FOOTERPRST_H
