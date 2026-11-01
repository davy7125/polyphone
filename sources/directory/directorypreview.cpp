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
#include <QTimer>
#include <QWidget>

// The voices of a sample read the data of the soundfont until their release is over
const int DirectoryPreview::DELETION_DELAY_MS = 5000;

DirectoryPreview::DirectoryPreview(DialogKeyboard * dialogKeyboard, QWidget * widget) : QObject(nullptr),
    _dialogKeyboard(dialogKeyboard),
    _widget(widget),
    _element(elementUnknown),
    _status(NOT_LOADED),
    _parser(nullptr),
    _sf2Index(-1),
    _isTemporary(false),
    _playedElement(elementUnknown)
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    connect(sm, SIGNAL(soundfontLoaded(int)), this, SLOT(onSoundfontLoaded(int)));
    connect(sm, SIGNAL(soundfontClosed(int)), this, SLOT(onSoundfontClosed(int)));
    connect(this, SIGNAL(processKeyMainThread(int,int)), this, SLOT(onProcessKeyMainThread(int,int)));
    ContextManager::midi()->addListener(this, 0);
}

DirectoryPreview::~DirectoryPreview()
{
    ContextManager::midi()->removeListener(this);
    release();
}

void DirectoryPreview::setFile(QString filePath)
{
    if (filePath == _filePath)
        return;

    release();
    _filePath = filePath;
}

void DirectoryPreview::setElement(EltID id)
{
    _element = id;
    _element.indexSf2 = -1;

    // The soundfont is loaded at the first selection of an element
    if (_element.typeElement != elementUnknown && _status == NOT_LOADED && !_filePath.isEmpty())
        load();
    else
        updatePlayedElement();
}

void DirectoryPreview::load()
{
    SoundfontManager * sm = SoundfontManager::getInstance();

    // A file already open in a tab is played with its current changes
    EltID idSf2(elementSf2);
    foreach (int index, sm->getSiblings(idSf2))
    {
        idSf2.indexSf2 = index;
        if (!sm->isTemporary(index) && sm->getQstr(idSf2, champ_filenameInitial) == _filePath)
        {
            _sf2Index = index;
            _isTemporary = false;
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
        // Possibly opened in a tab meanwhile
        _sf2Index = parser->getSf2Index();
        _isTemporary = sm->isTemporary(_sf2Index);
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
        Synth * synth = ContextManager::audio()->getSynth();
        if (_isTemporary)
        {
            // Mute the soundfont and delete it later, unless a tab has been opened with it meanwhile
            synth->play(EltID(elementSf2, _sf2Index), -2, -2, 0);
            int sf2Index = _sf2Index;
            QTimer::singleShot(DELETION_DELAY_MS, sm, [sf2Index, sm]() {
                if (sm->isTemporary(sf2Index))
                    sm->remove(EltID(elementSf2, sf2Index));
            });
        }
        else
        {
            // Only the notes played here are stopped
            synth->play(EltID(elementSf2, _sf2Index), -1, -2, 0);
        }
        _sf2Index = -1;
        _isTemporary = false;
    }

    if (_status != NOT_LOADED)
        setStatus(NOT_LOADED);
}

void DirectoryPreview::setStatus(Status status)
{
    _status = status;
    updatePlayedElement();
}

void DirectoryPreview::updatePlayedElement()
{
    // Element played if its soundfont is loaded
    EltID id(elementUnknown);
    if (_status == LOADED && _element.typeElement != elementUnknown)
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
}

void DirectoryPreview::onHidden()
{
    releaseNotes();
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

void DirectoryPreview::onSoundfontLoaded(int indexSf2)
{
    // A tab has been opened with the temporary soundfont, which is not to be deleted anymore
    if (indexSf2 == _sf2Index && !SoundfontManager::getInstance()->isTemporary(indexSf2))
        _isTemporary = false;
}

void DirectoryPreview::onSoundfontClosed(int indexSf2)
{
    // The soundfont was open in a tab that has been closed: the file is loaded again if an element is selected
    if (indexSf2 != _sf2Index)
        return;

    _sf2Index = -1;
    _isTemporary = false;
    if (_element.typeElement != elementUnknown && !_filePath.isEmpty())
        load();
    else
        setStatus(NOT_LOADED);
}
