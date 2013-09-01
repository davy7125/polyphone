/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013 Davy Triponney                                     **
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

#ifndef TREE_H
#define TREE_H

#include "qtreewidget.h"
#include "sf2_types.h"
#include <string>
    using std::string;

class MainWindow;

class Tree : public QTreeWidget
{
    Q_OBJECT
public:
    class menuClicDroit
    {
    public :
        // constructeur
        menuClicDroit(MainWindow *mainWindow);
        // attributs
        QAction *nouveauSample;
        QAction *nouvelInstrument;
        QAction *nouveauPreset;
        QAction *remplacer;
        QAction *associer;
        QAction *supprimer;
        QAction *renommer;
        QAction *fermer;
        QMenu *menu;
    };
    bool eventFilter(QObject* object, QEvent* event)
    {
        if (event->type() == QEvent::KeyPress && object == this)
        {
            QKeyEvent *keyEvent = dynamic_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Delete)
            {
                // Touche suppr : efface l'élément
                this->supprimerElt();
            }
            else if (keyEvent->matches(QKeySequence::Copy))
            {
                // Nombre d'éléments sélectionnés
                int nbElt = this->getSelectedItemsNumber();
                this->clearPastedID();
                if (nbElt >= 0)
                {
                    if (this->isSelectedItemsSf2Unique() && this->isSelectedItemsTypeUnique())
                    {
                        this->anticipateNewAction();
                        // Copie des éléments
                        for (int i = 0; i < nbElt; i++)
                            this->idList << this->getID(i);
                    }
                }
            }
            else if (keyEvent->matches(QKeySequence::Paste))
            {
                int nbElt = this->getSelectedItemsNumber();
                if (nbElt > 0 && this->idList.size())
                {
                    if (this->isSelectedItemsSf2Unique() && this->isSelectedItemsTypeUnique() && \
                            (this->getID(0).typeElement == elementSmpl || this->isSelectedItemsFamilyUnique()))
                    {
                        EltID idDest = this->getID(0);
                        int *msg = new int[4]; // Messages (delete dans mainwindow)
                        msg[0] = 0;
                        msg[1] = 0;
                        msg[2] = 0;
                        msg[3] = 0;
                        QByteArray * ba1 = new QByteArray; // Correspondances des index des samples (delete dans mainwindow)
                        ba1->clear();
                        QByteArray * ba2 = new QByteArray; // Correspondances des index des instruments (delete dans mainwindow)
                        ba2->clear();
                        for (int i = 0; i < this->idList.size(); i++)
                        {
                            if (this->idList.size() > 1)
                            {
                                if (i == 0)
                                    emit (dropped(idDest, this->idList[i], -1, msg, ba1, ba2));
                                else if (i == this->idList.size() - 1)
                                    emit (dropped(idDest, this->idList[i], 1, msg, ba1, ba2));
                                else
                                    emit (dropped(idDest, this->idList[i], 0, msg, ba1, ba2));
                            }
                            else
                                emit (dropped(idDest, this->idList[i], 2, msg, ba1, ba2));
                        }
                    }
                }
            }
        }
        else if (event->type() == QEvent::DragEnter && object == this->viewport())
        {
            if (!this->isSelectedItemsSf2Unique() || !this->isSelectedItemsTypeUnique())
                this->setDragDropMode(NoDragDrop);
        }
        else if (event->type() == QEvent::DragMove && object == this->viewport())
        {
            // Modification du curseur ?
        }
        else if (event->type() == QEvent::Drop && object == this->viewport())
        {
            this->anticipateNewAction();
            QDragMoveEvent* dragMoveEvent = static_cast<QDragMoveEvent*>(event);
            EltID idDest = this->getItemID(this->itemAt(dragMoveEvent->pos()));
            // Emission d'un signal pour chaque élément déplacé
            int nbElt = this->selectedItems().count();
            EltID *ids = new EltID[nbElt];
            // Enregistrement des ID
            for (int i = 0; i < nbElt; i++)
                ids[i] = this->getID(i);
            // Puis modification de la structure
            int *msg = new int[4]; // Messages (delete dans mainwindow)
            msg[0] = 0;
            msg[1] = 0;
            msg[2] = 0;
            msg[3] = 0;
            QByteArray * ba1 = new QByteArray(); // Correspondances des index des samples (delete dans mainwindow)
            QByteArray * ba2 = new QByteArray(); // Correspondances des index des instruments (delete dans mainwindow)
            for (int i = 0; i < nbElt; i++)
            {
                if (nbElt > 1)
                {
                    if (i == 0)
                        emit (dropped(idDest, ids[i], -1, msg, ba1, ba2));
                    else if (i == nbElt - 1)
                        emit (dropped(idDest, ids[i], 1, msg, ba1, ba2));
                    else
                        emit (dropped(idDest, ids[i], 0, msg, ba1, ba2));
                }
                else
                    emit (dropped(idDest, ids[i], 2, msg, ba1, ba2));
            }
            if (nbElt) delete [] ids;
            return true; // termine le drop
        }
        return false;
    }
    explicit Tree(QWidget *parent = 0);
    ~Tree();
    // Méthodes publiques
    void init(MainWindow *mainWindow);
    void trier(int forme);
    void updateAtNextSelectionRequest();
    unsigned int getSelectedItemsNumber();
    bool isSelectedItemsTypeUnique();
    bool isSelectedItemsSf2Unique();
    bool isSelectedItemsFamilyUnique();
    EltID getID(unsigned int pos);
    void selectNone(bool refresh = false);
    void select(EltID id, bool refresh = false);
    void desactiveSuppression();
    void activeSuppression();
    void clearPastedID();
public slots:
    void collapse() {this->trier(1);}       // Clic sur "enrouler"
    void searchTree(QString qStr);          // Lors d'une modification du champ recherche
    void clicTree();                        // Modification de la sélection dans l'arborescence
    void clicTreeRight();                   // Clic droit dans l'arborescence
signals:
    void dropped(EltID dest, EltID src, int temps, int *msg, QByteArray *ba1, QByteArray *ba2);
private:
    // Attributs privés
    MainWindow *mainWindow;
    menuClicDroit *menuArborescence;
    bool refresh;
    bool updateNext;
    QList<EltID> idList;
    void updateSelectionInfo();
    unsigned int infoSelectedItemsNumber;
    bool infoIsSelectedItemsTypeUnique;
    bool infoIsSelectedItemsSf2Unique;
    bool infoIsSelectedItemsFamilyUnique;
    // Méthodes privées
    void anticipateNewAction();
    QTreeWidgetItem * selectedItem(unsigned int pos);
    static EltID getItemID(QTreeWidgetItem *elt);
    void supprimerElt();
};

#endif // TREE_H
