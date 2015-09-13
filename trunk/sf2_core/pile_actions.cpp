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

#include "pile_actions.h"

Pile_actions::Pile_actions()
{
    this->undoAction = NULL;
    this->redoAction = NULL;
    this->nbEdition = 0;
}
Pile_actions::Maillon::Maillon()
{
    this->redoAction = NULL;
    this->undoAction = NULL;
    this->listeAction = NULL;
    this->edition = NULL;
}
Pile_actions::Maillon::Edition::Edition()
{
    this->suivant = NULL;
    this->numEdition = 0;
}
Pile_actions::Action::Action()
{
    this->suivant = NULL;
}

Pile_actions::Action::~Action()
{
    delete this->suivant;
}

// METHODES PUBLIQUES
void Pile_actions::nouvelleAction()
{
    // Création nouveau maillon
    if (this->undoAction)
        if (!this->undoAction->listeAction)
            return;
    Maillon *maillon = new Maillon;
    maillon->listeAction = NULL;
    maillon->redoAction = this->redoAction;
    maillon->undoAction = this->undoAction;
    maillon->edition = NULL;
    if (this->redoAction) this->redoAction->undoAction = maillon;
    if (this->undoAction)
    {
        this->undoAction->redoAction = maillon;
        // recopiage des éditions
        Maillon::Edition *editionTmp = this->undoAction->edition;
        Maillon::Edition *editionTmp2;
        while (editionTmp)
        {
            editionTmp2 = maillon->edition;
            maillon->edition = new Maillon::Edition;
            maillon->edition->indexSf2 = editionTmp->indexSf2;
            maillon->edition->numEdition = editionTmp->numEdition;
            maillon->edition->suivant = editionTmp2;
            editionTmp = editionTmp->suivant;
        }
    }
    this->undoAction = maillon;
}
void Pile_actions::add(Action *action)
{
    // Empilement de l'action dans un maillon
    if (!this->undoAction)
    {
        // Création d'un nouveau maillon
        this->nouvelleAction();
    }
    action->suivant = this->undoAction->listeAction;
    this->undoAction->listeAction = action;
    // gestion des éditions
    Maillon::Edition *editionTmp = this->undoAction->edition;
    if (editionTmp)
    {
        while (editionTmp->suivant && editionTmp->indexSf2 != action->id.indexSf2)
            editionTmp = editionTmp->suivant;
        if (editionTmp->indexSf2 != action->id.indexSf2)
        {
            // Ajout d'une édition
            editionTmp->suivant = new Maillon::Edition;
            editionTmp->suivant->indexSf2 = action->id.indexSf2;
            editionTmp->suivant->numEdition = ++this->nbEdition;
            editionTmp->suivant->suivant = NULL;
        }
        else
        {
            // Modification de l'édition
            editionTmp->numEdition = ++this->nbEdition;
        }
    }
    else
    {
        // Ajout d'une édition
        this->undoAction->edition = new Maillon::Edition;
        this->undoAction->edition->indexSf2 = action->id.indexSf2;
        this->undoAction->edition->numEdition = ++this->nbEdition;
        this->undoAction->edition->suivant = NULL;
    }
}
void Pile_actions::cleanActions()
{
    // Suppressions des listes d'actions vides
    int i, trouve;
    Maillon *tmp = NULL;
    // Undo
    do
    {
        trouve = 0;
        tmp = this->undoAction;
        i = -1;
        while (tmp && !trouve)
        {
            i++;
            if (!tmp->listeAction)
            {
                this->supprimerUndo(i);
                trouve = 1;
            }
            tmp = tmp->undoAction;
        }
    }
    while (trouve);
    // Redo
    do
    {
        trouve = 0;
        tmp = this->redoAction;
        i = -1;
        while (tmp && !trouve)
        {
            i++;
            if (!tmp->listeAction)
            {
                this->supprimerRedo(i);
                trouve = 1;
            }
            tmp = tmp->redoAction;
        }
    }
    while (trouve);
}

double Pile_actions::getEdition(int indexSf2)
{
    if (this->undoAction) return this->undoAction->edition->getEdition(indexSf2);
    else return -1;
}

bool Pile_actions::isUndoable() {return this->undoAction != NULL;}
bool Pile_actions::isRedoable() {return this->redoAction != NULL;}
Pile_actions::Action * Pile_actions::undo()
{
    // La file circule, un undo passe en redo
    if (!this->undoAction) return NULL; // rien à faire
    else
    {
        this->redoAction = this->undoAction;
        this->undoAction = this->undoAction->undoAction;
    }
    // On retourne la liste d'actions du 1er redo
    return this->redoAction->listeAction;
}
Pile_actions::Action * Pile_actions::redo()
{
    // La file circule, un redo passe en undo
    if (!this->redoAction) return NULL; // rien à faire
    else
    {
        this->undoAction = this->redoAction;
        this->redoAction = this->redoAction->redoAction;
    }
    // On retourne la liste d'actions du 1er undo
    return this->undoAction->listeAction;
}
int Pile_actions::nombreEltUndo() {return this->undoAction->nombreEltUndo();}
int Pile_actions::nombreEltRedo() {return this->redoAction->nombreEltRedo();}
Pile_actions::Action * Pile_actions::getEltUndo(int pos) {return this->undoAction->getEltUndo(pos)->listeAction;}
Pile_actions::Action * Pile_actions::getEltRedo(int pos) {return this->redoAction->getEltRedo(pos)->listeAction;}

void Pile_actions::supprimerUndo(int pos)
{
    Maillon *tmp = this->undoAction->getEltUndo(pos);
    if (!tmp) return;

    // Redirections
    if (tmp == this->undoAction) this->undoAction = this->undoAction->undoAction;
    if (tmp->undoAction) tmp->undoAction->redoAction = tmp->redoAction;
    if (tmp->redoAction) tmp->redoAction->undoAction = tmp->undoAction;

    // Suppression du maillon
    tmp->undoAction = NULL;
    tmp->redoAction = NULL;
    delete tmp;
}
void Pile_actions::supprimerRedo(int pos)
{
    Maillon *tmp = this->redoAction->getEltRedo(pos);
    if (!tmp) return;

    // Redirections
    if (tmp == this->redoAction) this->redoAction = this->redoAction->redoAction;
    if (tmp->redoAction) tmp->redoAction->undoAction = tmp->undoAction;
    if (tmp->undoAction) tmp->undoAction->redoAction = tmp->redoAction;

    // Suppression du maillon
    tmp->undoAction = NULL;
    tmp->redoAction = NULL;
    delete tmp;
}
void Pile_actions::decrementer(EltID id)
{
    Maillon *maillon;
    Action *action, *actionPrecedente, *actionSuivante;
    // Parcours de tous les undo
    maillon = this->undoAction;
    while (maillon)
    {
        action = maillon->listeAction;
        actionPrecedente = NULL;
        // Parcours de toutes les actions
        while (action)
        {
            actionSuivante = action->suivant;
            action = action->decrementer(id);
            if (!action)
            {
                // Si l'action a été effacée (id égaux)
                if (actionPrecedente)
                    actionPrecedente->suivant = actionSuivante;
                else
                    maillon->listeAction = actionSuivante;
            }
            else
                actionPrecedente = action;
            action = actionSuivante;
        }
        // Décrémentation des éditions
        if (id.typeElement == elementSf2)
            maillon->edition = maillon->edition->remove(id.indexSf2);
        maillon = maillon->undoAction;
    }
    // Parcours de tous les redo
    maillon = this->redoAction;
    while (maillon)
    {
        action = maillon->listeAction;
        actionPrecedente = NULL;
        // Parcours de toutes les actions
        while (action)
        {
            actionSuivante = action->suivant;
            action = action->decrementer(id);
            if (!action)
            {
                // Si l'action a été effacée (id égaux)
                if (actionPrecedente)
                    actionPrecedente->suivant = actionSuivante;
                else
                    maillon->listeAction = actionSuivante;
            }
            else
                actionPrecedente = action;
            action = actionSuivante;
        }
        // Décrémentation des éditions
        if (id.typeElement == elementSf2)
            maillon->edition = maillon->edition->remove(id.indexSf2);
        maillon = maillon->redoAction;
    }
}

// METHODES PRIVEES
int Pile_actions::Action::nombreElt(){
    if (this) return 1 + this->suivant->nombreElt();
    else return 0;}
int Pile_actions::Maillon::Edition::nombreElt(){
    if (this) return 1 + this->suivant->nombreElt();
    else return 0;}
int Pile_actions::Maillon::nombreEltUndo(){
    if (this) return 1 + this->undoAction->nombreEltUndo();
    else return 0;}
int Pile_actions::Maillon::nombreEltRedo(){
    if (this) return 1 + this->redoAction->nombreEltRedo();
    else return 0;}
Pile_actions::Action * Pile_actions::Action::getElt(int pos){
    Action *tmp = this;
    while (pos-- && tmp) tmp = tmp->suivant;
    return tmp;}
Pile_actions::Maillon::Edition * Pile_actions::Maillon::Edition::getElt(int pos){
    Edition *tmp = this;
    while (pos-- && tmp) tmp = tmp->suivant;
    return tmp;}
Pile_actions::Maillon * Pile_actions::Maillon::getEltUndo(int pos){
    Maillon *tmp = this;
    while (pos-- && tmp) tmp = tmp->undoAction;
    return tmp;}
Pile_actions::Maillon * Pile_actions::Maillon::getEltRedo(int pos){
    Maillon *tmp = this;
    while (pos-- && tmp) tmp = tmp->redoAction;
    return tmp;}
Pile_actions::Action * Pile_actions::Action::decrementer(EltID id)
{
    // Ajustement de la numérotation dans l'action
    switch ((int)id.typeElement)
    {
    case elementSf2: // indexSf2
        // décrémentation de l'indice des sf2
        if (this->id.indexSf2 > id.indexSf2) this->id.indexSf2--;
        else if (this->id.indexSf2 == id.indexSf2)
        {
            // Suppression d'un sf2 : suppression de toutes les actions correspondantes
            this->suivant = NULL;
            delete this;
            return NULL;
        }
        break;
    case elementSmpl: // indexElt
        if (this->id.indexSf2 == id.indexSf2)
        {
            if (this->id.typeElement == elementSmpl)
            {
                // décrémentation de l'indice des samples
                if (this->id.indexElt > id.indexElt) this->id.indexElt--;
                // décrémentation de l'indice des liens vers les samples (stéréo)
                if ((this->typeAction == actionModifier || this->typeAction == actionModifierFromDefault ||
                    this->typeAction == actionModifierToDefault) && this->champ == champ_wSampleLink)
                {
                    if (this->vNewValue.wValue > id.indexElt) this->vNewValue.wValue--;
                    if (this->vOldValue.wValue > id.indexElt) this->vOldValue.wValue--;
                }
            }
            else if (this->id.typeElement == elementInstSmpl || this->id.typeElement == elementInstSmplMod)
            {
                // décrémentation de l'indice des samples liés aux instruments
                if ((this->typeAction == actionModifier || this->typeAction == actionModifierFromDefault ||
                    this->typeAction == actionModifierToDefault) && this->champ == champ_sampleID)
                {
                    if (this->vNewValue.wValue > id.indexElt) this->vNewValue.wValue--;
                    if (this->vOldValue.wValue > id.indexElt) this->vOldValue.wValue--;
                }
            }
        }
        break;
    case elementInst: // indexElt
        if (this->id.indexSf2 == id.indexSf2)
        {
            if (this->id.typeElement == elementInst || this->id.typeElement == elementInstMod ||
                    this->id.typeElement == elementInstSmpl || this->id.typeElement == elementInstSmplMod)
            {
                // décrémentation de l'indice de inst, instmod, instsmpl et instsmplmod
                if (this->id.indexElt > id.indexElt) this->id.indexElt--;
            }
            else if (this->id.typeElement == elementPrstInst || this->id.typeElement == elementPrstInstMod)
            {
                // décrémentation de l'indice des instruments liés aux presets
                if ((this->typeAction == actionModifier || this->typeAction == actionModifierFromDefault ||
                    this->typeAction == actionModifierToDefault) && this->champ == champ_instrument)
                {
                    if (this->vNewValue.wValue > id.indexElt) this->vNewValue.wValue--;
                    if (this->vOldValue.wValue > id.indexElt) this->vOldValue.wValue--;
                }
            }
        }
        break;
    case elementPrst: // indexElt
        if (this->id.indexSf2 == id.indexSf2)
        {
            if (this->id.typeElement == elementPrst || this->id.typeElement == elementPrstMod ||
                    this->id.typeElement == elementPrstInst || this->id.typeElement == elementPrstInstMod)
            {
                // décrémentation de l'indice de prst, prstmod, prstinst, prstinstmod
                if (this->id.indexElt > id.indexElt) this->id.indexElt--;
            }
        }
        break;
    case elementInstSmpl: // indexElt2
        if (this->id.indexSf2 == id.indexSf2 && this->id.indexElt == id.indexElt)
        {
            if (this->id.typeElement == elementInstSmpl || this->id.typeElement == elementInstSmplMod)
            {
                // décrémentation instsmpl, instsmplmod
                if (this->id.indexElt2 > id.indexElt2) this->id.indexElt2--;
            }
        }
        break;
    case elementPrstInst: // indexElt2
        if (this->id.indexSf2 == id.indexSf2 && this->id.indexElt == id.indexElt)
        {
            if (this->id.typeElement == elementPrstInst || this->id.typeElement == elementPrstInstMod)
            {
                // décrémentation prstinst, prstinstmod
                if (this->id.indexElt2 > id.indexElt2) this->id.indexElt2--;
            }
        }
        break;
    case elementInstMod: case elementPrstMod: case elementInstSmplMod: case elementPrstInstMod:
        if (this->id.indexSf2 == id.indexSf2 && this->id.indexElt == id.indexElt
                && this->id.typeElement == id.typeElement)
        {
            // décrémentation Mod
            if (this->id.indexMod > id.indexMod) this->id.indexMod--;
            // Ajustement des index
            if (this->champ == champ_sfModDestOper)
            {
                if ((int)this->vOldValue.sfGenValue >= 32768)
                {
                    if (this->vOldValue.sfGenValue - 32768 == this->id.indexMod)
                        this->vOldValue.sfGenValue = (Champ)0;
                    else if (this->vOldValue.sfGenValue - 32768 > this->id.indexMod)
                        this->vOldValue.sfGenValue = (Champ)(this->vOldValue.sfGenValue - 1);
                }
                if ((int)this->vNewValue.sfGenValue >= 32768)
                {
                    if (this->vNewValue.sfGenValue - 32768 == this->id.indexMod)
                        this->vNewValue.sfGenValue = (Champ)0;
                    else if (this->vNewValue.sfGenValue - 32768 > this->id.indexMod)
                        this->vNewValue.sfGenValue = (Champ)(this->vNewValue.sfGenValue - 1);
                }
            }
        }
        break;
    }
    return this;
}
Pile_actions::Maillon::Edition * Pile_actions::Maillon::Edition::remove(int indexSf2)
{
    if (this)
    {
        if (this->indexSf2 > indexSf2) this->indexSf2--;
        else if (this->indexSf2 == indexSf2)
        {
            Edition *editedTmp = this->suivant;
            this->suivant = NULL;
            delete this;
            return editedTmp->remove(indexSf2);
        }
        this->suivant = this->suivant->remove(indexSf2);
    }
    return this;
}
double Pile_actions::Maillon::Edition::getEdition(int indexSf2)
{
    if (this)
    {
        if (this->indexSf2 == indexSf2) return this->numEdition;
        else return this->suivant->getEdition(indexSf2);
    }
    else return -1;
}
