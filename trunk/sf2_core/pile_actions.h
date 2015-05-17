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

#ifndef PILE_ACTIONS_H
#define PILE_ACTIONS_H

#include "sf2_types.h"

class Pile_actions
{
public:
    // Catégorie d'action
    typedef enum
    {
        actionSupprimer = 0,
        actionCreer = 1,
        actionModifier = 2,
        actionModifierFromDefault = 3,
        actionModifierToDefault = 4,
        actionNulle = 5
    } ActionType;

    class Action
    {
    public:
        ActionType typeAction;
        EltID id;
        Champ champ;
        QString qNewValue;
        QString qOldValue;
        Valeur vNewValue;
        Valeur vOldValue;
        QByteArray baNewValue;
        QByteArray baOldValue;
        Action *suivant;
        // METHODES PUBLIQUES DE LA CLASSE ACTION
        Action();
        ~Action();
        int nombreElt();
        Action *getElt(int pos);
        Action *decrementer(EltID id);
    };
    // METHODES PUBLIQUES DE LA CLASSE PILE D'ACTION
    Pile_actions(); // constructeur
    ~Pile_actions()
    {
        Maillon * pile = this->redoAction;
        if (pile != NULL)
        {
            while (pile->redoAction != NULL)
            {
                pile = pile->redoAction;
            }
            delete pile;
        }
    }

    void nouvelleAction();
    void add(Action *action);
    void cleanActions();
    double getEdition(int indexSf2);
    bool isUndoable();
    bool isRedoable();
    Action *undo();
    Action *redo();
    int nombreEltUndo();
    int nombreEltRedo();
    Action *getEltUndo(int pos);
    Action *getEltRedo(int pos);
    void supprimerUndo(int pos);
    void supprimerRedo(int pos);

    // Ajustement de la numérotation
    void decrementer(EltID id);
private:
    // DEFINITION DE LA CLASSE PRIVEE MAILLON
    class Maillon
    {
    public:
        // DEFINITION DE LA SOUS-CLASSE EDITION
        class Edition
        {
        public:
            int indexSf2;
            double numEdition;
            Edition *suivant;
            // Méthodes
            Edition();
            ~Edition() { delete suivant; }
            Edition *remove(int indexSf2);
            double getEdition(int indexSf2);
            int nombreElt();
            Edition *getElt(int pos);
        };
        Action *listeAction;
        Edition *edition;
        Maillon *redoAction;
        Maillon *undoAction;
        // METHODES PUBLIQUES DE LA CLASSE MAILLON
        Maillon();
        ~Maillon()
        {
            delete edition;
            delete undoAction;
            delete listeAction;
        }

        int nombreEltUndo();
        int nombreEltRedo();
        Maillon *getEltUndo(int pos);
        Maillon *getEltRedo(int pos);
    };
    // ELEMENTS CONSTITUANTS LA CLASSE PILE ACTIONS
    Maillon *redoAction;
    Maillon *undoAction;
    int nbEdition;
};

#endif // PILE_ACTIONS_H
