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
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "pile_sf2.h"
#include "tree.h"

#define UNDO_NUMBER 100

bool Pile_sf2::CONFIG_RAM = 0;

// CONSTRUCTEURS
Pile_sf2::Pile_sf2(Tree *tree, bool ram)
{
    this->sf2 = NULL;
    this->tree = tree;
    this->pileActions = new Pile_actions();
    this->CONFIG_RAM = ram;
}
Pile_sf2::~Pile_sf2()
{
    delete this->sf2;
    delete pileActions;
}
Pile_sf2::SF2::SF2()
{
    SfVersionTag sfVersionTmp;
    sfVersionTmp.wMajor = 2;
    sfVersionTmp.wMinor = 4;
    this->IFIL = sfVersionTmp;
    sfVersionTmp.wMajor = 0;
    sfVersionTmp.wMinor = 0;
    this->IVER = sfVersionTmp;
    this->ISNG = "EMU8000";
    this->INAM = "";
    this->IROM = "";
    this->ICRD = "";
    this->IENG = "";
    this->IPRD = "";
    this->ICOP = "";
    this->ICMT = "";
    this->ISFT = "";
    this->suivant = NULL;
    this->eltTree = NULL;
    this->eltTreeSmpl = NULL;
    this->eltTreeInst = NULL;
    this->eltTreePrst = NULL;
    this->fileName = "";
    this->hidden = 0;
    this->numEdition = 0;
    this->smpl = NULL;
    this->inst = NULL;
    this->prst = NULL;
}
Pile_sf2::SF2::SMPL::SMPL() :
    eltTree(NULL),
    hidden(false),
    suivant(NULL)
{}
Pile_sf2::SF2::INST::INST() :
    bag(NULL),
    eltTree(NULL),
    hidden(false),
    suivant(NULL)
{}
Pile_sf2::SF2::PRST::PRST() :
    wPreset(0),
    wBank(0),
    dwLibrary(0),
    dwGenre(0),
    dwMorphology(0),
    bag(NULL),
    eltTree(NULL),
    hidden(false),
    suivant(NULL)
{}
Pile_sf2::SF2::BAG::BAG() :
    mod(NULL),
    gen(NULL),
    eltTree(NULL),
    hidden(false),
    suivant(NULL)
{}
Pile_sf2::SF2::BAG::MOD::MOD() :
    hidden(false),
    suivant(NULL)
{}
Pile_sf2::SF2::BAG::GEN::GEN() : suivant(NULL)
{}

///////////////////////// METHODES PUBLIQUES /////////////////////////

// Ajout / suppression des données
void Pile_sf2::remove(EltID id, int *message) { this->remove(id, 0, 1, message); }

// Accès / modification des propriétés
bool Pile_sf2::isSet(EltID id, Champ champ)
{
    bool value = false;
    if (!this->isValide(id, champ == champ_hidden))
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
        SF2::INST *tmp = this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt);
        switch ((int)champ)
        {
        case champ_hidden:
            value = true; break;
        default:
            value = tmp->bagGlobal.gen->isSet(champ);
        }
        }break;
    case elementPrst:{
        // Analyse d'un preset
        SF2::PRST *tmp = this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt);
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
            value = tmp->bagGlobal.gen->isSet(champ);
        }
        }break;
    case elementInstSmpl:{
        // Analyse d'un sample lié à un instrument
        SF2::BAG *tmp = this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bag->getElt(id.indexElt2);
        switch ((int)champ)
        {
        case champ_hidden:
            value = true; break;
        default:
            value = tmp->gen->isSet(champ);
        }
        }break;
    case elementPrstInst:{
        // Analyse d'un instrument lié à un preset
        SF2::BAG *tmp = this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bag->getElt(id.indexElt2);
        switch ((int)champ)
        {
        case champ_hidden:
            value = true; break;
        default:
            value = tmp->gen->isSet(champ);
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
    if (!this->isValide(id, champ == champ_hidden))
        return value;

    // Type d'élément à analyser
    switch ((int)id.typeElement)
    {
    case elementSf2:{
        // Analyse d'un SF2
        SF2 *tmp = this->sf2->getElt(id.indexSf2);
        switch ((int)champ)
        {
        case champ_IFIL:
            value.sfVerValue = tmp->IFIL; break;
        case champ_IVER:
            value.sfVerValue = tmp->IVER; break;
        case champ_hidden:
            value.bValue = tmp->hidden; break;
        case champ_wBpsInit:
            value.wValue = tmp->wBpsInit; break;
        case champ_wBpsSave:
            value.wValue = tmp->wBpsSave; break;
        }
        }break;
    case elementSmpl:{
        // Analyse d'un sample
        SF2::SMPL *tmp = this->sf2->getElt(id.indexSf2)->smpl->getElt(id.indexElt);
        switch ((int)champ)
        {
        case champ_dwStart16:
            value.dwValue = tmp->son.get(champ_dwStart16); break;
        case champ_dwStart24:
            value.dwValue = tmp->son.get(champ_dwStart24); break;
        case champ_dwLength:
            value.dwValue = tmp->son.get(champ_dwLength); break;
        case champ_dwStartLoop:
            value.dwValue = tmp->son.get(champ_dwStartLoop); break;
        case champ_dwEndLoop:
            value.dwValue = tmp->son.get(champ_dwEndLoop); break;
        case champ_dwSampleRate:
            value.dwValue = tmp->son.get(champ_dwSampleRate); break;
        case champ_bpsFile:
            value.dwValue = tmp->son.get(champ_bpsFile); break;
        case champ_wChannel:
            value.dwValue = tmp->son.get(champ_wChannel); break;
        case champ_wChannels:
            value.dwValue = tmp->son.get(champ_wChannels); break;
        case champ_byOriginalPitch:
            value.bValue = tmp->son.get(champ_byOriginalPitch); break;
        case champ_chPitchCorrection:
            value.cValue = tmp->son.get(champ_chPitchCorrection); break;
        case champ_wSampleLink:
            value.wValue = tmp->wSampleLink; break;
        case champ_sfSampleType:
            value.sfLinkValue = tmp->sfSampleType; break;
        case champ_hidden:
            value.bValue = tmp->hidden; break;
        }
        }break;
    case elementInst:{
        // Analyse d'un instrument
        SF2::INST *tmp = this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt);
        switch ((int)champ)
        {
        case champ_hidden:
            value.bValue = tmp->hidden; break;
        default:
            value = tmp->bagGlobal.gen->getGen(champ);
        }
        }break;
    case elementPrst:{
        // Analyse d'un preset
        SF2::PRST *tmp = this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt);
        switch ((int)champ)
        {
        case champ_wPreset:
            value.wValue = tmp->wPreset; break;
        case champ_wBank:
            value.wValue = tmp->wBank; break;
        case champ_dwLibrary:
            value.dwValue = tmp->dwLibrary; break;
        case champ_dwGenre:
            value.dwValue = tmp->dwGenre; break;
        case champ_dwMorphology:
            value.dwValue = tmp->dwMorphology; break;
        case champ_hidden:
            value.bValue = tmp->hidden; break;
        default:
            value = tmp->bagGlobal.gen->getGen(champ);
        }
        }break;
    case elementInstSmpl:{
        // Analyse d'un sample lié à un instrument
        SF2::BAG *tmp = this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bag->getElt(id.indexElt2);
        switch ((int)champ)
        {
        case champ_hidden:
            value.bValue = tmp->hidden; break;
        default:
            value = tmp->gen->getGen(champ);
        }
        }break;
    case elementPrstInst:{
        // Analyse d'un instrument lié à un preset
        SF2::BAG *tmp = this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bag->getElt(id.indexElt2);
        switch ((int)champ)
        {
        case champ_hidden:
            value.bValue = tmp->hidden; break;
        default:
            value = tmp->gen->getGen(champ);
        }
        }break;
    case elementInstMod: case elementPrstMod: case elementInstSmplMod: case elementPrstInstMod:{
        // Analyse d'un mod
        SF2::BAG::MOD *tmp = NULL;
        switch ((int)id.typeElement)
        {
        case elementInstMod:
            tmp = this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bagGlobal.mod->getElt(id.indexMod);
            break;
        case elementPrstMod:
            tmp = this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bagGlobal.mod->getElt(id.indexMod);
            break;
        case elementInstSmplMod:
            tmp = this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->mod->getElt(id.indexMod);
            break;
        case elementPrstInstMod:
            tmp = this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->mod->getElt(id.indexMod);
            break;
        }
        switch ((int)champ)
        {
        case champ_sfModSrcOper:
            value.sfModValue = tmp->sfModSrcOper; break;
        case champ_sfModDestOper:
            value.sfGenValue = tmp->sfModDestOper; break;
        case champ_modAmount:
            value.shValue = tmp->modAmount; break;
        case champ_sfModAmtSrcOper:
            value.sfModValue = tmp->sfModAmtSrcOper; break;
        case champ_sfModTransOper:
            value.sfTransValue = tmp->sfModTransOper; break;
        case champ_indexMod:
            value.wValue = tmp->index; break;
        case champ_hidden:
            value.bValue = tmp->hidden; break;
        }
        }break;
    case elementInstGen: case elementPrstGen: case elementInstSmplGen: case elementPrstInstGen:{
        // Analyse d'un gen
        SF2::BAG::GEN *tmp = NULL;
        switch ((int)id.typeElement)
        {
        case elementInstGen:
            tmp = this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bagGlobal.gen->getElt(id.indexMod);
            break;
        case elementPrstGen:
            tmp = this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bagGlobal.gen->getElt(id.indexMod);
            break;
        case elementInstSmplGen:
            tmp = this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->gen->getElt(id.indexMod);
            break;
        case elementPrstInstGen:
            tmp = this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->gen->getElt(id.indexMod);
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
    if (!this->isValide(id, 0))
        return son;

    if (id.typeElement == elementSmpl)
        return this->sf2->getElt(id.indexSf2)->smpl->getElt(id.indexElt)->son;
    else return son;
}
QString Pile_sf2::getQstr(EltID id, Champ champ)
{
    if (!this->isValide(id, false))
        return "";

    // Type d'élément à analyser
    switch ((int)id.typeElement)
    {
    case elementSf2:{
        // Analyse d'un SF2
        SF2 *tmp = this->sf2->getElt(id.indexSf2);
        switch ((int)champ)
        {
        case champ_name:
            return tmp->INAM; break;
        case champ_ISNG:
            return tmp->ISNG; break;
        case champ_IROM:
            return tmp->IROM; break;
        case champ_ICRD:
            return tmp->ICRD; break;
        case champ_IENG:
            return tmp->IENG; break;
        case champ_IPRD:
            return tmp->IPRD; break;
        case champ_ICOP:
            return tmp->ICOP; break;
        case champ_ICMT:
            return tmp->ICMT; break;
        case champ_ISFT:
            return tmp->ISFT; break;
        case champ_filename:
            return tmp->fileName; break;
        }
        }break;
    case elementSmpl:{
        // Analyse d'un sample
        SF2::SMPL *tmp = this->sf2->getElt(id.indexSf2)->smpl->getElt(id.indexElt);
        switch ((int)champ)
        {
        case champ_name:
            return tmp->Name; break;
        case champ_filename:
            return tmp->son.getFileName(); break;
        }
        }break;
    case elementInst:{
        // Analyse d'un instrument
        SF2::INST *tmp = this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt);
        switch ((int)champ)
        {
        case champ_name:
            return tmp->Name; break;
        }
        }break;
    case elementPrst:{
        // Analyse d'un preset
        SF2::PRST *tmp = this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt);
        switch ((int)champ)
        {
        case champ_name:
            return tmp->Name; break;
        }
        }break;
    }
    return "";
}
QByteArray Pile_sf2::getData(EltID id, Champ champ)
{
    if (!this->isValide(id, 0))
        return QByteArray();

    // Type d'élément à analyser
    switch ((int)id.typeElement)
    {
    case elementSmpl:{
        SF2::SMPL *tmp = this->sf2->getElt(id.indexSf2)->smpl->getElt(id.indexElt);
        switch ((int)champ)
        {
        case champ_sampleData16:
            return tmp->son.getData(16);
            break;
        case champ_sampleData24:
            return tmp->son.getData(8);
            break;
        case champ_sampleDataFull24:
            return tmp->son.getData(24);
            break;
        case champ_sampleData32:
            return tmp->son.getData(32);
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
    if (!this->isValide(id, 1))
        return -1;

    int i = 0;
    // Type d'élément à compter
    switch ((int)id.typeElement)
    {
    case elementSf2:{
        // Comptage du nombre de SF2
        SF2 *tmp = this->sf2;
        while (tmp)
        {
            if (!tmp->hidden || withHidden)
                i++;
            tmp = tmp->suivant;
        }
        return i;
        }break;
    case elementSmpl:{
        // compte du nombre de samples
        SF2::SMPL *tmp = this->sf2->getElt(id.indexSf2)->smpl;
        while (tmp)
        {
            if (!tmp->hidden || withHidden)
                i++;
            tmp = tmp->suivant;
        }
        return i;
        }break;
    case elementInst:{
        // compte du nombre d'instruments
        SF2::INST *tmp = this->sf2->getElt(id.indexSf2)->inst;
        while (tmp)
        {
            if (!tmp->hidden || withHidden)
                i++;
            tmp = tmp->suivant;
        }
        return i;
        }break;
    case elementPrst:{
        // compte du nombre de presets
        SF2::PRST *tmp = this->sf2->getElt(id.indexSf2)->prst;
        while (tmp)
        {
            if (!tmp->hidden || withHidden)
                i++;
            tmp = tmp->suivant;
        }
        return i;
        }break;
    case elementInstSmpl:{
        // compte du nombre de samples liés à un instrument
        SF2::BAG *tmp = this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bag;
        while (tmp)
        {
            if (!tmp->hidden || withHidden)
                i++;
            tmp = tmp->suivant;
        }
        return i;
        }break;
    case elementPrstInst:{
        // compte du nombre d'instruments liés à un preset
        SF2::BAG *tmp = this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bag;
        while (tmp)
        {
            if (!tmp->hidden || withHidden)
                i++;
            tmp = tmp->suivant;
        }
        return i;
        }break;
    case elementInstMod:{
        // compte du nombre de mods d'un instrument
        SF2::BAG::MOD *tmp = this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bagGlobal.mod;
        while (tmp)
        {
            i++;
            tmp = tmp->suivant;
        }
        return i;
        }break;
    case elementPrstMod:{
        // compte du nombre de mods d'un preset
        SF2::BAG::MOD *tmp = this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bagGlobal.mod;
        while (tmp)
        {
            i++;
            tmp = tmp->suivant;
        }
        return i;
        }break;
    case elementInstSmplMod:{
        // compte du nombre de mods d'un sample lié à un instrument
        SF2::BAG::MOD *tmp = this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->mod;
        while (tmp)
        {
            i++;
            tmp = tmp->suivant;
        }
        return i;
        }break;
    case elementPrstInstMod:{
        // compte du nombre de mods d'un instrument lié à un preset
        SF2::BAG::MOD *tmp = this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->mod;
        while (tmp)
        {
            i++;
            tmp = tmp->suivant;
        }
        return i;
        }break;
    case elementInstGen:{
        // compte du nombre de gens d'un instrument
        SF2::BAG::GEN *tmp = this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bagGlobal.gen;
        while (tmp)
        {
            i++;
            tmp = tmp->suivant;
        }
        return i;
        }break;
    case elementPrstGen:{
        // compte du nombre de gens d'un preset
        SF2::BAG::GEN *tmp = this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bagGlobal.gen;
        while (tmp)
        {
            i++;
            tmp = tmp->suivant;
        }
        return i;
        }break;
    case elementInstSmplGen:{
        // compte du nombre de gens d'un sample lié à un instrument
        SF2::BAG::GEN *tmp = this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->gen;
        while (tmp)
        {
            i++;
            tmp = tmp->suivant;
        }
        return i;
        }break;
    case elementPrstInstGen:{
        // compte du nombre de gens d'un instrument lié à un preset
        SF2::BAG::GEN *tmp = this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->gen;
        while (tmp)
        {
            i++;
            tmp = tmp->suivant;
        }
        return i;
        }break;
    }
    return -1;
}

// Gestionnaire d'actions
void Pile_sf2::releaseActions(bool withVeryOldActions)
{
    // suppression définitive des redo
    Pile_actions::Action *action;
    for (int i = this->pileActions->nombreEltRedo()-1; i >= 0; i--)
    {
        // Parcours des actions
        action = this->pileActions->getEltRedo(i);
        while (action)
        {
            if (action->typeAction == Pile_actions::actionCreer)
            {
                // Suppression définitive de l'élément anciennement créé puis supprimé
                this->remove(action->id, true, false);

                // Mise à jour tableau
                emit(updateTable((int)action->id.typeElement, action->id.indexSf2,
                                 action->id.indexElt, action->id.indexElt2));
            }
            action = action->suivant;
        }
        // Suppression de la liste d'actions
        this->pileActions->supprimerRedo(i);
    }

    if (withVeryOldActions)
    {
        // suppression définitive des undo après un certain nombre
        for (int i = this->pileActions->nombreEltUndo()-1; i >= UNDO_NUMBER - 1; i--)
        {
            // Parcours des actions en sens inverse
            action = this->pileActions->getEltUndo(i);
            for (int j = action->nombreElt() - 1; j >= 0; j--)
            {
                Pile_actions::Action * actionTmp = action->getElt(j);
                if (actionTmp->typeAction == Pile_actions::actionSupprimer)
                {
                    // Suppression définitive d'un élément supprimé depuis longtemps
                    int message = 1;
                    this->remove(actionTmp->id, true, false, &message);

                    // Mise à jour tableau
                    emit(updateTable((int)actionTmp->id.typeElement, actionTmp->id.indexSf2,
                                     actionTmp->id.indexElt, actionTmp->id.indexElt2));
                }
            }

            // Modification de l'édition du sf2 s'il n'a jamais été sauvegardé
            action = this->pileActions->getEltUndo(i);
            if (action)
            {
                if (this->sf2->getElt(action->id.indexSf2)->numEdition == -1)
                    this->sf2->getElt(action->id.indexSf2)->numEdition = -2;
            }
            // Suppression de la liste d'actions
            this->pileActions->supprimerUndo(i);
        }
    }
}

void Pile_sf2::prepareNewActions(bool removeOldActions)
{
    releaseActions(removeOldActions);

    // Nouvelle action
    this->pileActions->nouvelleAction();
}

void Pile_sf2::cleanActions() {this->pileActions->cleanActions();}

bool Pile_sf2::isUndoable() {return this->pileActions->isUndoable();}

bool Pile_sf2::isRedoable() {return this->pileActions->isRedoable();}

void Pile_sf2::undo()
{
    Pile_actions::Action *action = this->pileActions->undo();
    // Déroulement dans l'ordre
    while (action)
    {
        switch ((int)action->typeAction)
        {
        case Pile_actions::actionCreer:{
            // Masquage de l'élément
            int message = 1;
            this->remove(action->id, 0, 0, &message);
            }break;
        case Pile_actions::actionSupprimer:
            // Affichage de l'élément
            this->display(action->id);
            break;
        case Pile_actions::actionModifier:
            // Retour à l'ancienne valeur
            if (action->champ >= 0 && action->champ < 164) this->set(action->id, action->champ, action->vOldValue, 0); // Valeur
            else if (action->champ >= 164 && action->champ < 200) this->set(action->id, action->champ, action->qOldValue, 0); // QString
            else if (action->champ >=200) this->set(action->id, action->champ, action->baOldValue, 0); // char*
            break;
        case Pile_actions::actionModifierToDefault:
            // Retour à l'ancienne valeur
            if (action->champ >= 0 && action->champ < 164) this->set(action->id, action->champ, action->vOldValue, 0); // Valeur
            else if (action->champ >= 164 && action->champ < 200) this->set(action->id, action->champ, action->qOldValue, 0); // QString
            else if (action->champ >=200) this->set(action->id, action->champ, action->baOldValue, 0); // char*
            break;
        case Pile_actions::actionModifierFromDefault:
            // Retour à l'ancienne valeur, reset
            this->reset(action->id, action->champ, 0);
            break;
        }
        action = action->suivant;
    }
}
void Pile_sf2::redo()
{
    Pile_actions::Action *listeActions = this->pileActions->redo();
    // Déroulement inverse
    Pile_actions::Action *action;
    int j;
    for (int i = listeActions->nombreElt()-1; i >= 0; i--)
    {
        action = listeActions; j = i;
        while (j-- && action) action = action->suivant;
        switch ((int)action->typeAction)
        {
        case Pile_actions::actionCreer:
            // Affichage de l'élément
            this->display(action->id);
            break;
        case Pile_actions::actionSupprimer:{
            // Masquage de l'élément
            int message = 1;
            this->remove(action->id, 0, 0, &message);
            }break;
        case Pile_actions::actionModifier:
            // Réapplication de la nouvelle valeur
            if (action->champ >= 0 && action->champ < 164) this->set(action->id, action->champ, action->vNewValue, 0); // Valeur
            else if (action->champ >= 164 && action->champ < 200) this->set(action->id, action->champ, action->qNewValue, 0); // QString
            else if (action->champ >=200) this->set(action->id, action->champ, action->baNewValue, 0); // char*
            break;
        case Pile_actions::actionModifierToDefault:
            // Réapplication de la nouvelle valeur, reset
            this->reset(action->id, action->champ, 0);
            break;
        case Pile_actions::actionModifierFromDefault:
            // Réapplication de la nouvelle valeur
            if (action->champ >= 0 && action->champ < 164) this->set(action->id, action->champ, action->vNewValue, 0); // Valeur
            else if (action->champ >= 164 && action->champ < 200) this->set(action->id, action->champ, action->qNewValue, 0); // QString
            else if (action->champ >=200) this->set(action->id, action->champ, action->baNewValue, 0); // char*
            break;
        }
    }
}
// Gestion de la sauvegarde
void Pile_sf2::storeEdition(int indexSf2) {this->sf2->getElt(indexSf2)->numEdition = this->pileActions->getEdition(indexSf2);}
bool Pile_sf2::isEdited(int indexSf2) {return this->pileActions->getEdition(indexSf2) != this->sf2->getElt(indexSf2)->numEdition;}

// Récupération liste de champs et valeurs de bags
void Pile_sf2::getListeBags(EltID id, QList<Champ> &listeChamps, QList<genAmountType> &listeValeurs)
{
    if (!this->isValide(id))
        return;

    SF2::BAG::GEN * gen = NULL;
    switch (id.typeElement)
    {
    case elementInst:
        gen = this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bagGlobal.gen;
        break;
    case elementInstSmpl:
        gen = this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->gen;
        break;
    case elementPrst:
        gen = this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bagGlobal.gen;
        break;
    case elementPrstInst:
        gen = this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->gen;
        break;
    default:
        break;
    }
    while (gen)
    {
        listeChamps << gen->sfGenOper;
        listeValeurs << gen->genAmount;
        gen = gen->suivant;
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
    if (!this->isValide(id))
        return -1;

    int i = -1;
    // Type d'élément à ajouter
    switch ((int)id.typeElement)
    {
    case elementSf2:{
        // Création d'un nouvel SF2
        storeAction = false; // pas de retour possible
        SF2 *sf2 = new SF2();
        sf2->suivant = NULL;
        sf2->smpl = NULL;
        sf2->inst = NULL;
        sf2->prst = NULL;
        sf2->numEdition = 0;
        i = 0;
        if (this->sf2 == NULL)
            this->sf2 = sf2;
        else
        {
            SF2 *tmp = this->sf2;
            i = 1;
            while (tmp->suivant)
            {
                tmp = tmp->suivant;
                i++;
            }
            tmp->suivant = sf2;
        }
        id.indexSf2 = i;
        if (tree)
        {
            // Ajout d'un élément graphique
            sf2->eltTree = new TreeWidgetItem(this->tree);
            QString str = QString::number(i);
            sf2->eltTree->setText(1, str);
            sf2->eltTree->setText(2, "R");
            QFont font = sf2->eltTree->font(0);
            font.setPointSize(15);
            font.setBold(true);
            sf2->eltTree->setFont(0, font);
            sf2->eltTree->setSizeHint(0, QSize(0,30));
            sf2->eltTree->setIcon(0, QIcon(":/icones/document"));

            // Conteneurs pour samples, instruments et presets
            sf2->eltTreeSmpl = new QTreeWidgetItem(sf2->eltTree);
            sf2->eltTreeSmpl->setText(0, QObject::trUtf8("Échantillons"));
            sf2->eltTreeSmpl->setText(1, str);
            sf2->eltTreeSmpl->setText(2, "S");
            sf2->eltTreeSmpl->setText(5, "a");
            sf2->eltTreeSmpl->setSizeHint(0, QSize(0,23));
            font.setPointSize(10);
            sf2->eltTreeSmpl->setFont(0, font);
            sf2->eltTreeInst = new QTreeWidgetItem(sf2->eltTree);
            sf2->eltTreeInst->setText(0, QObject::trUtf8("Instruments"));
            sf2->eltTreeInst->setText(1, str);
            sf2->eltTreeInst->setText(2, "I");
            sf2->eltTreeInst->setText(5, "b");
            sf2->eltTreeInst->setSizeHint(0, QSize(0,23));
            sf2->eltTreeInst->setFont(0, font);
            sf2->eltTreePrst = new QTreeWidgetItem(sf2->eltTree);
            sf2->eltTreePrst->setText(0, QObject::trUtf8("Presets"));
            sf2->eltTreePrst->setText(1, str);
            sf2->eltTreePrst->setText(2, "P");
            sf2->eltTreePrst->setText(5, "c");
            sf2->eltTreePrst->setSizeHint(0, QSize(0,23));
            sf2->eltTreePrst->setFont(0, font);

            this->tree->addSf2InComboBox(id.indexSf2);
            this->tree->trier(1);
        }

        // initialisation bps
        Valeur valTmp;
        valTmp.wValue = 16;
        this->set(id, champ_wBpsInit, valTmp, false);
        this->set(id, champ_wBpsSave, valTmp, false);
        }break;
    case elementSmpl:{
        // Création d'un nouveau sample
        SF2 *sf2 = this->sf2->getElt(id.indexSf2);
        SF2::SMPL *smpl = new SF2::SMPL;
        smpl->suivant = NULL;
        i = 0;
        if (sf2->smpl == NULL)
            sf2->smpl = smpl;
        else
        {
            SF2::SMPL *tmp = sf2->smpl;
            i = 1;
            while (tmp->suivant)
            {
                tmp = tmp->suivant;
                i++;
            }
            tmp->suivant = smpl;
        }
        id.indexElt = i;
        if (tree)
        {
            // Ajout d'un élément graphique
            smpl->eltTree = new TreeWidgetItem(sf2->eltTreeSmpl);
            smpl->eltTree->setText(1, QString::number(id.indexSf2));
            smpl->eltTree->setText(2, "smpl");
            smpl->eltTree->setText(3, QString::number(i));
            smpl->eltTree->setText(6, "0");
            smpl->eltTree->setSizeHint(0, QSize(0, 17));
            smpl->eltTree->setIcon(0, QIcon(":/icones/wave"));
        }
        }break;
    case elementInst:{
        // Création d'un nouvel instrument
        SF2 *sf2 = this->sf2->getElt(id.indexSf2);
        SF2::INST *inst = new SF2::INST;
        inst->suivant = NULL;
        i = 0;
        if (sf2->inst == NULL)
            sf2->inst = inst;
        else
        {
            SF2::INST *tmp = sf2->inst;
            i = 1;
            while (tmp->suivant)
            {
                tmp = tmp->suivant;
                i++;
            }
            tmp->suivant = inst;
        }
        id.indexElt = i;
        if (tree)
        {
            // Ajout d'un élément graphique
            inst->eltTree = new TreeWidgetItem(sf2->eltTreeInst);
            inst->eltTree->setText(1, QString::number(id.indexSf2));
            inst->eltTree->setText(2, "inst");
            inst->eltTree->setText(3, QString::number(i));
            inst->eltTree->setText(6, "0");
            inst->eltTree->setSizeHint(0, QSize(0, 17));
            inst->eltTree->setIcon(0, QIcon(":/icones/sound"));
        }
        }break;
    case elementPrst:{
        // Création d'un nouveau preset
        SF2 *sf2 = this->sf2->getElt(id.indexSf2);
        SF2::PRST *prst = new SF2::PRST;
        prst->suivant = NULL;
        i = 0;
        if (sf2->prst == NULL)
            sf2->prst= prst;
        else
        {
            SF2::PRST *tmp = sf2->prst;
            i = 1;
            while (tmp->suivant)
            {
                tmp = tmp->suivant;
                i++;
            }
            tmp->suivant = prst;
        }
        id.indexElt = i;
        if (tree)
        {
            // Ajout d'un élément graphique
            prst->eltTree = new TreeWidgetItem(sf2->eltTreePrst);
            prst->eltTree->setText(1, QString::number(id.indexSf2));
            prst->eltTree->setText(2, "prst");
            prst->eltTree->setText(3, QString::number(i));
            prst->eltTree->setText(6, "0");
            prst->eltTree->setSizeHint(0, QSize(0, 17));
            prst->eltTree->setIcon(0, QIcon(":/icones/music"));
        }
        }break;
    case elementInstSmpl:{
        // Ajout d'un nouveau sample pour un instrument
        SF2::INST *inst = this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt);
        SF2::BAG *bag = new SF2::BAG;
        bag->suivant = NULL;
        i = 0;
        if (inst->bag == NULL)
            inst->bag = bag;
        else
        {
            SF2::BAG *tmp = inst->bag;
            i = 1;
            while (tmp->suivant)
            {
                tmp = tmp->suivant;
                i++;
            }
            tmp->suivant = bag;
        }
        id.indexElt2 = i;
        if (tree)
        {
            // Ajout d'un élément graphique
            bag->eltTree = new TreeWidgetItem(inst->eltTree);
            bag->eltTree->setText(1, QString::number(id.indexSf2));
            bag->eltTree->setText(2, "IS");
            bag->eltTree->setText(3, QString::number(id.indexElt));
            bag->eltTree->setText(4, QString::number(i));
            bag->eltTree->setText(6, "0");
            bag->eltTree->setSizeHint(0, QSize(0, 17));
            bag->eltTree->setIcon(0, QIcon(":/icones/wave"));
        }
        }break;
    case elementPrstInst:{
        // Ajout d'un nouvel instrument pour un preset
        SF2::PRST *prst = this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt);
        SF2::BAG *bag = new SF2::BAG;
        bag->suivant = NULL;
        i = 0;
        if (prst->bag == NULL)
            prst->bag = bag;
        else
        {
            SF2::BAG *tmp = prst->bag;
            i = 1;
            while (tmp->suivant)
            {
                tmp = tmp->suivant;
                i++;
            }
            tmp->suivant = bag;
        }
        id.indexElt2 = i;
        if (tree)
        {
            // Ajout d'un élément graphique
            bag->eltTree = new TreeWidgetItem(prst->eltTree);
            bag->eltTree->setText(1, QString::number(id.indexSf2));
            bag->eltTree->setText(2, "PI");
            bag->eltTree->setText(3, QString::number(id.indexElt));
            bag->eltTree->setText(4, QString::number(i));
            bag->eltTree->setText(6, "0");
            bag->eltTree->setSizeHint(0, QSize(0, 17));
            bag->eltTree->setIcon(0, QIcon(":/icones/sound"));
        }
        }break;
    case elementInstMod: case elementPrstMod: case elementInstSmplMod: case elementPrstInstMod:{
        // Ajout d'un nouveau mod
        SF2::BAG *bag = NULL;
        SF2::BAG::MOD *mod = new SF2::BAG::MOD;
        switch ((int)id.typeElement)
        {
        case elementInstMod:
            // pour un instrument
            bag = &this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bagGlobal;
            break;
        case elementPrstMod:
            // pour un preset
            bag = &this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bagGlobal;
            break;
        case elementInstSmplMod:
            // pour un sample associé à un instrument
            bag = this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bag->getElt(id.indexElt2);
            break;
        case elementPrstInstMod:
            // pour un instrument associé à un preset
            bag = this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bag->getElt(id.indexElt2);
        }
        mod->suivant = NULL;
        i = 0;
        if (bag->mod == NULL)
            bag->mod = mod;
        else
        {
            SF2::BAG::MOD *tmp = bag->mod;
            i = 1;
            while (tmp->suivant)
            {
                tmp = tmp->suivant;
                i++;
            }
            tmp->suivant = mod;
        }
        id.indexMod = i;
        // renseignement de l'index
        Valeur val;
        val.wValue = id.indexMod;
        this->set(id, champ_indexMod, val, 0);
        }break;
    }
    // Création et stockage de l'action
    if (storeAction)
    {
        Pile_actions::Action *action = new Pile_actions::Action;
        action->typeAction = Pile_actions::actionCreer;
        action->id = id;
        this->pileActions->add(action);
    }
    return i;
}

int Pile_sf2::remove(EltID id, bool permanently, bool storeAction, int *message)
{
    if (!this->isValide(id, permanently)) // Les ID masqués sont acceptés pour une suppression définitive
        return 1;

    // Type d'élément à supprimer
    switch ((int)id.typeElement)
    {
    case elementSf2:{
        permanently = true; // pas de retour après fermeture d'un fichier
        storeAction = false;
        // suppression d'un SF2
        // nombre de presets associés
        EltID id2(elementPrst, id.indexSf2, 0, 0, 0);
        int max = this->count(id2);
        for (int i = max - 1; i >= 0; i--)
        {
            // propagation aux presets /!\ si temporaire, ne pas propager aux éléments déjà supprimés de manière temporaire
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
        this->pileActions->decrementer(id);

        // Ajustement de la numérotation
        this->sf2->getElt(id.indexSf2)->decrementerSF2();
        if (tree)
        {
            // Suppression des éléments graphiques
            delete this->sf2->getElt(id.indexSf2)->eltTreePrst;
            delete this->sf2->getElt(id.indexSf2)->eltTreeInst;
            delete this->sf2->getElt(id.indexSf2)->eltTreeSmpl;
            delete this->sf2->getElt(id.indexSf2)->eltTree;
        }
        // Suppression du sf2
        SF2 *tmp = this->sf2->getElt(id.indexSf2)->suivant;
        this->sf2->getElt(id.indexSf2)->suivant = NULL;
        delete this->sf2->getElt(id.indexSf2);
        if (id.indexSf2 == 0)
            this->sf2 = tmp;
        else
            this->sf2->getElt(id.indexSf2-1)->suivant = tmp;

        if (tree)
        {
            this->tree->removeSf2FromComboBox(id.indexSf2);
            this->tree->updateAtNextSelectionRequest();
        }
        }break;
    case elementSmpl:{
        // suppression d'un sample
        // vérification qu'aucun instrument n'utilise le sample
        int resultat = 0;
        SF2::INST *instTmp = this->sf2->getElt(id.indexSf2)->inst;
        SF2::BAG *bagTmp;
        while (instTmp)
        {
            if (!instTmp->hidden)
            {
                bagTmp = instTmp->bag;
                while (bagTmp)
                {
                    if (!bagTmp->hidden)
                    {
                        if (bagTmp->gen->isSet(champ_sampleID) && bagTmp->gen->getGen(champ_sampleID).wValue == id.indexElt)
                            resultat = 1;
                    }
                    bagTmp = bagTmp->suivant;
                }

            }
            instTmp = instTmp->suivant;
        }
        if (resultat)
        {
            if (*message %2 != 0 || !message)
                *message *= 2;
            return 1;
        }
        // sample lié ?
        EltID id2(elementSmpl, id.indexSf2, 0, 0, 0);
        int i = -1;
        Valeur value;
        SF2::SMPL *smplTmp = this->sf2->getElt(id.indexSf2)->smpl;
        while (smplTmp)
        {
            i++;
            if (!smplTmp->hidden)
            {
                if (smplTmp->wSampleLink == id.indexElt)
                {
                    id2.indexElt = i;
                    value.wValue = 0;
                    this->set(id2, champ_wSampleLink, value, storeAction);
                    value.sfLinkValue = monoSample;
                    this->set(id2, champ_sfSampleType, value, storeAction);
                }
            }
            smplTmp = smplTmp->suivant;
        }
        if (permanently)
        {
            // Ajustement de la numérotation dans les actions
            this->pileActions->decrementer(id);
            // Ajustement de la numérotation
            this->sf2->getElt(id.indexSf2)->smpl->getElt(id.indexElt)->decrementerSMPL();
            // Décrémentation des samples liés aux instruments
            for (int i = 0; i < this->sf2->getElt(id.indexSf2)->inst->nombreElt(); i++)
                this->sf2->getElt(id.indexSf2)->inst->getElt(i)->bag->decrementerSMPL(id.indexElt);
            // Décrémentation des samples liés par stéréo
            this->sf2->getElt(id.indexSf2)->smpl->decrementerLinkSMPL(id.indexElt);
            // Suppression de l'élément graphique
            if (tree)
                delete this->sf2->getElt(id.indexSf2)->smpl->getElt(id.indexElt)->eltTree;
            // Suppression du sample
            SF2::SMPL *tmp = this->sf2->getElt(id.indexSf2)->smpl->getElt(id.indexElt)->suivant;
            this->sf2->getElt(id.indexSf2)->smpl->getElt(id.indexElt)->suivant = NULL;
            delete this->sf2->getElt(id.indexSf2)->smpl->getElt(id.indexElt);
            if (id.indexElt == 0) this->sf2->getElt(id.indexSf2)->smpl = tmp;
            else this->sf2->getElt(id.indexSf2)->smpl->getElt(id.indexElt-1)->suivant = tmp;
        }
        else
        {
            if (tree)
            {
                // Masquage de l'élément graphique
                this->sf2->getElt(id.indexSf2)->smpl->getElt(id.indexElt)->eltTree->setHidden(1);
                this->sf2->getElt(id.indexSf2)->smpl->getElt(id.indexElt)->eltTree->setText(6, "1");
            }
            // Masquage du sample
            this->sf2->getElt(id.indexSf2)->smpl->getElt(id.indexElt)->hidden = 1;
        }
        if (tree)
            this->tree->updateAtNextSelectionRequest();
        }break;
    case elementInst:{
        // suppression d'un instrument
        // vérification qu'aucun preset n'utilise l'instrument
        int resultat = 0;
        SF2::PRST *prstTmp = this->sf2->getElt(id.indexSf2)->prst;
        SF2::BAG *bagTmp;
        while (prstTmp)
        {
            if (!prstTmp->hidden)
            {
                bagTmp = prstTmp->bag;
                while (bagTmp)
                {
                    if (!bagTmp->hidden)
                    {
                        if (bagTmp->gen->isSet(champ_instrument) && bagTmp->gen->getGen(champ_instrument).wValue == id.indexElt)
                            resultat = 1;
                    }
                    bagTmp = bagTmp->suivant;
                }

            }
            prstTmp = prstTmp->suivant;
        }
        if (resultat)
        {
            if (*message %3 != 0 || !message)
                (*message) = *message * 3;
            return 1;
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
            this->pileActions->decrementer(id);
            // Ajustement de la numérotation
            this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->decrementerINST();
            // Décrémentation des instruments liés aux presets
            for (int i = 0; i < this->sf2->getElt(id.indexSf2)->prst->nombreElt(); i++)
                this->sf2->getElt(id.indexSf2)->prst->getElt(i)->bag->decrementerINST(id.indexElt);
            // Suppression des gens
            this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bagGlobal.gen = \
                this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bagGlobal.gen->supprGenAndStore(id, storeAction, this);
            // Suppression de l'élément graphique
            if (tree)
                delete this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->eltTree;
            // Suppression de l'instrument
            SF2::INST *tmp = this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->suivant;
            this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->suivant = NULL;
            delete this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt);
            if (id.indexElt == 0) this->sf2->getElt(id.indexSf2)->inst = tmp;
            else this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt-1)->suivant = tmp;
        }
        else
        {
            // Suppression des gens
            this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bagGlobal.gen =
                this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bagGlobal.gen->supprGenAndStore(id, storeAction, this);
            if (tree)
            {
                // Masquage de l'élément graphique
                this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->eltTree->setHidden(1);
                this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->eltTree->setText(6, "1");
            }
            // Masquage de l'instrument
            this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->hidden = 1;
        }
        if (tree)
            this->tree->updateAtNextSelectionRequest();
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
            this->pileActions->decrementer(id);
            // Ajustement de la numérotation
            this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->decrementerPRST();
            // Suppression des gens
            this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bagGlobal.gen =
                this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bagGlobal.gen->supprGenAndStore(id, storeAction, this);
            // Suppression de l'élément graphique
            if (tree)
                delete this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->eltTree;
            // Suppression du preset
            SF2::PRST *tmp = this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->suivant;
            this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->suivant = NULL;
            delete this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt);
            if (id.indexElt == 0) this->sf2->getElt(id.indexSf2)->prst = tmp;
            else this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt-1)->suivant = tmp;
        }
        else
        {
            // Suppression des gens
            this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bagGlobal.gen =
                this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bagGlobal.gen->supprGenAndStore(id, storeAction, this);
            if (tree)
            {
                // Masquage de l'élément graphique
                this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->eltTree->setHidden(1);
                this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->eltTree->setText(6, "1");
            }
            // Masquage du preset
            this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->hidden = 1;
        }
        if (tree)
            this->tree->updateAtNextSelectionRequest();
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
            this->pileActions->decrementer(id);
            // Ajustement de la numérotation
            this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->decrementerBAG();
            // Suppression des gens
            this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->gen =
                this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->gen->supprGenAndStore(id, storeAction, this);
            // Suppression des éléments graphiques
            if (tree)
                delete this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->eltTree;
            // Suppression du sample lié à l'instrument
            SF2::BAG *tmp = this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->suivant;
            this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->suivant = NULL;
            delete this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bag->getElt(id.indexElt2);
            if (id.indexElt2 == 0) this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bag = tmp;
            else this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bag->getElt(id.indexElt2-1)->suivant = tmp;
        }
        else
        {
            // Suppression des gens
            this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->gen =
                this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->gen->supprGenAndStore(id, storeAction, this);
            if (tree)
            {
                // Masquage des éléments graphiques
                this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->eltTree->setHidden(1);
                this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->eltTree->setText(6, "1");
            }
            // Masquage du sample lié à l'instrument
            this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->hidden = 1;
        }
        if (tree)
            this->tree->updateAtNextSelectionRequest();
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
            this->pileActions->decrementer(id);
            // Ajustement de la numérotation
            this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->decrementerBAG();
            // Suppression des gens
            this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->gen =
                this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->gen->supprGenAndStore(id, storeAction, this);
            // Suppression des éléments graphiques
            if (tree)
                delete this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->eltTree;
            // Suppression de l'instrument lié au preset
            SF2::BAG *tmp = this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->suivant;
            this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->suivant = NULL;
            delete this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bag->getElt(id.indexElt2);
            if (id.indexElt2 == 0) this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bag = tmp;
            else this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bag->getElt(id.indexElt2-1)->suivant = tmp;
        }
        else
        {
            // Suppression des gens
            this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->gen =
                this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->gen->supprGenAndStore(id, storeAction, this);
            if (tree)
            {
                // Masquage des éléments graphiques
                this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->eltTree->setHidden(1);
                this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->eltTree->setText(6, "1");
            }
            // Masquage de l'instrument lié au preset
            this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->hidden = 1;
        }
        if (tree)
            this->tree->updateAtNextSelectionRequest();
        }break;
    case elementInstMod: case elementPrstMod: case elementInstSmplMod: case elementPrstInstMod:{
        // suppression d'un mod
        SF2::BAG *bag = NULL;
        switch ((int)id.typeElement)
        {
        case elementInstMod:
            // d'un instrument
            bag = &this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bagGlobal;
            break;
        case elementPrstMod:
            // d'un preset
            bag = &this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bagGlobal;
            break;
        case elementInstSmplMod:
            // d'un sample lié à un instrument
            bag = this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bag->getElt(id.indexElt2);
            break;
        case elementPrstInstMod:
            // d'un instrument lié à un prst
            bag = this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bag->getElt(id.indexElt2);
        }
        if (permanently)
        {
            // Ajustement de la numérotation dans les actions
            this->pileActions->decrementer(id);
            // ajustement de la numérotation
            bag->mod->enleverMod(id.indexMod);
            // Le mod pointe sur un autre ?
            int iVal = bag->mod->getElt(id.indexMod)->sfModDestOper;
            if (iVal >= 32768 && iVal - 32768 < this->count(id))
            {
                if (bag->mod->getElt(iVal-32768)->sfModSrcOper.CC == 0 && \
                        bag->mod->getElt(iVal-32768)->sfModSrcOper.Index == 127)
                    bag->mod->getElt(iVal-32768)->sfModSrcOper.Index = 0;
            }
            // suppression du mod
            SF2::BAG::MOD * tmp = bag->mod->getElt(id.indexMod)->suivant;
            bag->mod->getElt(id.indexMod)->suivant = NULL;
            delete bag->mod->getElt(id.indexMod);
            if (id.indexMod == 0)
                bag->mod = tmp;
            else
                bag->mod->getElt(id.indexMod - 1)->suivant = tmp;
        }
        else
        {
            // masquage du mod
            bag->mod->getElt(id.indexMod)->hidden = true;
        }
        }break;
    }
    // Création et stockage de l'action
    if (storeAction)
    {
        Pile_actions::Action *action = new Pile_actions::Action;
        action->typeAction = Pile_actions::actionSupprimer;
        action->id = id;
        this->pileActions->add(action);
    }
    return 0;
}

int Pile_sf2::set(EltID id, Champ champ, Valeur value, bool storeAction, bool sort)
{
    if (champ == champ_hidden)
        return 1;

    if (!this->isValide(id) && champ != champ_ram)
        return 1;

    Valeur oldValue;
    oldValue.wValue = 0;
    int defaultValue = 0;

    // Type d'élément à modifier
    switch ((int)id.typeElement)
    {
    case elementSf2:{
        // Modification d'un SF2
        SF2 *tmp = this->sf2->getElt(id.indexSf2);
        switch ((int)champ)
        {
        case champ_IFIL:
            oldValue.sfVerValue = tmp->IFIL;
            tmp->IFIL = value.sfVerValue; break;
        case champ_IVER:
            oldValue.sfVerValue = tmp->IVER;
            tmp->IVER = value.sfVerValue; break;
        case champ_wBpsInit:
            oldValue.wValue = tmp->wBpsInit;
            tmp->wBpsInit = value.wValue; break;
        case champ_wBpsSave:
            oldValue.wValue = tmp->wBpsSave;
            tmp->wBpsSave = value.wValue; break;
        }
        }break;
    case elementSmpl:{
        // Modification d'un sample
        SF2::SMPL *tmp = this->sf2->getElt(id.indexSf2)->smpl->getElt(id.indexElt);
        switch ((int)champ)
        {
        case champ_dwStart16:
            oldValue.dwValue = tmp->son.get(champ_dwStart16);
            tmp->son.set(champ_dwStart16, value); break;
        case champ_dwStart24:
            oldValue.dwValue = tmp->son.get(champ_dwStart24);
            tmp->son.set(champ_dwStart24, value); break;
        case champ_dwLength:
            oldValue.dwValue = tmp->son.get(champ_dwLength);
            tmp->son.set(champ_dwLength, value); break;
        case champ_dwStartLoop:
            oldValue.dwValue = tmp->son.get(champ_dwStartLoop);
            tmp->son.set(champ_dwStartLoop, value); break;
        case champ_dwEndLoop:
            oldValue.dwValue = tmp->son.get(champ_dwEndLoop);
            tmp->son.set(champ_dwEndLoop, value); break;
        case champ_dwSampleRate:
            oldValue.dwValue = tmp->son.get(champ_dwSampleRate);
            tmp->son.set(champ_dwSampleRate, value); break;
        case champ_byOriginalPitch:
            oldValue.bValue = tmp->son.get(champ_byOriginalPitch);
            tmp->son.set(champ_byOriginalPitch, value); break;
        case champ_chPitchCorrection:
            oldValue.cValue = tmp->son.get(champ_chPitchCorrection);
            tmp->son.set(champ_chPitchCorrection, value); break;
        case champ_wSampleLink:
            oldValue.wValue = tmp->wSampleLink;
            tmp->wSampleLink = value.wValue; break;
        case champ_sfSampleType:
            oldValue.sfLinkValue = tmp->sfSampleType;
            tmp->sfSampleType = value.sfLinkValue; break;
        case champ_bpsFile:
            oldValue.wValue = tmp->son.get(champ_bpsFile);
            tmp->son.set(champ_bpsFile, value); break;
        case champ_wChannel:
            oldValue.wValue = tmp->son.get(champ_wChannel);
            tmp->son.set(champ_wChannel, value); break;
        case champ_wChannels:
            oldValue.wValue = tmp->son.get(champ_wChannels);
            tmp->son.set(champ_wChannels, value); break;
        case champ_ram:
            // pas d'enregistrement de l'action
            storeAction = false;
            tmp->son.setRam(value.wValue);
            break;
        }
        }break;
    case elementInst:{
        // Modification d'un instrument
        SF2::INST *tmp = this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt);
        if (!tmp->bagGlobal.gen->isSet(champ)) defaultValue = 1;
        oldValue = tmp->bagGlobal.gen->getGen(champ);
        tmp->bagGlobal.gen = tmp->bagGlobal.gen->setGen(champ, value);
        }break;
    case elementPrst:{
        // Modification d'un preset
        SF2::PRST *tmp = this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt);
        switch ((int)champ)
        {
        case champ_wPreset:
            oldValue.wValue = tmp->wPreset;
            tmp->wPreset = value.wValue; break;
        case champ_wBank:
            oldValue.wValue = tmp->wBank;
            tmp->wBank = value.wValue; break;
        case champ_dwLibrary:
            oldValue.dwValue = tmp->dwLibrary;
            tmp->dwLibrary = value.dwValue; break;
        case champ_dwGenre:
            oldValue.dwValue = tmp->dwGenre;
            tmp->dwGenre = value.dwValue; break;
        case champ_dwMorphology:
            oldValue.dwValue = tmp->dwMorphology;
            tmp->dwMorphology = value.dwValue; break;
        default:
            if (!tmp->bagGlobal.gen->isSet(champ)) defaultValue = 1;
            oldValue = tmp->bagGlobal.gen->getGen(champ);
            tmp->bagGlobal.gen = tmp->bagGlobal.gen->setGen(champ, value);
        }
        if ((champ == champ_wPreset || champ == champ_wBank) && tree)
        {
            // Modification de l'élément graphique
            QString text = QString("%1:%2 %3")
                    .arg(tmp->wBank, 3, 10, QChar('0'))
                    .arg(tmp->wPreset, 3, 10, QChar('0'))
                    .arg(tmp->Name);
            tmp->eltTree->setText(0, text);
            tmp->eltTree->setText(5, text);
            if (sort)
                this->sf2->getElt(id.indexSf2)->eltTreePrst->sortChildren(5, Qt::AscendingOrder);
        }
        }break;
    case elementInstSmpl:{
        // Modification d'un sample lié à un instrument
        SF2::BAG *tmp = this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bag->getElt(id.indexElt2);
        if (!tmp->gen->isSet(champ)) defaultValue = 1;
        oldValue = tmp->gen->getGen(champ);
        tmp->gen = tmp->gen->setGen(champ, value);
        if (champ == champ_sampleID && tree)
        {
            // Nom du sample lié
            EltID id2(elementSmpl, id.indexSf2, value.wValue, 0, 0);
            QString qStr = this->getQstr(id2, champ_name);
            tmp->eltTree->setText(0, qStr);
            // Modification élément graphique
            char str[20];
            sprintf(str, "%.3d", this->get(id, champ_keyRange).rValue.byLo);
            QString qStr2 = QString(str);
            tmp->eltTree->setText(5, qStr2.append(qStr));
            if (sort)
                this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->eltTree->sortChildren(5, Qt::AscendingOrder);
        }
        else if (champ == champ_keyRange && tree)
        {
            // Modification élément graphique
            char str[20];
            sprintf(str, "%.3d", value.rValue.byLo);
            QString qStr = QString(str);
            EltID id2(elementSmpl, id.indexSf2, this->get(id, champ_sampleID).wValue, 0, 0);
            qStr.append(this->getQstr(id2, champ_name));
            tmp->eltTree->setText(5, qStr);
            if (sort)
                this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->eltTree->sortChildren(5, Qt::AscendingOrder);
        }
        }break;
    case elementPrstInst:{
        // Modification d'un instrument lié à un preset
        SF2::BAG *tmp = this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bag->getElt(id.indexElt2);
        if (!tmp->gen->isSet(champ)) defaultValue = 1;
        oldValue = tmp->gen->getGen(champ);
        tmp->gen = tmp->gen->setGen(champ, value);
        if (champ == champ_instrument && tree)
        {
            // Nom de l'instrument lié
            EltID id2(elementInst, id.indexSf2, value.wValue, 0, 0);
            QString qStr = this->getQstr(id2, champ_name);
            tmp->eltTree->setText(0, qStr);
            // Modification élément graphique
            char str[20];
            sprintf(str, "%.3d", this->get(id, champ_keyRange).rValue.byLo);
            QString qStr2 = QString(str);
            tmp->eltTree->setText(5, qStr2.append(qStr));
            if (sort)
                this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->eltTree->sortChildren(5, Qt::AscendingOrder);
        }
        else if (champ == champ_keyRange && tree)
        {
            // Modification élément graphique
            char str[20];
            sprintf(str, "%.3d", value.rValue.byLo);
            QString qStr = QString(str);
            EltID id2(elementInst, id.indexSf2, this->get(id, champ_instrument).wValue, 0, 0);
            qStr.append(this->getQstr(id2, champ_name));
            tmp->eltTree->setText(5, qStr);
            if (sort)
                this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->eltTree->sortChildren(5, Qt::AscendingOrder);
        }
        }break;
    case elementInstMod: case elementPrstMod: case elementInstSmplMod: case elementPrstInstMod:{
        // Modification d'un mod d'un instrument
        SF2::BAG::MOD *tmp = NULL;
        switch ((int)id.typeElement)
        {
        case elementInstMod:
            tmp = this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bagGlobal.mod->getElt(id.indexMod); break;
        case elementPrstMod:
            tmp = this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bagGlobal.mod->getElt(id.indexMod); break;
        case elementInstSmplMod:
            tmp = this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->mod->getElt(id.indexMod); break;
        case elementPrstInstMod:
            tmp = this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->mod->getElt(id.indexMod); break;
        }
        switch ((int)champ)
        {
        case champ_sfModSrcOper:
            oldValue.sfModValue = tmp->sfModSrcOper;
            tmp->sfModSrcOper = value.sfModValue; break;
        case champ_sfModDestOper:
            oldValue.sfGenValue = tmp->sfModDestOper;
            tmp->sfModDestOper = value.sfGenValue; break;
        case champ_modAmount:
            oldValue.shValue = tmp->modAmount;
            tmp->modAmount = value.shValue; break;
        case champ_sfModAmtSrcOper:
            oldValue.sfModValue = tmp->sfModAmtSrcOper;
            tmp->sfModAmtSrcOper = value.sfModValue; break;
        case champ_sfModTransOper:
            oldValue.sfTransValue = tmp->sfModTransOper;
            tmp->sfModTransOper = value.sfTransValue; break;
        case champ_indexMod:
            oldValue.wValue = tmp->index;
            tmp->index = value.wValue; break;
        }
        }break;
    }
    // Création et stockage de l'action
    if (storeAction)
        {
        Pile_actions::Action *action = new Pile_actions::Action;
        if (defaultValue) action->typeAction = Pile_actions::actionModifierFromDefault;
        else action->typeAction = Pile_actions::actionModifier;
        action->id = id;
        action->champ = champ;
        action->vOldValue = oldValue;
        action->vNewValue = value;
        this->pileActions->add(action);
    }
    return 0;
}
int Pile_sf2::set(EltID id, Champ champ, QString qStr, bool storeAction, bool sort)
{
    if (!this->isValide(id))
        return 1;

    QString qOldStr = "";
    // Type d'élément à modifier
    switch ((int)id.typeElement)
    {
    case elementSf2:{
        // Modification d'un SF2
        SF2 *tmp = this->sf2->getElt(id.indexSf2);
        switch ((int)champ)
        {
        case champ_name:
            qOldStr = tmp->INAM;
            qStr = qStr.left(256);
            tmp->INAM = qStr;
            if (tree)
            {
                // Modification de l'élément graphique
                tmp->eltTree->setText(0, qStr);
                tmp->eltTree->setText(5, qStr);
                this->tree->renameSf2InComboBox(id.indexSf2, qStr);
                this->tree->trier(0);
            }
            break;
        case champ_ISNG:
            qOldStr = tmp->ISNG;
            tmp->ISNG = qStr.left(255); break;
        case champ_IROM:
            qOldStr = tmp->IROM;
            tmp->IROM = qStr.left(255); break;
        case champ_ICRD:
            qOldStr = tmp->ICRD;
            tmp->ICRD = qStr.left(255); break;
        case champ_IENG:
            qOldStr = tmp->IENG;
            tmp->IENG = qStr.left(255); break;
        case champ_IPRD:
            qOldStr = tmp->IPRD;
            tmp->IPRD = qStr.left(255); break;
        case champ_ICOP:
            qOldStr = tmp->ICOP;
            tmp->ICOP = qStr.left(255); break;
        case champ_ICMT:
            qOldStr = tmp->ICMT;
            tmp->ICMT = qStr.left(65535); break;
        case champ_ISFT:
            qOldStr = tmp->ISFT;
            tmp->ISFT = qStr.left(255); break;
        case champ_filename:
            qOldStr = tmp->fileName;
            tmp->fileName = qStr;break;
        }
        }break;
    case elementSmpl:{
        qStr = qStr.trimmed();
        // Modification d'un sample
        SF2::SMPL *tmp = this->sf2->getElt(id.indexSf2)->smpl->getElt(id.indexElt);
        switch ((int)champ)
        {
        case champ_name:{
            // Modification du nom d'un sample
            qOldStr = tmp->Name;
            qStr = qStr.left(20);
            tmp->Name = qStr;
            if (tree)
            {
                // Modification de l'élément graphique
                tmp->eltTree->setText(0, qStr);
                tmp->eltTree->setText(5, qStr);
                // Modification des occurrences dans les instruments
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
                                if (this->get(id2, champ_sampleID).wValue == indSmpl)
                                    this->sf2->getElt(id2.indexSf2)->inst->getElt(id2.indexElt)->bag->getElt(id2.indexElt2)->eltTree->setText(0, qStr);
                        }
                    }
                }
                if (sort)
                    this->sf2->getElt(id.indexSf2)->eltTreeSmpl->sortChildren(5, Qt::AscendingOrder);
            }
            };break;
        case champ_filename:
            qOldStr = tmp->son.getFileName();
            tmp->son.setFileName(qStr);
            break;
        }
        }break;
    case elementInst:{
        qStr = qStr.trimmed();
        // Modification d'un instrument
        SF2::INST *tmp = this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt);
        switch ((int)champ)
        {
        case champ_name:
            // Modification du nom d'un instrument
            qOldStr = tmp->Name;
            qStr = qStr.left(20);
            tmp->Name = qStr;
            if (tree)
            {
                // Modification de l'élément graphique
                tmp->eltTree->setText(0, qStr);
                tmp->eltTree->setText(5, qStr);
                // Modification des occurrences dans les presets
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
                                if (this->get(id2, champ_instrument).wValue == indInst)
                                {
                                    this->sf2->getElt(id2.indexSf2)->prst->getElt(id2.indexElt)->bag->getElt(id2.indexElt2)->eltTree->setText(0, qStr);
                                    this->sf2->getElt(id2.indexSf2)->prst->getElt(id2.indexElt)->bag->getElt(id2.indexElt2)->eltTree->setText(5, qStr);
                                    this->sf2->getElt(id2.indexSf2)->prst->getElt(id2.indexElt)->eltTree->sortChildren(5, Qt::AscendingOrder);
                                }
                        }
                    }
                }
                if (sort)
                    this->sf2->getElt(id.indexSf2)->eltTreeInst->sortChildren(5, Qt::AscendingOrder);
            }
            break;
        }
        }break;
    case elementPrst:{
        qStr = qStr.trimmed();
        // Modification d'un preset
        SF2::PRST *tmp = this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt);
        switch ((int)champ)
        {
        case champ_name:
            // Modification du nom d'un preset
            qOldStr = tmp->Name;
            qStr = qStr.left(20);
            tmp->Name = qStr;
            if (tree)
            {
                // Modification de l'élément graphique
                char chaine[30];
                sprintf(chaine, "%.3d:%.3d %s", tmp->wBank, tmp->wPreset, tmp->Name.toStdString().c_str());
                tmp->eltTree->setText(0, chaine);
                tmp->eltTree->setText(5, chaine);
                if (sort)
                    this->sf2->getElt(id.indexSf2)->eltTreePrst->sortChildren(5, Qt::AscendingOrder);
            }
            break;
        }
        }break;
    }
    // Création et stockage de l'action
    if (storeAction)
    {
        Pile_actions::Action *action = new Pile_actions::Action;
        action->typeAction = Pile_actions::actionModifier;
        action->id = id;
        action->champ = champ;
        action->qOldValue = qOldStr;
        action->qNewValue = qStr;
        this->pileActions->add(action);
    }
    return 0;
}
int Pile_sf2::set(EltID id, Champ champ, QByteArray data, bool storeAction)
{
    if (!this->isValide(id))
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
            oldData = this->sf2->getElt(id.indexSf2)->smpl->getElt(id.indexElt)->son.getData(16);
            this->sf2->getElt(id.indexSf2)->smpl->getElt(id.indexElt)->son.setData(data, 16);
            break;
        case champ_sampleData24:
            oldData = this->sf2->getElt(id.indexSf2)->smpl->getElt(id.indexElt)->son.getData(8);
            this->sf2->getElt(id.indexSf2)->smpl->getElt(id.indexElt)->son.setData(data, 8);
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
        Pile_actions::Action *action = new Pile_actions::Action;
        action->typeAction = Pile_actions::actionModifier;
        action->id = id;
        action->champ = champ;
        action->baOldValue = oldData;
        action->baNewValue = data;
        this->pileActions->add(action);
    }
    return 0;
}

int Pile_sf2::reset(EltID id, Champ champ, bool storeAction)
{
    if (!this->isValide(id))
        return 0;

    Valeur oldValue;
    // Type d'élément à modifier
    switch ((int)id.typeElement)
    {
    case elementInst:{
        // Remise à zéro d'une propriété d'un instrument
        SF2::INST *tmp = this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt);
        if (!tmp->bagGlobal.gen->isSet(champ)) return 0;
        oldValue = tmp->bagGlobal.gen->getGen(champ);
        tmp->bagGlobal.gen = tmp->bagGlobal.gen->resetGen(champ);
        }break;
    case elementPrst:{
        // Remise à zéro d'une propriété d'un preset
        SF2::PRST *tmp = this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt);
        if (!tmp->bagGlobal.gen->isSet(champ)) return 0;
        oldValue = tmp->bagGlobal.gen->getGen(champ);
        tmp->bagGlobal.gen = tmp->bagGlobal.gen->resetGen(champ);
        }break;
    case elementInstSmpl:{
        // Remise à zéro d'une propriété d'un sample lié à un instrument
        SF2::BAG *tmp = this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bag->getElt(id.indexElt2);
        if (!tmp->gen->isSet(champ)) return 0;
        oldValue = tmp->gen->getGen(champ);
        tmp->gen = tmp->gen->resetGen(champ);
        if (champ == champ_sampleID && tree)
            tmp->eltTree->setText(0, "");
        else if (champ == champ_keyRange && tree)
        {
            // Modification élément graphique
            tmp->eltTree->setText(5, "");
            this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->eltTree->sortChildren(5, Qt::AscendingOrder);
        }
        }break;
    case elementPrstInst:{
        // Remise à zéro d'une propriété d'un instrument lié à un preset
        SF2::BAG *tmp = this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bag->getElt(id.indexElt2);
        if (!tmp->gen->isSet(champ)) return 0;
        oldValue = tmp->gen->getGen(champ);
        tmp->gen = tmp->gen->resetGen(champ);
        if (champ == champ_instrument && tree)
        {
            tmp->eltTree->setText(0, "");
            tmp->eltTree->setText(5, "");
            this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->eltTree->sortChildren(5, Qt::AscendingOrder);
        }
        }break;
    }
    // Création et stockage de l'action
    if (storeAction)
    {
        Pile_actions::Action *action = new Pile_actions::Action;
        action->typeAction = Pile_actions::actionModifierToDefault;
        action->id = id;
        action->champ = champ;
        action->vOldValue = oldValue;
        this->pileActions->add(action);
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

bool Pile_sf2::SF2::BAG::GEN::isSet(Champ champ)
{
    bool resultat = 0;
    if (this)
    {
        // Recherche si la propriété existe
        GEN *genTmp = this;
        while((genTmp->sfGenOper != champ) && genTmp->suivant != NULL)
        {
            genTmp = genTmp->suivant;
        };
        if (genTmp->sfGenOper == champ) resultat = 1;
    }
    return resultat;
}
Valeur Pile_sf2::SF2::BAG::GEN::getGen(Champ champ)
{
    Valeur value;
    value.genValue.wAmount = 0;
    if (this)
    {
        // Recherche si la propriété existe
        GEN *genTmp = this;
        while((genTmp->sfGenOper != champ) && genTmp->suivant != NULL)
            genTmp = genTmp->suivant;
        if (genTmp->sfGenOper == champ)
            value.genValue = genTmp->genAmount;
    }
    return value;
}
Pile_sf2::SF2::BAG::GEN * Pile_sf2::SF2::BAG::GEN::setGen(Champ champ, Valeur value)
{
    if (!this)
    {
        // Création gen
        GEN *gen = new GEN;
        gen->sfGenOper = champ;
        gen->genAmount = value.genValue;
        gen->suivant = NULL;
        return gen;
    }
    else
    {
        // Recherche si la propriété existe déjà
        GEN *genTmp = this;
        while(genTmp->sfGenOper != champ && genTmp->suivant != NULL)
        {
            genTmp = genTmp->suivant;
        };
        if (genTmp->sfGenOper == champ)
        {
            genTmp->genAmount = value.genValue;
        }
        else
        {
            // Création gen
            GEN *gen = new GEN;
            gen->sfGenOper = champ;
            gen->genAmount = value.genValue;
            gen->suivant = NULL;
            genTmp->suivant = gen;
        }
        return this;
    }
}
Pile_sf2::SF2::BAG::GEN * Pile_sf2::SF2::BAG::GEN::resetGen(Champ champ)
{
    if (this)
    {
        if (this->sfGenOper == champ)
        {
            GEN *genTmp = this->suivant;
            this->suivant = NULL;
            delete this;
            return genTmp->resetGen(champ);
        }
        else this->suivant = this->suivant->resetGen(champ);
    }
    return this;
}
Pile_sf2::SF2::BAG::GEN * Pile_sf2::SF2::BAG::GEN::supprGenAndStore(EltID id, int storeAction, Pile_sf2 *root)
{
    SF2::BAG::GEN *genTmp = this;
    SF2::BAG::GEN *genTmp2;
    Pile_actions::Action *action;
    while (genTmp)
    {
        // Création et sauvegarde d'une action
        if (storeAction)
        {
            action = new Pile_actions::Action;
            action->typeAction = Pile_actions::actionModifierToDefault;
            action->id = id;
            action->vOldValue.genValue = genTmp->genAmount;
            action->champ = genTmp->sfGenOper;
            root->pileActions->add(action);
        }
        // Destruction gen
        genTmp2 = genTmp;
        genTmp = genTmp->suivant;
        genTmp2->suivant = NULL;
        delete genTmp2;
    }
    return genTmp;
}

void Pile_sf2::SF2::decrementerSF2()
{
    if (this)
    {
        this->suivant->decrementerSF2();
        this->inst->decrementerSF2();
        this->prst->decrementerSF2();
        this->smpl->decrementerSF2();
        if (this->eltTreeInst) this->eltTreeInst->setText(1, decrementerQstr(this->eltTreeInst->text(1)));
        if (this->eltTreeSmpl) this->eltTreeSmpl->setText(1, decrementerQstr(this->eltTreeSmpl->text(1)));
        if (this->eltTreePrst) this->eltTreePrst->setText(1, decrementerQstr(this->eltTreePrst->text(1)));
        if (this->eltTree) this->eltTree->setText(1, decrementerQstr(this->eltTree->text(1)));
    }
}
void Pile_sf2::SF2::INST::decrementerSF2()
{
    if (this)
    {
        this->suivant->decrementerSF2();
        this->bag->decrementerSF2();
        if (this->eltTree) this->eltTree->setText(1, decrementerQstr(this->eltTree->text(1)));
    }
}
void Pile_sf2::SF2::SMPL::decrementerSF2()
{
    if (this)
    {
        this->suivant->decrementerSF2();
        if (this->eltTree) this->eltTree->setText(1, decrementerQstr(this->eltTree->text(1)));
    }
}
void Pile_sf2::SF2::PRST::decrementerSF2()
{
    if (this)
    {
        this->suivant->decrementerSF2();
        this->bag->decrementerSF2();
        if (this->eltTree) this->eltTree->setText(1, decrementerQstr(this->eltTree->text(1)));
    }
}
void Pile_sf2::SF2::BAG::decrementerSF2()
{
    if (this)
    {
        this->suivant->decrementerSF2();
        if (this->eltTree) this->eltTree->setText(1, decrementerQstr(this->eltTree->text(1)));
    }
}
void Pile_sf2::SF2::SMPL::decrementerSMPL()
{
    if (this)
    {
        this->suivant->decrementerSMPL();
        if (this->eltTree) this->eltTree->setText(3, decrementerQstr(this->eltTree->text(3)));
    }
}
void Pile_sf2::SF2::BAG::decrementerSMPL(int indexSmpl)
{
    if (this)
    {
        this->suivant->decrementerSMPL(indexSmpl);
        int n = this->gen->getGen(champ_sampleID).wValue;
        if (this->gen->isSet(champ_sampleID) && n >= indexSmpl)
        {
            Valeur value;
            value.wValue = n-1;
            this->gen->setGen(champ_sampleID, value);
        }
    }
}
void Pile_sf2::SF2::SMPL::decrementerLinkSMPL(int indexSmpl)
{
    if (this)
    {
        this->suivant->decrementerLinkSMPL(indexSmpl);
        if (this->wSampleLink >= indexSmpl) this->wSampleLink = this->wSampleLink - 1;
    }
}
void Pile_sf2::SF2::INST::decrementerINST()
{
    if (this)
    {
        this->suivant->decrementerINST();
        this->bag->decrementerINST_PRST();
        if (this->eltTree) this->eltTree->setText(3, decrementerQstr(this->eltTree->text(3)));
    }
}
void Pile_sf2::SF2::BAG::decrementerINST_PRST()
{
    if (this)
    {
        this->suivant->decrementerINST_PRST();
        if (this->eltTree) this->eltTree->setText(3, decrementerQstr(this->eltTree->text(3)));
    }
}
void Pile_sf2::SF2::BAG::decrementerINST(int indexInst)
{
    if (this)
    {
        this->suivant->decrementerINST(indexInst);
        int n = this->gen->getGen(champ_instrument).wValue;
        if (this->gen->isSet(champ_instrument) && n >= indexInst)
        {
            Valeur value;
            value.wValue = n-1;
            this->gen->setGen(champ_instrument, value);
        }
    }
}
void Pile_sf2::SF2::PRST::decrementerPRST()
{
    if (this)
    {
        this->suivant->decrementerPRST();
        this->bag->decrementerINST_PRST();
        if (this->eltTree) this->eltTree->setText(3, decrementerQstr(this->eltTree->text(3)));
    }
}
void Pile_sf2::SF2::BAG::decrementerBAG()
{
    if (this)
    {
        this->suivant->decrementerBAG();
        if (this->eltTree) this->eltTree->setText(4, decrementerQstr(this->eltTree->text(4)));
    }
}
void Pile_sf2::SF2::BAG::MOD::enleverMod(int index)
{
    // suppression de la référence à index
    // décrémentation des références supérieures
    if ((int)this->sfModDestOper >= 32768)
    {
        // mode "link"
        if (this->sfModDestOper - 32768 == index)
            this->sfModDestOper = (Champ)0;
        else if (this->sfModDestOper > index + 32768)
            this->sfModDestOper = (Champ)(this->sfModDestOper - 1);
    }
    // parcours des éléments suivants
    if (this->suivant)
        this->suivant->enleverMod(index);
}

int Pile_sf2::display(EltID id)
{
    if (!this->isValide(id, 1))
        return 1;

    // Type d'élément à afficher (suite à une suppression non définitive)
    switch ((int)id.typeElement)
    {
    case elementSf2:{
        // affichage d'un SF2
        SF2 *tmp = this->sf2->getElt(id.indexSf2);
        tmp->hidden = 0;
        if (tree)
        {
            tmp->eltTree->setHidden(0);
            tmp->eltTreeSmpl->setHidden(0);
            tmp->eltTreeInst->setHidden(0);
            tmp->eltTreePrst->setHidden(0);
        }
        }break;
    case elementSmpl:{
        // affichage d'un sample
        SF2::SMPL *tmp = this->sf2->getElt(id.indexSf2)->smpl->getElt(id.indexElt);
        tmp->hidden = 0;
        if (tree)
        {
            tmp->eltTree->setHidden(0);
            tmp->eltTree->setText(6, "0");
        }
        }break;
    case elementInst:{
        // affichage d'un instrument
        SF2::INST *tmp = this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt);
        tmp->hidden = 0;
        if (tree)
        {
            tmp->eltTree->setHidden(0);
            tmp->eltTree->setText(6, "0");
        }
        }break;
    case elementPrst:{
        // affichage d'un preset
        SF2::PRST *tmp = this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt);
        tmp->hidden = 0;
        if (tree)
        {
            tmp->eltTree->setHidden(0);
            tmp->eltTree->setText(6, "0");
        }
        }break;
    case elementInstSmpl:{
        // affichage d'un sample lié à un instrument
        SF2::BAG *tmp = this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bag->getElt(id.indexElt2);
        tmp->hidden = 0;
        if (tree)
        {
            tmp->eltTree->setHidden(0);
            tmp->eltTree->setText(6, "0");
        }
        }break;
    case elementPrstInst:{
        // affichage d'un instrument lié à un preset
        SF2::BAG *tmp = this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bag->getElt(id.indexElt2);
        tmp->hidden = 0;
        if (tree)
        {
            tmp->eltTree->setHidden(0);
            tmp->eltTree->setText(6, "0");
        }
        }break;
    case elementInstMod:{
        // affichage d'un mod d'un instrument
        SF2::BAG::MOD *tmp = this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bagGlobal.mod->getElt(id.indexMod);
        tmp->hidden = 0;
        }break;
    case elementPrstMod:{
        // affichage d'un mod d'un preset
        SF2::BAG::MOD *tmp = this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bagGlobal.mod->getElt(id.indexMod);
        tmp->hidden = 0;
        }break;
    case elementInstSmplMod:{
        // affichage d'un mod d'un sample lié à un instrument
        SF2::BAG::MOD *tmp = this->sf2->getElt(id.indexSf2)->inst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->mod->getElt(id.indexMod);
        tmp->hidden = 0;
        }break;
    case elementPrstInstMod:{
        // affichage d'un mod d'un instrument lié à un preset
        SF2::BAG::MOD *tmp = this->sf2->getElt(id.indexSf2)->prst->getElt(id.indexElt)->bag->getElt(id.indexElt2)->mod->getElt(id.indexMod);
        tmp->hidden = 0;
        }break;
    }
    return 0;
}

Pile_sf2::SF2::BAG::GEN * Pile_sf2::SF2::BAG::GEN::getElt(int pos){
    GEN *tmp = this;
    while (pos-- && tmp) tmp = tmp->suivant;
    return tmp;}
Pile_sf2::SF2::BAG::MOD * Pile_sf2::SF2::BAG::MOD::getElt(int pos){
    MOD *tmp = this;
    while (pos-- && tmp) tmp = tmp->suivant;
    return tmp;}
Pile_sf2::SF2::BAG * Pile_sf2::SF2::BAG::getElt(int pos){
    BAG *tmp = this;
    while (pos-- && tmp) tmp = tmp->suivant;
    return tmp;}
Pile_sf2::SF2::PRST * Pile_sf2::SF2::PRST::getElt(int pos){
    PRST *tmp = this;
    while (pos-- && tmp) tmp = tmp->suivant;
    return tmp;}
Pile_sf2::SF2::INST * Pile_sf2::SF2::INST::getElt(int pos){
    INST *tmp = this;
    while (pos-- && tmp) tmp = tmp->suivant;
    return tmp;}
Pile_sf2::SF2::SMPL * Pile_sf2::SF2::SMPL::getElt(int pos){
    SMPL *tmp = this;
    while (pos-- && tmp) tmp = tmp->suivant;
    return tmp;}
Pile_sf2::SF2 * Pile_sf2::SF2::getElt(int pos){
    SF2 *tmp = this;
    while (pos-- && tmp) tmp = tmp->suivant;
    return tmp;}
int Pile_sf2::SF2::BAG::nombreElt(){
    if (this) return 1 + this->suivant->nombreElt();
    else return 0;}
int Pile_sf2::SF2::BAG::MOD::nombreElt(){
    if (this) return 1 + this->suivant->nombreElt();
    else return 0;}
int Pile_sf2::SF2::BAG::GEN::nombreElt(){
    if (this) return 1 + this->suivant->nombreElt();
    else return 0;}
int Pile_sf2::SF2::PRST::nombreElt(){
    if (this) return 1 + this->suivant->nombreElt();
    else return 0;}
int Pile_sf2::SF2::INST::nombreElt(){
    if (this) return 1 + this->suivant->nombreElt();
    else return 0;}
int Pile_sf2::SF2::SMPL::nombreElt(){
    if (this) return 1 + this->suivant->nombreElt();
    else return 0;}
int Pile_sf2::SF2::nombreElt(){
    if (this) return 1 + this->suivant->nombreElt();
    else return 0;}

bool Pile_sf2::isValide(EltID id, bool acceptHidden)
{
    int i;
    SF2 *sf2Tmp;
    if (id.typeElement < elementSf2 || id.typeElement > elementPrstInstGen)
        return false;

    if (id.indexSf2 < 0)
        return (id.typeElement == elementSf2 || id.typeElement == elementRootSmpl || id.typeElement == elementRootInst
             || id.typeElement == elementRootPrst);

    // Vérification qu'indexSf2 est correct
    i = 0;
    sf2Tmp = this->sf2;
    while (i < id.indexSf2 && sf2Tmp)
    {
        sf2Tmp = sf2Tmp->suivant;
        i++;
    }
    if (!sf2Tmp) return false;
    if (sf2Tmp->hidden && !acceptHidden) return false;
    if (id.indexElt < 0 && id.typeElement != elementSf2 && id.typeElement != elementRootInst &&
            id.typeElement != elementRootSmpl && id.typeElement != elementRootPrst)
        return (id.typeElement == elementSmpl || id.typeElement == elementInst || id.typeElement == elementPrst);

    // Vérification qu'indexElt est correct
    if (id.typeElement == elementSmpl)
    {
        i = 0;
        SF2::SMPL *smpl;
        smpl = sf2Tmp->smpl;
        while (i < id.indexElt && smpl)
        {
            smpl = smpl->suivant;
            i++;
        }
        if (!smpl) return false;
        if (smpl->hidden && !acceptHidden) return false;
    }
    else if (id.typeElement == elementInst || id.typeElement == elementInstSmpl ||
             id.typeElement == elementInstMod || id.typeElement == elementInstSmplMod ||
             id.typeElement == elementInstGen || id.typeElement == elementInstSmplGen)
    {
        i = 0;
        SF2::INST *inst = sf2Tmp->inst;
        while (i < id.indexElt && inst)
        {
            inst = inst->suivant;
            i++;
        }
        if (!inst) return false;
        if (inst->hidden && !acceptHidden) return false;
        if (id.typeElement == elementInstSmpl || id.typeElement == elementInstSmplMod || id.typeElement == elementInstSmplGen)
        {
            if (id.indexElt2 < 0 && id.typeElement != elementInstSmplMod && id.typeElement != elementInstSmplGen)
                return (id.typeElement == elementInstSmpl);

            // Vérification qu'indexElt2 est correct
            i = 0;
            SF2::BAG *bag = inst->bag;
            while (i < id.indexElt2 && bag)
            {
                bag = bag->suivant;
                i++;
            }
            if (!bag) return false;
            if (bag->hidden && !acceptHidden) return false;
            if (id.typeElement == elementInstSmplMod || id.typeElement == elementInstSmplGen)
            {
                if (id.indexMod < 0) return true;
                // Vérification qu'indexMod est correct
                i = 0;
                if (id.typeElement == elementInstSmplMod)
                {
                    SF2::BAG::MOD *mod = bag->mod;
                    while (i < id.indexMod && mod)
                    {
                        mod = mod->suivant;
                        i++;
                    }
                    if (!mod) return false;
                    if (mod->hidden && !acceptHidden) return false;
                }
                else
                {
                    SF2::BAG::GEN *gen = bag->gen;
                    while (i < id.indexMod && gen)
                    {
                        gen = gen->suivant;
                        i++;
                    }
                    if (!gen) return false;
                }
            }
        }
        else if (id.typeElement == elementInstMod || id.typeElement == elementInstGen)
        {
            if (id.indexMod < 0) return true;
            // Vérification qu'indexMod est correct
            i = 0;
            if (id.typeElement == elementInstMod)
            {
                SF2::BAG::MOD *mod = inst->bagGlobal.mod;
                while (i < id.indexMod && mod)
                {
                    mod = mod->suivant;
                    i++;
                }
                if (!mod) return false;
                if (mod->hidden && !acceptHidden) return false;
            }
            else
            {
                SF2::BAG::GEN *gen = inst->bagGlobal.gen;
                while (i < id.indexMod && gen)
                {
                    gen = gen->suivant;
                    i++;
                }
                if (!gen) return false;
            }
        }
    }
    else if (id.typeElement == elementPrst || id.typeElement == elementPrstInst ||
             id.typeElement == elementPrstMod || id.typeElement == elementPrstInstMod ||
             id.typeElement == elementPrstGen || id.typeElement == elementPrstInstGen)
    {
        i = 0;
        SF2::PRST *prst;
        prst = sf2Tmp->prst;
        while (i < id.indexElt && prst)
        {
            prst = prst->suivant;
            i++;
        }
        if (!prst) return false;
        if (prst->hidden && !acceptHidden) return false;
        if (id.typeElement == elementPrstInst || id.typeElement == elementPrstInstMod || id.typeElement == elementPrstInstGen)
        {
            if (id.indexElt2 < 0 && id.typeElement != elementPrstInstMod && id.typeElement != elementPrstInstGen)
                return (id.typeElement == elementPrstInst);

            // Vérification qu'indexElt2 est correct
            i = 0;
            SF2::BAG *bag = prst->bag;
            while (i < id.indexElt2 && bag)
            {
                bag = bag->suivant;
                i++;
            }
            if (!bag) return false;
            if (bag->hidden && !acceptHidden) return false;
            if (id.typeElement == elementPrstInstMod || id.typeElement == elementPrstInstGen)
            {
                if (id.indexMod < 0) return true;
                // Vérification qu'indexMod est correct
                i = 0;
                if (id.typeElement == elementPrstInstMod)
                {
                    SF2::BAG::MOD *mod = bag->mod;
                    while (i < id.indexMod && mod)
                    {
                        mod = mod->suivant;
                        i++;
                    }
                    if (!mod) return false;
                    if (mod->hidden && !acceptHidden) return false;
                }
                else
                {
                    SF2::BAG::GEN *gen = bag->gen;
                    while (i < id.indexMod && gen)
                    {
                        gen = gen->suivant;
                        i++;
                    }
                    if (!gen) return false;
                }
            }
        }
        else if (id.typeElement == elementPrstMod || id.typeElement == elementPrstGen)
        {
            if (id.indexMod < 0) return true;
            // Vérification qu'indexMod est correct
            i = 0;
            if (id.typeElement == elementPrstMod)
            {
                SF2::BAG::MOD *mod = prst->bagGlobal.mod;
                while (i < id.indexMod && mod)
                {
                    mod = mod->suivant;
                    i++;
                }
                if (!mod) return false;
                if (mod->hidden && !acceptHidden) return false;
            }
            else
            {
                SF2::BAG::GEN *gen = prst->bagGlobal.gen;
                while (i < id.indexMod && gen)
                {
                    gen = gen->suivant;
                    i++;
                }
                if (!gen) return false;
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
            if (this->get(id, champ_wBank).wValue == wBank && \
                    this->get(id, champ_wPreset).wValue == wPreset)
                return false;
        }
    }
    return true;
}
