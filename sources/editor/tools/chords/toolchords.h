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

#ifndef TOOLCHORDS_H
#define TOOLCHORDS_H

#include "abstracttool.h"
#include "toolchords_parameters.h"
#include "waitingtooldialog.h"

class ToolChords: public AbstractTool
{
    Q_OBJECT

public:
    ToolChords();
    ~ToolChords() override;

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/chord.svg";
    }

    QString getCategory() const override
    {
        return trUtf8("Transformation");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return "inst:chords";
    }

signals:
    void elementProcessed(EltID idSmpl, int key, int minKey, double minAtt);

protected:
    QString getLabelInternal() const override
    {
        return trUtf8("Create chords");
    }

    /// Return true if the tool can be used on the specified ids
    bool isCompatible(IdList ids) override;

    /// Run the tool, emit the signal "finished" at the end
    void runInternal(SoundfontManager * sm, QWidget * parent, IdList ids, AbstractToolParameters * parameters) override;

    /// Get the warning to display after the tool is run
    QString getWarning() override;

private slots:
    void onElementProcessed(EltID idSmpl, int key, int minKey, double minAtt);
    void onCancel();

private:
    RangesType getInstrumentRange(EltID idInst);

    WaitingToolDialog * _waitingDialog;
    int _steps;
    int _currentStep;
    bool _canceled;

    EltID _idNewInst;
    QString _instrumentName;
    bool _stereoSamples;
    QString _warning;
    QMap<int, EltID> _sampleKey;
};

#endif // TOOLCHORDS_H
