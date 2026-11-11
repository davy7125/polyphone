/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2026 Davy Triponney                                **
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

#ifndef TOOLMISSINGSAMPLES_H
#define TOOLMISSINGSAMPLES_H

#include "abstracttool.h"
#include "missingsamplecreator.h"
class WaitingToolDialog;
class ToolMissingSamples_parameters;

class ToolMissingSamples: public AbstractTool
{
    Q_OBJECT

public:
    ToolMissingSamples();
    ~ToolMissingSamples() override;

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/missing_samples.svg";
    }

    QString getCategory() const override
    {
        return tr("Transformation");
    }

    /// Internal identification
    QString getToolName() const override
    {
        return "missingSamples";
    }

    /// Sample created at a key of an instrument by the last run
    struct Creation
    {
        EltID idInst;
        int key;
        int closestKey; // Root key transposed
        int secondKey; // Root key used for the interpolation or the extrapolation of the harmonics
        QList<EltID> samples;
    };
    QList<Creation> getCreations() const { return _creations; }

    /// Divisions processed: all (-1), or only the subset playing at a velocity (instruments with several velocity
    /// ranges being then accepted), for the next run
    void setVelocity(int velocity) { _velocity = velocity; }

signals:
    void jobProcessed(int runId);

protected:
    QString getLabelInternal() const override
    {
        return tr("Create missing samples");
    }

    /// Return true if the tool can be used on the specified ids
    bool isCompatible(IdList ids) override;

    /// Error preventing the tool from running
    QString getBlockingError(IdList ids) override;

    /// Run the tool, emit the signal "finished" at the end
    void runInternal(SoundfontManager * sm, QWidget * parent, IdList ids, AbstractToolParameters * parameters) override;
    bool runSynchronously(SoundfontManager * sm, IdList ids, AbstractToolParameters * parameters) override;

    /// Get the warning to display after the tool is run
    QString getWarning() override;

private slots:
    void onJobProcessed(int runId);
    void onCancel();

private:
    // Jobs to compute, instruments being listed in _instruments
    void prepare(SoundfontManager * sm, IdList ids, ToolMissingSamples_parameters * parameters);

    // Write the results in the soundfont
    void finish(SoundfontManager * sm);

    struct Instrument
    {
        EltID idInst;
        QMap<int, QList<EltID> > divisions; // Existing divisions by root key
    };
    QList<Instrument> _instruments;
    QSharedPointer<MissingSampleRun> _run;
    int _runId;
    int _processedCount;
    int _velocity;
    WaitingToolDialog * _waitingDialog;

    QList<Creation> _creations;
    QString _nothingToDo;
    QStringList _ignoredKeys, _notLooped, _levelOnly;
};

#endif // TOOLMISSINGSAMPLES_H
