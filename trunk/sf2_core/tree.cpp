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

#include "tree.h"
#include "mainwindow.h"
#include <QMenu>
#include <QAction>
#include <QComboBox>
#include "pile_sf2.h"

Tree::Tree(QWidget *parent) : QTreeWidget(parent),
    mainWindow(NULL),
    menuArborescence(NULL),
    refresh(true),
    updateNext(true),
    infoSelectedItemsNumber(0),
    infoIsSelectedItemsTypeUnique(false),
    infoIsSelectedItemsSf2Unique(false),
    infoIsSelectedItemsFamilyUnique(false),
    _sf2(NULL),
    _searchedText(""),
    _displayedSf2(-1)
{}

Tree::~Tree()
{
    delete this->menuArborescence;
}

Tree::menuClicDroit::menuClicDroit(MainWindow *mainWindow)
{
    // Constructeur menu clic droit sur l'arborescence
    this->menu = new QMenu();
    this->nouveauSample = new QAction(trUtf8("Nouvel échantillon..."), this->menu);
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

    // Copier / coller / supprimer
    this->copier = new QAction(trUtf8("Copier"), this->menu);
    this->copier->setShortcut(QString("Ctrl+C"));
    connect(this->copier, SIGNAL(triggered()), mainWindow, SLOT(copier()));
    this->menu->addAction(this->copier);
    this->coller = new QAction(trUtf8("Coller"), this->menu);
    this->coller->setShortcut(QString("Ctrl+V"));
    connect(this->coller, SIGNAL(triggered()), mainWindow, SLOT(coller()));
    this->menu->addAction(this->coller);
    this->supprimer = new QAction(trUtf8("Supprimer"), this->menu);
    this->supprimer->setShortcut(QString("Del"));
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
    this->fermer->setShortcut(QString("Ctrl+W"));
    connect(this->fermer, SIGNAL(triggered()), mainWindow, SLOT(Fermer()));
    this->menu->addAction(this->fermer);
}

Tree::menuClicDroit::~menuClicDroit()
{
    delete this->menu;
}

void Tree::init(MainWindow *mainWindow, Pile_sf2 * sf2, QComboBox * comboSf2)
{
    this->mainWindow = mainWindow;
    _sf2 = sf2;
    _comboSf2 = comboSf2;
    this->connect(_comboSf2, SIGNAL(currentIndexChanged(int)), SLOT(comboSf2IndexChanged(int)));
    _comboSf2->addItem(trUtf8("Tout afficher"), -1);

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

void Tree::addSf2InComboBox(int numSf2)
{
    _comboSf2->addItem("", numSf2);
    if (_comboSf2->itemData(_comboSf2->currentIndex()).toInt() != -1)
        _comboSf2->setCurrentIndex(_comboSf2->count() - 1);
}

void Tree::removeSf2FromComboBox(int numSf2)
{
    int indexToRemove = -1;
    for (int i = 0; i < _comboSf2->count(); i++)
    {
        int iData = _comboSf2->itemData(i).toInt();
        if (iData == numSf2)
            indexToRemove = i;
        else if (iData > numSf2)
            _comboSf2->setItemData(i, iData - 1);
    }
    if (_comboSf2->currentIndex() == indexToRemove)
        _comboSf2->setCurrentIndex(0);
    _comboSf2->removeItem(indexToRemove);
}

void Tree::renameSf2InComboBox(int numSf2, QString name)
{
    int currentData = _comboSf2->itemData(_comboSf2->currentIndex()).toInt();
    _comboSf2->blockSignals(true);
    _comboSf2->removeItem(0);
    for (int i = 0; i < _comboSf2->count(); i++)
    {
        if (_comboSf2->itemData(i).toInt() == numSf2)
        {
            _comboSf2->setItemText(i, name);
            break;
        }
    }
    _comboSf2->model()->sort(0);
    _comboSf2->insertItem(0, trUtf8("Tout afficher"), -1);
    _comboSf2->blockSignals(false);

    if (currentData == -1)
        _comboSf2->setCurrentIndex(0);
    else
    {
        for (int i = 0; i < _comboSf2->count(); i++)
        {
            if (_comboSf2->itemData(i).toInt() == currentData)
            {
                _comboSf2->setCurrentIndex(i);
                return;
            }
        }
    }
}

void Tree::comboSf2IndexChanged(int index)
{
    if (index >= 0)
        _displayedSf2 = _comboSf2->itemData(index).toInt();
    else
        _displayedSf2 = -1;
    searchTree(_searchedText, _displayedSf2);
}

void Tree::searchTree(QString qStr)
{
    _searchedText = qStr;
    searchTree(_searchedText, _displayedSf2);
}

void Tree::searchTree(QString qStr, int displayedSf2)
{
    qStr = qStr.toLower();
    unsigned int max = this->topLevelItemCount();
    unsigned int max2;
    QTreeWidgetItem *child;

    // Etat initial
    _displayedElements.clear();
    bool isDisplayed = qStr.isEmpty();
    for (unsigned int i = 0; i < max; i ++)
    {
        child = this->topLevelItem(i);
        child->setHidden(false);
        max2 = child->childCount();

        // Niveau 1: en-têtes "échantillons", "instruments", "presets"
        for (unsigned int j = 0; j < max2; j++)
        {
            // Niveau 2: sample, instrument ou preset
            for (int k = 0; k < child->child(j)->childCount(); k++)
            {
                QTreeWidgetItem * item = child->child(j)->child(k);
                if (item->text(6) == "0") // Si l'élément n'est pas masqué par une suppression non définitive
                    item->setHidden(!isDisplayed);
            }
        }
    }

    for (unsigned int i = 0; i < max; i ++)
    {
        child = this->topLevelItem(i);

        if (displayedSf2 == -1 || child->text(1).toInt() == displayedSf2)
        {
            if (!isDisplayed)
            {
                max2 = child->childCount();

                // Niveau 1: en-têtes "échantillons", "instruments", "presets"
                for (unsigned int j = 0; j < max2; j++)
                {
                    // Niveau 2: sample, instrument ou preset
                    for (int k = 0; k < child->child(j)->childCount(); k++)
                    {
                        QTreeWidgetItem * item = child->child(j)->child(k);
                        if (item->text(6) == "0") // Si l'élément n'est pas masqué par une suppression non définitive
                        {
                            if (item->text(0).toLower().indexOf(qStr) != -1 || item->isSelected())
                            {
                                if (item->text(2) == "smpl")
                                    displaySample(item->text(1).toInt(), item->text(3).toInt());
                                else if (item->text(2) == "inst")
                                    displayInstrument(item->text(1).toInt(), item->text(3).toInt());
                                else if (item->text(2) == "prst")
                                    displayPreset(item->text(1).toInt(), item->text(3).toInt());
                            }
                        }
                    }
                }
            }
        }
        else
        {
            child->setHidden(true);
        }
    }
}

void Tree::displaySample(int idSf2, int index, bool repercute)
{
    // Affichage de l'échantillon
    EltID idSmpl(elementSmpl, idSf2, index, -1, -1);
    displayElement(idSmpl);

    // Affichage des instruments utilisant le sample
    if (repercute)
    {
        EltID idInst = idSmpl;
        idInst.typeElement = elementInst;
        EltID idInstSmpl = idSmpl;
        idInstSmpl.typeElement = elementInstSmpl;
        int nbInst = _sf2->count(idInst);
        for (int i = 0; i < nbInst; i++)
        {
            idInst.indexElt = i;
            idInstSmpl.indexElt = i;
            if (!_sf2->get(idInst, champ_hidden).bValue)
            {
                int nbInstSmpl = _sf2->count(idInstSmpl);
                for (int j = 0; j < nbInstSmpl; j++)
                {
                    idInstSmpl.indexElt2 = j;
                    if (!_sf2->get(idInstSmpl, champ_hidden).bValue)
                    {
                        if (_sf2->get(idInstSmpl, champ_sampleID).wValue == index)
                            displayInstrument(idSf2, i, false, true);
                    }
                }
            }
        }
    }
}

void Tree::displayInstrument(int idSf2, int index, bool repercuteSmpl, bool repercutePrst)
{
    EltID idInst(elementInst, idSf2, index, -1, -1);
    displayElement(idInst);

    // Affichage des samples
    if (repercuteSmpl)
    {
        EltID idInstSmpl = idInst;
        idInstSmpl.typeElement = elementInstSmpl;
        int nbElement = _sf2->count(idInstSmpl);
        for (int i = 0; i < nbElement; i++)
        {
            idInstSmpl.indexElt2 = i;
            if (!_sf2->get(idInstSmpl, champ_hidden).bValue)
                displaySample(idSf2, _sf2->get(idInstSmpl, champ_sampleID).wValue, false);
        }
    }

    // Affichage des presets utilisant l'instrument
    if (repercutePrst)
    {
        EltID idPrst = idInst;
        idPrst.typeElement = elementPrst;
        EltID idPrstInst = idInst;
        idPrstInst.typeElement = elementPrstInst;
        int nbPrst = _sf2->count(idPrst);
        for (int i = 0; i < nbPrst; i++)
        {
            idPrst.indexElt = i;
            idPrstInst.indexElt = i;
            if (!_sf2->get(idPrst, champ_hidden).bValue)
            {
                int nbPrstInst = _sf2->count(idPrstInst);
                for (int j = 0; j < nbPrstInst; j++)
                {
                    idPrstInst.indexElt2 = j;
                    if (!_sf2->get(idPrstInst, champ_hidden).bValue)
                    {
                        if (_sf2->get(idPrstInst, champ_instrument).wValue == index)
                            displayPreset(idSf2, i, false);
                    }
                }
            }
        }
    }
}

void Tree::displayPreset(int idSf2, int index, bool repercute)
{
    EltID idPrst(elementPrst, idSf2, index, -1, -1);
    displayElement(idPrst);

    // Affichage des instruments
    if (repercute)
    {
        EltID idPrstInst = idPrst;
        idPrstInst.typeElement = elementPrstInst;
        int nbElement = _sf2->count(idPrstInst);
        for (int i = 0; i < nbElement; i++)
        {
            idPrstInst.indexElt2 = i;
            if (!_sf2->get(idPrstInst, champ_hidden).bValue)
                displayInstrument(idSf2, _sf2->get(idPrstInst, champ_instrument).wValue, true, false);
        }
    }
}

void Tree::displayElement(EltID id)
{
    if (_displayedElements.contains(id))
        return;
    _displayedElements << id;

    // Affichage
    QTreeWidgetItem *child;

    int max = this->topLevelItemCount();
    for (int i = 0; i < max; i++)
    {
        child = this->topLevelItem(i);
        if (child->text(1).toInt() == id.indexSf2)
        {
            if (id.typeElement == elementSmpl)
                child = child->child(0);
            else if (id.typeElement == elementInst)
                child = child->child(1);
            else
                child = child->child(2);
            int numChilds = child->childCount();
            for (int j = 0; j < numChilds; j++)
            {
                // Affichage de la sous-arborescence
                if (child->child(j)->text(3).toInt() == id.indexElt)
                    child->child(j)->setHidden(false);
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
    ElementType type = elementUnknown;
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
        id = this->getFirstID();
        type = id.typeElement;
        fichierUnique = this->isSelectedItemsSf2Unique();
        typeUnique = this->isSelectedItemsTypeUnique();
        familleUnique = this->isSelectedItemsFamilyUnique();
    }

    if (fichierUnique)
    {
        // Actions possibles : fermer, nouveau sample, instrument, preset
        menuArborescence->fermer->setEnabled(true);
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

        // Supprimer, copier, coller
        if (typeUnique && (((type == elementInstSmpl || type == elementPrstInst) && familleUnique)
                           || type == elementSmpl || type == elementInst || type == elementPrst)
                       && !this->mainWindow->isPlaying())
        {
            menuArborescence->supprimer->setEnabled(true);
            menuArborescence->copier->setEnabled(true);
            menuArborescence->coller->setEnabled(true);
        }
        else
        {
            menuArborescence->supprimer->setEnabled(false);
            menuArborescence->copier->setEnabled(false);
            menuArborescence->coller->setEnabled(false);
        }

        // Renommer
        if (nb == 1 && typeUnique && (type == elementSmpl || type == elementInst || type == elementPrst || type == elementSf2))
        {
            menuArborescence->renommer->setText(trUtf8("&Renommer..."));
            menuArborescence->renommer->setEnabled(true);
        }
        else
        {
            if (nb > 1 && typeUnique && type == elementSmpl)
            {
                menuArborescence->renommer->setText(trUtf8("&Renommer en masse..."));
                menuArborescence->renommer->setEnabled(1);
            }
            else
            {
                menuArborescence->renommer->setText(trUtf8("&Renommer..."));
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
EltID Tree::getFirstID()
{
    return this->getItemID(this->selectedItem(0));
}
QList<EltID> Tree::getAllIDs()
{
    QList<EltID> listRet;
    QList<QTreeWidgetItem *> items = this->selectedItems();
    foreach (QTreeWidgetItem * item, items)
        listRet << this->getItemID(item);
    return listRet;
}

EltID Tree::getNextID(bool closeFile)
{
    // Cette fonction est appelée avant une suppression ou une fermeture de fichier
    // Elle permet de connaitre le prochain élément à sélectionner dans l'arborescence
    // closeFile si une fermeture est demandée, sinon il s'agit d'une suppression d'éléments

    EltID idRet(elementUnknown, -1, -1, -1, -1);
    if (this->getSelectedItemsNumber() == 0)
        return idRet;

    if (closeFile)
    {
        if (!isSelectedItemsSf2Unique())
            return idRet;

        // Liste des idSf2 dans l'ordre
        QList<EltID> listSf2;
        int nbSf2 = this->topLevelItemCount();
        for (int i = 0; i < nbSf2; i++)
            listSf2 << getItemID(this->topLevelItem(i));

        if (listSf2.size() >= 2)
        {
            // Sf2 sélectionné actuellement
            EltID idCurrentSf2 = getFirstID();
            idCurrentSf2.typeElement = elementSf2;

            // Prochain sf2
            int iTmp = listSf2.indexOf(idCurrentSf2);
            if (iTmp != -1)
            {
                if (iTmp >= listSf2.size() - 1)
                    idRet = listSf2.at(listSf2.size() - 2);
                else
                    idRet = listSf2.at(iTmp + 1);
                if (idRet.indexSf2 > idCurrentSf2.indexSf2)
                    idRet.indexSf2--;
            }
        }
    }
    else
    {
        if (!isSelectedItemsTypeUnique() || !isSelectedItemsFamilyUnique() || !isSelectedItemsSf2Unique())
            return idRet;
    }

    return idRet;
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

    // Recherche du sf2
    int iTmp = -1;
    unsigned int i = 0;
    while (i < max && id.indexSf2 != iTmp)
    {
        child = this->topLevelItem(i);
        if (!child->isHidden())
            iTmp = child->text(1).toInt();
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
            if (id.typeElement == elementRootSmpl)
            {
                child->child(0)->setSelected(true);
                child->child(0)->setExpanded(true);
            }
            else if (id.typeElement == elementRootInst)
            {
                child->child(1)->setSelected(true);
                child->child(1)->setExpanded(true);
            }
            else if (id.typeElement == elementRootPrst)
            {
                child->child(2)->setSelected(true);
                child->child(2)->setExpanded(true);
            }

            if (id.typeElement == elementSmpl)
                child1 = child->child(0);
            else if (id.typeElement == elementInst || id.typeElement == elementInstSmpl)
                child1 = child->child(1);
            else if (id.typeElement == elementPrst || id.typeElement == elementPrstInst)
                child1 = child->child(2);
            if (!child1)
                return;

            max = child1->childCount();
            i = 0;
            iTmp = -1;
            while (i < max && id.indexElt != iTmp)
            {
                child2 = child1->child(i);
                if (!child2->isHidden())
                    iTmp = child2->text(3).toInt();
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
                            iTmp = child3->text(4).toInt();
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
    }
    this->refresh = true;
}

void Tree::selectNone(bool refresh)
{
    this->refresh = refresh;
    this->clearSelection();
    this->refresh = true;
}

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

EltID Tree::getItemID(QTreeWidgetItem *elt)
{
    EltID ID(elementUnknown, -1, -1, -1, -1);
    if (!elt) return ID;
    int indexSf2, indexElt, indexElt2;

    // Index Sf2, Elt et Elt2
    indexSf2 = elt->text(1).toInt();
    indexElt = elt->text(3).toInt();
    indexElt2 = elt->text(4).toInt();

    // Type
    QString type = elt->text(2);

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
    if (ID.typeElement == elementSmpl || ID.typeElement == elementInst || ID.typeElement == elementPrst
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

EltID Tree::getElementToSelectAfterDeletion()
{
    // Liste des éléments sélectionnés (ne doit pas être vide)
    QList<QTreeWidgetItem *> listSelectedItems = this->selectedItems();
    if (listSelectedItems.isEmpty())
        return EltID();

    // Vérification que le parent est le même pour tous les éléments
    QTreeWidgetItem * itemParent = listSelectedItems.first()->parent();
    if (!itemParent)
        return EltID();

    foreach (QTreeWidgetItem * item, listSelectedItems)
    {
        if (item->parent() != itemParent)
            return EltID();
    }

    // Récupération de la liste des enfants
    QList<QTreeWidgetItem *> listChildItems;
    for (int i = 0; i < itemParent->childCount(); i++)
        if (!itemParent->child(i)->isHidden())
            listChildItems << itemParent->child(i);

    // Séparation en 2 listes, d'après le dernier élément sélectionné
    int indexMid = listChildItems.indexOf(listSelectedItems.last());
    QList<QTreeWidgetItem *> listDebut = listChildItems.mid(0, indexMid + 1);
    QList<QTreeWidgetItem *> listFin = listChildItems.mid(indexMid + 1, -1);

    // Suppression de toutes les occurences des éléments sélectionnés dans la 1ère liste
    foreach (QTreeWidgetItem * item, listSelectedItems)
        listDebut.removeAll(item);

    // Détermination de l'élément à sélectionner
    QTreeWidgetItem * itemToSelect = itemParent;
    if (!listFin.isEmpty())
        itemToSelect = listFin.first();
    else if (!listDebut.isEmpty())
        itemToSelect = listDebut.last();

    // Récupération de l'identifiant
    return getItemID(itemToSelect);
}


////////////////
// EVENEMENTS //
////////////////

void Tree::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete)
    {
        // Touche suppr : efface l'élément
        this->supprimerElt();
    }
    else if (event->matches(QKeySequence::Copy))
    {
        // Nombre d'éléments sélectionnés
        int nbElt = this->getSelectedItemsNumber();
        this->clearPastedID();
        if (nbElt >= 0)
        {
            if (this->isSelectedItemsSf2Unique() && this->isSelectedItemsTypeUnique())
            {
                mainWindow->prepareNewAction();
                mainWindow->updateDo();

                // Copie des éléments
                this->idList = this->getAllIDs();
            }
        }
    }
    else if (event->matches(QKeySequence::Paste))
    {
        mainWindow->prepareNewAction();
        if (this->getSelectedItemsNumber() == 1 && this->idList.size())
            mainWindow->dragAndDrop(this->getFirstID(), this->idList);
        mainWindow->updateDo();
    }
    else if (event->key() == Qt::Key_Space)
    {
        mainWindow->spaceKeyPressedInTree();
    }
    else
        QTreeWidget::keyPressEvent(event);
}

void Tree::dragEnterEvent(QDragEnterEvent * event)
{
    if ((this->isSelectedItemsSf2Unique() && this->isSelectedItemsTypeUnique()) ||
            event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}

void Tree::dropEvent(QDropEvent *event)
{
    mainWindow->prepareNewAction();

    if (event->mimeData()->hasUrls() && event->source() == NULL)
    {
        int numSf2 = -1;
        int commandCopy = 0;

        this->clearSelection();
        for (int i = 0; i < event->mimeData()->urls().count(); i++)
        {
            QString path = QUrl::fromPercentEncoding(event->mimeData()->urls().at(i).toEncoded());
            if (!path.isEmpty())
            {
                QString extension = path.split(".").last().toLower();
                if (extension == "sfz" || extension == "sf2"  || extension == "sf3" || extension == "sfark")
                    mainWindow->dragAndDrop(path, getItemID(itemAt(event->pos())), &numSf2);
                else if (extension == "wav")
                    mainWindow->dragAndDrop(path, getItemID(itemAt(event->pos())), &commandCopy);
            }
        }
        this->mainWindow->updateActions();
    }
    else
    {
        // Destination
        EltID idDest = this->getItemID(this->itemAt(event->pos()));

        // Constitution de la liste des éléments à copier / lier
        QList<EltID> liste = this->getAllIDs();

        mainWindow->dragAndDrop(idDest, liste);
    }

    mainWindow->updateDo();
}

void Tree::mousePressEvent(QMouseEvent *event)
{
    // Réaction seulement si un objet se trouve sous le clic
    if (this->indexAt(event->pos()).isValid())
        QTreeWidget::mousePressEvent(event);
}
