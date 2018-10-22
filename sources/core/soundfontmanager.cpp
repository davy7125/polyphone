/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2017 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://polyphone-soundfonts.com                      **
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

SoundfontManager * SoundfontManager::s_instance = NULL;
SoundfontManager * SoundfontManager::getInstance()
{
    if (s_instance == NULL)
        s_instance = new SoundfontManager();
    return s_instance;
}
void SoundfontManager::kill()
{
    if (s_instance != NULL)
    {
        delete s_instance;
        s_instance = NULL;
    }
}

QAbstractItemModel * SoundfontManager::getModel(int indexSf2)
{
    return _soundfonts->getModel(indexSf2);
}

SoundfontManager::SoundfontManager() :
    _soundfonts(new Soundfonts()),
    _undoRedo(new ActionManager())
{
    connect(_undoRedo, SIGNAL(dropId(EltID)), this, SLOT(onDropId(EltID)));
}

SoundfontManager::~SoundfontManager()
{
    delete _undoRedo;
    delete _soundfonts;
}

///////////////////////// METHODES PUBLIQUES /////////////////////////

// Ajout / suppression des données
void SoundfontManager::remove(EltID id, int *message)
{
    this->remove(id, false, true, message);
}

void SoundfontManager::onDropId(EltID id)
{
    this->remove(id, true, false);
}

// Accès / modification des propriétés
bool SoundfontManager::isSet(EltID id, AttributeType champ)
{
    bool value = false;
    if (!this->isValid(id))
        return value;

    // Type d'élément à analyser
    switch ((int)id.typeElement)
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
        switch ((int)champ)
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
        value = true; break;
        break;
    case elementPrstMod:
        // Analyse d'un mod d'un preset
        value = true; break;
        break;
    case elementInstSmplMod:
        // Analyse d'un mod d'un sample lié à un instrument
        value = true; break;
        break;
    case elementPrstInstMod:
        // Analyse d'un mod d'un instrument lié à un preset
        value = true; break;
        break;
    }
    return value;
}

AttributeValue SoundfontManager::get(EltID id, AttributeType champ)
{
    AttributeValue value;
    value.dwValue = 0;
    if (!this->isValid(id))
        return value;

    // Type d'élément à analyser
    switch ((int)id.typeElement)
    {
    case elementSf2:{
        // Analyse d'un SF2
        Soundfont *tmp = _soundfonts->getSoundfont(id.indexSf2);
        switch ((int)champ)
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
        }
    }break;
    case elementSmpl:{
        // Analyse d'un sample
        Smpl *tmp = _soundfonts->getSoundfont(id.indexSf2)->getSample(id.indexElt);
        switch ((int)champ)
        {
        case champ_dwStart16:
            value.dwValue = tmp->_sound.get(champ_dwStart16); break;
        case champ_dwStart24:
            value.dwValue = tmp->_sound.get(champ_dwStart24); break;
        case champ_dwLength:
            value.dwValue = tmp->_sound.get(champ_dwLength); break;
        case champ_dwStartLoop:
            value.dwValue = tmp->_sound.get(champ_dwStartLoop); break;
        case champ_dwEndLoop:
            value.dwValue = tmp->_sound.get(champ_dwEndLoop); break;
        case champ_dwSampleRate:
            value.dwValue = tmp->_sound.get(champ_dwSampleRate); break;
        case champ_bpsFile:
            value.dwValue = tmp->_sound.get(champ_bpsFile); break;
        case champ_wChannel:
            value.dwValue = tmp->_sound.get(champ_wChannel); break;
        case champ_wChannels:
            value.dwValue = tmp->_sound.get(champ_wChannels); break;
        case champ_byOriginalPitch:
            value.bValue = tmp->_sound.get(champ_byOriginalPitch); break;
        case champ_chPitchCorrection:
            value.cValue = tmp->_sound.get(champ_chPitchCorrection); break;
        case champ_wSampleLink:
            value.wValue = tmp->_wSampleLink; break;
        case champ_sfSampleType:
            value.sfLinkValue = tmp->_sfSampleType; break;
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
        switch ((int)champ)
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
        value = tmp->getGen(champ);
    }break;
    case elementPrstInst:{
        // Analyse d'un instrument lié à un preset
        Division *tmp = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getDivision(id.indexElt2);
        value = tmp->getGen(champ);
    }break;
    case elementInstMod: case elementPrstMod: case elementInstSmplMod: case elementPrstInstMod:{
        // Analyse d'un mod
        Modulator *tmp = NULL;
        switch ((int)id.typeElement)
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
        }
        switch ((int)champ)
        {
        case champ_sfModSrcOper:
            value.sfModValue = tmp->_sfModSrcOper; break;
        case champ_sfModDestOper:
            value.sfGenValue = tmp->_sfModDestOper; break;
        case champ_modAmount:
            value.shValue = tmp->_modAmount; break;
        case champ_sfModAmtSrcOper:
            value.sfModValue = tmp->_sfModAmtSrcOper; break;
        case champ_sfModTransOper:
            value.sfTransValue = tmp->_sfModTransOper; break;
        case champ_indexMod:
            value.wValue = tmp->_index; break;
        }
    }break;
    case elementInstGen: case elementPrstGen: case elementInstSmplGen: case elementPrstInstGen:{
        // Analyse d'un gen
        Division * division = NULL;
        switch ((int)id.typeElement)
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
        }
    }break;
    }
    return value;
}

Sound SoundfontManager::getSon(EltID id)
{
    Sound son;
    if (!this->isValid(id))
        return son;

    if (id.typeElement == elementSmpl)
        return _soundfonts->getSoundfont(id.indexSf2)->getSample(id.indexElt)->_sound;
    else return son;
}

QString SoundfontManager::getQstr(EltID id, AttributeType champ)
{
    if (!this->isValid(id))
        return "";

    // Type d'élément à analyser
    switch ((int)id.typeElement)
    {
    case elementSf2:{
        // Analyse d'un SF2
        Soundfont *tmp = _soundfonts->getSoundfont(id.indexSf2);
        switch ((int)champ)
        {
        case champ_name:
            return tmp->_INAM; break;
        case champ_ISNG:
            return tmp->_ISNG; break;
        case champ_IROM:
            return tmp->_IROM; break;
        case champ_ICRD:
            return tmp->_ICRD; break;
        case champ_IENG:
            return tmp->_IENG; break;
        case champ_IPRD:
            return tmp->_IPRD; break;
        case champ_ICOP:
            return tmp->_ICOP; break;
        case champ_ICMT:
            return tmp->_ICMT; break;
        case champ_ISFT:
            return tmp->_ISFT; break;
        case champ_filenameInitial:
            return tmp->_fileNameInitial; break;
        case champ_filenameForData:
            return tmp->_fileNameForData; break;
        }
    }break;
    case elementSmpl:{
        // Analyse d'un sample
        Smpl *tmp = _soundfonts->getSoundfont(id.indexSf2)->getSample(id.indexElt);
        switch ((int)champ)
        {
        case champ_name:
            return tmp->getName(); break;
        case champ_filenameForData:
            return tmp->_sound.getFileName(); break;
        }
    }break;
    case elementInst:{
        // Analyse d'un instrument
        InstPrst *tmp = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt);
        switch ((int)champ)
        {
        case champ_name:
            return tmp->getName(); break;
        }
    }break;
    case elementPrst:{
        // Analyse d'un preset
        InstPrst *tmp = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt);
        switch ((int)champ)
        {
        case champ_name:
            return tmp->getName(); break;
        }
    }break;
    }
    return "";
}

QByteArray SoundfontManager::getData(EltID id, AttributeType champ)
{
    if (!this->isValid(id))
        return QByteArray();

    // Type d'élément à analyser
    switch ((int)id.typeElement)
    {
    case elementSmpl:{
        Smpl *tmp = _soundfonts->getSoundfont(id.indexSf2)->getSample(id.indexElt);
        switch ((int)champ)
        {
        case champ_sampleData16:
            return tmp->_sound.getData(16);
            break;
        case champ_sampleData24:
            return tmp->_sound.getData(8);
            break;
        case champ_sampleDataFull24:
            return tmp->_sound.getData(24);
            break;
        case champ_sampleData32:
            return tmp->_sound.getData(32);
            break;
        }
    }break;
    }
    QByteArray baRet;
    return baRet;
}

// Freres de id (id compris)
QList<int> SoundfontManager::getSiblings(EltID id)
{
    switch ((int)id.typeElement)
    {
    case elementSf2:
        id.indexSf2 = -1;
    case elementSmpl: case elementInst: case elementPrst:
        id.indexElt = -1;
    case elementInstSmpl: case elementPrstInst:
    case elementInstMod: case elementPrstMod: case elementInstGen: case elementPrstGen:
        id.indexElt2 = -1;
    case elementInstSmplMod: case elementPrstInstMod: case elementInstSmplGen: case elementPrstInstGen:
        id.indexMod = -1;
    }
    if (!this->isValid(id, true))
        return QList<int>();

    QList<int> result;
    switch ((int)id.typeElement)
    {
    case elementSf2: result = _soundfonts->getSoundfonts().keys(); break;
    case elementSmpl:
        foreach (Smpl * elt, _soundfonts->getSoundfont(id.indexSf2)->getSamples().values())
            if (!elt->isHidden())
                result << elt->getId().indexElt;
        break;
    case elementInst:
        foreach (InstPrst * elt, _soundfonts->getSoundfont(id.indexSf2)->getInstruments().values())
            if (!elt->isHidden())
                result << elt->getId().indexElt;
        break;
    case elementPrst:
        foreach (InstPrst * elt, _soundfonts->getSoundfont(id.indexSf2)->getPresets().values())
            if (!elt->isHidden())
                result << elt->getId().indexElt;
        break;
    case elementInstSmpl:
        foreach (Division * elt, _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getDivisions().values())
            if (!elt->isHidden())
                result << elt->getId().indexElt2;
        break;
    case elementPrstInst:
        foreach (Division * elt, _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getDivisions().values())
            if (!elt->isHidden())
                result << elt->getId().indexElt2;
        break;
    case elementInstMod:
        foreach (Modulator * elt, _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getGlobalDivision()->getMods().values())
            if (!elt->isHidden())
                result << elt->_id;
        break;
    case elementPrstMod:
        foreach (Modulator * elt, _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getGlobalDivision()->getMods().values())
            if (!elt->isHidden())
                result << elt->_id;
        break;
    case elementInstSmplMod:
        foreach (Modulator * elt, _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getDivision(id.indexElt2)->getMods().values())
            if (!elt->isHidden())
                result << elt->_id;
        break;
    case elementPrstInstMod:
        foreach (Modulator * elt, _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getDivision(id.indexElt2)->getMods().values())
            if (!elt->isHidden())
                result << elt->_id;
        break;
    case elementInstGen:
        foreach (int key, _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getGlobalDivision()->getGens().keys())
            result << key;
        break;
    case elementPrstGen:
        foreach (int key, _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getGlobalDivision()->getGens().keys())
            result << key;
        break;
    case elementInstSmplGen:
        foreach (int key, _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getDivision(id.indexElt2)->getGens().keys())
            result << key;
        break;
    case elementPrstInstGen:
        foreach (int key, _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getDivision(id.indexElt2)->getGens().keys())
            result << key;
        break;
    }

    return result;
}

/// ACTION MANAGER ///

void SoundfontManager::endEditing(QString editingSource)
{
    // Close the action set and get the list of sf2 that have been edited
    QList<int> sf2Indexes = _undoRedo->commitActionSet();
    if (!sf2Indexes.empty())
        emit(editingDone(editingSource, sf2Indexes));
}

void SoundfontManager::clearNewEditing()
{
    _undoRedo->clearCurrentActionSet();
}

void SoundfontManager::revertNewEditing()
{
    undo(_undoRedo->clearCurrentActionSet());
}

bool SoundfontManager::isUndoable(int indexSf2)
{
    return _undoRedo->isUndoable(indexSf2);
}

bool SoundfontManager::isRedoable(int indexSf2)
{
    return _undoRedo->isRedoable(indexSf2);
}

void SoundfontManager::undo(int indexSf2)
{
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
    _soundfonts->getSoundfont(indexSf2)->_numEdition = this->_undoRedo->getEdition(indexSf2);
}

bool SoundfontManager::isEdited(int indexSf2)
{
    return this->_undoRedo->getEdition(indexSf2) != _soundfonts->getSoundfont(indexSf2)->_numEdition ||
            !this->getQstr(EltID(elementSf2, indexSf2), champ_filenameInitial).endsWith(".sf2");
}

// Récupération liste de champs et valeurs de bags
void SoundfontManager::getListeBags(EltID id, QList<AttributeType> &listeChamps, QList<AttributeValue> &listeValeurs)
{
    if (!this->isValid(id))
        return;

    Division * division = NULL;
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
        break;
    }

    listeChamps = division->getGens().keys();
    listeValeurs = division->getGens().values();
}

///////////////////////// METHODES PRIVEES /////////////////////////

// Ajoute un enfant à id
int SoundfontManager::add(EltID id)
{
    if (id.typeElement == elementSf2)
        id.indexSf2 = -1;
    else if (id.typeElement == elementSmpl || id.typeElement == elementInst || id.typeElement == elementPrst)
        id.indexElt = -1;
    else if (id.typeElement == elementInstSmpl || id.typeElement == elementPrstInst)
        id.indexElt2 = -1;
    else id.indexMod = -1;
    if (!this->isValid(id))
        return -1;

    int i = -1;

    switch ((int)id.typeElement)
    {
    case elementSf2:
        // Create a new SF2
        i = id.indexSf2 = _soundfonts->addSoundfont();

        // Initialisation bps
    {
        AttributeValue valTmp;
        valTmp.wValue = 16;
        this->set(id, champ_wBpsInit, valTmp);
        this->set(id, champ_wBpsSave, valTmp);
    }
        break;
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
        Division *bag = NULL;
        switch ((int)id.typeElement)
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
        }
        i = id.indexMod = bag->addMod();

        // Fill the index
        AttributeValue val;
        val.wValue = id.indexMod;
        this->set(id, champ_indexMod, val);
    }break;
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

    switch ((int)id.typeElement)
    {
    case elementSf2:{
        permanently = true; // No undo possible after a file is closed
        storeAction = false;
        // suppression d'un SF2

        // Propagation aux presets /!\ si temporaire, ne pas propager aux éléments déjà supprimés de manière temporaire
        EltID id2(elementPrst, id.indexSf2, 0, 0, 0);
        QList<int> indexes = _soundfonts->getSoundfont(id.indexSf2)->getPresets().keys();
        foreach (int i, indexes)
        {
            id2.indexElt = i;
            this->remove(id2, true, storeAction, message);
        }

        // propagation aux instruments
        id2.typeElement = elementInst;
        indexes = _soundfonts->getSoundfont(id.indexSf2)->getInstruments().keys();
        foreach (int i, indexes)
        {
            id2.indexElt = i;
            this->remove(id2, true, storeAction, message);
        }

        // propagation aux samples
        id2.typeElement = elementSmpl;
        indexes = _soundfonts->getSoundfont(id.indexSf2)->getSamples().keys();
        foreach (int i, indexes)
        {
            id2.indexElt = i;
            this->remove(id2, true, storeAction, message);
        }

        // Suppression du sf2
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
                        if (message != NULL && (*message) % 2 != 0)
                            *message *= 2;
                        return 1;
                    }
                }
            }
        }

        // Linked sample?
        EltID id2(elementSmpl, id.indexSf2, 0, 0, 0);
        AttributeValue value;
        foreach (int i, _soundfonts->getSoundfont(id.indexSf2)->getSamples().keys())
        {
            Smpl * smplTmp = _soundfonts->getSoundfont(id.indexSf2)->getSample(i);
            if (!smplTmp->isHidden())
            {
                if (smplTmp->_wSampleLink == id.indexElt)
                {
                    id2.indexElt = i;
                    value.wValue = 0;
                    this->set(id2, champ_wSampleLink, value);
                    value.sfLinkValue = monoSample;
                    this->set(id2, champ_sfSampleType, value);
                }
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
                        if (message != NULL && (*message) % 3 != 0)
                            *message *= 3;
                        return 1;
                    }
                }
            }
        }

        // Propagation aux samples liés
        EltID id2(elementInstSmpl, id.indexSf2, id.indexElt, 0, 0);
        QMap<int, Division*> divisions = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getDivisions();
        foreach (int i, divisions.keys())
        {
            id2.indexElt2 = i;
            if (permanently || !divisions[i]->isHidden())
                this->remove(id2, permanently, storeAction, message);
        }

        // Propagation aux mods associés
        id2.typeElement = elementInstMod;
        QMap<int, Modulator*> mods = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getGlobalDivision()->getMods();
        foreach (int i, mods.keys())
        {
            id2.indexMod = i;
            if (permanently || !mods[i]->isHidden())
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
        EltID id2(elementPrstInst, id.indexSf2, id.indexElt, 0, 0);
        QMap<int, Division*> divisions = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getDivisions();
        foreach (int i, divisions.keys())
        {
            id2.indexElt2 = i;
            if (permanently || !divisions[i]->isHidden())
                this->remove(id2, permanently, storeAction, message);
        }

        // Propagation aux mods associés
        id2.typeElement = elementPrstMod;
        QMap<int, Modulator*> mods = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getGlobalDivision()->getMods();
        foreach (int i, mods.keys())
        {
            id2.indexMod = i;
            if (permanently || !mods[i]->isHidden())
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
        // suppression d'un sample lié à un instrument
        EltID id2(elementInstSmplMod, id.indexSf2, id.indexElt, id.indexElt2, 0);

        // Propagation aux mods associés
        QMap<int, Modulator*> mods = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getDivision(id.indexElt2)->getMods();
        foreach (int i, mods.keys())
        {
            id2.indexMod = i;
            if (permanently || !mods[i]->isHidden())
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
        QMap<int, Modulator*> mods = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getDivision(id.indexElt2)->getMods();
        foreach (int i, mods.keys())
        {
            id2.indexMod = i;
            if (permanently || !mods[i]->isHidden())
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
        Division *bag = NULL;
        switch ((int)id.typeElement)
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
        }

        // If the destination of a modulator is the modulator to delete, remove the link
        foreach (int indexMod, bag->getMods().keys())
        {
            Modulator * mod = bag->getMod(indexMod);
            if ((int)mod->_sfModDestOper - 32768 == id.indexMod)
            {
                EltID id2 = id;
                id2.indexMod = indexMod;
                AttributeValue value;
                value.dwValue = 0;
                this->set(id2, champ_sfModDestOper, value);
            }
        }

        // If the src of a modulator is the modulator to delete, remove the link
        int iVal = bag->getMod(id.indexMod)->_sfModDestOper;
        if (iVal >= 32768 && bag->getMod(iVal - 32768) != NULL)
        {
            Modulator * mod2 = bag->getMod(iVal - 32768);
            if (mod2->_sfModSrcOper.CC == 0 && mod2->_sfModSrcOper.Index == 127)
            {
                EltID id2 = id;
                id2.indexMod = iVal - 32768;
                AttributeValue value;
                value.dwValue = 0;
                this->set(id2, champ_sfModSrcOper, value);
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
    Action *action = new Action();
    action->typeAction = Action::TypeRemoval;
    action->id = id;
    this->_undoRedo->add(action);

    return 0;
}

void SoundfontManager::supprGenAndStore(EltID id, int storeAction)
{
    Division *division = NULL;
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
    if (!this->isValid(id))
        return 1;

    AttributeValue oldValue;
    oldValue.wValue = 0;
    int defaultValue = 0;

    // Type d'élément à modifier
    switch ((int)id.typeElement)
    {
    case elementSf2:{
        // Modification d'un SF2
        Soundfont *tmp = _soundfonts->getSoundfont(id.indexSf2);
        switch ((int)champ)
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
        }
    }break;
    case elementSmpl:{
        // Modification d'un sample
        Smpl *tmp = _soundfonts->getSoundfont(id.indexSf2)->getSample(id.indexElt);
        switch ((int)champ)
        {
        case champ_dwStart16:
            oldValue.dwValue = tmp->_sound.get(champ_dwStart16);
            tmp->_sound.set(champ_dwStart16, value); break;
        case champ_dwStart24:
            oldValue.dwValue = tmp->_sound.get(champ_dwStart24);
            tmp->_sound.set(champ_dwStart24, value); break;
        case champ_dwLength:
            oldValue.dwValue = tmp->_sound.get(champ_dwLength);
            tmp->_sound.set(champ_dwLength, value); break;
        case champ_dwStartLoop:
            oldValue.dwValue = tmp->_sound.get(champ_dwStartLoop);
            tmp->_sound.set(champ_dwStartLoop, value); break;
        case champ_dwEndLoop:
            oldValue.dwValue = tmp->_sound.get(champ_dwEndLoop);
            tmp->_sound.set(champ_dwEndLoop, value); break;
        case champ_dwSampleRate:
            oldValue.dwValue = tmp->_sound.get(champ_dwSampleRate);
            tmp->_sound.set(champ_dwSampleRate, value); break;
        case champ_byOriginalPitch:
            oldValue.bValue = tmp->_sound.get(champ_byOriginalPitch);
            tmp->_sound.set(champ_byOriginalPitch, value); break;
        case champ_chPitchCorrection:
            oldValue.cValue = tmp->_sound.get(champ_chPitchCorrection);
            tmp->_sound.set(champ_chPitchCorrection, value); break;
        case champ_wSampleLink:
            oldValue.wValue = tmp->_wSampleLink;
            tmp->_wSampleLink = value.wValue; break;
        case champ_sfSampleType:
            oldValue.sfLinkValue = tmp->_sfSampleType;
            tmp->_sfSampleType = value.sfLinkValue; break;
        case champ_bpsFile:
            oldValue.wValue = tmp->_sound.get(champ_bpsFile);
            tmp->_sound.set(champ_bpsFile, value); break;
        case champ_wChannel:
            oldValue.wValue = tmp->_sound.get(champ_wChannel);
            tmp->_sound.set(champ_wChannel, value); break;
        case champ_wChannels:
            oldValue.wValue = tmp->_sound.get(champ_wChannels);
            tmp->_sound.set(champ_wChannels, value); break;
        }
    }break;
    case elementInst:{
        // Update an instrument
        InstPrst *tmp = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt);
        if (!tmp->getGlobalDivision()->isSet(champ)) defaultValue = 1;
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
        if (!tmp->isSet(champ)) defaultValue = 1;
        oldValue = tmp->getGen(champ);
        tmp->setGen(champ, value);
    }break;
    case elementPrstInst:{
        // Modification of an instrument linked to a preset
        Division *tmp = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getDivision(id.indexElt2);
        if (!tmp->isSet(champ)) defaultValue = 1;
        oldValue = tmp->getGen(champ);
        tmp->setGen(champ, value);
    }break;
    case elementInstMod: case elementPrstMod: case elementInstSmplMod: case elementPrstInstMod:{
        // Modification d'un mod d'un instrument
        Modulator *tmp = NULL;
        switch ((int)id.typeElement)
        {
        case elementInstMod:
            tmp = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getGlobalDivision()->getMod(id.indexMod); break;
        case elementPrstMod:
            tmp = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getGlobalDivision()->getMod(id.indexMod); break;
        case elementInstSmplMod:
            tmp = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getDivision(id.indexElt2)->getMod(id.indexMod); break;
        case elementPrstInstMod:
            tmp = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getDivision(id.indexElt2)->getMod(id.indexMod); break;
        }

        switch (champ)
        {
        case champ_sfModSrcOper:
            oldValue.sfModValue = tmp->_sfModSrcOper;
            tmp->_sfModSrcOper = value.sfModValue; break;
        case champ_sfModDestOper:
            oldValue.sfGenValue = tmp->_sfModDestOper;
            tmp->_sfModDestOper = value.sfGenValue; break;
        case champ_modAmount:
            oldValue.shValue = tmp->_modAmount;
            tmp->_modAmount = value.shValue; break;
        case champ_sfModAmtSrcOper:
            oldValue.sfModValue = tmp->_sfModAmtSrcOper;
            tmp->_sfModAmtSrcOper = value.sfModValue; break;
        case champ_sfModTransOper:
            oldValue.sfTransValue = tmp->_sfModTransOper;
            tmp->_sfModTransOper = value.sfTransValue; break;
        case champ_indexMod:
            oldValue.wValue = tmp->_index;
            tmp->_index = value.wValue; break;
        default:
            break;
        }
    }break;
    }

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

    return 0;
}

int SoundfontManager::set(EltID id, AttributeType champ, QString qStr)
{
    if (!this->isValid(id))
        return 1;

    QString qOldStr = "";

    // Type of element to edit
    switch ((int)id.typeElement)
    {
    case elementSf2:{
        // Editing of a sf2
        Soundfont *tmp = _soundfonts->getSoundfont(id.indexSf2);
        switch ((int)champ)
        {
        case champ_name:
            qOldStr = tmp->_INAM;
            qStr = qStr.left(256);
            tmp->_INAM = qStr;
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
        }
    }break;
    case elementSmpl:{
        // Editing of a sample
        qStr = qStr.trimmed();
        Smpl *tmp = _soundfonts->getSoundfont(id.indexSf2)->getSample(id.indexElt);
        switch ((int)champ)
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
        }
    }break;
    case elementInst:{
        qStr = qStr.trimmed();
        // Modification d'un instrument
        InstPrst *tmp = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt);
        switch ((int)champ)
        {
        case champ_name:
            // Modification du nom d'un instrument
            qOldStr = tmp->getName();
            qStr = qStr.left(20);
            tmp->setName(qStr);
            break;
        }
    }break;
    case elementPrst:{
        qStr = qStr.trimmed();
        // Modification d'un preset
        InstPrst *tmp = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt);
        switch ((int)champ)
        {
        case champ_name:
            // Modification du nom d'un preset
            qOldStr = tmp->getName();
            qStr = qStr.left(20);
            tmp->setName(qStr);
            break;
        }
    }break;
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
    if (!this->isValid(id))
        return 1;

    QByteArray oldData;
    oldData.clear();
    // Type d'élément à modifier
    switch ((int)id.typeElement)
    {
    case elementSmpl:
        // modification d'un sample
        switch ((int)champ)
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
            QByteArray baData = Sound::bpsConversion(data, 24, 16);
            this->set(id, champ_sampleData16, baData);
            // Modification 8 bits de poids faibles
            baData = Sound::bpsConversion(data, 24, 824);
            this->set(id, champ_sampleData24, baData);
        }break;
        case champ_sampleData32:{
            // Modification 16 bits
            QByteArray baData = Sound::bpsConversion(data, 32, 16);
            this->set(id, champ_sampleData16, baData);
            // Modification 8 bits de poids faibles
            baData = Sound::bpsConversion(data, 32, 824);
            this->set(id, champ_sampleData24, baData);
        }break;
        }
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

int SoundfontManager::reset(EltID id, AttributeType champ)
{
    if (!this->isValid(id))
        return 0;

    AttributeValue oldValue;
    // Type d'élément à modifier
    switch ((int)id.typeElement)
    {
    case elementInst:{
        // Remise à zéro d'une propriété d'un instrument
        InstPrst *tmp = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt);
        if (!tmp->getGlobalDivision()->isSet(champ)) return 0;
        oldValue = tmp->getGlobalDivision()->getGen(champ);
        tmp->getGlobalDivision()->resetGen(champ);
    }break;
    case elementPrst:{
        // Remise à zéro d'une propriété d'un preset
        InstPrst *tmp = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt);
        if (!tmp->getGlobalDivision()->isSet(champ)) return 0;
        oldValue = tmp->getGlobalDivision()->getGen(champ);
        tmp->getGlobalDivision()->resetGen(champ);
    }break;
    case elementInstSmpl:{
        // Remise à zéro d'une propriété d'un sample lié à un instrument
        Division *tmp = _soundfonts->getSoundfont(id.indexSf2)->getInstrument(id.indexElt)->getDivision(id.indexElt2);
        if (!tmp->isSet(champ))
            return 0;
        oldValue = tmp->getGen(champ);
        tmp->resetGen(champ);
    }break;
    case elementPrstInst:{
        // Remise à zéro d'une propriété d'un instrument lié à un preset
        Division *tmp = _soundfonts->getSoundfont(id.indexSf2)->getPreset(id.indexElt)->getDivision(id.indexElt2);
        if (!tmp->isSet(champ)) return 0;
        oldValue = tmp->getGen(champ);
        tmp->resetGen(champ);
    }break;
    }

    // Create and store the action
    Action *action = new Action();
    action->typeAction = Action::TypeChangeToDefault;
    action->id = id;
    action->champ = champ;
    action->vOldValue = oldValue;
    this->_undoRedo->add(action);

    return 1;
}

void SoundfontManager::simplify(EltID id, AttributeType champ)
{
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
    else return;

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
        else return;
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
    switch ((int)id.typeElement)
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
    }
    return 0;
}

bool SoundfontManager::isValid(EltID id, bool acceptHidden)
{
    Soundfont *sf2Tmp;
    if (id.typeElement < elementSf2 || id.typeElement > elementPrstInstGen)
        return false;

    if (id.indexSf2 < 0)
        return (id.typeElement == elementSf2 || id.typeElement == elementRootSmpl ||
                id.typeElement == elementRootInst || id.typeElement == elementRootPrst);

    // Check indexSf2
    if (_soundfonts->getSoundfont(id.indexSf2) == NULL)
        return false;
    sf2Tmp = _soundfonts->getSoundfont(id.indexSf2);

    if (id.indexElt < 0 && id.typeElement != elementSf2 && id.typeElement != elementRootInst &&
            id.typeElement != elementRootSmpl && id.typeElement != elementRootPrst)
        return (id.typeElement == elementSmpl || id.typeElement == elementInst || id.typeElement == elementPrst);

    if (id.typeElement == elementSmpl)
    {
        // Check indexElt
        if (sf2Tmp->getSample(id.indexElt) == NULL)
            return false;

        if (sf2Tmp->getSample(id.indexElt)->isHidden() && !acceptHidden)
            return false;
    }
    else if (id.typeElement == elementInst || id.typeElement == elementInstSmpl ||
             id.typeElement == elementInstMod || id.typeElement == elementInstSmplMod ||
             id.typeElement == elementInstGen || id.typeElement == elementInstSmplGen)
    {
        // Check indexElt
        if (sf2Tmp->getInstrument(id.indexElt) == NULL)
            return false;

        InstPrst *inst = sf2Tmp->getInstrument(id.indexElt);
        if (inst->isHidden() && !acceptHidden)
            return false;

        if (id.typeElement == elementInstSmpl || id.typeElement == elementInstSmplMod || id.typeElement == elementInstSmplGen)
        {
            if (id.indexElt2 < 0 && id.typeElement != elementInstSmplMod && id.typeElement != elementInstSmplGen)
                return (id.typeElement == elementInstSmpl);

            // Vérification qu'indexElt2 est correct
            if (inst->getDivision(id.indexElt2) == NULL)
                return false;
            Division * bag = inst->getDivision(id.indexElt2);

            if (bag->isHidden() && !acceptHidden)
                return false;

            if (id.typeElement == elementInstSmplMod || id.typeElement == elementInstSmplGen)
            {
                if (id.indexMod < 0)
                    return true;

                // Check indexMod
                if (id.typeElement == elementInstSmplMod)
                {
                    if (bag->getMod(id.indexMod) == NULL)
                        return false;

                    if (bag->getMod(id.indexMod)->isHidden() && !acceptHidden)
                        return false;
                }
                else
                {
                    if (id.indexMod >= bag->getGens().count())
                        return false;
                }
            }
        }
        else if (id.typeElement == elementInstMod || id.typeElement == elementInstGen)
        {
            if (id.indexMod < 0)
                return true;

            // Check indexMod
            if (id.typeElement == elementInstMod)
            {
                if (inst->getGlobalDivision()->getMod(id.indexMod) == NULL)
                    return false;

                if (inst->getGlobalDivision()->getMod(id.indexMod)->isHidden() && !acceptHidden)
                    return false;
            }
            else
            {
                if (id.indexMod >= inst->getGlobalDivision()->getGens().count())
                    return false;
            }
        }
    }
    else if (id.typeElement == elementPrst || id.typeElement == elementPrstInst ||
             id.typeElement == elementPrstMod || id.typeElement == elementPrstInstMod ||
             id.typeElement == elementPrstGen || id.typeElement == elementPrstInstGen)
    {
        // Check indexElt
        if (sf2Tmp->getPreset(id.indexElt) == NULL)
            return false;

        InstPrst *prst = sf2Tmp->getPreset(id.indexElt);
        if (prst->isHidden() && !acceptHidden)
            return false;

        if (id.typeElement == elementPrstInst || id.typeElement == elementPrstInstMod || id.typeElement == elementPrstInstGen)
        {
            if (id.indexElt2 < 0 && id.typeElement != elementPrstInstMod && id.typeElement != elementPrstInstGen)
                return (id.typeElement == elementPrstInst);

            // Vérification qu'indexElt2 est correct
            if (prst->getDivision(id.indexElt2) == NULL)
                return false;
            Division * bag = prst->getDivision(id.indexElt2);

            if (bag->isHidden() && !acceptHidden)
                return false;

            if (id.typeElement == elementPrstInstMod || id.typeElement == elementPrstInstGen)
            {
                if (id.indexMod < 0)
                    return true;

                // Check indexMod
                if (id.typeElement == elementPrstInstMod)
                {
                    if (bag->getMod(id.indexMod) == NULL)
                        return false;

                    if (bag->getMod(id.indexMod)->isHidden() && !acceptHidden)
                        return false;
                }
                else
                {
                    if (id.indexMod >= bag->getGens().count())
                        return false;
                }
            }
        }
        else if (id.typeElement == elementPrstMod || id.typeElement == elementPrstGen)
        {
            if (id.indexMod < 0)
                return true;

            // Check indexMod
            if (id.typeElement == elementPrstMod)
            {
                if (prst->getGlobalDivision()->getMod(id.indexMod) == NULL)
                    return false;

                if (prst->getGlobalDivision()->getMod(id.indexMod)->isHidden() && !acceptHidden)
                    return false;
            }
            else
            {
                if (id.indexMod >= prst->getGlobalDivision()->getGens().count())
                    return false;
            }
        }
    }

    return true;
}

void SoundfontManager::firstAvailablePresetBank(EltID id, int &nBank, int &nPreset)
{
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
