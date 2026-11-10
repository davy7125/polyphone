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

#include "toolmissingsamples.h"
#include "toolmissingsamples_gui.h"
#include "toolmissingsamples_parameters.h"
#include "soundfontmanager.h"
#include "waitingtooldialog.h"
#include "contextmanager.h"
#include <QThreadPool>
#include <QRunnable>

// Computation of one job
class RunnableMissingSample: public QRunnable
{
public:
    RunnableMissingSample(ToolMissingSamples * tool, int runId, QSharedPointer<MissingSampleRun> run, int index) : QRunnable(),
        _tool(tool),
        _runId(runId),
        _run(run),
        _index(index)
    {}

    ~RunnableMissingSample() override
    {
        // Also called if the task is removed from the pool before running (cancel)
        emit _tool->jobProcessed(_runId);
    }

    void run() override
    {
        if (!_run->canceled.loadRelaxed())
            MissingSampleCreator::compute(_run->jobs[static_cast<size_t>(_index)], _run->cache);
    }

private:
    ToolMissingSamples * _tool;
    int _runId;
    QSharedPointer<MissingSampleRun> _run;
    int _index;
};

ToolMissingSamples::ToolMissingSamples() : AbstractTool(new ToolMissingSamples_parameters(), new ToolMissingSamples_gui()),
    _runId(0),
    _processedCount(0),
    _waitingDialog(nullptr)
{
    connect(this, SIGNAL(jobProcessed(int)), this, SLOT(onJobProcessed(int)), Qt::QueuedConnection);
}

ToolMissingSamples::~ToolMissingSamples()
{
    delete _waitingDialog;
}

bool ToolMissingSamples::isCompatible(IdList ids)
{
    // Instruments having at least two root keys
    if (ids.isEmpty() || !ids.areAllWithType(elementInst))
        return false;
    IdList instruments = ids.getSelectedIds(elementInst);
    if (instruments.isEmpty())
        return false;
    SoundfontManager * sm = SoundfontManager::getInstance();
    foreach (EltID idInst, instruments)
        if (MissingSampleCreator::getPositions(sm, idInst).count() < 2)
            return false;
    return true;
}

QString ToolMissingSamples::getBlockingError(IdList ids)
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    QStringList names;
    foreach (EltID idInst, ids.getSelectedIds(elementInst))
        if (!MissingSampleCreator::hasSingleVelocityRange(sm, idInst))
            names << sm->getQstr(idInst, champ_name);
    if (names.isEmpty())
        return "";
    return tr("This tool cannot be used on instruments having several velocity ranges:") +
            "<ul><li>" + names.join("</li><li>") + "</li></ul>";
}

void ToolMissingSamples::prepare(SoundfontManager * sm, IdList ids, ToolMissingSamples_parameters * parameters)
{
    _instruments.clear();
    _creations.clear();
    _nothingToDo.clear();
    _ignoredKeys.clear();
    _notLooped.clear();
    _levelOnly.clear();
    _run = QSharedPointer<MissingSampleRun>(new MissingSampleRun());

    foreach (EltID idInst, ids.getSelectedIds(elementInst))
    {
        idInst.typeElement = elementInst;
        if (!sm->isValid(idInst))
            continue;
        QMap<int, MissingSamplePosition> positions = MissingSampleCreator::getPositions(sm, idInst);
        if (positions.count() < 2)
            continue;

        // Keys to create
        QList<int> keys;
        if (parameters->getMode() == ToolMissingSamples_parameters::MODE_KEYS)
        {
            QStringList ignored;
            foreach (int key, parameters->getKeys())
            {
                if (positions.contains(key))
                    ignored << ContextManager::keyName()->getKeyName(static_cast<unsigned int>(key));
                else
                    keys << key;
            }
            if (!ignored.isEmpty())
                _ignoredKeys << sm->getQstr(idInst, champ_name) + ": " + ignored.join(", ");
        }
        else
            keys = MissingSampleCreator::getKeysToFill(positions.keys(), parameters->getMaxSpacing());
        if (keys.isEmpty())
            continue;

        Instrument instrument;
        instrument.idInst = idInst;
        foreach (MissingSamplePosition position, positions.values())
            foreach (MissingSampleSource source, position.sources)
                instrument.divisions[position.key] << source.idDivision;
        _instruments << instrument;

        foreach (int key, keys)
            _run->jobs.push_back(MissingSampleCreator::createJob(idInst, key, positions));
    }

    if (_run->jobs.empty() && _ignoredKeys.isEmpty())
    {
        if (parameters->getMode() == ToolMissingSamples_parameters::MODE_KEYS)
            _nothingToDo = tr("No keys to create.");
        else
            _nothingToDo = tr("No sample to create: the root keys are at most %n semitone(s) apart.", "",
                              parameters->getMaxSpacing());
    }
}

void ToolMissingSamples::runInternal(SoundfontManager * sm, QWidget * parent, IdList ids, AbstractToolParameters * parameters)
{
    prepare(sm, ids, dynamic_cast<ToolMissingSamples_parameters *>(parameters));
    if (_run->jobs.empty())
    {
        emit finished(false);
        return;
    }

    // Progress dialog
    _runId++;
    _processedCount = 0;
    delete _waitingDialog;
    _waitingDialog = new WaitingToolDialog(this->getLabel(), static_cast<quint32>(_run->jobs.size()), parent);
    _waitingDialog->show();
    connect(_waitingDialog, SIGNAL(canceled()), this, SLOT(onCancel()));

    // Computation in several threads, the soundfont being modified at the end
    for (size_t i = 0; i < _run->jobs.size(); i++)
        QThreadPool::globalInstance()->start(new RunnableMissingSample(this, _runId, _run, static_cast<int>(i)));
}

bool ToolMissingSamples::runSynchronously(SoundfontManager * sm, IdList ids, AbstractToolParameters * parameters)
{
    prepare(sm, ids, dynamic_cast<ToolMissingSamples_parameters *>(parameters));
    if (!_run->jobs.empty())
    {
        QThreadPool pool;
        for (size_t i = 0; i < _run->jobs.size(); i++)
            pool.start(new RunnableMissingSample(this, -1, _run, static_cast<int>(i)));
        pool.waitForDone();
        finish(sm);
    }
    return true;
}

void ToolMissingSamples::onJobProcessed(int runId)
{
    if (runId != _runId || _waitingDialog == nullptr)
        return;

    if (_run->canceled.loadRelaxed())
    {
        // Nothing has been written in the soundfont
        delete _waitingDialog;
        _waitingDialog = nullptr;
        _runId++; // The next notifications of this run are ignored
        _nothingToDo.clear();
        _ignoredKeys.clear();
        emit finished(false);
        return;
    }

    _waitingDialog->setValue(++_processedCount);
    if (_processedCount >= static_cast<int>(_run->jobs.size()))
    {
        delete _waitingDialog;
        _waitingDialog = nullptr;
        finish(SoundfontManager::getInstance());
        emit finished(true);
    }
}

void ToolMissingSamples::onCancel()
{
    if (!_run.isNull())
        _run->canceled.storeRelaxed(1);
    QThreadPool::globalInstance()->clear();
}

void ToolMissingSamples::finish(SoundfontManager * sm)
{
    // Samples and divisions created in the order of the instruments and keys
    foreach (Instrument instrument, _instruments)
    {
        QMap<int, QList<EltID> > newDivisions;
        for (size_t i = 0; i < _run->jobs.size(); i++)
        {
            const MissingSampleJob &job = _run->jobs[i];
            if (job.idInst != instrument.idInst)
                continue;
            QList<EltID> samples;
            QList<EltID> divisions = MissingSampleCreator::apply(sm, job, samples, _notLooped);
            if (divisions.isEmpty())
                continue;
            newDivisions[job.key] = divisions;

            Creation creation;
            creation.idInst = job.idInst;
            creation.key = job.key;
            creation.closestKey = job.closest.key;
            creation.secondKey = job.second.key;
            creation.samples = samples;
            _creations << creation;
            if (!job.harmonicsFound)
                _levelOnly << sm->getQstr(job.idInst, champ_name) + ": " +
                              ContextManager::keyName()->getKeyName(static_cast<unsigned int>(job.key));
        }
        MissingSampleCreator::adjustKeyRanges(sm, instrument.divisions, newDivisions);
    }
}

QString ToolMissingSamples::getWarning()
{
    QStringList parts;
    if (!_nothingToDo.isEmpty())
        parts << _nothingToDo;
    if (!_ignoredKeys.isEmpty())
        parts << tr("Keys already having a sample, ignored:") + "<ul><li>" + _ignoredKeys.join("</li><li>") + "</li></ul>";
    if (!_levelOnly.isEmpty())
        parts << tr("No harmonics found around the following keys, only the level was interpolated:") +
                 "<ul><li>" + _levelOnly.join("</li><li>") + "</li></ul>";
    if (!_notLooped.isEmpty())
        parts << tr("The following samples couldn't be looped:") + "<ul><li>" + _notLooped.join("</li><li>") + "</li></ul>";
    return parts.join("<br/>");
}
