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

#include "directorypreview.h"
#include "contextmanager.h"
#include "soundfontmanager.h"
#include "inputfactory.h"
#include "abstractinputparser.h"
#include "dialogkeyboard.h"
#include "pianokeybdcustom.h"
#include "editor.h"
#include "synth.h"
#include "temporarysoundfonts.h"
#include <QTimer>
#include <QWidget>

QList<DirectoryPreview *> DirectoryPreview::s_instances;
bool DirectoryPreview::s_isFileLoadingRequested = false;

// Files browsed with the arrow keys are not loaded for the MIDI player
const int DirectoryPreview::LOADING_DELAY_MS = 200;

DirectoryPreview::DirectoryPreview(DialogKeyboard * dialogKeyboard, QWidget * widget) : QObject(nullptr),
    _dialogKeyboard(dialogKeyboard),
    _widget(widget),
    _isScanning(false),
    _isLoadable(false),
    _element(elementUnknown),
    _status(NOT_LOADED),
    _parser(nullptr),
    _sf2Index(-1),
    _isHeld(false),
    _loadTimer(new QTimer(this)),
    _playedElement(elementUnknown)
{
    _loadTimer->setSingleShot(true);
    _loadTimer->setInterval(LOADING_DELAY_MS);
    connect(_loadTimer, SIGNAL(timeout()), this, SLOT(onLoadTimeout()));
    connect(SoundfontManager::getInstance(), SIGNAL(soundfontClosed(int)), this, SLOT(onSoundfontClosed(int)));
    if (_dialogKeyboard != nullptr)
    {
        connect(this, SIGNAL(processKeyMainThread(int,int)), this, SLOT(onProcessKeyMainThread(int,int)));
        ContextManager::midi()->addListener(this, 0);
    }
    s_instances << this;
}

DirectoryPreview::~DirectoryPreview()
{
    s_instances.removeAll(this);
    if (_dialogKeyboard != nullptr)
        ContextManager::midi()->removeListener(this);
    this->blockSignals(true);
    releaseSoundfont();
}

void DirectoryPreview::setFile(QString filePath, bool isScanning, bool isLoadable)
{
    if (filePath == _filePath && isScanning == _isScanning && isLoadable == _isLoadable)
        return;

    // The soundfont of another file is released
    if (filePath != _filePath)
    {
        releaseSoundfont();
        _filePath = filePath;
    }
    _isScanning = isScanning;
    _isLoadable = isLoadable;
    scheduleLoading();
    emit fileStateChanged();
}

DirectoryPreview::FileState DirectoryPreview::getFileState(int &sf2Index)
{
    sf2Index = -1;
    if (_filePath.isEmpty())
        return FILE_NONE;

    switch (_status)
    {
    case LOADED:
        sf2Index = _sf2Index;
        return FILE_READY;
    case LOADING:
        return FILE_PENDING;
    case FAILED:
        return FILE_NONE;
    default:
        break;
    }

    // Loaded once the file is scanned and selected for a while
    return (s_isFileLoadingRequested && (_isScanning || _isLoadable)) ? FILE_PENDING : FILE_NONE;
}

void DirectoryPreview::setFileLoadingRequested(bool isRequested)
{
    if (isRequested == s_isFileLoadingRequested)
        return;

    s_isFileLoadingRequested = isRequested;
    foreach (DirectoryPreview * preview, s_instances)
    {
        preview->scheduleLoading();
        emit preview->fileStateChanged();
    }
}

void DirectoryPreview::scheduleLoading()
{
    // Only the file of the explorer displayed is loaded for the MIDI player
    if (s_isFileLoadingRequested && _status == NOT_LOADED && _isLoadable && !_filePath.isEmpty() && _widget->isVisible())
        _loadTimer->start();
    else
        _loadTimer->stop();
}

void DirectoryPreview::onLoadTimeout()
{
    if (s_isFileLoadingRequested && _status == NOT_LOADED && _isLoadable && !_filePath.isEmpty())
        load();
}

void DirectoryPreview::setElement(EltID id)
{
    _element = id;
    _element.indexSf2 = -1;

    // The soundfont is loaded at the first selection of an element, if it can be played
    if (_dialogKeyboard != nullptr && _element.typeElement != elementUnknown && _status == NOT_LOADED && !_filePath.isEmpty())
        load();
    else
        updatePlayedElement();
}

void DirectoryPreview::load()
{
    _loadTimer->stop();
    SoundfontManager * sm = SoundfontManager::getInstance();

    // A file already open in a tab is played with its current changes
    EltID idSf2(elementSf2);
    foreach (int index, sm->getSiblings(idSf2))
    {
        idSf2.indexSf2 = index;
        if (!sm->isTemporary(index) && sm->getQstr(idSf2, champ_filenameInitial) == _filePath)
        {
            _sf2Index = index;
            _isHeld = false;
            setStatus(LOADED);
            return;
        }
    }

    // Otherwise it is loaded in the background
    _parser = InputFactory::getInput(_filePath);
    _parser->setTemporary(true);
    connect(_parser, SIGNAL(finished()), this, SLOT(onInputProcessed()));
    setStatus(LOADING);
    _parser->process(true);
}

void DirectoryPreview::onInputProcessed()
{
    AbstractInputParser * parser = dynamic_cast<AbstractInputParser *>(QObject::sender());
    SoundfontManager * sm = SoundfontManager::getInstance();
    _parser = nullptr;
    if (parser->isSuccess())
    {
        // Possibly opened in a tab meanwhile, otherwise held until it is released
        _sf2Index = parser->getSf2Index();
        _isHeld = sm->isTemporary(_sf2Index);
        if (_isHeld)
            TemporarySoundfonts::hold(_sf2Index);
        delete parser;
        setStatus(LOADED);
    }
    else
    {
        // Delete the soundfont possibly created
        int tmpIndex = parser->getSf2Index();
        if (tmpIndex != -1 && sm->isTemporary(tmpIndex))
            sm->remove(EltID(elementSf2, tmpIndex));
        delete parser;
        setStatus(FAILED);
    }
}

void DirectoryPreview::release()
{
    releaseSoundfont();
    scheduleLoading();
}

void DirectoryPreview::releaseSoundfont()
{
    _loadTimer->stop();
    SoundfontManager * sm = SoundfontManager::getInstance();

    // A loading in progress is abandoned, the soundfont being deleted when it is over
    if (_parser != nullptr)
    {
        disconnect(_parser, SIGNAL(finished()), this, SLOT(onInputProcessed()));
        AbstractInputParser * parser = _parser;
        connect(parser, &AbstractInputParser::finished, parser, [parser, sm]() {
            int tmpIndex = parser->getSf2Index();
            if (tmpIndex != -1 && sm->isTemporary(tmpIndex))
                sm->remove(EltID(elementSf2, tmpIndex));
            parser->deleteLater();
        });
        _parser = nullptr;
    }

    if (_sf2Index != -1)
    {
        // The notes played with the keyboard are released, the MIDI player possibly going on playing the soundfont. A
        // temporary soundfont is deleted when nothing uses it anymore, unless a tab has been opened with it.
        ContextManager::audio()->getSynth()->play(EltID(elementSf2, _sf2Index), -1, -2, 0);
        if (_isHeld)
            TemporarySoundfonts::release(_sf2Index);
        _sf2Index = -1;
        _isHeld = false;
    }

    if (_status != NOT_LOADED)
        setStatus(NOT_LOADED);
}

void DirectoryPreview::setStatus(Status status)
{
    _status = status;
    updatePlayedElement();
    emit fileStateChanged();
}

void DirectoryPreview::updatePlayedElement()
{
    // Element played if its soundfont is loaded
    EltID id(elementUnknown);
    if (_status == LOADED && _element.typeElement != elementUnknown && _dialogKeyboard != nullptr)
    {
        id = _element;
        id.indexSf2 = _sf2Index;
        if (!SoundfontManager::getInstance()->isValid(id))
            id = EltID(elementUnknown);
    }

    // The notes of the previous element are released
    EltID previousId = getPlayedElement();
    if (previousId.typeElement != elementUnknown && !(previousId == id))
        releaseNotes();

    _mutex.lock();
    _playedElement = id;
    _mutex.unlock();

    if (_widget->isVisible())
        updateKeyboardAndSynth();
}

void DirectoryPreview::updateKeyboardAndSynth()
{
    if (_dialogKeyboard == nullptr)
        return;

    // Ranges and root keys displayed as in the editor
    EltID id = getPlayedElement();
    if (id.typeElement == elementUnknown)
        _dialogKeyboard->getKeyboard()->clearCustomization();
    else
        Editor::customizeKeyboard(_dialogKeyboard->getKeyboard(), IdList(id));

    // A sample is looped if it has a loop (as the checkbox of the editor), in stereo according to the option of the
    // editor, without the equalizer and the calibration sine
    if (id.typeElement == elementSmpl)
    {
        SoundfontManager * sm = SoundfontManager::getInstance();
        qint64 loopLength = static_cast<qint64>(sm->get(id, champ_dwEndLoop).dwValue) - sm->get(id, champ_dwStartLoop).dwValue;
        Synth * synth = ContextManager::audio()->getSynth();
        synth->setLoopEnabled(qAbs(loopLength) > 10);
        synth->setStereo(ContextManager::configuration()->getValue(ConfManager::SECTION_AUDIO, "stereo_playback", false).toBool());
        synth->activateSmplEq(false);
        synth->setSinus(false, sm->get(id, champ_byOriginalPitch).bValue);
    }
}

void DirectoryPreview::releaseNotes()
{
    EltID id = getPlayedElement();
    if (id.typeElement != elementUnknown)
        ContextManager::audio()->getSynth()->play(EltID(elementSf2, id.indexSf2), -1, -2, 0);
}

EltID DirectoryPreview::getPlayedElement()
{
    QMutexLocker locker(&_mutex);
    return _playedElement;
}

void DirectoryPreview::onShown()
{
    updateKeyboardAndSynth();
    scheduleLoading();
}

void DirectoryPreview::onHidden()
{
    _loadTimer->stop();
    releaseNotes();
    if (_dialogKeyboard != nullptr)
        _dialogKeyboard->getKeyboard()->clearCustomization();
}

bool DirectoryPreview::processKey(int channel, int key, int vel)
{
    if (channel != -1)
        return false;

    // The explorer may not be the tab that is displayed
    if (!_widget->isVisible())
        return false;

    EltID id = getPlayedElement();
    if (id.typeElement == elementUnknown)
        return false;

    // Same as the editor
    if (id.typeElement == elementSmpl || vel > 0)
        ContextManager::audio()->getSynth()->play(id, -1, key, vel);
    else
        ContextManager::audio()->getSynth()->play(EltID(elementSf2, id.indexSf2), -1, key, 0);

    // GUI thread
    emit processKeyMainThread(key, vel);

    return false;
}

void DirectoryPreview::onProcessKeyMainThread(int key, int vel)
{
    // Ranges of the instrument divisions played
    EltID id = getPlayedElement();
    if (id.typeElement != elementInst || !_widget->isVisible())
        return;

    if (vel > 0)
        Editor::showPlayedRanges(_dialogKeyboard->getKeyboard(), id, key, vel);
    else
        _dialogKeyboard->getKeyboard()->removeCurrentRange(key);
}

void DirectoryPreview::onSoundfontClosed(int indexSf2)
{
    // The soundfont was open in a tab that has been closed: the file is loaded again if it is still needed
    if (indexSf2 != _sf2Index)
        return;

    if (_isHeld)
        TemporarySoundfonts::release(indexSf2);
    _sf2Index = -1;
    _isHeld = false;
    if (_dialogKeyboard != nullptr && _element.typeElement != elementUnknown && !_filePath.isEmpty())
        load();
    else
    {
        setStatus(NOT_LOADED);
        scheduleLoading();
    }
}
