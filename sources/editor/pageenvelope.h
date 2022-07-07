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

#ifndef PAGEENVELOPE_H
#define PAGEENVELOPE_H

#include "page.h"
#include "soundfontmanager.h"
#include "graphicsviewenvelop.h"
#include "envelop.h"

namespace Ui {
class PageEnvelope;
}

class PageEnvelope : public Page
{
    Q_OBJECT

public:
    explicit PageEnvelope(QWidget *parent = nullptr);
    ~PageEnvelope();

    bool isSuitableFor(ElementType elementType) override { return elementType == elementInst || elementType == elementInstSmpl; }
    QString getLabel() override { return tr("Envelopes"); }
    QString getIconName() override { return ":/icons/adsr.svg"; }

    void init(SoundfontManager * sf2);

protected:
    void updateInterface(QString editingSource) override;

private slots:
    void on_pushVolume_clicked();
    void on_pushModulation_clicked();
    void populate();
    void on_doubleSpinDelay_editingFinished();
    void on_doubleSpinAttack_editingFinished();
    void on_doubleSpinHold_editingFinished();
    void on_doubleSpinDecay_editingFinished();
    void on_doubleSpinSustain_editingFinished();
    void on_doubleSpinRelease_editingFinished();
    void on_spinKeyHold_editingFinished();
    void on_spinKeyDecay_editingFinished();
    void on_pushAttack_clicked();
    void on_pushDelay_clicked();
    void on_pushHold_clicked();
    void on_pushKeyHold_clicked();
    void on_pushSustain_clicked();
    void on_pushRelease_clicked();
    void on_pushDecay_clicked();
    void on_pushKeyDecay_clicked();

private:
    Ui::PageEnvelope *ui;
    bool _isVolume;

    void enableEditor(bool isEnabled);
    void stopSignals(bool isStopped);
    void addEnvelop(EltID id, bool isVolume, bool isMain);
    void addSample(EltID idInstSmpl);
    double computeValue(EltID id, AttributeType champ, bool &isOverridden);
    void processEdit(AttributeType champ, double value);
    void processClear(AttributeType champ);
};

#endif // PAGEENVELOPE_H
