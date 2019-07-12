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

#include "soundfontmanager.h"
#include "actionmanager.h"
#include "soundfonts.h"
#include "soundfont.h"
#include "smpl.h"
#include "instprst.h"
#include "division.h"
#include "modulator.h"
#include "action.h"
#include "indexedelementlist.h"
#include "utils.h"
#include "sampleutils.h"

SoundfontManager * SoundfontManager::s_instance = nullptr;

SoundfontManager * SoundfontManager::getInstance()
{
    if (s_instance == nullptr)
        s_instance = new SoundfontManager();
    return s_instance;
}

void SoundfontManager::kill()
{
    if (s_instance != nullptr)
    {
        delete s_instance;
        s_instance = nullptr;
    }
}

QAbstractItemModel * SoundfontManager::getModel(int indexSf2)
{
    return _soundfonts->getModel(indexSf2);
}

SoundfontManager::SoundfontManager() :
    _soundfonts(new Soundfonts()),
    _undoRedo(new ActionManager()),
    _mutex(QMutex::Recursive)
{
    connect(_undoRedo, SIGNAL(dropId(EltID)), this, SLOT(onDropId(EltID)));
}

SoundfontManager::~SoundfontManager()
{
    delete _undoRedo;
    delete _soundfonts;
}

// Ajout / suppression des données
void SoundfontManager::remove(EltID id, int *message)
{
    QMutexLocker locker(&_mutex);
    this->remove(id, false, true, message);
}

void SoundfontManager::onDropId(EltID id)
{
    QMutexLocker locker(&_mutex);
    this->remove(id, true, false);
}

// Accès / modification des propriétés
bool SoundfontManager::isSet(EltID id, AttributeType champ)
{
    QMutexLocker locker(&_mutex);
    bool value = false;
    if (!this->isValid(id))
        return value;

    // Type d'élément à analyser
    switch (id.typeElement)
    {
    case elementSf2:
        // Analyse d'un SF2
        value = true;
        break;
    case elementSmpl:
        // Analyse d'un sample
        value = true;
        break;
    case elementInst:
        // Analyse d'un instrument
        value = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getGlobalDivision()->isSet(champ);
        break;
    case elementPrst:
        // Analyse d'un preset
        switch (champ)
        {
        case champ_wPreset:
        case champ_wBank:
        case champ_dwLibrary:
        case champ_dwGenre:
        case champ_dwMorphology:
            value = true;
            break;
        default:
            value = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getGlobalDivision()->isSet(champ);
        }
        break;
    case elementInstSmpl:{
        // Analyse d'un sample lié à un instrument
        Division *tmp = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getDivision(id.indexElt2);
        value = tmp->isSet(champ);
    }break;
    case elementPrstInst:{
        // Analyse d'un instrument lié à un preset
        Division *tmp = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getDivision(id.indexElt2);
        value = tmp->isSet(champ);
    }break;
    case elementInstMod:
        // Analyse d'un mod d'un instrument
        value = true;
        break;
    case elementPrstMod:
        // Analyse d'un mod d'un preset
        value = true;
        break;
    case elementInstSmplMod:
        // Analyse d'un mod d'un sample lié à un instrument
        value = true;
        break;
    case elementPrstInstMod:
        // Analyse d'un mod d'un instrument lié à un preset
        value = true;
        break;
    default:
        break;
    }
    return value;
}

AttributeValue SoundfontManager::get(EltID id, AttributeType champ)
{
    QMutexLocker locker(&_mutex);
    AttributeValue value;
    value.dwValue = 0;
    if (!this->isValid(id))
        return value;

    // Type d'élément à analyser
    switch (id.typeElement)
    {
    case elementSf2:{
        // Analyse d'un SF2
        Soundfont *tmp = _soundfonts->getSoundfont(id.indexSf2);
        switch (champ)
        {
        case champ_IFIL:
            value.sfVerValue = tmp->_IFIL;
            break;
        case champ_IVER:
            value.sfVerValue = tmp->_IVER;
            break;
        case champ_wBpsInit:
            value.wValue = tmp->_wBpsInit;
            break;
        case champ_wBpsSave:
            value.wValue = tmp->_wBpsSave;
            break;
        default:
            break;
        }
    }break;
    case elementSmpl:{
        // Analyse d'un sample
        Smpl *tmp = _soundfonts->getSoundfont(id.indexSf2)->getSample(id.indexElt);
        switch (champ)
        {
        case champ_dwStart16:
            value.dwValue = tmp->_sound.getUInt32(champ_dwStart16); break;
        case champ_dwStart24:
            value.dwValue = tmp->_sound.getUInt32(champ_dwStart24); break;
        case champ_dwLength:
            value.dwValue = tmp->_sound.getUInt32(champ_dwLength); break;
        case champ_dwStartLoop:
            value.dwValue = tmp->_sound.getUInt32(champ_dwStartLoop); break;
        case champ_dwEndLoop:
            value.dwValue = tmp->_sound.getUInt32(champ_dwEndLoop); break;
        case champ_dwSampleRate:
            value.dwValue = tmp->_sound.getUInt32(champ_dwSampleRate); break;
        case champ_bpsFile:
            value.dwValue = tmp->_sound.getUInt32(champ_bpsFile); break;
        case champ_wChannel:
            value.dwValue = tmp->_sound.getUInt32(champ_wChannel); break;
        case champ_wChannels:
            value.dwValue = tmp->_sound.getUInt32(champ_wChannels); break;
        case champ_byOriginalPitch:
            value.bValue = tmp->_sound.getUInt32(champ_byOriginalPitch); break;
        case champ_chPitchCorrection:
            value.cValue = tmp->_sound.getInt32(champ_chPitchCorrection); break;
        case champ_wSampleLink:
            value.wValue = tmp->_wSampleLink; break;
        case champ_sfSampleType:
            value.sfLinkValue = tmp->_sfSampleType; break;
        default:
            break;
        }
    }break;
    case elementInst:{
        // Analyse d'un instrument
        InstPrst *tmp = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt);
        value = tmp->getGlobalDivision()->getGen(champ);
    }break;
    case elementPrst:{
        // Analyse d'un preset
        InstPrst *tmp = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt);
        switch (champ)
        {
        case champ_wPreset: case champ_wBank:
            value.wValue = tmp->getExtraField(champ);
            break;
        case champ_dwLibrary: case champ_dwGenre: case champ_dwMorphology:
            value.dwValue = tmp->getExtraField(champ);
            break;
        default:
            value = tmp->getGlobalDivision()->getGen(champ);
            break;
        }
    }break;
    case elementInstSmpl:{
        // Analyse d'un sample lié à un instrument
        Division *tmp = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getDivision(id.indexElt2);
        if (champ == champ_mute)
            value.bValue = (tmp->isMute() ? 1 : 0);
        else
            value = tmp->getGen(champ);
    }break;
    case elementPrstInst:{
        // Analyse d'un instrument lié à un preset
        Division *tmp = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getDivision(id.indexElt2);
        if (champ == champ_mute)
            value.bValue = (tmp->isMute() ? 1 : 0);
        else
            value = tmp->getGen(champ);
    }break;
    case elementInstMod: case elementPrstMod: case elementInstSmplMod: case elementPrstInstMod:{
        // Analyse d'un mod
        Modulator *tmp = nullptr;
        switch (id.typeElement)
        {
        case elementInstMod:
            tmp = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getGlobalDivision()->getMod(id.indexMod);
            break;
        case elementPrstMod:
            tmp = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getGlobalDivision()->getMod(id.indexMod);
            break;
        case elementInstSmplMod:
            tmp = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getDivision(id.indexElt2)->getMod(id.indexMod);
            break;
        case elementPrstInstMod:
            tmp = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getDivision(id.indexElt2)->getMod(id.indexMod);
            break;
        default:
            break;
        }
        switch (champ)
        {
        case champ_sfModSrcOper:
            value.sfModValue = tmp->_data.srcOper; break;
        case champ_sfModDestOper:
            value.wValue = tmp->_data.destOper; break;
        case champ_modAmount:
            value.shValue = tmp->_data.amount; break;
        case champ_sfModAmtSrcOper:
            value.sfModValue = tmp->_data.amtSrcOper; break;
        case champ_sfModTransOper:
            value.sfTransValue = tmp->_data.transOper; break;
        case champ_indexMod:
            value.wValue = tmp->_data.index; break;
        default:
            break;
        }
    }break;
    case elementInstGen: case elementPrstGen: case elementInstSmplGen: case elementPrstInstGen:{
        // Analyse d'un gen
        Division * division = nullptr;
        switch (id.typeElement)
        {
        case elementInstGen:
            division = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getGlobalDivision();
            break;
        case elementPrstGen:
            division = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getGlobalDivision();
            break;
        case elementInstSmplGen:
            division = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getDivision(id.indexElt2);
            break;
        case elementPrstInstGen:
            division = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getDivision(id.indexElt2);
            break;
        default:
            break;
        }
        value = division->getGen(champ);
    }break;
    default:
        break;
    }

    return value;
}

Sound * SoundfontManager::getSound(EltID id)
{
    QMutexLocker locker(&_mutex);
    Sound * son = nullptr;
    if (!this->isValid(id))
        return son;

    if (id.typeElement == elementSmpl)
        son = &_soundfonts->getSoundfont(id.indexSf2)->getSample(id.indexElt)->_sound;

    return son;
}

QString SoundfontManager::getQstr(EltID id, AttributeType champ)
{
    QMutexLocker locker(&_mutex);
    if (!this->isValid(id))
        return "";

    // Type d'élément à analyser
    QString ret = "";
    switch (id.typeElement)
    {
    case elementSf2:{
        // Analyse d'un SF2
        Soundfont *tmp = _soundfonts->getSoundfont(id.indexSf2);
        switch (champ)
        {
        case champ_name:
            ret = tmp->_INAM; break;
        case champ_nameSort:
            ret = tmp->_nameSort; break;
        case champ_ISNG:
            ret = tmp->_ISNG; break;
        case champ_IROM:
            ret = tmp->_IROM; break;
        case champ_ICRD:
            ret = tmp->_ICRD; break;
        case champ_IENG:
            ret = tmp->_IENG; break;
        case champ_IPRD:
            ret = tmp->_IPRD; break;
        case champ_ICOP:
            ret = tmp->_ICOP; break;
        case champ_ICMT:
            ret = tmp->_ICMT; break;
        case champ_ISFT:
            ret = tmp->_ISFT; break;
        case champ_filenameInitial:
            ret = tmp->_fileNameInitial; break;
        case champ_filenameForData:
            ret = tmp->_fileNameForData; break;
        default:
            break;
        }
    }break;
    case elementSmpl:{
        // Analyse d'un sample
        Smpl *tmp = _soundfonts->getSoundfont(id.indexSf2)->getSample(id.indexElt);
        switch (champ)
        {
        case champ_name:
            ret = tmp->getName(); break;
        case champ_nameSort:
            ret = tmp->sortText(); break;
        case champ_filenameForData:
            ret = tmp->_sound.getFileName(); break;
        default:
            break;
        }
    }break;
    case elementInst:{
        // Analyse d'un instrument
        InstPrst *tmp = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt);
        switch (champ)
        {
        case champ_name:
            ret = tmp->getName(); break;
        case champ_nameSort:
            ret = tmp->sortText(); break;
        default:
            break;
        }
    }break;
    case elementPrst:{
        // Analyse d'un preset
        InstPrst *tmp = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt);
        switch (champ)
        {
        case champ_name:
            ret = tmp->getName(); break;
        case champ_nameSort:
            ret = tmp->sortText(); break;
        default:
            break;
        }
    }break;
    default:
        break;
    }

    return ret;
}

QByteArray SoundfontManager::getData(EltID id, AttributeType champ)
{
    QMutexLocker locker(&_mutex);
    QByteArray baRet;
    if (!this->isValid(id))
        return baRet;

    // Type d'élément à analyser
    switch (id.typeElement)
    {
    case elementSmpl:{
        Smpl *tmp = _soundfonts->getSoundfont(id.indexSf2)->getSample(id.indexElt);
        switch (champ)
        {
        case champ_sampleData16:
            baRet = tmp->_sound.getData(16);
            break;
        case champ_sampleData24:
            baRet = tmp->_sound.getData(8);
            break;
        case champ_sampleDataFull24:
            baRet = tmp->_sound.getData(24);
            break;
        case champ_sampleData32:
            baRet = tmp->_sound.getData(32);
            break;
        default:
            break;
        }
    }break;
    default:
        break;
    }

    return baRet;
}

QList<int> SoundfontManager::getSiblings(EltID &id)
{
    QMutexLocker locker(&_mutex);

    QList<int> result;
    if (!this->isValid(id, true, true))
        return result;

    switch (id.typeElement)
    {
    case elementSf2:
        result = _soundfonts->getSoundfonts().keys();
        break;
    case elementSmpl: {
        QVectorIterator<Smpl*> i(_soundfonts->getSoundfont(id.indexSf2)->getSamples().values());
        while (i.hasNext())
        {
            Smpl * elt = i.next();
            if (!elt->isHidden())
                result << elt->getId().indexElt;
        }
    } break;
    case elementInst: {
        QVectorIterator<InstPrst*> i(_soundfonts->getSoundfont(id.indexSf2)->getInstruments().values());
        while (i.hasNext())
        {
            InstPrst* elt = i.next();
            if (!elt->isHidden())
                result << elt->getId().indexElt;
        }
    } break;
    case elementPrst: {
        QVectorIterator<InstPrst*> i(_soundfonts->getSoundfont(id.indexSf2)->getPresets().values());
        while (i.hasNext())
        {
            InstPrst* elt = i.next();
            if (!elt->isHidden())
                result << elt->getId().indexElt;
        }
    } break;
    case elementInstSmpl: {
        QVectorIterator<Division*> i(_soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getDivisions().values());
        while (i.hasNext())
        {
            Division* elt = i.next();
            if (!elt->isHidden())
                result << elt->getId().indexElt2;
        }
    } break;
    case elementPrstInst: {
        QVectorIterator<Division*> i(_soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getDivisions().values());
        while (i.hasNext())
        {
            Division* elt = i.next();
            if (!elt->isHidden())
                result << elt->getId().indexElt2;
        }
    } break;
    case elementInstMod: {
        QVectorIterator<Modulator*> i(_soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getGlobalDivision()->getMods().values());
        while (i.hasNext())
        {
            Modulator* elt = i.next();
            if (!elt->isHidden())
                result << elt->_id;
        }
    } break;
    case elementPrstMod: {
        QVectorIterator<Modulator*> i(_soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getGlobalDivision()->getMods().values());
        while (i.hasNext())
        {
            Modulator* elt = i.next();
            if (!elt->isHidden())
                result << elt->_id;
        }
    } break;
    case elementInstSmplMod: {
        QVectorIterator<Modulator*> i(_soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getDivision(id.indexElt2)->getMods().values());
        while (i.hasNext())
        {
            Modulator* elt = i.next();
            if (!elt->isHidden())
                result << elt->_id;
        }
    } break;
    case elementPrstInstMod: {
        QVectorIterator<Modulator*> i(_soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getDivision(id.indexElt2)->getMods().values());
        while (i.hasNext())
        {
            Modulator* elt = i.next();
            if (!elt->isHidden())
                result << elt->_id;
        }
    } break;
    case elementInstGen: {
        QListIterator<AttributeType> i(_soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getGlobalDivision()->getGens().keys());
        while (i.hasNext())
            result << (int)i.next();
    } break;
    case elementPrstGen: {
        QListIterator<AttributeType> i(_soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getGlobalDivision()->getGens().keys());
        while (i.hasNext())
            result << (int)i.next();
    } break;
    case elementInstSmplGen: {
        QListIterator<AttributeType> i(_soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getDivision(id.indexElt2)->getGens().keys());
        while (i.hasNext())
            result << (int)i.next();
    } break;
    case elementPrstInstGen: {
        QListIterator<AttributeType> i(_soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getDivision(id.indexElt2)->getGens().keys());
        while (i.hasNext())
            result << (int)i.next();
    } break;
    default:
        break;
    }

    return result;
}

/// ACTION MANAGER ///

void SoundfontManager::endEditing(QString editingSource)
{
    QMutexLocker locker(&_mutex);

    // Close the action set and get the list of sf2 that have been edited
    QList<int> sf2Indexes = _undoRedo->commitActionSet();
    if (!sf2Indexes.empty())
        emit(editingDone(editingSource, sf2Indexes));
}

void SoundfontManager::clearNewEditing()
{
    QMutexLocker locker(&_mutex);
    _undoRedo->clearCurrentActionSet();
}

void SoundfontManager::revertNewEditing()
{
    QMutexLocker locker(&_mutex);
    undo(_undoRedo->getCurrentActions());
    _undoRedo->clearCurrentActionSet();
}

bool SoundfontManager::isUndoable(int indexSf2)
{
    QMutexLocker locker(&_mutex);
    return _undoRedo->isUndoable(indexSf2);
}

bool SoundfontManager::isRedoable(int indexSf2)
{
    QMutexLocker locker(&_mutex);
    return _undoRedo->isRedoable(indexSf2);
}

void SoundfontManager::undo(int indexSf2)
{
    QMutexLocker locker(&_mutex);
    QList<int> sf2Indexes = undo(_undoRedo->undo(indexSf2));
    if (!sf2Indexes.empty())
        emit(editingDone("command:undo", sf2Indexes));
}

QList<int> SoundfontManager::undo(QList<Action *> actions)
{
    // Process actions in order
    QList<int> sf2Indexes;
    foreach (Action * action, actions)
    {
        if (!sf2Indexes.contains(action->id.indexSf2))
            sf2Indexes << action->id.indexSf2;
        switch (action->typeAction)
        {
        case Action::TypeCreation: {
            // Hide the element
            int message = 1;
            this->remove(action->id, false, false, &message);
        }break;
        case Action::TypeRemoval:
            // Display the element
            this->display(action->id);
            break;
        case Action::TypeUpdate:
        case Action::TypeChangeToDefault:
            // Back to the old value
            if (action->champ >= 0 && action->champ < 164)
                this->set(action->id, action->champ, action->vOldValue); // Valeur
            else if (action->champ >= 164 && action->champ < 200)
                this->set(action->id, action->champ, action->qOldValue); // QString
            else if (action->champ >= 200)
                this->set(action->id, action->champ, action->baOldValue); // char*
            break;
        case Action::TypeChangeFromDefault:
            // Retour to the old value, reset
            this->reset(action->id, action->champ);
            break;
        default:
            break;
        }
    }
    _undoRedo->clearCurrentActionSet();

    return sf2Indexes;
}

void SoundfontManager::redo(int indexSf2)
{
    QMutexLocker locker(&_mutex);
    QList<Action *> actions = _undoRedo->redo(indexSf2);

    // Process actions in reverse order
    Action * action;
    QList<int> sf2Indexes;
    for (int i = actions.count()-1; i >= 0; i--)
    {
        action = actions[i];
        if (!sf2Indexes.contains(action->id.indexSf2))
            sf2Indexes << action->id.indexSf2;
        switch (action->typeAction)
        {
        case Action::TypeCreation:
            // Display element
            this->display(action->id);
            break;
        case Action::TypeRemoval:{
            // Hide elemnet
            int message = 1;
            this->remove(action->id, false, false, &message);
        }break;
        case Action::TypeUpdate:
        case Action::TypeChangeFromDefault:
            // Apply again the new value
            if (action->champ >= 0 && action->champ < 164)
                this->set(action->id, action->champ, action->vNewValue); // Valeur
            else if (action->champ >= 164 && action->champ < 200)
                this->set(action->id, action->champ, action->qNewValue); // QString
            else if (action->champ >= 200)
                this->set(action->id, action->champ, action->baNewValue); // char*
            break;
        case Action::TypeChangeToDefault:
            // Apply the new value, reset
            this->reset(action->id, action->champ);
            break;
        default:
            break;
        }
    }
    _undoRedo->clearCurrentActionSet();

    if (!sf2Indexes.empty())
        emit(editingDone("command:redo", sf2Indexes));
}

void SoundfontManager::markAsSaved(int indexSf2)
{
    QMutexLocker locker(&_mutex);
    _soundfonts->getSoundfont(indexSf2)->_numEdition = this->_undoRedo->getEdition(indexSf2);
    emit(editingDone("command:save", QList<int>() << indexSf2));
}

bool SoundfontManager::isEdited(int indexSf2)
{
    QMutexLocker locker(&_mutex);
    if (_soundfonts->getSoundfont(indexSf2) == nullptr)
        return false;
    return this->_undoRedo->getEdition(indexSf2) != _soundfonts->getSoundfont(indexSf2)->_numEdition ||
            !this->getQstr(EltID(elementSf2, indexSf2), champ_filenameInitial).toLower().endsWith(".sf2");
}

void SoundfontManager::getAllAttributes(EltID id, QList<AttributeType> &listeChamps, QList<AttributeValue> &listeValeurs)
{
    QMutexLocker locker(&_mutex);
    if (!this->isValid(id))
        return;

    Division * division = nullptr;
    switch (id.typeElement)
    {
    case elementInst:
        division = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getGlobalDivision();
        break;
    case elementInstSmpl:
        division = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getDivision(id.indexElt2);
        break;
    case elementPrst:
        division = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getGlobalDivision();
        break;
    case elementPrstInst:
        division = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getDivision(id.indexElt2);
        break;
    default:
        return;
    }

    listeChamps = division->getGens().keys();
    listeValeurs = division->getGens().values();
}

void SoundfontManager::getAllModulators(EltID id, QList<ModulatorData> &modulators)
{
    QMutexLocker locker(&_mutex);
    if (!this->isValid(id))
        return;

    Division * division = nullptr;
    switch (id.typeElement)
    {
    case elementInst:
        division = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getGlobalDivision();
        break;
    case elementInstSmpl:
        division = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getDivision(id.indexElt2);
        break;
    case elementPrst:
        division = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getGlobalDivision();
        break;
    case elementPrstInst:
        division = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getDivision(id.indexElt2);
        break;
    default:
        return;
    }

    // Fill the lists with the modulators that are not hidden
    QVector<Modulator *> mods = division->getMods().values();
    foreach (Modulator * mod, mods)
        if (!mod->isHidden())
            modulators << mod->_data;
}

// Add a child to ID
int SoundfontManager::add(EltID id)
{
    QMutexLocker locker(&_mutex);
    if (!this->isValid(id, false, true))
        return -1;

    int i = -1;

    switch (id.typeElement)
    {
    case elementSf2: {
        // Create a new SF2
        i = id.indexSf2 = _soundfonts->addSoundfont();

        // Initialisation bps
        AttributeValue valTmp;
        valTmp.wValue = 16;
        this->set(id, champ_wBpsInit, valTmp);
        this->set(id, champ_wBpsSave, valTmp);
    } break;
    case elementSmpl:
        // Create a new sample
        i = id.indexElt = _soundfonts->getSoundfont(id.indexSf2)->addSample();
        break;
    case elementInst:
        // Create a new instrument
        i = id.indexElt = _soundfonts->getSoundfont(id.indexSf2)->addInstrument();
        break;
    case elementPrst:
        // Create a new preset
        i = id.indexElt = _soundfonts->getSoundfont(id.indexSf2)->addPreset();
        break;
    case elementInstSmpl:
        // Add a new sample in an instrument
        i = id.indexElt2 = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->addDivision();
        break;
    case elementPrstInst:
        // Add a new instrument in a preset
        i = id.indexElt2 = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->addDivision();
        break;
    case elementInstMod: case elementPrstMod: case elementInstSmplMod: case elementPrstInstMod:{
        // Add a new mod
        Division *bag = nullptr;
        switch (id.typeElement)
        {
        case elementInstMod:
            // For an instrument
            bag = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getGlobalDivision();
            break;
        case elementPrstMod:
            // For a preset
            bag = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getGlobalDivision();
            break;
        case elementInstSmplMod:
            // For a sample linked to an instrument
            bag = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getDivision(id.indexElt2);
            break;
        case elementPrstInstMod:
            // For an instrument linked to a preset
            bag = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getDivision(id.indexElt2);
            break;
        default:
            break;
        }
        i = id.indexMod = bag->addMod();

        // Fill the index
        AttributeValue val;
        val.wValue = id.indexMod;
        this->set(id, champ_indexMod, val);
    }break;
    default:
        break;
    }

    // Create and store an action
    Action *action = new Action();
    action->typeAction = Action::TypeCreation;
    action->id = id;
    this->_undoRedo->add(action);

    return i;
}

int SoundfontManager::remove(EltID id, bool permanently, bool storeAction, int *message)
{
    if (!this->isValid(id, permanently)) // Hidden ID are accepted for a permanent removal
        return 1;

    switch (id.typeElement)
    {
    case elementSf2:{
        permanently = true; // No undo possible after a file is closed
        storeAction = false;

        // Delete presets
        QVector<InstPrst *> presets = _soundfonts->getSoundfont(id.indexSf2)->getPresets().values();
        for (int i = presets.count() - 1; i >= 0; i--)
            this->remove(presets[i]->getId(), true, storeAction, message);

        // Delete instruments
        QVector<InstPrst *> instruments = _soundfonts->getSoundfont(id.indexSf2)->getInstruments().values();
        for (int i = instruments.count() - 1; i >= 0; i--)
            this->remove(instruments[i]->getId(), true, storeAction, message);

        // Delete samples
        QVector<Smpl *> samples = _soundfonts->getSoundfont(id.indexSf2)->getSamples().values();
        for (int i = samples.count() - 1; i >= 0; i--)
            this->remove(samples[i]->getId(), true, storeAction, message);

        // Finally delete sf2
        _soundfonts->deleteSoundfont(id.indexSf2);
        _undoRedo->dropSoundfont(id.indexSf2);
    }break;
    case elementSmpl:{
        // Check that no instruments use the sample
        foreach (InstPrst * instTmp, _soundfonts->getSoundfont(id.indexSf2)->getInstruments().values())
        {
            if (!instTmp->isHidden())
            {
                foreach (Division *bagTmp, instTmp->getDivisions().values())
                {
                    if (!bagTmp->isHidden() && bagTmp->isSet(champ_sampleID) &&
                            bagTmp->getGen(champ_sampleID).wValue == id.indexElt)
                    {
                        if (message != nullptr && (*message) % 2 != 0)
                            *message *= 2;
                        return 1;
                    }
                }
            }
        }

        // Linked sample?
        EltID id2(elementSmpl, id.indexSf2, 0, 0, 0);
        AttributeValue value;
        foreach (Smpl * smplTmp, _soundfonts->getSoundfont(id.indexSf2)->getSamples().values())
        {
            if (!smplTmp->isHidden() && smplTmp->_wSampleLink == id.indexElt)
            {
                id2 = smplTmp->getId();
                value.wValue = 0;
                this->set(id2, champ_wSampleLink, value);
                value.sfLinkValue = monoSample;
                this->set(id2, champ_sfSampleType, value);
            }
        }

        // Delete or hide the sample?
        if (permanently)
            _soundfonts->getSoundfont(id.indexSf2)->deleteSample(id.indexElt);
        else
            _soundfonts->getSoundfont(id.indexSf2)->getSample(id.indexElt)->setHidden(true);
    }break;
    case elementInst:{
        // Check that no presets use the instrument
        foreach (InstPrst * prstTmp, _soundfonts->getSoundfont(id.indexSf2)->getPresets().values())
        {
            if (!prstTmp->isHidden())
            {
                foreach (Division *bagTmp, prstTmp->getDivisions().values())
                {
                    if (!bagTmp->isHidden() && bagTmp->isSet(champ_instrument) &&
                            bagTmp->getGen(champ_instrument).wValue == id.indexElt)
                    {
                        if (message != nullptr && (*message) % 3 != 0)
                            *message *= 3;
                        return 1;
                    }
                }
            }
        }

        // Propagation aux samples liés
        QVector<Division*> divisions = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getDivisions().values();
        for (int i = divisions.count() - 1; i >= 0; i--)
        {
            Division * division = divisions[i];
            if (permanently || !division->isHidden())
                this->remove(division->getId(), permanently, storeAction, message);
        }

        // Propagation aux mods associés
        EltID id2(elementInstMod, id.indexSf2, id.indexElt, 0, 0);
        QVector<Modulator*> mods = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getGlobalDivision()->getMods().values();
        for (int i = mods.count() - 1; i >= 0; i--)
        {
            Modulator * mod = mods[i];
            id2.indexMod = mod->_id;
            if (permanently || !mod->isHidden())
                this->remove(id2, permanently, storeAction, message);
        }

        // Suppression des gens
        supprGenAndStore(id, storeAction);

        // Delete or hide the instrument?
        if (permanently)
            _soundfonts->getSoundfont(id.indexSf2)->deleteInstrument(id.indexElt);
        else
            _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->setHidden(true);
    }break;
    case elementPrst:{
        // Propagation aux instruments liés
        QVector<Division*> divisions = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getDivisions().values();
        for (int i = divisions.count() - 1; i >= 0; i--)
        {
            Division * division = divisions[i];
            if (permanently || !division->isHidden())
                this->remove(division->getId(), permanently, storeAction, message);
        }

        // Propagation aux mods associés
        EltID id2(elementPrstMod, id.indexSf2, id.indexElt, 0, 0);
        QVector<Modulator*> mods = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getGlobalDivision()->getMods().values();
        for (int i = mods.count() - 1; i >= 0; i--)
        {
            Modulator * mod = mods[i];
            id2.indexMod = mod->_id;
            if (permanently || !mod->isHidden())
                this->remove(id2, permanently, storeAction, message);
        }

        // Suppression des gens
        supprGenAndStore(id, storeAction);

        // Delete or hide the preset?
        if (permanently)
            _soundfonts->getSoundfont(id.indexSf2)->deletePreset(id.indexElt);
        else
            _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->setHidden(true);
    }break;
    case elementInstSmpl:{
        // Propagation aux mods associés
        EltID id2(elementInstSmplMod, id.indexSf2, id.indexElt, id.indexElt2, 0);
        QVector<Modulator*> mods = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getDivision(id.indexElt2)->getMods().values();
        for (int i = mods.count() - 1; i >= 0; i--)
        {
            Modulator * mod = mods[i];
            id2.indexMod = mod->_id;
            if (permanently || !mod->isHidden())
                this->remove(id2, permanently, storeAction, message);
        }

        // Suppression des gens
        supprGenAndStore(id, storeAction);

        // Delete or hide the InstSmpl?
        if (permanently)
            _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->deleteDivision(id.indexElt2);
        else
            _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getDivision(id.indexElt2)->setHidden(true);
    }break;
    case elementPrstInst:{
        // Propagation aux mods associés
        EltID id2(elementPrstInstMod, id.indexSf2, id.indexElt, id.indexElt2, 0);
        QVector<Modulator*> mods = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getDivision(id.indexElt2)->getMods().values();
        for (int i = mods.count() - 1; i >= 0; i--)
        {
            Modulator * mod = mods[i];
            id2.indexMod = mod->_id;
            if (permanently || !mod->isHidden())
                this->remove(id2, permanently, storeAction, message);
        }

        // Suppression des gens
        supprGenAndStore(id, storeAction);

        // Delete or hide the PrstInst?
        if (permanently)
            _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->deleteDivision(id.indexElt2);
        else
            _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getDivision(id.indexElt2)->setHidden(true);
    }break;
    case elementInstMod: case elementPrstMod: case elementInstSmplMod: case elementPrstInstMod:{
        // suppression d'un mod
        Division *bag = nullptr;
        switch (id.typeElement)
        {
        case elementInstMod:
            // d'un instrument
            bag = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getGlobalDivision();
            break;
        case elementPrstMod:
            // d'un preset
            bag = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getGlobalDivision();
            break;
        case elementInstSmplMod:
            // d'un sample lié à un instrument
            bag = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getDivision(id.indexElt2);
            break;
        case elementPrstInstMod:
            // d'un instrument lié à un prst
            bag = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getDivision(id.indexElt2);
            break;
        default:
            break;
        }

        // If the destination of a modulator is the modulator to delete, remove the link
        foreach (Modulator * mod, bag->getMods().values())
        {
            if (mod->_data.destOper - 32768 == id.indexMod)
            {
                EltID id2 = id;
                id2.indexMod = mod->_id;
                AttributeValue value;
                value.dwValue = 0;
                this->set(id2, champ_sfModDestOper, value);
            }
        }

        // Delete or hide the mod
        if (permanently)
            bag->deleteMod(id.indexMod);
        else
            bag->getMod(id.indexMod)->setHidden(true);
    }break;
    default:
        return 1;
    }

    // Create and store the action
    if (storeAction)
    {
        Action *action = new Action();
        action->typeAction = Action::TypeRemoval;
        action->id = id;
        this->_undoRedo->add(action);
    }

    return 0;
}

void SoundfontManager::supprGenAndStore(EltID id, int storeAction)
{
    Division *division = nullptr;
    switch (id.typeElement)
    {
    case elementInst:
        division = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getGlobalDivision();
        break;
    case elementPrst:
        division = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getGlobalDivision();
        break;
    case elementInstSmpl:
        division = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getDivision(id.indexElt2);
        break;
    case elementPrstInst:
        division = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getDivision(id.indexElt2);
        break;
    default:
        return;
    }

    const QMap<AttributeType, AttributeValue> parameters = division->getGens();
    foreach (AttributeType champ, parameters.keys())
    {
        // Create and store an action
        if (storeAction)
        {
            Action * action = new Action();
            action->typeAction = Action::TypeChangeToDefault;
            action->id = id;
            action->champ = champ;
            action->vOldValue = parameters[champ];
            _undoRedo->add(action);
        }

        division->resetGen(champ);
    }
}

int SoundfontManager::set(EltID id, AttributeType champ, AttributeValue value)
{
    QMutexLocker locker(&_mutex);
    if (!this->isValid(id))
        return 1;
    bool storeAction = true;

    AttributeValue oldValue;
    oldValue.wValue = 0;
    int defaultValue = 0;

    // Type d'élément à modifier
    switch (id.typeElement)
    {
    case elementSf2:{
        // Modification d'un SF2
        Soundfont *tmp = _soundfonts->getSoundfont(id.indexSf2);
        switch (champ)
        {
        case champ_IFIL:
            oldValue.sfVerValue = tmp->_IFIL;
            tmp->_IFIL = value.sfVerValue; break;
        case champ_IVER:
            oldValue.sfVerValue = tmp->_IVER;
            tmp->_IVER = value.sfVerValue; break;
        case champ_wBpsInit:
            oldValue.wValue = tmp->_wBpsInit;
            tmp->_wBpsInit = value.wValue; break;
        case champ_wBpsSave:
            oldValue.wValue = tmp->_wBpsSave;
            tmp->_wBpsSave = value.wValue; break;
        default:
            break;
        }
    }break;
    case elementSmpl:{
        // Modification d'un sample
        Smpl *tmp = _soundfonts->getSoundfont(id.indexSf2)->getSample(id.indexElt);
        switch (champ)
        {
        case champ_dwStart16:
            oldValue.dwValue = tmp->_sound.getUInt32(champ_dwStart16);
            tmp->_sound.set(champ_dwStart16, value); break;
        case champ_dwStart24:
            oldValue.dwValue = tmp->_sound.getUInt32(champ_dwStart24);
            tmp->_sound.set(champ_dwStart24, value); break;
        case champ_dwLength:
            oldValue.dwValue = tmp->_sound.getUInt32(champ_dwLength);
            tmp->_sound.set(champ_dwLength, value); break;
        case champ_dwStartLoop:
            oldValue.dwValue = tmp->_sound.getUInt32(champ_dwStartLoop);
            tmp->_sound.set(champ_dwStartLoop, value); break;
        case champ_dwEndLoop:
            oldValue.dwValue = tmp->_sound.getUInt32(champ_dwEndLoop);
            tmp->_sound.set(champ_dwEndLoop, value); break;
        case champ_dwSampleRate:
            oldValue.dwValue = tmp->_sound.getUInt32(champ_dwSampleRate);
            tmp->_sound.set(champ_dwSampleRate, value); break;
        case champ_byOriginalPitch:
            oldValue.bValue = tmp->_sound.getUInt32(champ_byOriginalPitch);
            tmp->_sound.set(champ_byOriginalPitch, value); break;
        case champ_chPitchCorrection:
            oldValue.cValue = tmp->_sound.getInt32(champ_chPitchCorrection);
            tmp->_sound.set(champ_chPitchCorrection, value); break;
        case champ_wSampleLink:
            oldValue.wValue = tmp->_wSampleLink;
            tmp->_wSampleLink = value.wValue; break;
        case champ_sfSampleType:
            oldValue.sfLinkValue = tmp->_sfSampleType;
            tmp->_sfSampleType = value.sfLinkValue; break;
        case champ_bpsFile:
            oldValue.wValue = tmp->_sound.getUInt32(champ_bpsFile);
            tmp->_sound.set(champ_bpsFile, value); break;
        case champ_wChannel:
            oldValue.wValue = tmp->_sound.getUInt32(champ_wChannel);
            tmp->_sound.set(champ_wChannel, value); break;
        case champ_wChannels:
            oldValue.wValue = tmp->_sound.getUInt32(champ_wChannels);
            tmp->_sound.set(champ_wChannels, value); break;
        default:
            break;
        }
    }break;
    case elementInst:{
        // Update an instrument
        InstPrst *tmp = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt);
        if (!tmp->getGlobalDivision()->isSet(champ))
            defaultValue = 1;
        oldValue = tmp->getGlobalDivision()->getGen(champ);
        tmp->getGlobalDivision()->setGen(champ, value);
    }break;
    case elementPrst:{
        // Update a preset
        InstPrst *tmp = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt);
        switch (champ)
        {
        case champ_wPreset: case champ_wBank:
            oldValue.wValue = tmp->getExtraField(champ);
            tmp->setExtraField(champ, value.wValue);
            break;
        case champ_dwLibrary: case champ_dwGenre: case champ_dwMorphology:
            oldValue.dwValue = tmp->getExtraField(champ);
            tmp->setExtraField(champ, value.dwValue);
            break;
        default:
            if (!tmp->getGlobalDivision()->isSet(champ)) defaultValue = 1;
            oldValue = tmp->getGlobalDivision()->getGen(champ);
            tmp->getGlobalDivision()->setGen(champ, value);
        }
    }break;
    case elementInstSmpl:{
        // Modification of a sample linked to an instrument
        Division *tmp = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getDivision(id.indexElt2);
        if (champ == champ_mute)
        {
            tmp->setMute(value.bValue > 0);
            storeAction = false;
        }
        else
        {
            if (!tmp->isSet(champ))
                defaultValue = 1;
            oldValue = tmp->getGen(champ);
            tmp->setGen(champ, value);
        }
    }break;
    case elementPrstInst:{
        // Modification of an instrument linked to a preset
        Division *tmp = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getDivision(id.indexElt2);
        if (champ == champ_mute)
        {
            tmp->setMute(value.bValue > 0);
            storeAction = false;
        }
        else
        {
            if (!tmp->isSet(champ))
                defaultValue = 1;
            oldValue = tmp->getGen(champ);
            tmp->setGen(champ, value);
        }
    }break;
    case elementInstMod: case elementPrstMod: case elementInstSmplMod: case elementPrstInstMod:{
        // Modification d'un mod d'un instrument
        Modulator *tmp = nullptr;
        switch (id.typeElement)
        {
        case elementInstMod:
            tmp = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getGlobalDivision()->getMod(id.indexMod); break;
        case elementPrstMod:
            tmp = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getGlobalDivision()->getMod(id.indexMod); break;
        case elementInstSmplMod:
            tmp = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getDivision(id.indexElt2)->getMod(id.indexMod); break;
        case elementPrstInstMod:
            tmp = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getDivision(id.indexElt2)->getMod(id.indexMod); break;
        default:
            break;
        }

        switch (champ)
        {
        case champ_sfModSrcOper:
            oldValue.sfModValue = tmp->_data.srcOper;
            tmp->_data.srcOper = value.sfModValue; break;
        case champ_sfModDestOper:
            oldValue.wValue = tmp->_data.destOper;
            tmp->_data.destOper = value.wValue; break;
        case champ_modAmount:
            oldValue.shValue = tmp->_data.amount;
            tmp->_data.amount = value.shValue; break;
        case champ_sfModAmtSrcOper:
            oldValue.sfModValue = tmp->_data.amtSrcOper;
            tmp->_data.amtSrcOper = value.sfModValue; break;
        case champ_sfModTransOper:
            oldValue.sfTransValue = tmp->_data.transOper;
            tmp->_data.transOper = value.sfTransValue; break;
        case champ_indexMod:
            oldValue.wValue = tmp->_data.index;
            tmp->_data.index = value.wValue; break;
        default:
            break;
        }
    }break;
    default:
        break;
    }

    if (storeAction)
    {
        // Create and store the action
        Action *action = new Action();
        if (defaultValue)
            action->typeAction = Action::TypeChangeFromDefault;
        else
            action->typeAction = Action::TypeUpdate;
        action->id = id;
        action->champ = champ;
        action->vOldValue = oldValue;
        action->vNewValue = value;
        this->_undoRedo->add(action);
    }

    return 0;
}

int SoundfontManager::set(EltID id, AttributeType champ, QString qStr)
{
    QMutexLocker locker(&_mutex);
    if (!this->isValid(id))
        return 1;

    QString qOldStr = "";

    // Type of element to edit
    switch (id.typeElement)
    {
    case elementSf2:{
        // Editing of a sf2
        Soundfont *tmp = _soundfonts->getSoundfont(id.indexSf2);
        switch (champ)
        {
        case champ_name:
            qOldStr = tmp->_INAM;
            qStr = qStr.left(256);
            tmp->_INAM = qStr;
            tmp->_nameSort = Utils::removeAccents(tmp->_INAM).toLower();
            break;
        case champ_ISNG:
            qOldStr = tmp->_ISNG;
            tmp->_ISNG = qStr.left(255); break;
        case champ_IROM:
            qOldStr = tmp->_IROM;
            tmp->_IROM = qStr.left(255); break;
        case champ_ICRD:
            qOldStr = tmp->_ICRD;
            tmp->_ICRD = qStr.left(255); break;
        case champ_IENG:
            qOldStr = tmp->_IENG;
            tmp->_IENG = qStr.left(255); break;
        case champ_IPRD:
            qOldStr = tmp->_IPRD;
            tmp->_IPRD = qStr.left(255); break;
        case champ_ICOP:
            qOldStr = tmp->_ICOP;
            tmp->_ICOP = qStr.left(255); break;
        case champ_ICMT:
            qOldStr = tmp->_ICMT;
            tmp->_ICMT = qStr.left(65535); break;
        case champ_ISFT:
            qOldStr = tmp->_ISFT;
            tmp->_ISFT = qStr.left(255); break;
        case champ_filenameInitial:
            qOldStr = tmp->_fileNameInitial;
            tmp->_fileNameInitial = qStr; break;
        case champ_filenameForData:
            qOldStr = tmp->_fileNameForData;
            tmp->_fileNameForData = qStr; break;
        default:
            break;
        }
    }break;
    case elementSmpl:{
        // Editing of a sample
        qStr = qStr.trimmed();
        Smpl *tmp = _soundfonts->getSoundfont(id.indexSf2)->getSample(id.indexElt);
        switch (champ)
        {
        case champ_name:
            qOldStr = tmp->getName();
            qStr = qStr.left(20);
            tmp->setName(qStr);
            break;
        case champ_filenameForData:
            qOldStr = tmp->_sound.getFileName();
            tmp->_sound.setFileName(qStr);
            break;
        default:
            break;
        }
    }break;
    case elementInst:{
        qStr = qStr.trimmed();
        // Modification d'un instrument
        InstPrst *tmp = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt);
        switch (champ)
        {
        case champ_name:
            // Modification du nom d'un instrument
            qOldStr = tmp->getName();
            qStr = qStr.left(20);
            tmp->setName(qStr);
            break;
        default:
            break;
        }
    }break;
    case elementPrst:{
        qStr = qStr.trimmed();
        // Modification d'un preset
        InstPrst *tmp = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt);
        switch (champ)
        {
        case champ_name:
            // Modification du nom d'un preset
            qOldStr = tmp->getName();
            qStr = qStr.left(20);
            tmp->setName(qStr);
            break;
        default:
            break;
        }
    }break;
    default:
        break;
    }

    // Create and store the action
    Action *action = new Action();
    action->typeAction = Action::TypeUpdate;
    action->id = id;
    action->champ = champ;
    action->qOldValue = qOldStr;
    action->qNewValue = qStr;
    this->_undoRedo->add(action);

    return 0;
}

int SoundfontManager::set(EltID id, AttributeType champ, QByteArray data)
{
    QMutexLocker locker(&_mutex);
    if (!this->isValid(id))
        return 1;

    QByteArray oldData;
    oldData.clear();
    // Type d'élément à modifier
    switch (id.typeElement)
    {
    case elementSmpl:
        // Update a sample
        switch (champ)
        {
        case champ_sampleData16:
            oldData = _soundfonts->getSoundfont(id.indexSf2)->getSample(id.indexElt)->_sound.getData(16);
            _soundfonts->getSoundfont(id.indexSf2)->getSample(id.indexElt)->_sound.setData(data, 16);
            break;
        case champ_sampleData24:
            oldData = _soundfonts->getSoundfont(id.indexSf2)->getSample(id.indexElt)->_sound.getData(8);
            _soundfonts->getSoundfont(id.indexSf2)->getSample(id.indexElt)->_sound.setData(data, 8);
            break;
        case champ_sampleDataFull24:{
            // Modification 16 bits
            QByteArray baData = SampleUtils::bpsConversion(data, 24, 16);
            this->set(id, champ_sampleData16, baData);

            // Modification 8 bits de poids faibles
            baData = SampleUtils::bpsConversion(data, 24, 824);
            this->set(id, champ_sampleData24, baData);
        }break;
        case champ_sampleData32:{
            // Modification 16 bits
            QByteArray baData = SampleUtils::bpsConversion(data, 32, 16);
            this->set(id, champ_sampleData16, baData);

            // Modification 8 bits de poids faibles
            baData = SampleUtils::bpsConversion(data, 32, 824);
            this->set(id, champ_sampleData24, baData);
        }break;
        default:
            break;
        }
        break;
    default:
        return 0;
    }

    // Création et stockage de l'action
    Action *action = new Action();
    action->typeAction = Action::TypeUpdate;
    action->id = id;
    action->champ = champ;
    action->baOldValue = oldData;
    action->baNewValue = data;
    this->_undoRedo->add(action);

    return 0;
}

void SoundfontManager::reset(EltID id, AttributeType champ)
{
    QMutexLocker locker(&_mutex);
    if (!this->isValid(id))
        return;

    AttributeValue oldValue;
    // Type d'élément à modifier
    switch (id.typeElement)
    {
    case elementInst:{
        // Remise à zéro d'une propriété d'un instrument
        InstPrst *tmp = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt);
        if (!tmp->getGlobalDivision()->isSet(champ))
            return;

        oldValue = tmp->getGlobalDivision()->getGen(champ);
        tmp->getGlobalDivision()->resetGen(champ);
    }break;
    case elementPrst:{
        // Remise à zéro d'une propriété d'un preset
        InstPrst *tmp = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt);
        if (!tmp->getGlobalDivision()->isSet(champ))
            return;

        oldValue = tmp->getGlobalDivision()->getGen(champ);
        tmp->getGlobalDivision()->resetGen(champ);
    }break;
    case elementInstSmpl:{
        // Remise à zéro d'une propriété d'un sample lié à un instrument
        Division *tmp = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getDivision(id.indexElt2);
        if (!tmp->isSet(champ))
            return;

        oldValue = tmp->getGen(champ);
        tmp->resetGen(champ);
    }break;
    case elementPrstInst:{
        // Remise à zéro d'une propriété d'un instrument lié à un preset
        Division *tmp = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getDivision(id.indexElt2);
        if (!tmp->isSet(champ))
            return;

        oldValue = tmp->getGen(champ);
        tmp->resetGen(champ);
    }break;
    default:
        return;
    }

    // Create and store the action
    Action *action = new Action();
    action->typeAction = Action::TypeChangeToDefault;
    action->id = id;
    action->champ = champ;
    action->vOldValue = oldValue;
    _undoRedo->add(action);
}

void SoundfontManager::simplify(EltID id, AttributeType champ)
{
    QMutexLocker locker(&_mutex);
    EltID idElement = id;
    if (id.typeElement == elementInst || id.typeElement == elementInstSmpl)
    {
        idElement.typeElement = elementInstSmpl;
        id.typeElement = elementInst;
    }
    else if (id.typeElement == elementPrst || id.typeElement == elementPrstInst)
    {
        idElement.typeElement = elementPrstInst;
        id.typeElement = elementPrst;
    }
    else
        return;

    bool firstValue = true;
    AttributeValue valeur;

    foreach (int i, this->getSiblings(idElement))
    {
        idElement.indexElt2 = i;
        if (this->isSet(idElement, champ))
        {
            if (firstValue)
            {
                valeur = this->get(idElement, champ);
                firstValue = false;
            }
            else if (valeur.dwValue != this->get(idElement, champ).dwValue)
                return;
        }
        else
            return;
    }

    if (!firstValue)
    {
        foreach (int i, this->getSiblings(idElement))
        {
            idElement.indexElt2 = i;
            this->reset(idElement, champ);
        }
        this->set(id, champ, valeur);
    }
}

int SoundfontManager::display(EltID id)
{
    if (!this->isValid(id, true))
        return 1;

    // Type d'élément à afficher (suite à une suppression non définitive)
    switch (id.typeElement)
    {
    case elementSf2: break;
    case elementSmpl:{
        // affichage d'un sample
        Smpl *tmp = _soundfonts->getSoundfont(id.indexSf2)->getSample(id.indexElt);
        tmp->setHidden(false);
    }break;
    case elementInst:{
        // affichage d'un instrument
        InstPrst *tmp = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt);
        tmp->setHidden(false);
    }break;
    case elementPrst:{
        // affichage d'un preset
        InstPrst *tmp = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt);
        tmp->setHidden(false);
    }break;
    case elementInstSmpl:{
        // affichage d'un sample lié à un instrument
        Division *tmp = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getDivision(id.indexElt2);
        tmp->setHidden(false);
    }break;
    case elementPrstInst:{
        // affichage d'un instrument lié à un preset
        Division *tmp = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getDivision(id.indexElt2);
        tmp->setHidden(false);
    }break;
    case elementInstMod:{
        // affichage d'un mod d'un instrument
        Modulator *tmp = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getGlobalDivision()->getMod(id.indexMod);
        tmp->setHidden(false);
    }break;
    case elementPrstMod:{
        // affichage d'un mod d'un preset
        Modulator *tmp = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getGlobalDivision()->getMod(id.indexMod);
        tmp->setHidden(false);
    }break;
    case elementInstSmplMod:{
        // affichage d'un mod d'un sample lié à un instrument
        Modulator *tmp = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getDivision(id.indexElt2)->getMod(id.indexMod);
        tmp->setHidden(false);
    }break;
    case elementPrstInstMod:{
        // affichage d'un mod d'un instrument lié à un preset
        Modulator *tmp = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getDivision(id.indexElt2)->getMod(id.indexMod);
        tmp->setHidden(false);
    }break;
    default:
        break;
    }
    return 0;
}

bool SoundfontManager::isValid(EltID &id, bool acceptHidden, bool justCheckParentLevel)
{
    QMutexLocker locker(&_mutex);
    if (id.typeElement < elementSf2 || id.typeElement > elementPrstInstGen)
        return false;

    // First level: sf2, rootSmpl, rootInst, rootPrst
    if (id.typeElement == elementSf2 || id.typeElement == elementRootSmpl ||
            id.typeElement == elementRootInst || id.typeElement == elementRootPrst)
    {
        if (id.indexSf2 < 0 || justCheckParentLevel)
            return true;
    }
    Soundfont *sf2Tmp = _soundfonts->getSoundfont(id.indexSf2);
    if (sf2Tmp == nullptr)
        return false;
    if (id.typeElement == elementSf2 || id.typeElement == elementRootSmpl ||
            id.typeElement == elementRootInst || id.typeElement == elementRootPrst)
        return true;

    // Second level: smpl, inst, prst
    if (id.typeElement == elementSmpl || id.typeElement == elementInst || id.typeElement == elementPrst)
    {
        if (id.indexElt < 0 || justCheckParentLevel)
            return true;
    }

    // Sample?
    if (id.typeElement == elementSmpl)
    {
        Smpl * smpl = sf2Tmp->getSample(id.indexElt);
        return smpl != nullptr && (!smpl->isHidden() || acceptHidden);
    }

    // Instrument or preset?
    InstPrst * instPrst = nullptr;
    if (id.typeElement == elementInst || id.typeElement == elementInstSmpl ||
            id.typeElement == elementInstMod || id.typeElement == elementInstSmplMod ||
            id.typeElement == elementInstGen || id.typeElement == elementInstSmplGen)
        instPrst = sf2Tmp->getInstrument(id.indexElt);
    else
        instPrst = sf2Tmp->getPreset(id.indexElt);
    if (instPrst == nullptr || (instPrst->isHidden() && !acceptHidden))
        return false;
    if (id.typeElement == elementInst || id.typeElement == elementPrst)
        return true;

    // Higher levels: in the global division or in local division
    if (id.typeElement == elementInstSmpl || id.typeElement == elementInstSmplMod || id.typeElement == elementInstSmplGen ||
            id.typeElement == elementPrstInst || id.typeElement == elementPrstInstMod || id.typeElement == elementPrstInstGen)
    {
        if (id.typeElement == elementInstSmpl || id.typeElement == elementPrstInst)
        {
            if (id.indexElt2 < 0 || justCheckParentLevel)
                return true;
        }
        Division * div = instPrst->getDivision(id.indexElt2);
        if (div == nullptr || (div->isHidden() && !acceptHidden))
            return false;
        if (id.typeElement == elementInstSmpl || id.typeElement == elementPrstInst)
            return true;

        // Last level: mod or gen
        if (id.indexMod < 0 || justCheckParentLevel)
            return true;

        // Check mods?
        if (id.typeElement == elementInstSmplMod || id.typeElement == elementPrstInstMod)
        {
            Modulator * mod = div->getMod(id.indexMod);
            return mod != nullptr && (!mod->isHidden() || acceptHidden);
        }

        // Gens: based on the count only
        return id.indexMod < div->getGens().count();
    }
    else if (id.typeElement == elementInstMod || id.typeElement == elementInstGen ||
             id.typeElement == elementPrstMod || id.typeElement == elementPrstGen)
    {
        if (id.indexMod < 0 || justCheckParentLevel)
            return true;

        // Check mods?
        if (id.typeElement == elementInstMod || id.typeElement == elementPrstMod)
        {
            Modulator * mod = instPrst->getGlobalDivision()->getMod(id.indexMod);
            return mod != nullptr && (!mod->isHidden() || acceptHidden);
        }

        // Gens: based on the count only
        return id.indexMod < instPrst->getGlobalDivision()->getGens().count();
    }

    // Not possible to be here
    return false;
}

void SoundfontManager::firstAvailablePresetBank(EltID id, int &nBank, int &nPreset)
{
    QMutexLocker locker(&_mutex);
    if (nBank != -1 && nPreset != -1)
    {
        // bank et preset par défaut disponibles ?
        if (isAvailable(id, nBank, nPreset))
            return;
    }
    nBank = -2;
    nPreset = -1;
    int nBankParcours = 0;
    int nPresetParcours = 0;
    do
    {
        if (isAvailable(id, nBankParcours, nPresetParcours))
        {
            nBank = nBankParcours;
            nPreset = nPresetParcours;
        }
        else
        {
            nPresetParcours++;
            if (nPresetParcours > 127)
            {
                nPresetParcours = 0;
                nBankParcours++;
                if (nBankParcours > 127)
                    nBank = -1;
            }
        }
    }
    while (nBank == -2);
}

int SoundfontManager::closestAvailablePreset(EltID id, quint16 wBank, quint16 wPreset)
{
    QMutexLocker locker(&_mutex);
    int initVal = wPreset;
    int delta = 0;
    int sens = 0;
    do
    {
        if (initVal + delta < 128)
        {
            if (isAvailable(id, wBank, initVal + delta))
                sens = 1;
        }
        if (initVal - delta > -1 && sens == 0)
        {
            if (isAvailable(id, wBank, initVal - delta))
                sens = -1;
        }
        delta++;
    } while (sens == 0 && delta < 128);
    if (sens == 1 || sens == -1)
        return initVal + sens * (delta-1);
    else
        return -1;
}

bool SoundfontManager::isAvailable(EltID id, quint16 wBank, quint16 wPreset)
{
    QMutexLocker locker(&_mutex);
    id.typeElement = elementPrst;
    foreach (int i, this->getSiblings(id))
    {
        id.indexElt = i;
        if (this->get(id, champ_wBank).wValue == wBank &&
                this->get(id, champ_wPreset).wValue == wPreset)
            return false;
    }
    return true;
}
