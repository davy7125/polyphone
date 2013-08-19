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

#include "tree.h"
#include "mainwindow.h"
#include <QMenu>
#include <QAction>

// Constructeur, destructeur
Tree::Tree(QWidget *parent) : QTreeWidget(parent)
{
    this->viewport()->installEventFilter(this); // drag & drop
    this->installEventFilter(this);             // keypress
    this->menuArborescence = NULL;
    this->mainWindow = NULL;
    this->refresh = true;
    this->infoSelectedItemsNumber = 0;
    this->infoIsSelectedItemsFamilyUnique = false;
    this->infoIsSelectedItemsSf2Unique = false;
    this->infoIsSelectedItemsTypeUnique = false;
    this->updateNext = true;
}
Tree::~Tree()
{
    delete this->menuArborescence;
}
Tree::menuClicDroit::menuClicDroit(MainWindow *mainWindow)
{
    // Constructeur menu clic droit sur l'arborescence
    this->menu = new QMenu();
    this->nouveauSample = new QAction(trUtf8("Nouveau sample..."), this->menu);
    connect(this->nouveauSample, SIGNAL(triggered()), mainWindow, SLOT(importerSmpl()));
    this->menu->addAction(this->nouveauSample);
    this->nouvelInstrument = new QAction(trUtf8("Nouvel instrument..."), this->menu);
    connect(this->nouvelInstrument, SIGNAL(triggered()), mainWindow, SLOT(nouvelInstrument()));
    this->menu->addAction(this->nouvelInstrument);
    this->nouveauPreset = new QAction(trUtf8("Nouveau preset..."), this->menu);
    connect(this->nouveauPreset, SIGNAL(triggered()), mainWindow, SLOT(nouveauPreset()));
    this->menu->addAction(this->nouveauPreset);
    this->menu->addSeparator();
    // Remplacer / associer
    this->associer = new QAction(trUtf8("Associer à..."), this->menu);
    connect(this->associer, SIGNAL(triggered()), mainWindow, SLOT(associer()));
    this->menu->addAction(this->associer);
    this->remplacer = new QAction(trUtf8("Remplacer par..."), this->menu);
    connect(this->remplacer, SIGNAL(triggered()), mainWindow, SLOT(remplacer()));
    this->menu->addAction(this->remplacer);
    this->menu->addSeparator();
    // Supprimer
    this->supprimer = new QAction(trUtf8("Supprimer"), this->menu);
    this->supprimer->setShortcut(trUtf8("Del"));
    connect(this->supprimer, SIGNAL(triggered()), mainWindow, SLOT(supprimerElt()));
    this->menu->addAction(this->supprimer);
    this->menu->addSeparator();
    // Renommer
    this->renommer = new QAction(trUtf8("Renommer..."), this->menu);
    this->renommer->setShortcut(Qt::Key_F2);
    connect(this->renommer, SIGNAL(triggered()), mainWindow, SLOT(renommer()));
    this->menu->addAction(this->renommer);
    this->menu->addSeparator();
    // Fermer
    this->fermer = new QAction(trUtf8("Fermer le fichier"), this->menu);
    this->fermer->setShortcut(trUtf8("Ctrl+W"));
    connect(this->fermer, SIGNAL(triggered()), mainWindow, SLOT(Fermer()));
    this->menu->addAction(this->fermer);
}

// Méthodes publiques

void Tree::init(MainWindow *mainWindow)
{
    this->mainWindow = mainWindow;
    // Création menu contextuel
    this->menuArborescence = new menuClicDroit(mainWindow);
}

void Tree::trier(int forme)
{
    // fichiers SF2 par ordre alphabétique
    int max = this->topLevelItemCount();
    QTreeWidgetItem *child;
    this->sortItems(0, Qt::AscendingOrder); // colonne 5 ne fonctionne pas ?!
    for (int i = 0; i < max; i ++)
        this->topLevelItem(i)->sortChildren(5, Qt::AscendingOrder);
    if (forme)
    {
        // Mise en forme (expanded / collapsed)
        for (int i = 0; i < max; i++)
        {
            child = this->topLevelItem(i);
            child->setExpanded(1);
            for (int j = 0; j < child->childCount(); j++)
            {
                child->child(j)->setExpanded(0);
                for (int k = 0; k < child->child(j)->childCount(); k++)
                {
                    child->child(j)->child(k)->setExpanded(0);
                }
            }
        }
    }
}
void Tree::searchTree(QString qStr)
{
    qStr = qStr.toLower();
    unsigned int max = this->topLevelItemCount();
    unsigned int max2;
    QTreeWidgetItem *child;
    for (unsigned int i = 0; i < max; i ++)
    {
        child = this->topLevelItem(i);
        max2 = child->childCount();
        for (unsigned int j = 0; j < max2; j++)
        {
            for (int k = 0; k < child->child(j)->childCount(); k++)
            {
                if (child->child(j)->child(k)->text(6) == "0")
                {
                    if (child->child(j)->child(k)->text(0).toLower().indexOf(qStr) != -1)
                        // La chaine a été trouvée
                        child->child(j)->child(k)->setHidden(0);
                    else
                        child->child(j)->child(k)->setHidden(1);
                }
            }
        }
    }
}
void Tree::clicTree()
{
    // Informations concernant la sélection
    this->updateSelectionInfo();
    this->setDragDropMode(DragDrop);
    if (!this->refresh) return;
    // Activation, désactivation des actions de menuArborescence
    int nb;
    bool fichierUnique = 1;
    bool familleUnique = 1;
    bool typeUnique = 1;
    ElementType type;
    EltID id;
    // Caractéristiques de la sélection
    // Nombre d'éléments sélectionnés
    nb = this->getSelectedItemsNumber();
    if (nb == 0)
    {
        // Rien n'est sélectionné
        fichierUnique = 0;
    }
    else
    {
        // Au moins un élément est sélectionné
        id = this->getID(0);
        type = id.typeElement;
        fichierUnique = this->isSelectedItemsSf2Unique();
        typeUnique = this->isSelectedItemsTypeUnique();
        familleUnique = this->isSelectedItemsFamilyUnique();
    }
    if (fichierUnique)
    {
        // Action possible : fermer
        menuArborescence->fermer->setEnabled(true);
        // Actions possibles : nouveau sample, instrument, preset
        menuArborescence->nouveauSample->setEnabled(true);
        menuArborescence->nouvelInstrument->setEnabled(true);
        menuArborescence->nouveauPreset->setEnabled(true);
        // Associer
        if (typeUnique && (type == elementSmpl || type == elementInst))
            menuArborescence->associer->setEnabled(true);
        else
            menuArborescence->associer->setEnabled(false);
        // Remplacer
        if (nb == 1 && (type == elementInstSmpl || type == elementPrstInst))
            menuArborescence->remplacer->setEnabled(true);
        else
            menuArborescence->remplacer->setEnabled(false);
        // Supprimer
        if (typeUnique && (((type == elementInstSmpl || type == elementPrstInst) && familleUnique) \
                           || type == elementSmpl || type == elementInst || type == elementPrst)
                       && !this->mainWindow->isPlaying())
            menuArborescence->supprimer->setEnabled(true);
        else
            menuArborescence->supprimer->setEnabled(false);
        // Renommer
        if (nb == 1 && typeUnique && (type == elementSmpl || type == elementInst || type == elementPrst || type == elementSf2))
        {
            menuArborescence->renommer->setText(tr("&Renommer..."));
            menuArborescence->renommer->setEnabled(true);
        }
        else
        {
            if (nb > 1 && typeUnique && type == elementSmpl)
            {
                menuArborescence->renommer->setText(tr("&Renommer en masse..."));
                menuArborescence->renommer->setEnabled(1);
            }
            else
            {
                menuArborescence->renommer->setText(tr("&Renommer..."));
                menuArborescence->renommer->setEnabled(false);
            }
        }
    }
    else
    {
        // Aucune action possible
        menuArborescence->fermer->setEnabled(false);
        menuArborescence->nouveauSample->setEnabled(false);
        menuArborescence->nouvelInstrument->setEnabled(false);
        menuArborescence->nouveauPreset->setEnabled(false);
        menuArborescence->associer->setEnabled(false);
        menuArborescence->supprimer->setEnabled(false);
        menuArborescence->renommer->setText(tr("&Renommer..."));
        menuArborescence->renommer->setEnabled(false);
        menuArborescence->remplacer->setEnabled(false);
    }
    // Activation, désactivation des actions de MainWindow
    this->mainWindow->updateActions();
}
void Tree::clicTreeRight()
{
    // Si au moins 1 élément de l'arborescence est sélectionné
    if (this->selectedItems().count())
    {
        // Affichage du menu contextuel
        this->menuArborescence->menu->exec(QCursor::pos());
    }
}
void Tree::desactiveSuppression()
{
    menuArborescence->supprimer->setEnabled(0);
}
void Tree::activeSuppression()
{
    menuArborescence->supprimer->setEnabled(1);
}
void Tree::clearPastedID()
{
    this->idList.clear();
}

void Tree::updateAtNextSelectionRequest() {this->updateNext = true;}
unsigned int Tree::getSelectedItemsNumber() {if (this->updateNext) updateSelectionInfo(); return this->infoSelectedItemsNumber;}
bool Tree::isSelectedItemsTypeUnique() {if (this->updateNext) updateSelectionInfo(); return infoIsSelectedItemsTypeUnique;}
bool Tree::isSelectedItemsSf2Unique() {if (this->updateNext) updateSelectionInfo(); return this->infoIsSelectedItemsSf2Unique;}
bool Tree::isSelectedItemsFamilyUnique() {if (this->updateNext) updateSelectionInfo(); return this->infoIsSelectedItemsFamilyUnique;}
EltID Tree::getID(unsigned int pos)
{
    QTreeWidgetItem *Elt = this->selectedItem(pos);
    return this->getItemID(Elt);
}
void Tree::select(EltID id, bool refresh)
{
    this->refresh = refresh;
    // Sélection et visibilité de id dans l'arborescence si présent
    unsigned int max = this->topLevelItemCount();
    QTreeWidgetItem *child = NULL;
    QTreeWidgetItem *child1 = NULL;
    QTreeWidgetItem *child2 = NULL;
    QTreeWidgetItem *child3 = NULL;
    string str;
    // Recherche du sf2
    int iTmp = -1;
    unsigned int i = 0;
    while (i < max && id.indexSf2 != iTmp)
    {
        child = this->topLevelItem(i);
        if (!child->isHidden())
        {
            str = child->text(1).toStdString();
            sscanf(str.c_str(), "%d", &iTmp);
        }
        i++;
    }
    if (id.indexSf2 == iTmp)
    {
        // Sf2 trouvé
        if (id.typeElement == elementSf2)
        {
            // Sélection
            child->setSelected(true);
            child->setExpanded(true);
        }
        else
        {
            if (id.typeElement == elementSmpl)
            {
                child1 = child->child(0);
            }
            else if (id.typeElement == elementInst || id.typeElement == elementInstSmpl)
            {
                child1 = child->child(1);
            }
            else if (id.typeElement == elementPrst || id.typeElement == elementPrstInst)
            {
                child1 = child->child(2);
            }
        }
        max = child1->childCount();
        i = 0;
        iTmp = -1;
        while (i < max && id.indexElt != iTmp)
        {
            child2 = child1->child(i);
            if (!child2->isHidden())
            {
                str = child2->text(3).toStdString();
                sscanf(str.c_str(), "%d", &iTmp);
            }
            i++;
        }
        if (id.indexElt == iTmp)
        {
            // Elt trouvé
            if (id.typeElement == elementSmpl || id.typeElement == elementInst || id.typeElement == elementPrst)
            {
                // Sélection
                child2->setSelected(true);
                child1->setExpanded(true);
                child->setExpanded(true);
            }
            else
            {
                max = child2->childCount();
                i = 0;
                iTmp = -1;
                while (i < max && id.indexElt2 != iTmp)
                {
                    child3 = child2->child(i);
                    if (!child3->isHidden())
                    {
                        str = child3->text(4).toStdString();
                        sscanf(str.c_str(), "%d", &iTmp);
                    }
                    i++;
                }
                if (id.indexElt2 == iTmp)
                {
                    // Elt2 trouvé
                    if (id.typeElement == elementInstSmpl || id.typeElement == elementPrstInst)
                    {
                        // Sélection
                        child3->setSelected(true);
                        child2->setExpanded(true);
                        child1->setExpanded(true);
                        child->setExpanded(true);
                    }
                }
            }
        }
    }
    this->refresh = true;
}
void Tree::selectNone(bool refresh)
{
    this->refresh = refresh;
    this->clearSelection();
    this->refresh = true;
}

// Méthodes privées
void Tree::updateSelectionInfo()
{
    this->updateNext = false;
    this->infoIsSelectedItemsSf2Unique = true;
    this->infoIsSelectedItemsTypeUnique = true;
    this->infoIsSelectedItemsFamilyUnique = true;
    this->infoSelectedItemsNumber = 0;
    int indexSf2 = -1;
    int type = -1;
    int family = -1;
    bool isInst = true;
    bool isPrst = true;
    EltID id;
    QTreeWidgetItem * itemTmp;
    for (int i = 0; i < this->selectedItems().count(); i++)
    {
        itemTmp = this->selectedItems().at(i);
        if (!itemTmp->isHidden())
        {
            id = this->getItemID(itemTmp);
            this->infoSelectedItemsNumber++;
            if (this->infoSelectedItemsNumber == 1)
            {
                indexSf2 = id.indexSf2;
                type = id.typeElement;
                family = id.indexElt;
            }
            else
            {
                this->infoIsSelectedItemsSf2Unique = this->infoIsSelectedItemsSf2Unique && (indexSf2 == id.indexSf2);
                this->infoIsSelectedItemsTypeUnique = this->infoIsSelectedItemsTypeUnique && (type == id.typeElement);
                this->infoIsSelectedItemsFamilyUnique = this->infoIsSelectedItemsFamilyUnique && (family == id.indexElt);
            }
            isInst = isInst && (id.typeElement == elementInst || id.typeElement == elementInstSmpl);
            isPrst = isPrst && (id.typeElement == elementPrst || id.typeElement == elementPrstInst);
        }
    }
    this->infoIsSelectedItemsFamilyUnique = this->infoIsSelectedItemsFamilyUnique && (isInst || isPrst);
    if (!this->infoSelectedItemsNumber)
    {
        this->infoIsSelectedItemsSf2Unique = false;
        this->infoIsSelectedItemsTypeUnique = false;
        this->infoIsSelectedItemsFamilyUnique = false;
    }
}
QTreeWidgetItem * Tree::selectedItem(unsigned int pos)
{
    if (this->getSelectedItemsNumber() > pos)
    {
        unsigned int nbHidden = 0;
        for (unsigned int i = 0; i < pos; i++)
        {
            while (this->selectedItems().at(i+nbHidden)->isHidden())
                nbHidden++;
        }
        return this->selectedItems().at(pos+nbHidden);
    }
    else
        return NULL;
}
EltID Tree::getItemID(QTreeWidgetItem *Elt)
{
    eltID ID;
    if (!Elt) return ID;
    string type, str;
    int indexSf2, indexElt, indexElt2;
    // Index Sf2, Elt et Elt2
    str = Elt->text(1).toStdString();
    sscanf(str.c_str(), "%d", &indexSf2);
    str = Elt->text(3).toStdString();
    sscanf(str.c_str(), "%d", &indexElt);
    str = Elt->text(4).toStdString();
    sscanf(str.c_str(), "%d", &indexElt2);
    // Type
    type = Elt->text(2).toStdString();
    // Création de l'ID
    if (type == "smpl") ID.typeElement = elementSmpl;
    else if (type == "inst") ID.typeElement = elementInst;
    else if (type == "prst") ID.typeElement = elementPrst;
    else if (type == "IS") ID.typeElement = elementInstSmpl;
    else if (type == "PI") ID.typeElement = elementPrstInst;
    else if (type == "R") ID.typeElement = elementSf2;
    else if (type == "S") ID.typeElement = elementRootSmpl;
    else if (type == "I") ID.typeElement = elementRootInst;
    else if (type == "P") ID.typeElement = elementRootPrst;
    else ID.typeElement = elementUnknown;
    ID.indexSf2 = indexSf2;
    if (ID.typeElement == elementSmpl || ID.typeElement == elementInst || ID.typeElement == elementPrst \
            || ID.typeElement == elementInstSmpl || ID.typeElement == elementPrstInst)
    {
        ID.indexElt = indexElt;
        if (ID.typeElement == elementInstSmpl || ID.typeElement == elementPrstInst)
            ID.indexElt2 = indexElt2;
        else
            ID.indexElt2 = -2;
    }
    else
    {
        ID.indexElt = -2;
        ID.indexElt2 = -2;
    }
    return ID;
}
void Tree::supprimerElt()
{
    if (this->menuArborescence->supprimer->isEnabled())
        this->mainWindow->supprimerElt();
}
void Tree::anticipateNewAction()
{
    this->mainWindow->anticipateNewAction(); // Les ID ne changeront pas lors du prochain prepareNewAction
}
