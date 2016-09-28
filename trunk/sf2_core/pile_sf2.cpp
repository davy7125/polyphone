/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
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

#include "pile_sf2.h"
#include "tree.h"

#define UNDO_NUMBER 100

// CONSTRUCTEURS
Pile_sf2::Pile_sf2()
{
    this->_undoRedo = new ActionManager();
}

Pile_sf2::~Pile_sf2()
{
    while (!_sf2.isEmpty())
        delete _sf2.takeFirst();
    delete _undoRedo;
}

Pile_sf2::SF2::SF2()
{
    SfVersionTag sfVersionTmp;
    sfVersionTmp.wMajor = 2;
    sfVersionTmp.wMinor = 4;
    this->_IFIL = sfVersionTmp;
    sfVersionTmp.wMajor = 0;
    sfVersionTmp.wMinor = 0;
    this->_IVER = sfVersionTmp;
    this->_ISNG = "EMU8000";
    this->_INAM = "";
    this->_IROM = "";
    this->_ICRD = "";
    this->_IENG = "";
    this->_IPRD = "";
    this->_ICOP = "";
    this->_ICMT = "";
    this->_ISFT = "";
    this->_fileName = "";
    this->_hidden = 0;
    this->_numEdition = 0;
}

Pile_sf2::SF2::SMPL::SMPL() :
    _hidden(false)
{}

Pile_sf2::SF2::INST::INST() :
    _hidden(false)
{}

Pile_sf2::SF2::PRST::PRST() :
    _wPreset(0),
    _wBank(0),
    _dwLibrary(0),
    _dwGenre(0),
    _dwMorphology(0),
    _hidden(false)
{}

Pile_sf2::SF2::BAG::BAG() :
    _hidden(false)
{}

///////////////////////// METHODES PUBLIQUES /////////////////////////

// Ajout / suppression des données
void Pile_sf2::remove(EltID id, int *message) { this->remove(id, 0, 1, message); }

// Accès / modification des propriétés
bool Pile_sf2::isSet(EltID id, Champ champ)
{
    bool value = false;
    if (!this->isValid(id, champ == champ_hidden))
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
    case elementInst:{
        // Analyse d'un instrument
        SF2::INST *tmp = _sf2[id.indexSf2]->_inst[id.indexElt];
        switch ((int)champ)
        {
        case champ_hidden:
            value = true; break;
        default:
            value = tmp->_bagGlobal.isSet(champ);
        }
    }break;
    case elementPrst:{
        // Analyse d'un preset
        SF2::PRST *tmp = _sf2[id.indexSf2]->_prst[id.indexElt];
        switch ((int)champ)
        {
        case champ_wPreset:
            value = true; break;
        case champ_wBank:
            value = true; break;
        case champ_dwLibrary:
            value = true; break;
        case champ_dwGenre:
            value = true; break;
        case champ_dwMorphology:
            value = true; break;
        case champ_hidden:
            value = true; break;
        default:
            value = tmp->_bagGlobal.isSet(champ);
        }
    }break;
    case elementInstSmpl:{
        // Analyse d'un sample lié à un instrument
        SF2::BAG *tmp = _sf2[id.indexSf2]->_inst[id.indexElt]->_bags[id.indexElt2];
        switch ((int)champ)
        {
        case champ_hidden:
            value = true; break;
        default:
            value = tmp->isSet(champ);
        }
    }break;
    case elementPrstInst:{
        // Analyse d'un instrument lié à un preset
        SF2::BAG *tmp = _sf2[id.indexSf2]->_prst[id.indexElt]->_bags[id.indexElt2];
        switch ((int)champ)
        {
        case champ_hidden:
            value = true; break;
        default:
            value = tmp->isSet(champ);
        }
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

Valeur Pile_sf2::get(EltID id, Champ champ)
{
    Valeur value;
    value.bValue = 0;
    if (!this->isValid(id, champ == champ_hidden))
        return value;

    // Type d'élément à analyser
    switch ((int)id.typeElement)
    {
    case elementSf2:{
        // Analyse d'un SF2
        SF2 *tmp = _sf2[id.indexSf2];
        switch ((int)champ)
        {
        case champ_IFIL:
            value.sfVerValue = tmp->_IFIL; break;
        case champ_IVER:
            value.sfVerValue = tmp->_IVER; break;
        case champ_hidden:
            value.bValue = tmp->_hidden; break;
        case champ_wBpsInit:
            value.wValue = tmp->_wBpsInit; break;
        case champ_wBpsSave:
            value.wValue = tmp->_wBpsSave; break;
        }
    }break;
    case elementSmpl:{
        // Analyse d'un sample
        SF2::SMPL *tmp = _sf2[id.indexSf2]->_smpl[id.indexElt];
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
        case champ_hidden:
            value.bValue = tmp->_hidden; break;
        }
    }break;
    case elementInst:{
        // Analyse d'un instrument
        SF2::INST *tmp = _sf2[id.indexSf2]->_inst[id.indexElt];
        switch ((int)champ)
        {
        case champ_hidden:
            value.bValue = tmp->_hidden; break;
        default:
            value = tmp->_bagGlobal.getGen(champ);
        }
    }break;
    case elementPrst:{
        // Analyse d'un preset
        SF2::PRST *tmp = _sf2[id.indexSf2]->_prst[id.indexElt];
        switch ((int)champ)
        {
        case champ_wPreset:
            value.wValue = tmp->_wPreset; break;
        case champ_wBank:
            value.wValue = tmp->_wBank; break;
        case champ_dwLibrary:
            value.dwValue = tmp->_dwLibrary; break;
        case champ_dwGenre:
            value.dwValue = tmp->_dwGenre; break;
        case champ_dwMorphology:
            value.dwValue = tmp->_dwMorphology; break;
        case champ_hidden:
            value.bValue = tmp->_hidden; break;
        default:
            value = tmp->_bagGlobal.getGen(champ);
        }
    }break;
    case elementInstSmpl:{
        // Analyse d'un sample lié à un instrument
        SF2::BAG *tmp = _sf2[id.indexSf2]->_inst[id.indexElt]->_bags[id.indexElt2];
        switch ((int)champ)
        {
        case champ_hidden:
            value.bValue = tmp->_hidden; break;
        default:
            value = tmp->getGen(champ);
        }
    }break;
    case elementPrstInst:{
        // Analyse d'un instrument lié à un preset
        SF2::BAG *tmp = _sf2[id.indexSf2]->_prst[id.indexElt]->_bags[id.indexElt2];
        switch ((int)champ)
        {
        case champ_hidden:
            value.bValue = tmp->_hidden; break;
        default:
            value = tmp->getGen(champ);
        }
    }break;
    case elementInstMod: case elementPrstMod: case elementInstSmplMod: case elementPrstInstMod:{
        // Analyse d'un mod
        SF2::BAG::MOD *tmp = NULL;
        switch ((int)id.typeElement)
        {
        case elementInstMod:
            tmp = &_sf2[id.indexSf2]->_inst[id.indexElt]->_bagGlobal._mods[id.indexMod];
            break;
        case elementPrstMod:
            tmp = &_sf2[id.indexSf2]->_prst[id.indexElt]->_bagGlobal._mods[id.indexMod];
            break;
        case elementInstSmplMod:
            tmp = &_sf2[id.indexSf2]->_inst[id.indexElt]->_bags[id.indexElt2]->_mods[id.indexMod];
            break;
        case elementPrstInstMod:
            tmp = &_sf2[id.indexSf2]->_prst[id.indexElt]->_bags[id.indexElt2]->_mods[id.indexMod];
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
        case champ_hidden:
            value.bValue = tmp->_hidden; break;
        }
    }break;
    case elementInstGen: case elementPrstGen: case elementInstSmplGen: case elementPrstInstGen:{
        // Analyse d'un gen
        SF2::BAG::GEN *tmp = NULL;
        switch ((int)id.typeElement)
        {
        case elementInstGen:
            tmp = &_sf2[id.indexSf2]->_inst[id.indexElt]->_bagGlobal._gens[id.indexMod];
            break;
        case elementPrstGen:
            tmp = &_sf2[id.indexSf2]->_prst[id.indexElt]->_bagGlobal._gens[id.indexMod];
            break;
        case elementInstSmplGen:
            tmp = &_sf2[id.indexSf2]->_inst[id.indexElt]->_bags[id.indexElt2]->_gens[id.indexMod];
            break;
        case elementPrstInstGen:
            tmp = &_sf2[id.indexSf2]->_prst[id.indexElt]->_bags[id.indexElt2]->_gens[id.indexMod];
            break;
        }
        switch ((int)champ)
        {
        case champ_sfGenOper:
            value.sfGenValue = tmp->sfGenOper; break;
        case champ_sfGenAmount:
            value.genValue = tmp->genAmount; break;
        }
    }break;
    }
    return value;
}

Sound Pile_sf2::getSon(EltID id)
{
    Sound son;
    if (!this->isValid(id, 0))
        return son;

    if (id.typeElement == elementSmpl)
        return _sf2[id.indexSf2]->_smpl[id.indexElt]->_sound;
    else return son;
}

QString Pile_sf2::getQstr(EltID id, Champ champ)
{
    if (!this->isValid(id, false))
        return "";

    // Type d'élément à analyser
    switch ((int)id.typeElement)
    {
    case elementSf2:{
        // Analyse d'un SF2
        SF2 *tmp = _sf2[id.indexSf2];
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
        case champ_filename:
            return tmp->_fileName; break;
        }
    }break;
    case elementSmpl:{
        // Analyse d'un sample
        SF2::SMPL *tmp = _sf2[id.indexSf2]->_smpl[id.indexElt];
        switch ((int)champ)
        {
        case champ_name:
            return tmp->_name; break;
        case champ_filename:
            return tmp->_sound.getFileName(); break;
        }
    }break;
    case elementInst:{
        // Analyse d'un instrument
        SF2::INST *tmp = _sf2[id.indexSf2]->_inst[id.indexElt];
        switch ((int)champ)
        {
        case champ_name:
            return tmp->_name; break;
        }
    }break;
    case elementPrst:{
        // Analyse d'un preset
        SF2::PRST *tmp = _sf2[id.indexSf2]->_prst[id.indexElt];
        switch ((int)champ)
        {
        case champ_name:
            return tmp->_name; break;
        }
    }break;
    }
    return "";
}

QByteArray Pile_sf2::getData(EltID id, Champ champ)
{
    if (!this->isValid(id, 0))
        return QByteArray();

    // Type d'élément à analyser
    switch ((int)id.typeElement)
    {
    case elementSmpl:{
        SF2::SMPL *tmp = _sf2[id.indexSf2]->_smpl[id.indexElt];
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

// Nombre de freres de id (id compris)
int Pile_sf2::count(EltID id, bool withHidden)
{
    switch ((int)id.typeElement)
    {
    case elementSf2:
        id.indexSf2 = -1;
    case elementSmpl: case elementInst: case elementPrst:
        id.indexElt = -1;
    case elementInstSmpl: case elementPrstInst:
        id.indexElt2 = -1;
    case elementInstMod: case elementPrstMod: case elementInstGen: case elementPrstGen:
        id.indexElt2 = -1;
    case elementInstSmplMod: case elementPrstInstMod: case elementInstSmplGen: case elementPrstInstGen:
        id.indexMod = -1;
    }
    if (!this->isValid(id, 1))
        return -1;

    int count = 0;

    if (withHidden)
    {
        switch ((int)id.typeElement)
        {
        case elementSf2:         count = _sf2.count(); break;
        case elementSmpl:        count = _sf2[id.indexSf2]->_smpl.count(); break;
        case elementInst:        count = _sf2[id.indexSf2]->_inst.count(); break;
        case elementPrst:        count = _sf2[id.indexSf2]->_prst.count(); break;
        case elementInstSmpl:    count = _sf2[id.indexSf2]->_inst[id.indexElt]->_bags.count(); break;
        case elementPrstInst:    count = _sf2[id.indexSf2]->_prst[id.indexElt]->_bags.count(); break;
        case elementInstMod:     count = _sf2[id.indexSf2]->_inst[id.indexElt]->_bagGlobal._mods.count(); break;
        case elementPrstMod:     count = _sf2[id.indexSf2]->_prst[id.indexElt]->_bagGlobal._mods.count(); break;
        case elementInstSmplMod: count = _sf2[id.indexSf2]->_inst[id.indexElt]->_bags[id.indexElt2]->_mods.count(); break;
        case elementPrstInstMod: count = _sf2[id.indexSf2]->_prst[id.indexElt]->_bags[id.indexElt2]->_mods.count(); break;
        case elementInstGen:     count = _sf2[id.indexSf2]->_inst[id.indexElt]->_bagGlobal._gens.count(); break;
        case elementPrstGen:     count = _sf2[id.indexSf2]->_prst[id.indexElt]->_bagGlobal._gens.count(); break;
        case elementInstSmplGen: count = _sf2[id.indexSf2]->_inst[id.indexElt]->_bags[id.indexElt2]->_gens.count(); break;
        case elementPrstInstGen: count = _sf2[id.indexSf2]->_prst[id.indexElt]->_bags[id.indexElt2]->_gens.count(); break;
        default:                 count = -1;
        }
    }
    else
    {
        switch ((int)id.typeElement)
        {
        case elementSf2:
            foreach (SF2 * elt, _sf2)
                if (!elt->_hidden)
                    ++count;
            break;
        case elementSmpl:
            foreach (SF2::SMPL * elt, _sf2[id.indexSf2]->_smpl)
                if (!elt->_hidden)
                    ++count;
            break;
        case elementInst:
            foreach (SF2::INST * elt, _sf2[id.indexSf2]->_inst)
                if (!elt->_hidden)
                    ++count;
            break;
        case elementPrst:
            foreach (SF2::PRST * elt, _sf2[id.indexSf2]->_prst)
                if (!elt->_hidden)
                    ++count;
            break;
        case elementInstSmpl:
            foreach (SF2::BAG * elt, _sf2[id.indexSf2]->_inst[id.indexElt]->_bags)
                if (!elt->_hidden)
                    ++count;
            break;
        case elementPrstInst:
            foreach (SF2::BAG * elt, _sf2[id.indexSf2]->_prst[id.indexElt]->_bags)
                if (!elt->_hidden)
                    ++count;
            break;
        case elementInstMod:
            foreach (SF2::BAG::MOD elt, _sf2[id.indexSf2]->_inst[id.indexElt]->_bagGlobal._mods)
                if (!elt._hidden)
                    ++count;
            break;
        case elementPrstMod:
            foreach (SF2::BAG::MOD elt, _sf2[id.indexSf2]->_prst[id.indexElt]->_bagGlobal._mods)
                if (!elt._hidden)
                    ++count;
            break;
        case elementInstSmplMod:
            foreach (SF2::BAG::MOD elt, _sf2[id.indexSf2]->_inst[id.indexElt]->_bags[id.indexElt2]->_mods)
                if (!elt._hidden)
                    ++count;
            break;
        case elementPrstInstMod:
            foreach (SF2::BAG::MOD elt, _sf2[id.indexSf2]->_prst[id.indexElt]->_bags[id.indexElt2]->_mods)
                if (!elt._hidden)
                    ++count;
            break;
        case elementInstGen:     count = _sf2[id.indexSf2]->_inst[id.indexElt]->_bagGlobal._gens.count(); break;
        case elementPrstGen:     count = _sf2[id.indexSf2]->_prst[id.indexElt]->_bagGlobal._gens.count(); break;
        case elementInstSmplGen: count = _sf2[id.indexSf2]->_inst[id.indexElt]->_bags[id.indexElt2]->_gens.count(); break;
        case elementPrstInstGen: count = _sf2[id.indexSf2]->_prst[id.indexElt]->_bags[id.indexElt2]->_gens.count(); break;
        }
    }

    return count;
}

// Gestionnaire d'actions
void Pile_sf2::releaseActions(bool withVeryOldActions)
{
    // Definitely remove redo (they are outdated now)
    for (int i = _undoRedo->countRedo() - 1; i >= 0; i--)
    {
        // Browse actions within the redo
        QList<Action *> actions = _undoRedo->getRedoActions(i);
        foreach (Action * action, actions)
        {
            if (action->typeAction == Action::TypeCreation)
            {
                // Definitely delete an element that has been created and deleted
                this->remove(action->id, true, false);

                // Update table (a hidden element will be definitely removed)
                emit(updateTable((int)action->id.typeElement, action->id.indexSf2,
                                 action->id.indexElt, action->id.indexElt2));
            }
        }

        // Delete the redo
        _undoRedo->deleteRedo(i);
    }

    if (withVeryOldActions)
    {
        // Delete very old undo
        for (int i = _undoRedo->countUndo() - 1; i >= UNDO_NUMBER - 1; i--)
        {
            // Reverse browse actions within the undo
            QList<Action *> actions = _undoRedo->getUndoActions(i);
            for (int j = actions.count() - 1; j >= 0; j--)
            {
                Action * actionTmp = actions[j];
                if (actionTmp->typeAction == Action::TypeRemoval)
                {
                    // Definitely delete an element that has been deleted a long time ago
                    int message = 1;
                    this->remove(actionTmp->id, true, false, &message);

                    // Update table (a hidden element will be definitely removed)
                    emit(updateTable((int)actionTmp->id.typeElement, actionTmp->id.indexSf2,
                                     actionTmp->id.indexElt, actionTmp->id.indexElt2));
                }
            }

            // Change the edition number of the sf2 if it has never been saved
            actions = _undoRedo->getUndoActions(i);
            if (!actions.isEmpty())
            {
                if (_sf2[actions[0]->id.indexSf2]->_numEdition == -1)
                    _sf2[actions[0]->id.indexSf2]->_numEdition = -2;
            }

            // Delete the undo
            _undoRedo->deleteUndo(i);
        }
    }
}

void Pile_sf2::prepareNewActions(bool removeOldActions)
{
    releaseActions(removeOldActions);

    // Nouvelle action
    this->_undoRedo->newActionSet();
}

void Pile_sf2::cleanActions() { _undoRedo->cleanActions(); }

bool Pile_sf2::isUndoable() { return _undoRedo->isUndoable(); }

bool Pile_sf2::isRedoable() { return _undoRedo->isRedoable(); }

void Pile_sf2::undo()
{
    QList<Action *> actions = _undoRedo->undo();

    // Process actions in order
    foreach (Action * action, actions)
    {
        switch (action->typeAction)
        {
        case Action::TypeCreation: {
            // Hide the element
            int message = 1;
            this->remove(action->id, 0, 0, &message);
        }break;
        case Action::TypeRemoval:
            // Display the element
            this->display(action->id);
            break;
        case Action::TypeUpdate:
        case Action::TypeChangeToDefault:
            // Back to the old value
            if (action->champ >= 0 && action->champ < 164) this->set(action->id, action->champ, action->vOldValue, 0); // Valeur
            else if (action->champ >= 164 && action->champ < 200) this->set(action->id, action->champ, action->qOldValue, 0); // QString
            else if (action->champ >=200) this->set(action->id, action->champ, action->baOldValue, 0); // char*
            break;
        case Action::TypeChangeFromDefault:
            // Retour to the old value, reset
            this->reset(action->id, action->champ, 0);
            break;
        default:
            break;
        }
    }
}

void Pile_sf2::redo()
{
    QList<Action *> actions = _undoRedo->redo();

    // Process actions in reverse order
    Action * action;
    for (int i = actions.count()-1; i >= 0; i--)
    {
        action = actions[i];
        switch (action->typeAction)
        {
        case Action::TypeCreation:
            // Display element
            this->display(action->id);
            break;
        case Action::TypeRemoval:{
            // Hide elemnet
            int message = 1;
            this->remove(action->id, 0, 0, &message);
        }break;
        case Action::TypeUpdate:
        case Action::TypeChangeFromDefault:
            // Apply again the new value
            if (action->champ >= 0 && action->champ < 164) this->set(action->id, action->champ, action->vNewValue, 0); // Valeur
            else if (action->champ >= 164 && action->champ < 200) this->set(action->id, action->champ, action->qNewValue, 0); // QString
            else if (action->champ >=200) this->set(action->id, action->champ, action->baNewValue, 0); // char*
            break;
        case Action::TypeChangeToDefault:
            // Apply the new value, reset
            this->reset(action->id, action->champ, 0);
            break;
        default:
            break;
        }
    }
}

// Gestion de la sauvegarde
void Pile_sf2::storeEdition(int indexSf2) { _sf2[indexSf2]->_numEdition = this->_undoRedo->getEdition(indexSf2); }
bool Pile_sf2::isEdited(int indexSf2) { return this->_undoRedo->getEdition(indexSf2) != _sf2[indexSf2]->_numEdition; }

// Récupération liste de champs et valeurs de bags
void Pile_sf2::getListeBags(EltID id, QList<Champ> &listeChamps, QList<genAmountType> &listeValeurs)
{
    if (!this->isValid(id))
        return;

    QList<SF2::BAG::GEN> * gens = NULL;
    switch (id.typeElement)
    {
    case elementInst:
        gens = &_sf2[id.indexSf2]->_inst[id.indexElt]->_bagGlobal._gens;
        break;
    case elementInstSmpl:
        gens = &_sf2[id.indexSf2]->_inst[id.indexElt]->_bags[id.indexElt2]->_gens;
        break;
    case elementPrst:
        gens = &_sf2[id.indexSf2]->_prst[id.indexElt]->_bagGlobal._gens;
        break;
    case elementPrstInst:
        gens = &_sf2[id.indexSf2]->_prst[id.indexElt]->_bags[id.indexElt2]->_gens;
        break;
    default:
        break;
    }
    if (gens)
    {
        for (int i = 0; i < gens->count(); i++)
        {
            listeChamps << gens->at(i).sfGenOper;
            listeValeurs << gens->at(i).genAmount;
        }
    }
}

///////////////////////// METHODES PRIVEES /////////////////////////

// Ajoute un enfant à id
int Pile_sf2::add(EltID id, bool storeAction)
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
        storeAction = false; // No undo possible
        _sf2 << new SF2();
        i = id.indexSf2 = _sf2.count() - 1;

        // Initialisation bps
    {
        Valeur valTmp;
        valTmp.wValue = 16;
        this->set(id, champ_wBpsInit, valTmp, false);
        this->set(id, champ_wBpsSave, valTmp, false);
    }

        // Emit signal for a new element
        emit(newElement(id));
        break;
    case elementSmpl:
        // Create a new sample
        _sf2[id.indexSf2]->_smpl << new SF2::SMPL();
        i = id.indexElt = _sf2[id.indexSf2]->_smpl.count() - 1;

        // Emit signal for a new element
        emit(newElement(id));
        break;
    case elementInst:
        // Create a new instrument
        _sf2[id.indexSf2]->_inst << new SF2::INST();
        i = id.indexElt = _sf2[id.indexSf2]->_inst.count() - 1;

        // Emit signal for a new element
        emit(newElement(id));
        break;
    case elementPrst:
        // Create a new preset
        _sf2[id.indexSf2]->_prst << new SF2::PRST();
        i = id.indexElt = _sf2[id.indexSf2]->_prst.count() - 1;

        // Emit signal for a new element
        emit(newElement(id));
        break;
    case elementInstSmpl:
        // Add a new sample in an instrument
        _sf2[id.indexSf2]->_inst[id.indexElt]->_bags << new SF2::BAG();
        i = id.indexElt2 = _sf2[id.indexSf2]->_inst[id.indexElt]->_bags.count() - 1;

        // Emit signal for a new element
        emit(newElement(id));
        break;
    case elementPrstInst:
        // Add a new instrument in a preset
        _sf2[id.indexSf2]->_prst[id.indexElt]->_bags << new SF2::BAG();
        i = id.indexElt2 = _sf2[id.indexSf2]->_prst[id.indexElt]->_bags.count() - 1;

        // Emit signal for a new element
        emit(newElement(id));
        break;
    case elementInstMod: case elementPrstMod: case elementInstSmplMod: case elementPrstInstMod:{
        // Add a new mod
        SF2::BAG *bag = NULL;
        switch ((int)id.typeElement)
        {
        case elementInstMod:
            // For an instrument
            bag = &_sf2[id.indexSf2]->_inst[id.indexElt]->_bagGlobal;
            break;
        case elementPrstMod:
            // For a preset
            bag = &_sf2[id.indexSf2]->_prst[id.indexElt]->_bagGlobal;
            break;
        case elementInstSmplMod:
            // For a sample linked to an instrument
            bag = _sf2[id.indexSf2]->_inst[id.indexElt]->_bags[id.indexElt2];
            break;
        case elementPrstInstMod:
            // For an instrument linked to a preset
            bag = _sf2[id.indexSf2]->_prst[id.indexElt]->_bags[id.indexElt2];
        }
        bag->_mods << SF2::BAG::MOD();
        i = id.indexMod = bag->_mods.count() - 1;

        // Fill the index
        Valeur val;
        val.wValue = id.indexMod;
        this->set(id, champ_indexMod, val, 0);
    }break;
    }

    // Create and store an action
    if (storeAction)
    {
        Action *action = new Action();
        action->typeAction = Action::TypeCreation;
        action->id = id;
        this->_undoRedo->add(action);
    }
    return i;
}

int Pile_sf2::remove(EltID id, bool permanently, bool storeAction, int *message)
{
    if (!this->isValid(id, permanently)) // Hidden ID are acceped for a permanent removal
        return 1;

    switch ((int)id.typeElement)
    {
    case elementSf2:{
        permanently = true; // No undo possible after a file is closed
        storeAction = false;
        // suppression d'un SF2
        // nombre de presets associés
        EltID id2(elementPrst, id.indexSf2, 0, 0, 0);
        int max = this->count(id2);
        for (int i = max - 1; i >= 0; i--)
        {
            // Propagation aux presets /!\ si temporaire, ne pas propager aux éléments déjà supprimés de manière temporaire
            id2.indexElt = i;
            this->remove(id2, true, storeAction, message);
        }
        id2.typeElement = elementInst;
        max = this->count(id2);
        for (int i = max - 1; i >= 0; i--)
        {
            // propagation aux instruments
            id2.indexElt = i;
            this->remove(id2, true, storeAction, message);
        }
        id2.typeElement = elementSmpl;
        max = this->count(id2);
        for (int i = max - 1; i >= 0; i--)
        {
            // propagation aux samples
            id2.indexElt = i;
            this->remove(id2, true, storeAction, message);
        }

        // Ajustement de la numérotation dans les actions
        this->_undoRedo->decrement(id);

        // Suppression du sf2
        delete _sf2.takeAt(id.indexSf2);
        emit(removeElement(id));
    }break;
    case elementSmpl:{
        // Check that no instruments use the sample
        foreach (SF2::INST * instTmp, _sf2[id.indexSf2]->_inst)
        {
            if (!instTmp->_hidden)
            {
                foreach (SF2::BAG *bagTmp, instTmp->_bags)
                {
                    if (!bagTmp->_hidden && bagTmp->isSet(champ_sampleID) &&
                            bagTmp->getGen(champ_sampleID).wValue == id.indexElt)
                    {
                        if (*message % 2 != 0 || !message)
                            *message *= 2;
                        return 1;
                    }
                }
            }
        }

        // Linked sample?
        EltID id2(elementSmpl, id.indexSf2, 0, 0, 0);
        Valeur value;
        for (int i = 0; i < _sf2[id.indexSf2]->_smpl.count(); i++)
        {
            SF2::SMPL * smplTmp = _sf2[id.indexSf2]->_smpl[i];
            if (!smplTmp->_hidden)
            {
                if (smplTmp->_wSampleLink == id.indexElt)
                {
                    id2.indexElt = i;
                    value.wValue = 0;
                    this->set(id2, champ_wSampleLink, value, storeAction);
                    value.sfLinkValue = monoSample;
                    this->set(id2, champ_sfSampleType, value, storeAction);
                }
            }
        }

        if (permanently)
        {
            // Ajustement de la numérotation dans les actions
            this->_undoRedo->decrement(id);

            // Décrémentation des samples liés aux instruments
            for (int i = 0; i < _sf2[id.indexSf2]->_inst.count(); i++)
                _sf2[id.indexSf2]->_inst[i]->decrementerSMPL(id.indexElt);

            // Décrémentation des samples liés par stéréo
            _sf2[id.indexSf2]->decrementerLinkSMPL(id.indexElt);

            // Suppression du sample
            delete _sf2[id.indexSf2]->_smpl.takeAt(id.indexElt);
            emit(removeElement(id));
        }
        else
        {
            // Masquage du sample
            emit(hideElement(id, true));
            _sf2[id.indexSf2]->_smpl[id.indexElt]->_hidden = 1;
        }
    }break;
    case elementInst:{
        // Check that no presets use the instrument
        foreach (SF2::PRST * prstTmp, _sf2[id.indexSf2]->_prst)
        {
            if (!prstTmp->_hidden)
            {
                foreach (SF2::BAG *bagTmp, prstTmp->_bags)
                {
                    if (!bagTmp->_hidden && bagTmp->isSet(champ_instrument) &&
                            bagTmp->getGen(champ_instrument).wValue == id.indexElt)
                    {
                        if (*message % 3 != 0 || !message)
                            *message *= 3;
                        return 1;
                    }
                }
            }
        }

        // Propagation aux samples liés
        EltID id2(elementInstSmpl, id.indexSf2, id.indexElt, 0, 0);
        int nbInstSmpl = this->count(id2);
        for (int i = nbInstSmpl - 1; i >= 0; i--)
        {
            id2.indexElt2 = i;
            if (permanently || !this->get(id2, champ_hidden).bValue) this->remove(id2, permanently, storeAction, message);
        }
        // Propagation aux mods associés
        id2.typeElement = elementInstMod;
        int nbInstMod = this->count(id2);
        for (int i = nbInstMod - 1; i >= 0; i--)
        {
            id2.indexMod = i;
            if (permanently || !this->get(id2, champ_hidden).bValue) this->remove(id2, permanently, storeAction, message);
        }
        if (permanently)
        {
            // Ajustement de la numérotation dans les actions
            this->_undoRedo->decrement(id);

            // Décrémentation des instruments liés aux presets
            for (int i = 0; i < _sf2[id.indexSf2]->_prst.count(); i++)
                _sf2[id.indexSf2]->_prst[i]->decrementerINST(id.indexElt);

            // Suppression des gens
            _sf2[id.indexSf2]->_inst[id.indexElt]->_bagGlobal.supprGenAndStore(id, storeAction, this);

            // Suppression de l'instrument
            delete _sf2[id.indexSf2]->_inst.takeAt(id.indexElt);
            emit(removeElement(id));
        }
        else
        {
            // Suppression des gens
            _sf2[id.indexSf2]->_inst[id.indexElt]->_bagGlobal.supprGenAndStore(id, storeAction, this);

            // Masquage de l'instrument
            emit(hideElement(id, true));
            _sf2[id.indexSf2]->_inst[id.indexElt]->_hidden = 1;
        }
    }break;
    case elementPrst:{
        // suppression d'un preset
        EltID id2(elementPrstInst, id.indexSf2, id.indexElt, 0, 0);
        // Propagation aux instruments liés
        int nbPrstInst = this->count(id2);
        for (int i = nbPrstInst - 1; i >= 0; i--)
        {
            id2.indexElt2 = i;
            if (permanently || !this->get(id2, champ_hidden).bValue) this->remove(id2, permanently, storeAction, message);
        }
        // Propagation aux mods associés
        id2.typeElement = elementPrstMod;
        int nbPrstMod = this->count(id2);
        for (int i = nbPrstMod - 1; i >= 0; i--)
        {
            id2.indexMod = i;
            if (permanently || !this->get(id2, champ_hidden).bValue) this->remove(id2, permanently, storeAction, message);
        }
        if (permanently)
        {
            // Ajustement de la numérotation dans les actions
            this->_undoRedo->decrement(id);

            // Suppression des gens
            _sf2[id.indexSf2]->_prst[id.indexElt]->_bagGlobal.supprGenAndStore(id, storeAction, this);

            // Suppression du preset
            delete _sf2[id.indexSf2]->_prst.takeAt(id.indexElt);
            emit(removeElement(id));
        }
        else
        {
            // Suppression des gens
            _sf2[id.indexSf2]->_prst[id.indexElt]->_bagGlobal.supprGenAndStore(id, storeAction, this);

            // Masquage du preset
            emit(hideElement(id, true));
            _sf2[id.indexSf2]->_prst[id.indexElt]->_hidden = 1;
        }
    }break;
    case elementInstSmpl:{
        // suppression d'un sample lié à un instrument
        EltID id2(elementInstSmplMod, id.indexSf2, id.indexElt, id.indexElt2, 0);
        // Propagation aux mods associés
        int nbInstSmplMod = this->count(id2);
        for (int i = nbInstSmplMod -1; i >= 0; i--)
        {
            id2.indexMod = i;
            if (permanently || !this->get(id2, champ_hidden).bValue) this->remove(id2, permanently, storeAction, message);
        }
        if (permanently)
        {
            // Ajustement de la numérotation dans les actions
            this->_undoRedo->decrement(id);

            // Suppression des gens
            _sf2[id.indexSf2]->_inst[id.indexElt]->_bags[id.indexElt2]->supprGenAndStore(id, storeAction, this);

            // Suppression du sample lié à l'instrument
            delete _sf2[id.indexSf2]->_inst[id.indexElt]->_bags.takeAt(id.indexElt2);
            emit(removeElement(id));
        }
        else
        {
            // Suppression des gens
            _sf2[id.indexSf2]->_inst[id.indexElt]->_bags[id.indexElt2]->supprGenAndStore(id, storeAction, this);

            // Masquage du sample lié à l'instrument
            emit(hideElement(id, true));
            _sf2[id.indexSf2]->_inst[id.indexElt]->_bags[id.indexElt2]->_hidden = 1;
        }
    }break;
    case elementPrstInst:{
        // suppression d'un instrument lié à un preset
        EltID id2(elementPrstInstMod, id.indexSf2, id.indexElt, id.indexElt2, 0);
        // Propagation aux mods associés
        int nbPrstInstMod = this->count(id2);
        for (int i = nbPrstInstMod -1; i >= 0; i--)
        {
            id2.indexMod = i;
            if (permanently || !this->get(id2, champ_hidden).bValue) this->remove(id2, permanently, storeAction, message);
        }
        if (permanently)
        {
            // Ajustement de la numérotation dans les actions
            this->_undoRedo->decrement(id);

            // Suppression des gens
            _sf2[id.indexSf2]->_prst[id.indexElt]->_bags[id.indexElt2]->supprGenAndStore(id, storeAction, this);

            // Suppression de l'instrument lié au preset
            delete _sf2[id.indexSf2]->_prst[id.indexElt]->_bags.takeAt(id.indexElt2);
            emit(removeElement(id));
        }
        else
        {
            // Suppression des gens
            _sf2[id.indexSf2]->_prst[id.indexElt]->_bags[id.indexElt2]->supprGenAndStore(id, storeAction, this);

            // Masquage de l'instrument lié au preset
            emit(hideElement(id, true));
            _sf2[id.indexSf2]->_prst[id.indexElt]->_bags[id.indexElt2]->_hidden = 1;
        }
    }break;
    case elementInstMod: case elementPrstMod: case elementInstSmplMod: case elementPrstInstMod:{
        // suppression d'un mod
        SF2::BAG *bag = NULL;
        switch ((int)id.typeElement)
        {
        case elementInstMod:
            // d'un instrument
            bag = &_sf2[id.indexSf2]->_inst[id.indexElt]->_bagGlobal;
            break;
        case elementPrstMod:
            // d'un preset
            bag = &_sf2[id.indexSf2]->_prst[id.indexElt]->_bagGlobal;
            break;
        case elementInstSmplMod:
            // d'un sample lié à un instrument
            bag = _sf2[id.indexSf2]->_inst[id.indexElt]->_bags[id.indexElt2];
            break;
        case elementPrstInstMod:
            // d'un instrument lié à un prst
            bag = _sf2[id.indexSf2]->_prst[id.indexElt]->_bags[id.indexElt2];
        }
        if (permanently)
        {
            // Ajustement de la numérotation dans les actions
            this->_undoRedo->decrement(id);

            // ajustement de la numérotation
            bag->enleverMod(id.indexMod);

            // Le mod pointe sur un autre ?
            int iVal = bag->_mods[id.indexMod]._sfModDestOper;
            if (iVal >= 32768 && iVal - 32768 < this->count(id))
            {
                if (bag->_mods[iVal-32768]._sfModSrcOper.CC == 0 &&
                        bag->_mods[iVal-32768]._sfModSrcOper.Index == 127)
                    bag->_mods[iVal-32768]._sfModSrcOper.Index = 0;
            }
            // suppression du mod
            bag->_mods.removeAt(id.indexMod);
        }
        else
        {
            // masquage du mod
            bag->_mods[id.indexMod]._hidden = true;
        }
    }break;
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

int Pile_sf2::set(EltID id, Champ champ, Valeur value, bool storeAction, bool sort)
{
    if (champ == champ_hidden)
        return 1;

    if (!this->isValid(id) && champ != champ_ram)
        return 1;

    Valeur oldValue;
    oldValue.wValue = 0;
    int defaultValue = 0;

    // Type d'élément à modifier
    switch ((int)id.typeElement)
    {
    case elementSf2:{
        // Modification d'un SF2
        SF2 *tmp = _sf2[id.indexSf2];
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
        SF2::SMPL *tmp = _sf2[id.indexSf2]->_smpl[id.indexElt];
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
        case champ_ram:
            // pas d'enregistrement de l'action
            storeAction = false;
            tmp->_sound.setRam(value.wValue);
            break;
        }
    }break;
    case elementInst:{
        // Update an instrument
        SF2::INST *tmp = _sf2[id.indexSf2]->_inst[id.indexElt];
        if (!tmp->_bagGlobal.isSet(champ)) defaultValue = 1;
        oldValue = tmp->_bagGlobal.getGen(champ);
        tmp->_bagGlobal.setGen(champ, value);
    }break;
    case elementPrst:{
        // Update a preset
        SF2::PRST *tmp = _sf2[id.indexSf2]->_prst[id.indexElt];
        switch ((int)champ)
        {
        case champ_wPreset:
            oldValue.wValue = tmp->_wPreset;
            tmp->_wPreset = value.wValue; break;
        case champ_wBank:
            oldValue.wValue = tmp->_wBank;
            tmp->_wBank = value.wValue; break;
        case champ_dwLibrary:
            oldValue.dwValue = tmp->_dwLibrary;
            tmp->_dwLibrary = value.dwValue; break;
        case champ_dwGenre:
            oldValue.dwValue = tmp->_dwGenre;
            tmp->_dwGenre = value.dwValue; break;
        case champ_dwMorphology:
            oldValue.dwValue = tmp->_dwMorphology;
            tmp->_dwMorphology = value.dwValue; break;
        default:
            if (!tmp->_bagGlobal.isSet(champ)) defaultValue = 1;
            oldValue = tmp->_bagGlobal.getGen(champ);
            tmp->_bagGlobal.setGen(champ, value);
        }
        if (champ == champ_wPreset || champ == champ_wBank)
        {
            // Notification that the element display changed
            QString text = QString("%1:%2 %3")
                    .arg(tmp->_wBank, 3, 10, QChar('0'))
                    .arg(tmp->_wPreset, 3, 10, QChar('0'))
                    .arg(tmp->_name);
            emit(changeElementName(id, text));
            emit(changeElementOrder(id, text, sort));
        }
    }break;
    case elementInstSmpl:{
        // Modification of a sample linked to an instrument
        SF2::BAG *tmp = _sf2[id.indexSf2]->_inst[id.indexElt]->_bags[id.indexElt2];
        if (!tmp->isSet(champ)) defaultValue = 1;
        oldValue = tmp->getGen(champ);
        tmp->setGen(champ, value);

        // Notifications
        if (champ == champ_sampleID || champ == champ_keyRange || champ == champ_velRange)
        {
            QString name = this->getQstr(EltID(elementSmpl, id.indexSf2, this->get(id, champ_sampleID).wValue, 0, 0), champ_name);
            QString order = QString("%1:%2 %3")
                    .arg(this->get(id, champ_keyRange).rValue.byLo, 3, 10, QChar('0'))
                    .arg(this->get(id, champ_velRange).rValue.byLo, 3, 10, QChar('0'))
                    .arg(name);
            emit(changeElementName(id, name));
            emit(changeElementOrder(id, order, sort));
        }
    }break;
    case elementPrstInst:{
        // Modification of an instrument linked to a preset
        SF2::BAG *tmp = _sf2[id.indexSf2]->_prst[id.indexElt]->_bags[id.indexElt2];
        if (!tmp->isSet(champ)) defaultValue = 1;
        oldValue = tmp->getGen(champ);
        tmp->setGen(champ, value);

        // Notifications
        if (champ == champ_instrument || champ == champ_keyRange || champ == champ_velRange)
        {
            QString name = this->getQstr(EltID(elementInst, id.indexSf2, this->get(id, champ_instrument).wValue, 0, 0), champ_name);
            QString order = QString("%1:%2 %3")
                    .arg(this->get(id, champ_keyRange).rValue.byLo, 3, 10, QChar('0'))
                    .arg(this->get(id, champ_velRange).rValue.byLo, 3, 10, QChar('0'))
                    .arg(name);
            emit(changeElementName(id, name));
            emit(changeElementOrder(id, order, sort));
        }
    }break;
    case elementInstMod: case elementPrstMod: case elementInstSmplMod: case elementPrstInstMod:{
        // Modification d'un mod d'un instrument
        SF2::BAG::MOD *tmp = NULL;
        switch ((int)id.typeElement)
        {
        case elementInstMod:
            tmp = &_sf2[id.indexSf2]->_inst[id.indexElt]->_bagGlobal._mods[id.indexMod]; break;
        case elementPrstMod:
            tmp = &_sf2[id.indexSf2]->_prst[id.indexElt]->_bagGlobal._mods[id.indexMod]; break;
        case elementInstSmplMod:
            tmp = &_sf2[id.indexSf2]->_inst[id.indexElt]->_bags[id.indexElt2]->_mods[id.indexMod]; break;
        case elementPrstInstMod:
            tmp = &_sf2[id.indexSf2]->_prst[id.indexElt]->_bags[id.indexElt2]->_mods[id.indexMod]; break;
        }
        switch ((int)champ)
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
        }
    }break;
    }

    // Create and store the action
    if (storeAction)
    {
        Action *action = new Action();
        if (defaultValue) action->typeAction = Action::TypeChangeFromDefault;
        else action->typeAction = Action::TypeUpdate;
        action->id = id;
        action->champ = champ;
        action->vOldValue = oldValue;
        action->vNewValue = value;
        this->_undoRedo->add(action);
    }
    return 0;
}

int Pile_sf2::set(EltID id, Champ champ, QString qStr, bool storeAction, bool sort)
{
    if (!this->isValid(id))
        return 1;

    QString qOldStr = "";
    // Type of element to edit
    switch ((int)id.typeElement)
    {
    case elementSf2:{
        // Editing of a sf2
        SF2 *tmp = _sf2[id.indexSf2];
        switch ((int)champ)
        {
        case champ_name:
            qOldStr = tmp->_INAM;
            qStr = qStr.left(256);
            tmp->_INAM = qStr;

            // Notify the name update
            emit(changeElementName(id, qStr));
            emit(changeElementOrder(id, qStr, true));
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
        case champ_filename:
            qOldStr = tmp->_fileName;
            tmp->_fileName = qStr; break;
        }
    }break;
    case elementSmpl:{
        // Editing of a sample
        qStr = qStr.trimmed();
        SF2::SMPL *tmp = _sf2[id.indexSf2]->_smpl[id.indexElt];
        switch ((int)champ)
        {
        case champ_name:{
            qOldStr = tmp->_name;
            qStr = qStr.left(20);
            tmp->_name = qStr;

            // Notify the name update for the sample
            emit(changeElementName(id, qStr));
            emit(changeElementOrder(id, qStr, sort));

            // Notify the name change for all corresponding smpl inst
            int indSmpl = id.indexElt;
            EltID id2 = id;
            id2.typeElement = elementInst;
            int nbInst = this->count(id2);
            int nbInstSmpl;
            for (int i = 0; i < nbInst; i++)
            {
                id2.typeElement = elementInst;
                id2.indexElt = i;
                if (!this->get(id2, champ_hidden).bValue)
                {
                    id2.typeElement = elementInstSmpl;
                    nbInstSmpl = this->count(id2);
                    for (int j = 0; j < nbInstSmpl; j++)
                    {
                        id2.indexElt2 = j;
                        if (!this->get(id2, champ_hidden).bValue)
                        {
                            if (this->get(id2, champ_sampleID).wValue == indSmpl)
                            {
                                emit(changeElementName(id2, qStr));

                                QString order = QString("%1:%2 %3")
                                        .arg(this->get(id2, champ_keyRange).rValue.byLo, 3, 10, QChar('0'))
                                        .arg(this->get(id2, champ_velRange).rValue.byLo, 3, 10, QChar('0'))
                                        .arg(qStr);
                                emit(changeElementOrder(id2, order, true));
                            }
                        }
                    }
                }
            }
        };break;
        case champ_filename:
            qOldStr = tmp->_sound.getFileName();
            tmp->_sound.setFileName(qStr);
            break;
        }
    }break;
    case elementInst:{
        qStr = qStr.trimmed();
        // Modification d'un instrument
        SF2::INST *tmp = _sf2[id.indexSf2]->_inst[id.indexElt];
        switch ((int)champ)
        {
        case champ_name:
            // Modification du nom d'un instrument
            qOldStr = tmp->_name;
            qStr = qStr.left(20);
            tmp->_name = qStr;

            // Notify the name update for the instrument
            emit(changeElementName(id, qStr));
            emit(changeElementOrder(id, qStr, sort));

            // Change the corresponding inst prst
            int indInst = id.indexElt;
            EltID id2 = id;
            id2.typeElement = elementPrst;
            int nbPrst = this->count(id2);
            int nbPrstInst;
            for (int i = 0; i < nbPrst; i++)
            {
                id2.typeElement = elementPrst;
                id2.indexElt = i;
                if (!this->get(id2, champ_hidden).bValue)
                {
                    id2.typeElement = elementPrstInst;
                    nbPrstInst = this->count(id2);
                    for (int j = 0; j < nbPrstInst; j++)
                    {
                        id2.indexElt2 = j;
                        if (!this->get(id2, champ_hidden).bValue)
                        {
                            if (this->get(id2, champ_instrument).wValue == indInst)
                            {
                                emit(changeElementName(id2, qStr));

                                QString order = QString("%1:%2 %3")
                                        .arg(this->get(id2, champ_keyRange).rValue.byLo, 3, 10, QChar('0'))
                                        .arg(this->get(id2, champ_velRange).rValue.byLo, 3, 10, QChar('0'))
                                        .arg(qStr);
                                emit(changeElementOrder(id2, order, true));
                            }
                        }
                    }
                }
            }
            break;
        }
    }break;
    case elementPrst:{
        qStr = qStr.trimmed();
        // Modification d'un preset
        SF2::PRST *tmp = _sf2[id.indexSf2]->_prst[id.indexElt];
        switch ((int)champ)
        {
        case champ_name:
            // Modification du nom d'un preset
            qOldStr = tmp->_name;
            qStr = qStr.left(20);
            tmp->_name = qStr;

            // Notify the name update for the preset
            QString qStr = QString("%1:%2 %3")
                    .arg(tmp->_wBank, 3, 10, QChar('0'))
                    .arg(tmp->_wPreset, 3, 10, QChar('0'))
                    .arg(tmp->_name);

            emit(changeElementName(id, qStr));
            emit(changeElementOrder(id, qStr, sort));
            break;
        }
    }break;
    }

    // Create and store the action
    if (storeAction)
    {
        Action *action = new Action();
        action->typeAction = Action::TypeUpdate;
        action->id = id;
        action->champ = champ;
        action->qOldValue = qOldStr;
        action->qNewValue = qStr;
        this->_undoRedo->add(action);
    }
    return 0;
}

int Pile_sf2::set(EltID id, Champ champ, QByteArray data, bool storeAction)
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
            oldData = _sf2[id.indexSf2]->_smpl[id.indexElt]->_sound.getData(16);
            _sf2[id.indexSf2]->_smpl[id.indexElt]->_sound.setData(data, 16);
            break;
        case champ_sampleData24:
            oldData = _sf2[id.indexSf2]->_smpl[id.indexElt]->_sound.getData(8);
            _sf2[id.indexSf2]->_smpl[id.indexElt]->_sound.setData(data, 8);
            break;
        case champ_sampleDataFull24:{
            // Modification 16 bits
            QByteArray baData = Sound::bpsConversion(data, 24, 16);
            this->set(id, champ_sampleData16, baData, storeAction);
            // Modification 8 bits de poids faibles
            baData = Sound::bpsConversion(data, 24, 824);
            this->set(id, champ_sampleData24, baData, storeAction);
            storeAction = false;
        }break;
        case champ_sampleData32:{
            // Modification 16 bits
            QByteArray baData = Sound::bpsConversion(data, 32, 16);
            this->set(id, champ_sampleData16, baData, storeAction);
            // Modification 8 bits de poids faibles
            baData = Sound::bpsConversion(data, 32, 824);
            this->set(id, champ_sampleData24, baData, storeAction);
            storeAction = false;
        }break;
        }
    }
    // Création et stockage de l'action
    if (storeAction)
    {
        Action *action = new Action();
        action->typeAction = Action::TypeUpdate;
        action->id = id;
        action->champ = champ;
        action->baOldValue = oldData;
        action->baNewValue = data;
        this->_undoRedo->add(action);
    }
    return 0;
}

int Pile_sf2::reset(EltID id, Champ champ, bool storeAction)
{
    if (!this->isValid(id))
        return 0;

    Valeur oldValue;
    // Type d'élément à modifier
    switch ((int)id.typeElement)
    {
    case elementInst:{
        // Remise à zéro d'une propriété d'un instrument
        SF2::INST *tmp = _sf2[id.indexSf2]->_inst[id.indexElt];
        if (!tmp->_bagGlobal.isSet(champ)) return 0;
        oldValue = tmp->_bagGlobal.getGen(champ);
        tmp->_bagGlobal.resetGen(champ);
    }break;
    case elementPrst:{
        // Remise à zéro d'une propriété d'un preset
        SF2::PRST *tmp = _sf2[id.indexSf2]->_prst[id.indexElt];
        if (!tmp->_bagGlobal.isSet(champ)) return 0;
        oldValue = tmp->_bagGlobal.getGen(champ);
        tmp->_bagGlobal.resetGen(champ);
    }break;
    case elementInstSmpl:{
        // Remise à zéro d'une propriété d'un sample lié à un instrument
        SF2::BAG *tmp = _sf2[id.indexSf2]->_inst[id.indexElt]->_bags[id.indexElt2];
        if (!tmp->isSet(champ)) return 0;
        oldValue = tmp->getGen(champ);
        tmp->resetGen(champ);

        // Notifications
        if (champ == champ_sampleID || champ == champ_keyRange || champ == champ_velRange)
        {
            QString name = this->getQstr(EltID(elementSmpl, id.indexSf2, this->get(id, champ_sampleID).wValue, 0, 0), champ_name);
            emit(changeElementName(id, name));

            QString order = QString("%1:%2 %3")
                    .arg(this->get(id, champ_keyRange).rValue.byLo, 3, 10, QChar('0'))
                    .arg(this->get(id, champ_velRange).rValue.byLo, 3, 10, QChar('0'))
                    .arg(name);
            emit(changeElementOrder(id, order, true));
        }
    }break;
    case elementPrstInst:{
        // Remise à zéro d'une propriété d'un instrument lié à un preset
        SF2::BAG *tmp = _sf2[id.indexSf2]->_prst[id.indexElt]->_bags[id.indexElt2];
        if (!tmp->isSet(champ)) return 0;
        oldValue = tmp->getGen(champ);
        tmp->resetGen(champ);

        // Notifications
        if (champ == champ_instrument || champ == champ_keyRange || champ == champ_velRange)
        {
            QString name = this->getQstr(EltID(elementSmpl, id.indexSf2, this->get(id, champ_sampleID).wValue, 0, 0), champ_name);
            emit(changeElementName(id, name));

            QString order = QString("%1:%2 %3")
                    .arg(this->get(id, champ_keyRange).rValue.byLo, 3, 10, QChar('0'))
                    .arg(this->get(id, champ_velRange).rValue.byLo, 3, 10, QChar('0'))
                    .arg(name);
            emit(changeElementOrder(id, order, true));
        }
    }break;
    }

    // Create and store the action
    if (storeAction)
    {
        Action *action = new Action();
        action->typeAction = Action::TypeChangeToDefault;
        action->id = id;
        action->champ = champ;
        action->vOldValue = oldValue;
        this->_undoRedo->add(action);
    }
    return 1;
}

void Pile_sf2::simplify(EltID id, Champ champ)
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
    Valeur valeur;

    for (int i = 0; i < this->count(idElement); i++)
    {
        idElement.indexElt2 = i;
        if (!this->get(idElement, champ_hidden).bValue)
        {
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
    }

    if (!firstValue)
    {
        for (int i = 0; i < this->count(idElement); i++)
        {
            idElement.indexElt2 = i;
            if (!this->get(idElement, champ_hidden).bValue)
                this->reset(idElement, champ);
        }
        this->set(id, champ, valeur);
    }
}

bool Pile_sf2::SF2::BAG::isSet(Champ champ)
{
    bool result = false;

    foreach (SF2::BAG::GEN gen, this->_gens)
    {
        if (gen.sfGenOper == champ)
        {
            result = true;
            break;
        }
    }

    return result;
}

Valeur Pile_sf2::SF2::BAG::getGen(Champ champ)
{
    Valeur value;
    value.genValue.wAmount = 0;

    foreach (SF2::BAG::GEN gen, this->_gens)
    {
        if (gen.sfGenOper == champ)
        {
            value.genValue = gen.genAmount;
            break;
        }
    }

    return value;
}

void Pile_sf2::SF2::BAG::setGen(Champ champ, Valeur value)
{
    // Existing property?
    int index = -1;
    for (int i = 0; i < _gens.count(); i++)
    {
        if (_gens[i].sfGenOper == champ)
        {
            index = i;
            break;
        }
    }

    if (index == -1)
    {
        // Create a new gen
        GEN gen;
        gen.sfGenOper = champ;
        _gens << gen;
        index = _gens.count() - 1;
    }

    // Set the value
    _gens[index].genAmount = value.genValue;
}

void Pile_sf2::SF2::BAG::resetGen(Champ champ)
{
    for (int i = _gens.count() - 1; i >= 0; i--)
        if (_gens[i].sfGenOper == champ)
            _gens.removeAt(i);
}

void Pile_sf2::SF2::BAG::supprGenAndStore(EltID id, int storeAction, Pile_sf2 *root)
{
    while (!_gens.isEmpty())
    {
        SF2::BAG::GEN genTmp = _gens.takeFirst();

        // Create and store an action
        if (storeAction)
        {
            Action * action = new Action();
            action->typeAction = Action::TypeChangeToDefault;
            action->id = id;
            action->vOldValue.genValue = genTmp.genAmount;
            action->champ = genTmp.sfGenOper;
            root->_undoRedo->add(action);
        }
    }
}

void Pile_sf2::SF2::INST::decrementerSMPL(int indexSmpl)
{
    foreach (SF2::BAG * bag, _bags)
    {
        int n = bag->getGen(champ_sampleID).wValue;
        if (bag->isSet(champ_sampleID) && n >= indexSmpl)
        {
            Valeur value;
            value.wValue = n-1;
            bag->setGen(champ_sampleID, value);
        }
    }
}

void Pile_sf2::SF2::decrementerLinkSMPL(int indexSmpl)
{
    foreach (SF2::SMPL * smpl, _smpl)
        if (smpl->_wSampleLink >= indexSmpl)
            smpl->_wSampleLink = smpl->_wSampleLink - 1;
}

void Pile_sf2::SF2::PRST::decrementerINST(int indexInst)
{
    foreach (SF2::BAG * bag, _bags)
    {
        int n = bag->getGen(champ_instrument).wValue;
        if (bag->isSet(champ_instrument) && n >= indexInst)
        {
            Valeur value;
            value.wValue = n-1;
            bag->setGen(champ_instrument, value);
        }
    }
}

void Pile_sf2::SF2::BAG::enleverMod(int index)
{
    // suppression de la référence à index
    // décrémentation des références supérieures
    for (int i = 0; i < _mods.count(); i++)
    {
        if ((int)_mods[i]._sfModDestOper >= 32768)
        {
            // mode "link"
            if (_mods[i]._sfModDestOper - 32768 == index)
                _mods[i]._sfModDestOper = (Champ)0;
            else if (_mods[i]._sfModDestOper > index + 32768)
                _mods[i]._sfModDestOper = (Champ)(_mods[i]._sfModDestOper - 1);
        }
    }
}

int Pile_sf2::display(EltID id)
{
    if (!this->isValid(id, 1))
        return 1;

    // Type d'élément à afficher (suite à une suppression non définitive)
    switch ((int)id.typeElement)
    {
    case elementSf2:{
        // affichage d'un SF2
        SF2 *tmp = _sf2[id.indexSf2];
        tmp->_hidden = 0;
        emit(hideElement(id, false));
    }break;
    case elementSmpl:{
        // affichage d'un sample
        SF2::SMPL *tmp = _sf2[id.indexSf2]->_smpl[id.indexElt];
        tmp->_hidden = 0;
        emit(hideElement(id, false));
    }break;
    case elementInst:{
        // affichage d'un instrument
        SF2::INST *tmp = _sf2[id.indexSf2]->_inst[id.indexElt];
        tmp->_hidden = 0;
        emit(hideElement(id, false));
    }break;
    case elementPrst:{
        // affichage d'un preset
        SF2::PRST *tmp = _sf2[id.indexSf2]->_prst[id.indexElt];
        tmp->_hidden = 0;
        emit(hideElement(id, false));
    }break;
    case elementInstSmpl:{
        // affichage d'un sample lié à un instrument
        SF2::BAG *tmp = _sf2[id.indexSf2]->_inst[id.indexElt]->_bags[id.indexElt2];
        tmp->_hidden = 0;
        emit(hideElement(id, false));
    }break;
    case elementPrstInst:{
        // affichage d'un instrument lié à un preset
        SF2::BAG *tmp = _sf2[id.indexSf2]->_prst[id.indexElt]->_bags[id.indexElt2];
        tmp->_hidden = 0;
        emit(hideElement(id, false));
    }break;
    case elementInstMod:{
        // affichage d'un mod d'un instrument
        SF2::BAG::MOD *tmp = &_sf2[id.indexSf2]->_inst[id.indexElt]->_bagGlobal._mods[id.indexMod];
        tmp->_hidden = 0;
    }break;
    case elementPrstMod:{
        // affichage d'un mod d'un preset
        SF2::BAG::MOD *tmp = &_sf2[id.indexSf2]->_prst[id.indexElt]->_bagGlobal._mods[id.indexMod];
        tmp->_hidden = 0;
    }break;
    case elementInstSmplMod:{
        // affichage d'un mod d'un sample lié à un instrument
        SF2::BAG::MOD *tmp = &_sf2[id.indexSf2]->_inst[id.indexElt]->_bags[id.indexElt2]->_mods[id.indexMod];
        tmp->_hidden = 0;
    }break;
    case elementPrstInstMod:{
        // affichage d'un mod d'un instrument lié à un preset
        SF2::BAG::MOD *tmp = &_sf2[id.indexSf2]->_prst[id.indexElt]->_bags[id.indexElt2]->_mods[id.indexMod];
        tmp->_hidden = 0;
    }break;
    }
    return 0;
}

bool Pile_sf2::isValid(EltID id, bool acceptHidden)
{
    SF2 *sf2Tmp;
    if (id.typeElement < elementSf2 || id.typeElement > elementPrstInstGen)
        return false;

    if (id.indexSf2 < 0)
        return (id.typeElement == elementSf2 || id.typeElement == elementRootSmpl ||
                id.typeElement == elementRootInst || id.typeElement == elementRootPrst);

    // Check indexSf2
    if (id.indexSf2 >= _sf2.count())
        return false;
    sf2Tmp = _sf2[id.indexSf2];

    if (sf2Tmp->_hidden && !acceptHidden)
        return false;

    if (id.indexElt < 0 && id.typeElement != elementSf2 && id.typeElement != elementRootInst &&
            id.typeElement != elementRootSmpl && id.typeElement != elementRootPrst)
        return (id.typeElement == elementSmpl || id.typeElement == elementInst || id.typeElement == elementPrst);

    if (id.typeElement == elementSmpl)
    {
        // Check indexElt
        if (id.indexElt >= sf2Tmp->_smpl.count())
            return false;

        if (sf2Tmp->_smpl[id.indexElt]->_hidden && !acceptHidden)
            return false;
    }
    else if (id.typeElement == elementInst || id.typeElement == elementInstSmpl ||
             id.typeElement == elementInstMod || id.typeElement == elementInstSmplMod ||
             id.typeElement == elementInstGen || id.typeElement == elementInstSmplGen)
    {
        // Check indexElt
        if (id.indexElt >= sf2Tmp->_inst.count())
            return false;

        SF2::INST *inst = sf2Tmp->_inst[id.indexElt];
        if (inst->_hidden && !acceptHidden)
            return false;

        if (id.typeElement == elementInstSmpl || id.typeElement == elementInstSmplMod || id.typeElement == elementInstSmplGen)
        {
            if (id.indexElt2 < 0 && id.typeElement != elementInstSmplMod && id.typeElement != elementInstSmplGen)
                return (id.typeElement == elementInstSmpl);

            // Vérification qu'indexElt2 est correct
            if (id.indexElt2 >= inst->_bags.count())
                return false;
            SF2::BAG * bag = inst->_bags[id.indexElt2];

            if (bag->_hidden && !acceptHidden)
                return false;

            if (id.typeElement == elementInstSmplMod || id.typeElement == elementInstSmplGen)
            {
                if (id.indexMod < 0)
                    return true;

                // Check indexMod
                if (id.typeElement == elementInstSmplMod)
                {
                    if (id.indexMod >= bag->_mods.count())
                        return false;

                    if (bag->_mods[id.indexMod]._hidden && !acceptHidden)
                        return false;
                }
                else
                {
                    if (id.indexMod >= bag->_gens.count())
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
                if (id.indexMod >= inst->_bagGlobal._mods.count())
                    return false;

                if (inst->_bagGlobal._mods[id.indexMod]._hidden && !acceptHidden)
                    return false;
            }
            else
            {
                if (id.indexMod >= inst->_bagGlobal._gens.count())
                    return false;
            }
        }
    }
    else if (id.typeElement == elementPrst || id.typeElement == elementPrstInst ||
             id.typeElement == elementPrstMod || id.typeElement == elementPrstInstMod ||
             id.typeElement == elementPrstGen || id.typeElement == elementPrstInstGen)
    {
        // Check indexElt
        if (id.indexElt >= sf2Tmp->_prst.count())
            return false;

        SF2::PRST *prst = sf2Tmp->_prst[id.indexElt];
        if (prst->_hidden && !acceptHidden)
            return false;

        if (id.typeElement == elementPrstInst || id.typeElement == elementPrstInstMod || id.typeElement == elementPrstInstGen)
        {
            if (id.indexElt2 < 0 && id.typeElement != elementPrstInstMod && id.typeElement != elementPrstInstGen)
                return (id.typeElement == elementPrstInst);

            // Vérification qu'indexElt2 est correct
            if (id.indexElt2 >= prst->_bags.count())
                return false;
            SF2::BAG * bag = prst->_bags[id.indexElt2];

            if (bag->_hidden && !acceptHidden)
                return false;

            if (id.typeElement == elementPrstInstMod || id.typeElement == elementPrstInstGen)
            {
                if (id.indexMod < 0)
                    return true;

                // Check indexMod
                if (id.typeElement == elementPrstInstMod)
                {
                    if (id.indexMod >= bag->_mods.count())
                        return false;

                    if (bag->_mods[id.indexMod]._hidden && !acceptHidden)
                        return false;
                }
                else
                {
                    if (id.indexMod >= bag->_gens.count())
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
                if (id.indexMod >= prst->_bagGlobal._mods.count())
                    return false;

                if (prst->_bagGlobal._mods[id.indexMod]._hidden && !acceptHidden)
                    return false;
            }
            else
            {
                if (id.indexMod >= prst->_bagGlobal._gens.count())
                    return false;
            }
        }
    }

    return true;
}

void Pile_sf2::firstAvailablePresetBank(EltID id, int &nBank, int &nPreset)
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

int Pile_sf2::closestAvailablePreset(EltID id, quint16 wBank, quint16 wPreset)
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

bool Pile_sf2::isAvailable(EltID id, quint16 wBank, quint16 wPreset)
{
    id.typeElement = elementPrst;
    for (int i = 0; i < this->count(id); i++)
    {
        id.indexElt = i;
        if (!this->get(id, champ_hidden).bValue)
        {
            if (this->get(id, champ_wBank).wValue == wBank &&
                    this->get(id, champ_wPreset).wValue == wPreset)
                return false;
        }
    }
    return true;
}
