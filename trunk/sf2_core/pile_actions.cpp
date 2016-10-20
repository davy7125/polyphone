/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2016 Davy Triponney                                **
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

#include "pile_actions.h"

void ActionManager::newActionSet()
{
    // Create a new ActionSet only if the last one is not empty
    if (!_undoActions.isEmpty())
        if (_undoActions[0]->_actions.isEmpty())
            return;

    ActionSet *actionSet = new ActionSet();

    // Copy previous edition if possible
    if (!_undoActions.isEmpty())
        foreach (ActionSet::Edition previousEdition, _undoActions[0]->_editions)
            actionSet->_editions << previousEdition;

    _undoActions.insert(0, actionSet);
}

void ActionManager::add(Action *action)
{
    // Create a new action set if necessary
    if (_undoActions.isEmpty())
        this->newActionSet();

    // Insert the action
    _undoActions[0]->_actions.insert(0, action);

    // Update or create the sf2 edition
    bool found = false;
    for (int i = 0; i < _undoActions[0]->_editions.count(); i++)
    {
        if (_undoActions[0]->_editions[i].indexSf2 == action->id.indexSf2)
        {
            // Update the edition
            _undoActions[0]->_editions[i].numEdition = ++_numEdition;
            found = true;
            break;
        }
    }

    if (!found)
    {
        // Create an edition
        ActionSet::Edition edition;
        edition.indexSf2 = action->id.indexSf2;
        edition.numEdition = ++_numEdition;
        _undoActions[0]->_editions << edition;
    }
}

void ActionManager::cleanActions()
{
    // Delete empty undo
    for (int i = _undoActions.count() - 1; i >= 0; i--)
        if (_undoActions[i]->_actions.isEmpty())
            delete _undoActions.takeAt(i);

    // Delete empty redo
    for (int i = _redoActions.count() - 1; i >= 0; i--)
        if (_redoActions[i]->_actions.isEmpty())
            delete _redoActions.takeAt(i);
}

int ActionManager::getEdition(int indexSf2)
{
    // Get the information from the first undo (this is the last action processed)
    if (_undoActions.isEmpty())
        return -1;

    return _undoActions[0]->getEdition(indexSf2);
}

QList<Action *> ActionManager::undo()
{
    // Nothing to do if no undo possible
    if (_undoActions.isEmpty())
        return QList<Action *> ();

    // An undo becomes a redo
    _redoActions.insert(0, _undoActions.takeFirst());

    // Return the actions of the first redo
    return _redoActions[0]->_actions;
}

QList<Action *> ActionManager::redo()
{
    // Nothing to do if no redo possible
    if (_redoActions.isEmpty())
        return QList<Action *> ();

    // A redo becomes a undo
    _undoActions.insert(0, _redoActions.takeFirst());

    // Return the actions of the first undo
    return _undoActions[0]->_actions;
}

void ActionManager::deleteUndo(int pos)
{
    if (pos >= 0 && pos < _undoActions.count())
        delete _undoActions.takeAt(pos);
}

void ActionManager::deleteRedo(int pos)
{
    if (pos >= 0 && pos < _redoActions.count())
        delete _redoActions.takeAt(pos);
}

void ActionManager::decrement(EltID id)
{
    // Browse all undo
    foreach (ActionSet * actionSet, _undoActions)
    {
        // Browse all actions in reverse order and decrement / delete them
        for (int i = actionSet->_actions.count() - 1; i >= 0; i--)
           if (actionSet->_actions[i]->decrement(id))
                delete actionSet->_actions.takeAt(i);

        // Decrement editions
        if (id.typeElement == elementSf2)
            actionSet->removeSf2(id.indexSf2);
    }

    // Browse all redo
    foreach (ActionSet * actionSet, _redoActions)
    {
        // Browse all actions in reverse order and decrement / delete them
        for (int i = actionSet->_actions.count() - 1; i >= 0; i--)
           if (actionSet->_actions[i]->decrement(id))
                delete actionSet->_actions.takeAt(i);

        // Decrement editions
        if (id.typeElement == elementSf2)
            actionSet->removeSf2(id.indexSf2);
    }
}

bool Action::decrement(EltID id)
{
    switch ((int)id.typeElement)
    {
    case elementSf2: // indexSf2
        // Decrement sf2
        if (this->id.indexSf2 > id.indexSf2) this->id.indexSf2--;
        else if (this->id.indexSf2 == id.indexSf2)
        {
            // Sf2 removal: delete all corresponding actions
            return true;
        }
        break;
    case elementSmpl: // indexElt
        if (this->id.indexSf2 == id.indexSf2)
        {
            if (this->id.typeElement == elementSmpl)
            {
                // Decrement smpl
                if (this->id.indexElt > id.indexElt) this->id.indexElt--;

                // Decrement linked samples (stéréo)
                if ((this->typeAction == Action::TypeUpdate || this->typeAction == Action::TypeChangeFromDefault ||
                    this->typeAction == Action::TypeChangeToDefault) && this->champ == champ_wSampleLink)
                {
                    if (this->vNewValue.wValue > id.indexElt) this->vNewValue.wValue--;
                    if (this->vOldValue.wValue > id.indexElt) this->vOldValue.wValue--;
                }
            }
            else if (this->id.typeElement == elementInstSmpl || this->id.typeElement == elementInstSmplMod)
            {
                // Decrement instsmpl
                if ((this->typeAction == Action::TypeUpdate || this->typeAction == Action::TypeChangeFromDefault ||
                    this->typeAction == Action::TypeChangeToDefault) && this->champ == champ_sampleID)
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
                // Decrement inst, instmod, instsmpl et instsmplmod
                if (this->id.indexElt > id.indexElt) this->id.indexElt--;
            }
            else if (this->id.typeElement == elementPrstInst || this->id.typeElement == elementPrstInstMod)
            {
                // Decrement prstinst
                if ((this->typeAction == Action::TypeUpdate || this->typeAction == Action::TypeChangeFromDefault ||
                    this->typeAction == Action::TypeChangeToDefault) && this->champ == champ_instrument)
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
                // Decrement prst, prstmod, prstinst, prstinstmod
                if (this->id.indexElt > id.indexElt) this->id.indexElt--;
            }
        }
        break;
    case elementInstSmpl: // indexElt2
        if (this->id.indexSf2 == id.indexSf2 && this->id.indexElt == id.indexElt)
        {
            if (this->id.typeElement == elementInstSmpl || this->id.typeElement == elementInstSmplMod)
            {
                // Decrement instsmpl, instsmplmod
                if (this->id.indexElt2 > id.indexElt2) this->id.indexElt2--;
            }
        }
        break;
    case elementPrstInst: // indexElt2
        if (this->id.indexSf2 == id.indexSf2 && this->id.indexElt == id.indexElt)
        {
            if (this->id.typeElement == elementPrstInst || this->id.typeElement == elementPrstInstMod)
            {
                // Decrementat prstinst, prstinstmod
                if (this->id.indexElt2 > id.indexElt2) this->id.indexElt2--;
            }
        }
        break;
    case elementInstMod: case elementPrstMod: case elementInstSmplMod: case elementPrstInstMod:
        if (this->id.indexSf2 == id.indexSf2 && this->id.indexElt == id.indexElt
                && this->id.typeElement == id.typeElement)
        {
            // Decrement Mod
            if (this->id.indexMod > id.indexMod) this->id.indexMod--;

            // Adjust index
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

    return false;
}

void ActionManager::ActionSet::removeSf2(int indexSf2)
{
    // Decrement all sf2 > indexSf2
    int indexToRemove = -1;
    for (int i = 0; i < _editions.count(); i++)
    {
        if (_editions[i].indexSf2 > indexSf2)
            _editions[i].indexSf2--;
        else if (_editions[i].indexSf2 == indexSf2)
            indexToRemove = i;
    }

    // Remove indexSf2
    if (indexToRemove != -1)
        _editions.removeAt(indexToRemove);
}

int ActionManager::ActionSet::getEdition(int indexSf2)
{
    // Try to find the edition number of indexSf2
    foreach (Edition edition, _editions)
        if (edition.indexSf2 == indexSf2)
            return edition.numEdition;

    // Default value
    return -1;
}
