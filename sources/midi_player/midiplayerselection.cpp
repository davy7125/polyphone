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

#include "midiplayerselection.h"
#include <QMutexLocker>
#include <QSet>
#include <chrono>
#include "soundfontmanager.h"
#include "tabmanager.h"
#include "directorypreview.h"
#include "temporarysoundfonts.h"
#include "midiplayerchannel.h"

MidiPlayerSelection::MidiPlayerSelection() : QObject(nullptr),
    _isActive(false),
    _sf2Index(-1),
    _nameSf2Index(-1),
    _isPending(false),
    _loadingSf2Index(-1),
    _loadingId(0),
    _isCanceled(false),
    _loadedSf2Index(-1)
{}

MidiPlayerSelection::~MidiPlayerSelection()
{
    stopLoading();
    TemporarySoundfonts::release(_sf2Index);
    if (_isActive)
        DirectoryPreview::setFileLoadingRequested(false);
}

void MidiPlayerSelection::setActive(bool isActive)
{
    if (isActive == _isActive)
        return;
    _isActive = isActive;

    // The soundfont explorer loads the files selected while they can be played
    DirectoryPreview::setFileLoadingRequested(isActive);
    if (isActive)
        update();
    else
    {
        // The soundfont played is kept, unless it has been loaded by the soundfont explorer (it would use memory while
        // nothing plays it, it is loaded again if needed)
        stopLoading();
        _isPending = false;
        if (_sf2Index >= 0 && SoundfontManager::getInstance()->isTemporary(_sf2Index))
        {
            int previousIndex = _sf2Index;
            _sf2Index = -1;
            _fileName.clear();
            emit soundfontChanged();
            TemporarySoundfonts::release(previousIndex);
        }
        setName(_sf2Index, _fileName);
        emit stateChanged();
    }
}

void MidiPlayerSelection::setPrograms(QList<int> programs)
{
    std::sort(programs.begin(), programs.end());
    if (programs == _programs)
        return;
    _programs = programs;
    update();
}

QString MidiPlayerSelection::getName()
{
    if (!_nameFileName.isEmpty())
        return _nameFileName;
    return _nameSf2Index >= 0 ? MidiPlayerChannel::getSoundfontName(_nameSf2Index) : "";
}

void MidiPlayerSelection::update()
{
    if (!_isActive)
        return;

    int sf2Index = -1;
    QString fileName;
    TabManager * tabManager = TabManager::getInstance();
    TabManager::SoundfontSelection selection = (tabManager == nullptr) ?
                                                   TabManager::SELECTION_NONE : tabManager->getSoundfontSelection(sf2Index, fileName);
    switch (selection)
    {
    case TabManager::SELECTION_PENDING:
        // File being scanned or loaded
        stopLoading();
        _isPending = true;
        setName(-1, fileName);
        break;
    case TabManager::SELECTION_READY:
        // Played once the samples are loaded
        _isPending = false;
        setName(sf2Index, fileName);
        if (sf2Index == _loadingSf2Index && _programs == _loadingPrograms)
            break;
        if (sf2Index == _sf2Index && areSamplesLoaded(sf2Index))
        {
            stopLoading();
            _fileName = fileName;
            break;
        }
        startLoading(sf2Index, fileName);
        break;
    default:
        // Nothing selected: the soundfont played is kept, or the first soundfont open is played if there is none
        _isPending = false;
        if (_sf2Index == -1)
        {
            SoundfontManager * sm = SoundfontManager::getInstance();
            EltID idSf2(elementSf2);
            foreach (int index, sm->getSiblings(idSf2))
            {
                if (!sm->isTemporary(index))
                {
                    sf2Index = index;
                    break;
                }
            }
        }
        if (sf2Index != -1)
        {
            setName(sf2Index, "");
            if (sf2Index != _loadingSf2Index || _programs != _loadingPrograms)
                startLoading(sf2Index, "");
        }
        else
        {
            stopLoading();
            setName(_sf2Index, _fileName);
        }
        break;
    }

    emit stateChanged();
}

void MidiPlayerSelection::onSoundfontClosed(int sf2Index)
{
    if (sf2Index == _loadingSf2Index)
        stopLoading();
    if (sf2Index == _sf2Index)
    {
        TemporarySoundfonts::release(sf2Index);
        _sf2Index = -1;
        _fileName.clear();
        emit soundfontChanged();
    }

    // Possibly another selection
    if (_isActive)
        update();
    else
    {
        setName(_sf2Index, _fileName);
        emit stateChanged();
    }
}

void MidiPlayerSelection::startLoading(int sf2Index, QString fileName)
{
    stopLoading();

    // Samples of the presets used, listed here (the other thread only loads them)
    SoundfontManager * sm = SoundfontManager::getInstance();
    QList<EltID> sampleIds;
    {
        QMutexLocker locker(sm->getMutex());
        QSet<int> sampleIndexes;
        EltID idPrst(elementPrst, sf2Index);
        EltID idPrstInst(elementPrstInst, sf2Index);
        EltID idInstSmpl(elementInstSmpl, sf2Index);
        EltID idSmpl(elementSmpl, sf2Index);
        foreach (int presetIndex, sm->getSiblings(idPrst))
        {
            idPrst.indexElt = presetIndex;
            if (!_programs.contains(sm->get(idPrst, champ_wBank).wValue * 128 + sm->get(idPrst, champ_wPreset).wValue))
                continue;

            idPrstInst.indexElt = presetIndex;
            foreach (int i, sm->getSiblings(idPrstInst))
            {
                idPrstInst.indexElt2 = i;
                idInstSmpl.indexElt = sm->get(idPrstInst, champ_instrument).wValue;
                foreach (int j, sm->getSiblings(idInstSmpl))
                {
                    idInstSmpl.indexElt2 = j;
                    idSmpl.indexElt = sm->get(idInstSmpl, champ_sampleID).wValue;
                    if (!sampleIndexes.contains(idSmpl.indexElt))
                    {
                        sampleIndexes << idSmpl.indexElt;
                        sampleIds << idSmpl;
                    }
                }
            }
        }
    }

    // Loaded one by one with the soundfonts locked, so that the synth (which may load samples at the same time) is not
    // blocked for long. The main thread may wait for the end of this thread while it locks the soundfonts (for instance
    // when a soundfont is closed): the lock is only tried until the loading is canceled.
    _loadingSf2Index = sf2Index;
    _loadingFileName = fileName;
    _loadingPrograms = _programs;
    int loadingId = ++_loadingId;
    _isCanceled = false;
    _thread = std::thread([this, sampleIds, loadingId]() {
        SoundfontManager * sm = SoundfontManager::getInstance();
        QRecursiveMutex * mutex = sm->getMutex();
        for (EltID id : sampleIds)
        {
            while (!mutex->tryLock(10))
                if (_isCanceled)
                    return;
            if (_isCanceled)
            {
                mutex->unlock();
                return;
            }
            Sound * sound = sm->isValid(id) ? sm->getSound(id) : nullptr;
            if (sound != nullptr)
                sound->loadSample();
            mutex->unlock();

            // The threads waiting for the lock get it (otherwise this thread would take it again at once)
            std::this_thread::sleep_for(std::chrono::microseconds(50));
        }
        QMetaObject::invokeMethod(this, [this, loadingId]() { onSamplesLoaded(loadingId); }, Qt::QueuedConnection);
    });
}

void MidiPlayerSelection::stopLoading()
{
    if (_thread.joinable())
    {
        _isCanceled = true;
        _thread.join();
    }
    _loadingSf2Index = -1;
    _loadingFileName.clear();
    _loadingPrograms.clear();
    _loadingId++; // The end of a loading possibly notified is ignored
}

void MidiPlayerSelection::onSamplesLoaded(int loadingId)
{
    if (loadingId != _loadingId)
        return;
    if (_thread.joinable())
        _thread.join();

    int sf2Index = _loadingSf2Index;
    QString fileName = _loadingFileName;
    _loadedSf2Index = sf2Index;
    _loadedPrograms = _loadingPrograms;
    _loadingSf2Index = -1;
    _loadingFileName.clear();
    _loadingPrograms.clear();

    // The soundfont is played if it is still open
    EltID idSf2(elementSf2, sf2Index);
    if (SoundfontManager::getInstance()->isValid(idSf2))
        setSoundfont(sf2Index, fileName);
    emit stateChanged();
}

bool MidiPlayerSelection::areSamplesLoaded(int sf2Index)
{
    if (sf2Index != _loadedSf2Index)
        return false;
    foreach (int program, _programs)
        if (!_loadedPrograms.contains(program))
            return false;
    return true;
}

void MidiPlayerSelection::setSoundfont(int sf2Index, QString fileName)
{
    _fileName = fileName;
    if (sf2Index == _sf2Index)
        return;

    // A soundfont loaded by the soundfont explorer is held while it is played
    TemporarySoundfonts::hold(sf2Index);
    int previousIndex = _sf2Index;
    _sf2Index = sf2Index;
    emit soundfontChanged();
    TemporarySoundfonts::release(previousIndex);
}

void MidiPlayerSelection::setName(int sf2Index, QString fileName)
{
    _nameSf2Index = sf2Index;
    _nameFileName = fileName;
}
