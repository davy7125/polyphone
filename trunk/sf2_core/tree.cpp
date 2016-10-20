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

#include "tree.h"
#include "mainwindow.h"
#include <QMenu>
#include <QAction>
#include <QUrl>
#include <QComboBox>
#include "pile_sf2.h"

Tree::Tree(QWidget *parent) : QTreeWidget(parent),
    _mainWindow(NULL),
    _treeMenu(NULL),
    _refresh(true),
    _updateSelectionInfo(true),
    _infoSelectedItemsNumber(0),
    _infoIsSelectedItemsTypeUnique(false),
    _infoIsSelectedItemsSf2Unique(false),
    _infoIsSelectedItemsFamilyUnique(false),
    _sf2(NULL),
    _searchedText(""),
    _displayedSf2(-1)
{}

Tree::~Tree()
{
    delete this->_treeMenu;
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
    this->_mainWindow = mainWindow;
    _sf2 = sf2;

    // Init combobox
    _comboSf2 = comboSf2;
    this->connect(_comboSf2, SIGNAL(currentIndexChanged(int)), SLOT(comboSf2IndexChanged(int)));
    _comboSf2->addItem(trUtf8("Tout afficher"), -1);

    // Connect the tree to the sf2 update
    this->connect(_sf2, SIGNAL(newElement(EltID)), SLOT(newElement(EltID)));
    this->connect(_sf2, SIGNAL(changeElementName(EltID,QString)), SLOT(changeElementName(EltID,QString)));
    this->connect(_sf2, SIGNAL(changeElementOrder(EltID,QString,bool)), SLOT(changeElementOrder(EltID,QString,bool)));
    this->connect(_sf2, SIGNAL(hideElement(EltID,bool)), this, SLOT(hideElement(EltID,bool)));
    this->connect(_sf2, SIGNAL(removeElement(EltID)), SLOT(removeElement(EltID)));

    // Create contextual menu
    this->_treeMenu = new menuClicDroit(mainWindow);
}

void Tree::trier(bool reinitCollapse)
{
    // Sort sf2 file in the alphabetical order
    int max = this->topLevelItemCount();
    QTreeWidgetItem *child;
    this->sortItems(0, Qt::AscendingOrder); // Column doesn't matter
    for (int i = 0; i < max; i ++)
        this->topLevelItem(i)->sortChildren(0, Qt::AscendingOrder);

    if (reinitCollapse)
    {
        // Expand first level only
        for (int i = 0; i < max; i++)
        {
            child = this->topLevelItem(i);
            child->setExpanded(1);
            for (int j = 0; j < child->childCount(); j++)
            {
                child->child(j)->setExpanded(0);
                for (int k = 0; k < child->child(j)->childCount(); k++)
                    child->child(j)->child(k)->setExpanded(0);
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
        else if (iData > numSf2) // Decrement the id
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
    TreeWidgetItem *child;

    // Etat initial
    _displayedElements.clear();
    bool isDisplayed = qStr.isEmpty();
    for (unsigned int i = 0; i < max; i ++)
    {
        child = (TreeWidgetItem *)this->topLevelItem(i);
        child->setHidden(false);
        max2 = child->childCount();

        // Niveau 1: en-têtes "échantillons", "instruments", "presets"
        for (unsigned int j = 0; j < max2; j++)
        {
            // Niveau 2: sample, instrument ou preset
            for (int k = 0; k < child->child(j)->childCount(); k++)
            {
                TreeWidgetItem * item = (TreeWidgetItem *)child->child(j)->child(k);
                if (!item->getEltHidden()) // Si l'élément n'est pas masqué par une suppression non définitive
                    item->setHidden(!isDisplayed);
            }
        }
    }

    for (unsigned int i = 0; i < max; i ++)
    {
        child = (TreeWidgetItem *)this->topLevelItem(i);

        if (displayedSf2 == -1 || child->getEltID().indexSf2 == displayedSf2)
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
                        TreeWidgetItem * item = (TreeWidgetItem *)child->child(j)->child(k);
                        if (!item->getEltHidden()) // Si l'élément n'est pas masqué par une suppression non définitive
                        {
                            if (item->text(0).toLower().indexOf(qStr) != -1 || item->isSelected())
                            {
                                if (item->getEltID().typeElement == elementSmpl)
                                    displaySample(item->getEltID().indexSf2, item->getEltID().indexElt);
                                else if (item->getEltID().typeElement == elementInst)
                                    displayInstrument(item->getEltID().indexSf2, item->getEltID().indexElt);
                                else if (item->getEltID().typeElement == elementPrst)
                                    displayPreset(item->getEltID().indexSf2, item->getEltID().indexElt);
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
    TreeWidgetItem *child;

    int max = this->topLevelItemCount();
    for (int i = 0; i < max; i++)
    {
        child = (TreeWidgetItem *)this->topLevelItem(i);
        if (child->getEltID().indexSf2 == id.indexSf2)
        {
            if (id.typeElement == elementSmpl)
                child = (TreeWidgetItem *)child->child(0);
            else if (id.typeElement == elementInst)
                child = (TreeWidgetItem *)child->child(1);
            else
                child = (TreeWidgetItem *)child->child(2);
            int numChilds = child->childCount();
            for (int j = 0; j < numChilds; j++)
            {
                // Affichage de la sous-arborescence
                if (((TreeWidgetItem *)child->child(j))->getEltID().indexElt == id.indexElt)
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
    if (!this->_refresh) return;

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
        _treeMenu->fermer->setEnabled(true);
        _treeMenu->nouveauSample->setEnabled(true);
        _treeMenu->nouvelInstrument->setEnabled(true);
        _treeMenu->nouveauPreset->setEnabled(true);

        // Associer
        if (typeUnique && (type == elementSmpl || type == elementInst))
            _treeMenu->associer->setEnabled(true);
        else
            _treeMenu->associer->setEnabled(false);

        // Remplacer
        if (nb == 1 && (type == elementInstSmpl || type == elementPrstInst))
            _treeMenu->remplacer->setEnabled(true);
        else
            _treeMenu->remplacer->setEnabled(false);

        // Supprimer, copier, coller
        if (typeUnique && (((type == elementInstSmpl || type == elementPrstInst) && familleUnique)
                           || type == elementSmpl || type == elementInst || type == elementPrst)
                && !this->_mainWindow->isPlaying())
        {
            _treeMenu->supprimer->setEnabled(true);
            _treeMenu->copier->setEnabled(true);
            _treeMenu->coller->setEnabled(true);
        }
        else
        {
            _treeMenu->supprimer->setEnabled(false);
            _treeMenu->copier->setEnabled(false);
            _treeMenu->coller->setEnabled(false);
        }

        // Renommer
        if (nb == 1 && typeUnique && (type == elementSmpl || type == elementInst || type == elementPrst || type == elementSf2))
        {
            _treeMenu->renommer->setText(trUtf8("&Renommer..."));
            _treeMenu->renommer->setEnabled(true);
        }
        else
        {
            if (nb > 1 && typeUnique && type == elementSmpl)
            {
                _treeMenu->renommer->setText(trUtf8("&Renommer en masse..."));
                _treeMenu->renommer->setEnabled(1);
            }
            else
            {
                _treeMenu->renommer->setText(trUtf8("&Renommer..."));
                _treeMenu->renommer->setEnabled(false);
            }
        }
    }
    else
    {
        // Aucune action possible
        _treeMenu->fermer->setEnabled(false);
        _treeMenu->nouveauSample->setEnabled(false);
        _treeMenu->nouvelInstrument->setEnabled(false);
        _treeMenu->nouveauPreset->setEnabled(false);
        _treeMenu->associer->setEnabled(false);
        _treeMenu->supprimer->setEnabled(false);
        _treeMenu->renommer->setText(tr("&Renommer..."));
        _treeMenu->renommer->setEnabled(false);
        _treeMenu->remplacer->setEnabled(false);
    }

    // Activation, désactivation des actions de MainWindow
    this->_mainWindow->updateActions();
}

void Tree::clicTreeRight()
{
    // Si au moins 1 élément de l'arborescence est sélectionné
    if (this->selectedItems().count())
    {
        // Affichage du menu contextuel
        this->_treeMenu->menu->exec(QCursor::pos());
    }
}

void Tree::desactiveSuppression()
{
    _treeMenu->supprimer->setEnabled(0);
}

void Tree::activeSuppression()
{
    _treeMenu->supprimer->setEnabled(1);
}

void Tree::clearPastedID()
{
    this->_idList.clear();
}

unsigned int Tree::getSelectedItemsNumber() {if (this->_updateSelectionInfo) updateSelectionInfo(); return this->_infoSelectedItemsNumber;}
bool Tree::isSelectedItemsTypeUnique() {if (this->_updateSelectionInfo) updateSelectionInfo(); return _infoIsSelectedItemsTypeUnique;}
bool Tree::isSelectedItemsSf2Unique() {if (this->_updateSelectionInfo) updateSelectionInfo(); return this->_infoIsSelectedItemsSf2Unique;}
bool Tree::isSelectedItemsFamilyUnique() {if (this->_updateSelectionInfo) updateSelectionInfo(); return this->_infoIsSelectedItemsFamilyUnique;}
EltID Tree::getFirstID()
{
    return this->selectedItem(0)->getEltID();
}
QList<EltID> Tree::getAllIDs()
{
    QList<EltID> listRet;
    QList<QTreeWidgetItem *> items = this->selectedItems();
    foreach (QTreeWidgetItem * item, items)
        listRet << ((TreeWidgetItem *)item)->getEltID();
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
            listSf2 << ((TreeWidgetItem *)this->topLevelItem(i))->getEltID();

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
    this->_refresh = refresh;

    QList<TreeWidgetItem*> treeItems = getEltsToId(id);
    if (!treeItems.isEmpty()) {
        for (int i = 0; i < treeItems.count() - 1; i++)
            treeItems[i]->setExpanded(true);
        treeItems.last()->setSelected(true);
    }
    this->_updateSelectionInfo = true;

    this->_refresh = true;
}

void Tree::selectNone(bool refresh)
{
    this->_refresh = refresh;
    this->clearSelection();
    this->_updateSelectionInfo = true;
    this->_refresh = true;
}

void Tree::updateSelectionInfo()
{
    this->_updateSelectionInfo = false;
    this->_infoIsSelectedItemsSf2Unique = true;
    this->_infoIsSelectedItemsTypeUnique = true;
    this->_infoIsSelectedItemsFamilyUnique = true;
    this->_infoSelectedItemsNumber = 0;
    int indexSf2 = -1;
    int type = -1;
    int family = -1;
    bool isInst = true;
    bool isPrst = true;
    EltID id;
    TreeWidgetItem * itemTmp;
    for (int i = 0; i < this->selectedItems().count(); i++)
    {
        itemTmp = (TreeWidgetItem *)this->selectedItems().at(i);
        if (!itemTmp->isHidden())
        {
            id = itemTmp->getEltID();
            this->_infoSelectedItemsNumber++;
            if (this->_infoSelectedItemsNumber == 1)
            {
                indexSf2 = id.indexSf2;
                type = id.typeElement;
                family = id.indexElt;
            }
            else
            {
                this->_infoIsSelectedItemsSf2Unique = this->_infoIsSelectedItemsSf2Unique && (indexSf2 == id.indexSf2);
                this->_infoIsSelectedItemsTypeUnique = this->_infoIsSelectedItemsTypeUnique && (type == id.typeElement);
                this->_infoIsSelectedItemsFamilyUnique = this->_infoIsSelectedItemsFamilyUnique && (family == id.indexElt);
            }
            isInst = isInst && (id.typeElement == elementInst || id.typeElement == elementInstSmpl);
            isPrst = isPrst && (id.typeElement == elementPrst || id.typeElement == elementPrstInst);
        }
    }
    this->_infoIsSelectedItemsFamilyUnique = this->_infoIsSelectedItemsFamilyUnique && (isInst || isPrst);
    if (!this->_infoSelectedItemsNumber)
    {
        this->_infoIsSelectedItemsSf2Unique = false;
        this->_infoIsSelectedItemsTypeUnique = false;
        this->_infoIsSelectedItemsFamilyUnique = false;
    }
}

TreeWidgetItem * Tree::selectedItem(unsigned int pos)
{
    if (this->getSelectedItemsNumber() > pos)
    {
        unsigned int nbHidden = 0;
        for (unsigned int i = 0; i < pos; i++)
        {
            while (this->selectedItems().at(i+nbHidden)->isHidden())
                nbHidden++;
        }
        return (TreeWidgetItem *)this->selectedItems().at(pos+nbHidden);
    }
    else
        return NULL;
}

void Tree::supprimerElt()
{
    if (this->_treeMenu->supprimer->isEnabled())
        this->_mainWindow->supprimerElt();
}

EltID Tree::getElementToSelectAfterDeletion()
{
    // Liste des éléments sélectionnés (ne doit pas être vide)
    QList<QTreeWidgetItem *> listSelectedItems = this->selectedItems();
    if (listSelectedItems.isEmpty())
        return EltID();

    // Vérification que le parent est le même pour tous les éléments
    TreeWidgetItem * itemParent = (TreeWidgetItem *)listSelectedItems.first()->parent();
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
    TreeWidgetItem * itemToSelect = itemParent;
    if (!listFin.isEmpty())
        itemToSelect = (TreeWidgetItem *)listFin.first();
    else if (!listDebut.isEmpty())
        itemToSelect = (TreeWidgetItem *)listDebut.last();

    // Récupération de l'identifiant
    return itemToSelect->getEltID();
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
                _mainWindow->prepareNewAction();
                _mainWindow->updateDo();

                // Copie des éléments
                this->_idList = this->getAllIDs();
            }
        }
    }
    else if (event->matches(QKeySequence::Paste))
    {
        _mainWindow->prepareNewAction();
        if (this->getSelectedItemsNumber() == 1 && this->_idList.size())
            _mainWindow->dragAndDrop(this->getFirstID(), this->_idList);
        _mainWindow->updateDo();
    }
    else if (event->key() == Qt::Key_Space)
    {
        _mainWindow->spaceKeyPressedInTree();
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
    _mainWindow->prepareNewAction();

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
                    _mainWindow->dragAndDrop(path, ((TreeWidgetItem *)itemAt(event->pos()))->getEltID(), &numSf2);
                else if (extension == "wav")
                    _mainWindow->dragAndDrop(path, ((TreeWidgetItem *)itemAt(event->pos()))->getEltID(), &commandCopy);
            }
        }
        this->_mainWindow->updateActions();
    }
    else
    {
        // Destination
        EltID idDest = ((TreeWidgetItem *)itemAt(event->pos()))->getEltID();

        // Constitution de la liste des éléments à copier / lier
        QList<EltID> liste = this->getAllIDs();

        _mainWindow->dragAndDrop(idDest, liste);
    }

    _mainWindow->updateDo();
}

void Tree::mousePressEvent(QMouseEvent *event)
{
    // Réaction seulement si un objet se trouve sous le clic
    if (this->indexAt(event->pos()).isValid())
        QTreeWidget::mousePressEvent(event);
}

void Tree::mouseDoubleClickEvent(QMouseEvent *event)
{
    EltID id = ((TreeWidgetItem *)itemAt(event->pos()))->getEltID();
    if (event->button() == Qt::LeftButton && (id.typeElement == elementInstSmpl || id.typeElement == elementPrstInst))
    {
        if (id.typeElement == elementInstSmpl)
        {
            id.indexElt = _sf2->get(id, champ_sampleID).wValue;
            id.typeElement = elementSmpl;
        }
        else
        {
            id.indexElt =_sf2->get(id, champ_instrument).wValue;
            id.typeElement = elementInst;
        }

        this->selectNone();
        this->select(id, true);
        this->scrollToItem(this->selectedItems()[0]);
    }
}

QList<TreeWidgetItem *> Tree::getEltsToId(EltID id, bool includeHidden)
{
    QList<TreeWidgetItem *> list;

    // Find the sf2
    unsigned int max = this->topLevelItemCount();
    int iTmp = -1;
    unsigned int i = 0;
    TreeWidgetItem *child;
    while (i < max && id.indexSf2 != iTmp)
    {
        child = (TreeWidgetItem*)this->topLevelItem(i);
        if (!child->getEltHidden() || includeHidden)
            iTmp = child->getEltID().indexSf2;
        i++;
    }
    if (id.indexSf2 != iTmp) // No corresponding sf2
        return QList<TreeWidgetItem *>();

    // Sf2 found
    list << child;
    if (id.typeElement == elementSf2)
        return list;

    // We go deeper
    if (id.typeElement == elementRootSmpl || id.typeElement == elementSmpl)
        list << (TreeWidgetItem*)child->child(0);
    else if (id.typeElement == elementRootInst || id.typeElement == elementInst || id.typeElement == elementInstSmpl)
        list << (TreeWidgetItem*)child->child(1);
    else if (id.typeElement == elementRootPrst || id.typeElement == elementPrst || id.typeElement == elementPrstInst)
        list << (TreeWidgetItem*)child->child(2);
    else
        return QList<TreeWidgetItem *>(); // Bad element type

    if (id.typeElement == elementRootSmpl || id.typeElement == elementRootInst || id.typeElement == elementRootPrst)
        return list;

    // We go deeper
    max = list.last()->childCount();
    i = 0;
    iTmp = -1;
    while (i < max && id.indexElt != iTmp)
    {
        child = (TreeWidgetItem*)list.last()->child(i);
        if (!child->getEltHidden() || includeHidden)
            iTmp = child->getEltID().indexElt;
        i++;
    }

    if (id.indexElt != iTmp) // No corresponding smpl, inst or prst
        return QList<TreeWidgetItem *>();

    // Smpl, Inst or Prst found
    list << child;
    if (id.typeElement == elementSmpl || id.typeElement == elementInst || id.typeElement == elementPrst)
        return list;

    // We go deeper
    max = list.last()->childCount();
    i = 0;
    iTmp = -1;
    while (i < max && id.indexElt2 != iTmp)
    {
        child = (TreeWidgetItem*)list.last()->child(i);
        if (!child->getEltHidden() || includeHidden)
            iTmp = child->getEltID().indexElt2;
        i++;
    }

    if (id.indexElt2 != iTmp) // No corresponding InstSmpl or PrstInst
        return QList<TreeWidgetItem *>();

    // InstSmpl or PrstInst found
    list << child;
    return list;
}

void Tree::newElement(EltID id)
{
    if (id.typeElement == elementSf2)
    {
        // Add a root element in the tree
        TreeWidgetItem * elt = new TreeWidgetItem(this, id);

        // Add the headers "smpl", "inst" and "prst" in the tree
        id.typeElement = elementRootSmpl;
        TreeWidgetItem * eltSmpl = new TreeWidgetItem(elt, id);
        eltSmpl->setText(0, QObject::trUtf8("Échantillons"));
        eltSmpl->setOrderedText("a");

        id.typeElement = elementRootInst;
        TreeWidgetItem * eltInst = new TreeWidgetItem(elt, id);
        eltInst->setText(0, QObject::trUtf8("Instruments"));
        eltInst->setOrderedText("b");

        id.typeElement = elementRootPrst;
        TreeWidgetItem * eltPrst = new TreeWidgetItem(elt, id);
        eltPrst->setText(0, QObject::trUtf8("Presets"));
        eltPrst->setOrderedText("c");

        this->addSf2InComboBox(id.indexSf2);
        this->trier(true);
    }
    else
    {
        // Add an element with a parent
        QList<TreeWidgetItem*> treeItems = getEltsToId(id.parent(true));
        if (!treeItems.isEmpty())
            new TreeWidgetItem(treeItems.last(), id);
    }
}

void Tree::hideElement(EltID id, bool isHidden)
{
    QList<TreeWidgetItem *> elements = getEltsToId(id, true);
    if (!elements.isEmpty())
    {
        TreeWidgetItem * element = elements.last();
        element->setEltHidden(isHidden);
        if (id.typeElement == elementSf2)
        {
            // Headers smpl, inst and prst
            ((TreeWidgetItem *)element->child(0))->setEltHidden(isHidden);
            ((TreeWidgetItem *)element->child(1))->setEltHidden(isHidden);
            ((TreeWidgetItem *)element->child(2))->setEltHidden(isHidden);
        }
    }

    this->_updateSelectionInfo = true;
}

void Tree::changeElementName(EltID id, QString name)
{
    QList<TreeWidgetItem *> elements = getEltsToId(id);
    if (!elements.isEmpty())
    {
        TreeWidgetItem * element = elements.last();
        element->setText(0, name);

        if (id.typeElement == elementSf2)
            renameSf2InComboBox(id.indexSf2, name);
    }
}

void Tree::changeElementOrder(EltID id, QString order, bool sort)
{
    QList<TreeWidgetItem *> elements = getEltsToId(id);
    if (!elements.isEmpty())
    {
        TreeWidgetItem * element = elements.takeLast();
        element->setOrderedText(order);
        if (sort)
        {
            if (elements.count() > 0)
                elements.last()->sortChildren(1, Qt::AscendingOrder); // Column doesn't matter
            else
                this->sortItems(1, Qt::AscendingOrder); // Column doesn't matter
        }
    }
}

void Tree::removeElement(EltID id)
{
    QList<TreeWidgetItem *> elements = getEltsToId(id, true);
    if (elements.empty())
        return;

    // Delete the item
    TreeWidgetItem * lastElement = elements.takeLast();
    if (id.typeElement == elementSf2)
    {
        // Delete sample, instrument and preset headers
        delete lastElement->child(2);
        delete lastElement->child(1);
        delete lastElement->child(0);

        // Remove the sf2 from the combobox
        this->removeSf2FromComboBox(id.indexSf2);
    }
    delete lastElement;

    // Decrement the ids
    switch (id.typeElement)
    {
    case elementSf2:
        for (int i = 0; i < this->topLevelItemCount(); i++)
        {
            TreeWidgetItem * item = (TreeWidgetItem *)this->topLevelItem(i);
            if (item->getEltID().indexSf2 > id.indexSf2)
                item->decrementSf2();
        }
        break;
    case elementSmpl: case elementInst: case elementPrst:
        if (elements.count() > 0)
        {
            for (int i = 0; i < elements.last()->childCount(); i++)
            {
                TreeWidgetItem * item = (TreeWidgetItem *)elements.last()->child(i);
                if (item->getEltID().indexElt > id.indexElt)
                    item->decrementElement();
            }
        }
        break;
    case elementInstSmpl: case elementPrstInst:
        if (elements.count() > 0)
        {
            for (int i = 0; i < elements.last()->childCount(); i++)
            {
                TreeWidgetItem * item = (TreeWidgetItem *)elements.last()->child(i);
                if (item->getEltID().indexElt2 > id.indexElt2)
                    item->decrementElement2();
            }
        }
        break;
    default:
        // Nothing
        break;
    }

    this->_updateSelectionInfo = true;
}
