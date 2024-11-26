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

#ifndef TOOLMERGESAMPLES_H
#define TOOLMERGESAMPLES_H

#include "abstracttool.h"
#include "waitingtooldialog.h"

class ToolMergeSamples: public AbstractTool
{
    Q_OBJECT

public:
    ToolMergeSamples();
    ~ToolMergeSamples() override;

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/merge_samples.svg";
    }

    QString getCategory() const override
    {
        return tr("Transformation");
    }

    /// Internal identification
    QString getToolName() const override
    {
        return "merger";
    }

signals:
    void elementProcessed(EltID idSmplR, EltID idSmplL, quint32 key, quint32 minKey);

protected:
    QString getLabelInternal() const override
    {
        return tr("Merge samples for each note");
    }

    /// Return true if the tool can be used on the specified ids
    bool isCompatible(IdList ids) override;

    /// Run the tool, emit the signal "finished" at the end
    void runInternal(SoundfontManager * sm, QWidget * parent, IdList ids, AbstractToolParameters * parameters) override;

    /// Get the warning to display after the tool is run
    QString getWarning() override;

private slots:
    void onElementProcessed(EltID idSmplR, EltID idSmplL, quint32 key, quint32 minKey);
    void onCancel();
    void initVoices();
    void restoreVoices();

private:
    static RangesType getPresetRange(SoundfontManager * sm, EltID idPrst);
    static RangesType getInstrumentRange(SoundfontManager * sm, EltID idInst);
    static EltID createInstrumentInPreset(SoundfontManager *sm, QString name, int indexSf2, RangesType range, bool withLoop);

    WaitingToolDialog * _waitingDialog;
    quint32 _steps;
    quint32 _currentStep;
    bool _canceled;

    EltID _idNewInst;
    QString _presetName;
    QString _warning;
};

#endif // TOOLMERGESAMPLES_H
