/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
**                2014      Andrea Celani                                 **
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

#include "pagetable.h"
#include "mainwindow.h"
#include "dialog_paramglobal.h"
#include "dialog_visualizer.h"
#include "dialog_space.h"
#include "dialog_duplication.h"
#include "dialogselection.h"
#include "graphicsviewrange.h"
#include <QScrollBar>
#include <QMenu>

QList<PageTable::Modulator> PageTable::_modulatorCopy;

PageTable::PageTable(TypePage typePage, QWidget *parent) : Page(typePage, parent),
    table(NULL),
    tableMod(NULL)
{
}

void PageTable::afficher()
{
    // Prepare page
    if (_pushRangeMode->isChecked())
    {
        updateKeyboard();
        afficheRange();
    }
    else
        afficheTable();

    // Switch page
    this->_qStackedWidget->setCurrentWidget(this);
}

void PageTable::afficheTable()
{
    int posV = this->table->verticalScrollBar()->value();
    this->_preparation = true;

    // Destruction des cellules précédentes
    table->blockSignals(true);
    this->table->clear();
    table->blockSignals(false);

    bool error;
    QList<EltID> ids = getUniqueInstOrPrst(error, false, false);

    if (!ids.isEmpty())
    {
        ////// AFFICHAGE DES PARAMETRES GLOBAUX //////
        foreach (EltID id, ids)
            addGlobal(id, ids.count() > 1);

        ////// AFFICHAGE DES PARAMETRES PAR ELEMENT LIÉ //////
        if (ids.count() == 1)
            addDivisions(ids.first());

        ////// MISE EN FORME DE LA TABLE (couleurs, alignement) //////
        formatTable(ids.count() > 1);

        ///////////////////// REMPLISSAGE DES MODS //////////////////////////
        if (ids.count() == 1)
            this->afficheMod(ids.first());
        else
            afficheEditMod();
    }

    // Fin de la préparation
    this->_preparation = false;
    this->reselect();
    this->table->verticalScrollBar()->setValue(posV);
}

void PageTable::addGlobal(EltID id, bool multiGlobal)
{
    quint16 champTmp;
    genAmountType genValTmp;
    int offsetStart = 0;
    int offsetEnd = 0;
    int offsetStartLoop = 0;
    int offsetEndLoop = 0;
    int row;
    char T[20];

    QString qStr;
    if (multiGlobal)
    {
        qStr = _sf2->getQstr(id, champ_name).left(10);
        qStr.append("\n");
        qStr.append(_sf2->getQstr(id, champ_name).mid(10).left(10));
    }
    else
        qStr = trUtf8("Global");

    this->table->addColumn(0, qStr);
    id.typeElement = this->contenantGen;
    id.indexElt2 = 0;
    for (int i = 0; i < _sf2->count(id); i++)
    {
        id.indexMod = i;
        champTmp = _sf2->get(id, champ_sfGenOper).wValue;
        genValTmp = _sf2->get(id, champ_sfGenAmount).genValue;
        switch (champTmp)
        {
        case champ_startAddrsOffset:
            offsetStart += genValTmp.shAmount;
            break;
        case champ_startAddrsCoarseOffset:
            offsetStart += 32768 * genValTmp.shAmount;
            break;
        case champ_endAddrsOffset:
            offsetEnd += genValTmp.shAmount;
            break;
        case champ_endAddrsCoarseOffset:
            offsetEnd += 32768 * genValTmp.shAmount;
            break;
        case champ_startloopAddrsOffset:
            offsetStartLoop += genValTmp.shAmount;
            break;
        case champ_startloopAddrsCoarseOffset:
            offsetStartLoop += 32768 * genValTmp.shAmount;
            break;
        case champ_endloopAddrsOffset:
            offsetEndLoop += genValTmp.shAmount;
            break;
        case champ_endloopAddrsCoarseOffset:
            offsetEndLoop += 32768 * genValTmp.shAmount;
            break;
        default:
            row = this->table->getRow(champTmp);
            if (row > -1)
            {
                if (champTmp == champ_sampleModes)
                    this->table->setLoopModeImage(row, 0, genValTmp.wAmount);
                else
                    this->table->item(row, 0)->setText(getTextValue(T, champTmp, genValTmp));
            }
        }
    }
    if (offsetStart && this->table->getRow(champ_startAddrsOffset) > -1)
    {
        row = this->table->getRow(champ_startAddrsOffset);
        this->table->item(row, 0)->setText(getTextValue(T, champ_startAddrsOffset, offsetStart));
    }
    if (offsetEnd && this->table->getRow(champ_endAddrsOffset) > -1)
    {
        row = this->table->getRow(champ_endAddrsOffset);
        this->table->item(row, 0)->setText(getTextValue(T, champ_endAddrsOffset, offsetEnd));
    }
    if (offsetStartLoop && this->table->getRow(champ_startloopAddrsOffset) > -1)
    {
        row = this->table->getRow(champ_startloopAddrsOffset);
        this->table->item(row, 0)->setText(getTextValue(T, champ_startloopAddrsOffset, offsetStartLoop));
    }
    if (offsetEndLoop && this->table->getRow(champ_endloopAddrsOffset) > -1)
    {
        row = this->table->getRow(champ_endloopAddrsOffset);
        this->table->item(row, 0)->setText(getTextValue(T, champ_endloopAddrsOffset, offsetEndLoop));
    }
    this->table->setID(id, 0);
}

void PageTable::addDivisions(EltID id)
{
    quint16 champTmp;
    genAmountType genValTmp;
    char T[20]; char str[40]; char str2[40];
    int row;
    EltID id2 = id;
    EltID id3 = id;
    int nbSmplInst = 0;
    id.typeElement = this->lien;
    id2.typeElement = this->lienGen;
    id3.typeElement = this->contenu;

    for (int i = 0; i < _sf2->count(id); i++)
    {
        id.indexElt2 = i;
        if (!_sf2->get(id, champ_hidden).bValue)
        {
            id2.indexElt2 = i;

            // Ajout d'un élément lié
            int numCol = 1;

            // Détermination de la colonne
            Champ cElementLie;
            if (this->contenant == elementInst)
                cElementLie = champ_sampleID;
            else
                cElementLie = champ_instrument;

            id3.indexElt = _sf2->get(id, cElementLie).wValue;
            sprintf(str, "%.03d%.03d%s", _sf2->get(id, champ_keyRange).rValue.byLo,
                    _sf2->get(id, champ_velRange).rValue.byLo,
                    _sf2->getQstr(id3, champ_name).toStdString().c_str());
            QString qStr = _sf2->getQstr(id3, champ_name).left(10);
            qStr.append("\n");
            qStr.append(_sf2->getQstr(id3, champ_name).mid(10).left(10));
            for (int j = 1; j < nbSmplInst + 1; j++)
            {
                // note et vélocité basses de la colonne et prise en compte du nom de l'élément lié
                id3.indexElt = _sf2->get(this->table->getID(j), cElementLie).wValue;
                sprintf(str2, "%.03d%.03d%s", _sf2->get(this->table->getID(j), champ_keyRange).rValue.byLo,
                        _sf2->get(this->table->getID(j), champ_velRange).rValue.byLo,
                        _sf2->getQstr(id3, champ_name).toStdString().c_str());
                if (strcmp(str, str2) > 0)
                    numCol++;
            }

            nbSmplInst++;
            int offsetStart = 0;
            int offsetEnd = 0;
            int offsetStartLoop = 0;
            int offsetEndLoop = 0;
            this->table->addColumn(numCol, qStr);
            for (int j = 0; j < _sf2->count(id2); j++)
            {
                id2.indexMod = j;
                champTmp = _sf2->get(id2, champ_sfGenOper).wValue;
                genValTmp = _sf2->get(id2, champ_sfGenAmount).genValue;
                switch (champTmp)
                {
                case champ_startAddrsOffset:
                    offsetStart += genValTmp.shAmount;
                    break;
                case champ_startAddrsCoarseOffset:
                    offsetStart += 32768 * genValTmp.shAmount;
                    break;
                case champ_endAddrsOffset:
                    offsetEnd += genValTmp.shAmount;
                    break;
                case champ_endAddrsCoarseOffset:
                    offsetEnd += 32768 * genValTmp.shAmount;
                    break;
                case champ_startloopAddrsOffset:
                    offsetStartLoop += genValTmp.shAmount;
                    break;
                case champ_startloopAddrsCoarseOffset:
                    offsetStartLoop += 32768 * genValTmp.shAmount;
                    break;
                case champ_endloopAddrsOffset:
                    offsetEndLoop += genValTmp.shAmount;
                    break;
                case champ_endloopAddrsCoarseOffset:
                    offsetEndLoop += 32768 * genValTmp.shAmount;
                    break;
                default:
                    row = this->table->getRow(champTmp);
                    if (row > -1)
                    {
                        if (champTmp == champ_sampleModes)
                            this->table->setLoopModeImage(row, numCol, genValTmp.wAmount);
                        else
                            this->table->item(row, numCol)->setText(getTextValue(T, champTmp, genValTmp));
                    }
                }
            }

            if (offsetStart && this->table->getRow(champ_startAddrsOffset) > -1)
            {
                row = this->table->getRow(champ_startAddrsOffset);
                this->table->item(row, numCol)->setText(getTextValue(T, champ_startAddrsOffset, offsetStart));
            }
            if (offsetEnd && this->table->getRow(champ_endAddrsOffset) > -1)
            {
                row = this->table->getRow(champ_endAddrsOffset);
                this->table->item(row, numCol)->setText(getTextValue(T, champ_endAddrsOffset, offsetEnd));
            }
            if (offsetStartLoop && this->table->getRow(champ_startloopAddrsOffset) > -1)
            {
                row = this->table->getRow(champ_startloopAddrsOffset);
                this->table->item(row, numCol)->setText(getTextValue(T, champ_startloopAddrsOffset, offsetStartLoop));
            }
            if (offsetEndLoop && this->table->getRow(champ_endloopAddrsOffset) > -1)
            {
                row = this->table->getRow(champ_endloopAddrsOffset);
                this->table->item(row, numCol)->setText(getTextValue(T, champ_endloopAddrsOffset, offsetEndLoop));
            }
            this->table->setID(id2, numCol);
        }
    }

}

void PageTable::formatTable(bool multiGlobal)
{
    QBrush brush1(QPixmap(":/style/fond.png"));
    QBrush brush2(QPixmap(":/style/fondJaune.png"));
    if (this->contenant == elementInst)
    {
        // First column with a hatching pattern
        if (!multiGlobal)
        {
            for (int j = 0; j < this->table->rowCount(); j++)
            {
                if (j < 9)
                    this->table->item(j, 0)->setBackground(brush1);
                else if (j < 13)
                    this->table->item(j, 0)->setBackground(brush2);
                else if (j < 15)
                    this->table->item(j, 0)->setBackground(brush1);
                else if (j < 23)
                    this->table->item(j, 0)->setBackground(brush2);
                else if (j < 33)
                    this->table->item(j, 0)->setBackground(brush1);
                else if (j < 41)
                    this->table->item(j, 0)->setBackground(brush2);
                else if (j < 46)
                    this->table->item(j, 0)->setBackground(brush1);
                else
                    this->table->item(j, 0)->setBackground(brush2);
                this->table->item(j, 0)->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
            }
        }

        // Yellow rows
        for (int i = multiGlobal ? 0 : 1; i < this->table->columnCount(); i++)
        {
            for (int j = 0; j < this->table->rowCount(); j++)
            {
                if (j < 9) {}
                else if (j < 13)
                    this->table->item(j, i)->setBackgroundColor(QColor(255, 255, 200));
                else if (j < 15) {}
                else if (j < 23)
                    this->table->item(j, i)->setBackgroundColor(QColor(255, 255, 200));
                else if (j < 33) {}
                else if (j < 41)
                    this->table->item(j, i)->setBackgroundColor(QColor(255, 255, 200));
                else if (j < 46) {}
                else
                    this->table->item(j, i)->setBackgroundColor(QColor(255, 255, 200));
                this->table->item(j, i)->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
            }
        }
    }
    else
    {
        // First column with a hatching pattern
        if (!multiGlobal)
        {
            for (int j = 0; j < this->table->rowCount(); j++)
            {
                if (j < 8)
                    this->table->item(j, 0)->setBackground(brush1);
                else if (j < 11)
                    this->table->item(j, 0)->setBackground(brush2);
                else if (j < 13)
                    this->table->item(j, 0)->setBackground(brush1);
                else if (j < 21)
                    this->table->item(j, 0)->setBackground(brush2);
                else if (j < 31)
                    this->table->item(j, 0)->setBackground(brush1);
                else if (j < 39)
                    this->table->item(j, 0)->setBackground(brush2);
                else
                    this->table->item(j, 0)->setBackground(brush1);
                this->table->item(j, 0)->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
            }
        }

        // Yellow rows
        for (int i = multiGlobal ? 0 : 1; i < this->table->columnCount(); i++)
        {
            for (int j = 0; j < this->table->rowCount(); j++)
            {
                if (j < 8) {}
                else if (j < 11)
                    this->table->item(j, i)->setBackgroundColor(QColor(255, 255, 200));
                else if (j < 13) {}
                else if (j < 21)
                    this->table->item(j, i)->setBackgroundColor(QColor(255, 255, 200));
                else if (j < 31) {}
                else if (j < 39)
                    this->table->item(j, i)->setBackgroundColor(QColor(255, 255, 200));
                this->table->item(j, i)->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
            }
        }
    }
    for (int i = 0; i < 4; i++)
        this->table->hideRow(i);

    if (!Config::getInstance()->getSameWidthTable())
        this->table->resizeColumnsToContents();
}

void PageTable::afficheRange()
{
    _rangeEditor->display(this->_tree->getFirstID());
}

void PageTable::afficheMod(EltID id, int selectedIndex)
{
    // Affichage du menu ou non
    if (id.typeElement == elementPrst || id.typeElement == elementInst)
    {
        _pushCopyMod->setMenu(_menu);
        _pushCopyMod->setStyleSheet("QPushButton { text-align: left; }");
#ifdef Q_OS_MAC
        _pushCopyMod->setMaximumWidth(43);
#else
        _pushCopyMod->setMaximumWidth(38);
#endif
        _pushCopyMod->setToolTip("");
    }
    else
    {
        _pushCopyMod->setMenu(NULL);
        _pushCopyMod->setStyleSheet("");
        _pushCopyMod->setMaximumWidth(24);
    }

    QString qStr;
    genAmountType genValTmp;
    SFModulator sfModTmp;
    char T[60];
    if (id.typeElement == this->contenant)
        id.typeElement = this->contenantMod;
    else
        id.typeElement = this->lienMod;
    this->tableMod->clear();
    int iVal;
    QIcon icon;
    int numLigne = 0;
    for (int i = 0; i < _sf2->count(id); i++)
    {
        id.indexMod = i;
        if (!_sf2->get(id, champ_hidden).bValue)
        {
            // Ajout d'un modulateur
            this->tableMod->addRow(numLigne, id);
            this->tableMod->item(numLigne, 5)->setText(getTextValue(T, champ_indexMod, id.indexMod));

            sfModTmp = _sf2->get(id, champ_sfModSrcOper).sfModValue;
            iVal = 4*sfModTmp.D + 8*sfModTmp.P + sfModTmp.Type + 1;
            sprintf(T, ":/icones/courbe%.2d", iVal);
            icon = QIcon(T);
            this->tableMod->item(numLigne, 6)->setIcon(icon);
            if (sfModTmp.Index == 127 && sfModTmp.CC == 0)
            {
                // On cherche le lien
                iVal = getAssociatedMod(id);
                if (iVal > -1)
                {
                    sprintf(T, "%s: #%d", trUtf8("Modulateur").toStdString().c_str(), iVal+1);
                    qStr = T;
                }
                else
                    qStr = trUtf8("Lien (invalide)");
            }
            else qStr = getIndexName(sfModTmp.Index, sfModTmp.CC);
            this->tableMod->item(numLigne, 6)->setText(qStr);
            sfModTmp = _sf2->get(id, champ_sfModAmtSrcOper).sfModValue;
            iVal = 4*sfModTmp.D + 8*sfModTmp.P + sfModTmp.Type + 1;
            sprintf(T, ":/icones/courbe%.2d", iVal);
            icon = QIcon(T);
            this->tableMod->item(numLigne, 7)->setIcon(icon);
            qStr = getIndexName(sfModTmp.Index, sfModTmp.CC);
            genValTmp = _sf2->get(id, champ_modAmount).genValue;
            qStr.append(QString::fromUtf8(" × ")).append(getTextValue(T, champ_modAmount, genValTmp));
            this->tableMod->item(numLigne, 7)->setText(qStr);
            this->spinAmount->setValue(genValTmp.shAmount);
            genValTmp = _sf2->get(id, champ_sfModDestOper).genValue;
            if (genValTmp.wAmount > 99)
            {
                // lien vers modulateur
                qStr = trUtf8("Modulateur") + ": #" + QString::number(genValTmp.wAmount - 32767);
            }
            else qStr = getGenName(genValTmp.wAmount);
            genValTmp = _sf2->get(id, champ_sfModTransOper).genValue;
            qStr.append(getTextValue(T, champ_sfModTransOper, genValTmp));
            this->tableMod->item(numLigne, 8)->setText(qStr);
            numLigne++;
        }
    }
    for (int i = 0; i < 5; i++)
        this->tableMod->hideColumn(i);
    if (selectedIndex > -1)
    {
        for (int i = 0; i < this->tableMod->rowCount(); i++)
            if (this->tableMod->getID(i).indexMod == selectedIndex)
                this->tableMod->selectRow(i);
    }
    this->tableMod->resizeColumnToContents(5);
    this->tableMod->resizeColumnToContents(6);
    this->tableMod->resizeColumnToContents(7);

    this->afficheEditMod();
}

void PageTable::afficheEditMod()
{
    if (_pushCopyMod->menu())
    {
        _pushCopyMod->setToolTip(trUtf8("Copier / dupliquer des modulateurs"));
        if (this->tableMod->getSelectedIDs().isEmpty())
        {
            _menu->actions()[0]->setText(trUtf8("Dupliquer les modulateurs vers..."));
            _menu->actions()[1]->setText(trUtf8("Copier l'ensemble des modulateurs"));
        }
        else
        {
            _menu->actions()[0]->setText(trUtf8("Dupliquer la sélection vers..."));
            _menu->actions()[1]->setText(trUtf8("Copier les modulateurs sélectionnés"));
        }
    }
    else
    {
        if (this->tableMod->getSelectedIDs().isEmpty())
            _pushCopyMod->setToolTip(trUtf8("Copier l'ensemble des modulateurs"));
        else
            _pushCopyMod->setToolTip(trUtf8("Copier les modulateurs sélectionnés"));
    }

    // Only one selected division ?
    bool singleDivision = true;
    if (this->_tree->getSelectedItemsNumber() != 1)
    {
        singleDivision = false;
        this->tableMod->clear();
    }
    this->pushNouveauMod->setEnabled(singleDivision);
    this->_pushCopyMod->setEnabled(singleDivision);

    QList<EltID> selectedModIDs = this->tableMod->getSelectedIDs();
    this->pushSupprimerMod->setEnabled(singleDivision && !selectedModIDs.isEmpty());
    if (singleDivision && selectedModIDs.size() == 1)
    {
        EltID id = selectedModIDs.first();
        int iTmp;
        quint16 wTmp;
        bool bTmp;
        SFModulator sfMod;

        this->spinAmount->setValue(_sf2->get(id, champ_modAmount).shValue);
        this->spinAmount->setEnabled(true);
        this->checkAbs->setChecked(_sf2->get(id, champ_sfModTransOper).wValue == 2);
        this->checkAbs->setEnabled(true);
        sfMod = _sf2->get(id, champ_sfModSrcOper).sfModValue;
        iTmp = sfMod.D + 2 * sfMod.P + 4 * sfMod.Type;
        this->comboSource1Courbe->setEnabled(true);
        this->comboSource1Courbe->setCurrentIndex(iTmp / 4);
        this->comboSource1Courbe->setModelColumn(iTmp % 4);

        // COMBOBOX SOURCE 1
        this->comboSource1->removeItemsAfterLim();
        addAvailableSenderMod(this->comboSource1, id);

        // Sélection et activation
        wTmp = sfMod.Index;
        bTmp = sfMod.CC;
        if (wTmp == 127 && bTmp == 0)
        {
            // On cherche le lien
            int iVal = getAssociatedMod(id);
            if (iVal > -1) this->comboSource1->selectIndex(100, 32768 + iVal);
            else this->comboSource1->selectIndex(this->getSrcIndex(0, 0));
        }
        else this->comboSource1->selectIndex(this->getSrcIndex(wTmp, bTmp));
        this->comboSource1->setEnabled(true);
        sfMod = _sf2->get(id, champ_sfModAmtSrcOper).sfModValue;
        iTmp = sfMod.D + 2 * sfMod.P + 4 * sfMod.Type;
        this->comboSource2Courbe->setEnabled(true);
        this->comboSource2Courbe->setCurrentIndex(iTmp / 4);
        this->comboSource2Courbe->setModelColumn(iTmp % 4);

        // COMBOBOX SOURCE 2
        // Sélection et activation
        wTmp = sfMod.Index;
        bTmp = sfMod.CC;
        this->comboSource2->selectIndex(this->getSrcIndex(wTmp, bTmp));
        this->comboSource2->setEnabled(true);

        // COMBOBOX DESTINATION
        this->comboDestination->removeItemsAfterLim();
        addAvailableReceiverMod(this->comboDestination, id);

        // Sélection et activation
        wTmp = _sf2->get(id, champ_sfModDestOper).wValue;
        this->comboDestination->selectIndex(this->getDestIndex(wTmp), wTmp);
        this->comboDestination->setEnabled(true);
    }
    else
    {
        this->spinAmount->setValue(0);
        this->spinAmount->setEnabled(false);
        this->checkAbs->setChecked(false);
        this->checkAbs->setEnabled(false);
        this->comboSource1Courbe->setCurrentIndex(0);
        this->comboSource1Courbe->setModelColumn(0);
        this->comboSource1Courbe->setEnabled(false);
        this->comboSource2Courbe->setCurrentIndex(0);
        this->comboSource2Courbe->setModelColumn(0);
        this->comboSource2Courbe->setEnabled(false);
        this->comboDestination->setCurrentIndex(0);
        this->comboDestination->setEnabled(false);
        this->comboSource1->setCurrentIndex(0);
        this->comboSource1->setEnabled(false);
        this->comboSource2->setCurrentIndex(0);
        this->comboSource2->setEnabled(false);
    }
}

void PageTable::updateId(EltID id)
{
    // Mise à jour tableau, id a été supprimé
    EltID id2;
    if (id.typeElement == elementSf2)
    {
        // décrémentation sf2 ?
        for (int i = 0; i < this->table->columnCount(); i++)
        {
            id2 = this->table->getID(i);
            if (id2.indexSf2 > id.indexSf2)
            {
                id2.indexSf2--;
                this->table->setID(id2, i);
            }
        }
    }
    else if (id.typeElement == elementInst || id.typeElement == elementPrst)
    {
        // décrémentation elt ?
        for (int i = 0; i < this->table->columnCount(); i++)
        {
            id2 = this->table->getID(i);
            if (id2.indexElt > id.indexElt && id2.indexSf2 == id.indexSf2)
            {
                id2.indexElt--;
                this->table->setID(id2, i);
            }
        }
    }
    else if (id.typeElement == elementInstSmpl || id.typeElement == elementPrstInst)
    {
        // décrémentation elt2 ?
        for (int i = 0; i < this->table->columnCount(); i++)
        {
            id2 = this->table->getID(i);
            if (id2.indexElt2 > id.indexElt2  && id2.indexSf2 == id.indexSf2
                    && id2.indexElt == id.indexElt)
            {
                id2.indexElt2--;
                this->table->setID(id2, i);
            }
        }
    }

    // Affichage mod
    int currentRow = this->tableMod->currentRow();
    this->afficheMod(this->table->getID(0));
    this->tableMod->selectRow(currentRow);
}

void PageTable::resetChamp(int colonne, Champ champ1, Champ champ2)
{
    EltID id = table->getID(colonne);
    bool ok = _sf2->isSet(id, champ1);
    if (champ2 != champ_unknown)
        ok = ok || _sf2->isSet(id, champ2);

    if (ok)
    {
        // On efface la donnée
        id = table->getID(colonne);
        _sf2->reset(id, champ1);
        if (champ2 != champ_unknown)
            _sf2->reset(id, champ2);
    }
}

void PageTable::setOffset(int ligne, int colonne, Champ champ1, Champ champ2)
{
    EltID id = this->table->getID(colonne);
    bool ok;
    char T[20];
    QString texte = this->table->item(ligne, colonne)->text().left(9);
    genAmountType genAmount = getValue(texte, champ1, ok);
    if (ok)
    {
        // Enregistrement de la nouvelle valeur
        genAmountType genAmount2 = getValue(texte, champ2, ok);
        int iVal = limit(32768 * genAmount2.shAmount + genAmount.shAmount, champ1, id);
        genAmount2.shAmount = iVal / 32768;
        genAmount.shAmount = iVal % 32768;
        if (genAmount.shAmount != _sf2->get(id, champ1).shValue ||
                genAmount2.shAmount != _sf2->get(id, champ2).shValue)
        {
            // Modification du sf2
            id = this->table->getID(colonne);
            Valeur value;
            value.genValue = genAmount;
            _sf2->set(id, champ1, value);
            value.genValue = genAmount2;
            _sf2->set(id, champ2, value);
        }
        // Mise à jour de la valeur dans la cellule
        int offset = _sf2->get(id, champ1).genValue.shAmount +
                32768 * _sf2->get(id, champ2).genValue.shAmount;
        this->table->item(ligne, colonne)->setText(getTextValue(T, champ1, offset));
    }
    else
    {
        // Restauration valeur précédente
        if (_sf2->isSet(id, champ1) || _sf2->isSet(id, champ2))
        {
            int offset = _sf2->get(id, champ1).genValue.shAmount +
                    32768 * _sf2->get(id, champ2).genValue.shAmount;
            this->table->item(ligne, colonne)->setText(getTextValue(T, champ1, offset));
        }
        else
            this->table->item(ligne, colonne)->setText("");
    }
}

void PageTable::actionBegin()
{
    if (this->_preparation)
        return;
    _sf2->prepareNewActions();
}

void PageTable::actionFinished()
{
    if (this->_preparation)
        return;
    this->_mainWindow->updateDo();
}

void PageTable::set(int ligne, int colonne, bool allowPropagation)
{
    if (this->_preparation)
        return;

    // Modification d'un élément du tableau
    Champ champ = this->table->getChamp(ligne);
    if (champ == champ_unknown)
        return;

    EltID id = this->table->getID(colonne);
    if (allowPropagation && Config::getInstance()->getRepercussionStereo() && id.typeElement == elementInstSmpl &&
            champ != champ_pan)
    {
        // Répercussion des modifications sur le sample stéréo s'il est présent
        EltID idSmpl = id;
        idSmpl.typeElement = elementSmpl;
        idSmpl.indexElt = _sf2->get(id, champ_sampleID).wValue;
        SFSampleLink typeLink = _sf2->get(idSmpl, champ_sfSampleType).sfLinkValue;
        if (typeLink == rightSample || typeLink == leftSample || typeLink == linkedSample ||
                typeLink == RomRightSample || typeLink == RomLeftSample || typeLink == RomLinkedSample)
        {
            int numSmpl2 = _sf2->get(idSmpl, champ_wSampleLink).wValue;
            rangesType keyRange = _sf2->get(id, champ_keyRange).rValue;
            rangesType velRange = _sf2->get(id, champ_velRange).rValue;

            // Recherche d'une correspondance dans les samples liés
            bool ok = true;
            int numCol2 = -1;
            EltID idTmp = id;
            for (int i = 1; i < table->columnCount(); i++)
            {
                idTmp = this->table->getID(i);
                if (!_sf2->get(idTmp, champ_hidden).bValue && i != colonne)
                {
                    rangesType keyRange2 = _sf2->get(idTmp, champ_keyRange).rValue;
                    rangesType velRange2 = _sf2->get(idTmp, champ_velRange).rValue;
                    if (keyRange2.byLo == keyRange.byLo && keyRange2.byHi == keyRange.byHi &&
                            velRange2.byLo == velRange.byLo && velRange2.byHi == velRange.byHi)
                    {
                        int iTmp = _sf2->get(idTmp, champ_sampleID).wValue;
                        if (iTmp == idSmpl.indexElt)
                            ok = false; // ambiguité
                        else if (iTmp == numSmpl2)
                        {
                            if (numCol2 == -1)
                                numCol2 = i;
                            else
                                ok = false; // ambiguité
                        }
                    }
                }
            }

            // Application de la modification
            if (numCol2 != -1 && ok)
            {
                table->blockSignals(true);

                if (champ == champ_sampleModes)
                {
                    table->item(ligne, numCol2)->setData(Qt::DecorationRole, table->item(ligne, colonne)->data(Qt::DecorationRole));
                    table->item(ligne, numCol2)->setData(Qt::UserRole, table->item(ligne, colonne)->data(Qt::UserRole));
                }
                else
                    table->item(ligne, numCol2)->setText(table->item(ligne, colonne)->text());
                table->blockSignals(false);
                set(ligne, numCol2, false);
            }
        }
    }

    if (champ == champ_sampleModes)
    {
        // Effacement du champ?
        if (table->item(ligne, colonne)->data(Qt::UserRole).isNull())
            resetChamp(colonne, champ, champ_unknown);
        else
        {
            EltID id = this->table->getID(colonne);
            genAmountType genAmount;
            genAmount.wAmount = table->item(ligne, colonne)->data(Qt::UserRole).toInt();

            // Modification champ
            if (genAmount.wAmount != _sf2->get(id, champ).wValue || !_sf2->isSet(id, champ))
            {
                // Modification du sf2
                Valeur value;
                value.genValue = genAmount;
                _sf2->set(id, champ, value);
            }
        }
    }
    else
    {
        if (table->item(ligne, colonne)->text().isEmpty())
        {
            // Effacement d'un paramètre ?
            switch ((int)champ)
            {
            case champ_startAddrsOffset:
                resetChamp(colonne, champ_startAddrsOffset, champ_startAddrsCoarseOffset);
                break;
            case champ_endAddrsOffset:
                resetChamp(colonne, champ_endAddrsOffset, champ_endAddrsCoarseOffset);
                break;
            case champ_startloopAddrsOffset:
                resetChamp(colonne, champ_startloopAddrsOffset, champ_startloopAddrsCoarseOffset);
                break;
            case champ_endloopAddrsOffset:
                resetChamp(colonne, champ_endloopAddrsOffset, champ_endloopAddrsCoarseOffset);
                break;
            default:
                resetChamp(colonne, champ, champ_unknown);
            }
        }
        else
        {
            _preparation = 1;
            switch ((int)champ)
            {
            case champ_startAddrsOffset:
                setOffset(ligne, colonne, champ_startAddrsOffset, champ_startAddrsCoarseOffset);
                break;
            case champ_endAddrsOffset:
                setOffset(ligne, colonne, champ_endAddrsOffset, champ_endAddrsCoarseOffset);
                break;
            case champ_startloopAddrsOffset:
                setOffset(ligne, colonne, champ_startloopAddrsOffset, champ_startloopAddrsCoarseOffset);
                break;
            case champ_endloopAddrsOffset:
                setOffset(ligne, colonne, champ_endloopAddrsOffset, champ_endloopAddrsCoarseOffset);
                break;
            default:{
                QString texte = this->table->item(ligne, colonne)->text().left(9);
                bool ok;
                char T[20];
                EltID id = this->table->getID(colonne);
                genAmountType genAmount = getValue(texte, champ, ok);
                if (ok)
                {
                    // Modification champ
                    if (genAmount.wAmount != _sf2->get(id, champ).wValue || !_sf2->isSet(id, champ))
                    {
                        // Modification du sf2
                        Valeur value;
                        value.genValue = genAmount;
                        _sf2->set(id, champ, value);
                    }
                    // Mise à jour de la valeur dans la cellule
                    table->item(ligne, colonne)->setText(getTextValue(T, champ, genAmount));
                }
                else
                {
                    // Restauration valeur précédente
                    if (_sf2->isSet(id, champ))
                        this->table->item(ligne, colonne)->setText(getTextValue(T, champ, _sf2->get(id, champ).genValue));
                    else this->table->item(ligne, colonne)->setText("");
                }
            }
            }
            _preparation = 0;
        }
    }

    if (!Config::getInstance()->getSameWidthTable())
        this->table->resizeColumnsToContents();

    // Mise à jour partie mod (car entre 2 des mods peuvent être définitivement détruits, et les index peuvent être mis à jour)
    id = this->table->getID(colonne);
    this->afficheMod(id);

    if (champ == champ_overridingRootKey || champ == champ_keyRange)
        customizeKeyboard();
}

void PageTable::setAmount()
{
    if (this->_preparation) return;
    this->_preparation = 1;
    EltID id = this->tableMod->getID();
    EltID id2 = id;
    if (id2.typeElement == this->contenantMod)
        id.typeElement = this->contenant;
    else
        id.typeElement = this->lien;
    Valeur val;
    val.shValue = this->spinAmount->value();
    if (id2.typeElement != elementUnknown)
    {
        // Comparaison avec valeur précédente
        if (_sf2->get(id2, champ_modAmount).shValue != val.shValue)
        {
            _sf2->prepareNewActions();
            // Reprise des identificateurs si modification
            id = this->tableMod->getID();
            id2 = id;
            if (id2.typeElement == this->contenantMod)
                id.typeElement = this->contenant;
            else
                id.typeElement = this->lien;
            _sf2->set(id2, champ_modAmount, val);
            if (this->tableMod->selectedItems().count())
            {
                int currentRow = this->tableMod->selectedItems().takeFirst()->row();
                this->afficheMod(id);
                this->tableMod->selectRow(currentRow);
            }
            else
                this->afficheMod(id);
            this->_mainWindow->updateDo();
        }
    }
    this->_preparation = 0;
}

void PageTable::setAbs()
{
    if (this->_preparation) return;
    this->_preparation = 1;
    EltID id = this->tableMod->getID();
    EltID id2 = id;
    if (id2.typeElement == this->contenantMod)
        id.typeElement = this->contenant;
    else
        id.typeElement = this->lien;
    Valeur val;
    if (this->checkAbs->isChecked())
        val.wValue = 2;
    else
        val.wValue = 0;
    if (id2.typeElement != elementUnknown)
    {
        // Comparaison avec valeur précédente
        if (_sf2->get(id2, champ_sfModTransOper).wValue != val.wValue)
        {
            _sf2->prepareNewActions();
            // Reprise des identificateurs si modification
            id = this->tableMod->getID();
            id2 = id;
            if (id2.typeElement == this->contenantMod)
                id.typeElement = this->contenant;
            else
                id.typeElement = this->lien;
            _sf2->set(id2, champ_sfModTransOper, val);
            if (this->tableMod->selectedItems().count())
            {
                int currentRow = this->tableMod->selectedItems().takeFirst()->row();
                this->afficheMod(id);
                this->tableMod->selectRow(currentRow);
            }
            else
                this->afficheMod(id);
            this->_mainWindow->updateDo();
        }
    }
    this->_preparation = 0;
}

void PageTable::setSourceType(int row, int column)
{
    if (this->_preparation) return;
    this->_preparation = 1;
    EltID id = this->tableMod->getID();
    EltID id2 = id;
    if (id2.typeElement == this->contenantMod)
        id.typeElement = this->contenant;
    else
        id.typeElement = this->lien;
    Valeur val;
    int D = column % 2;
    int P = column / 2;
    int type = row;
    if (id2.typeElement != elementUnknown)
    {
        // Comparaison avec valeur précédente
        val.sfModValue = _sf2->get(id2, champ_sfModSrcOper).sfModValue;
        if (val.sfModValue.D != D || val.sfModValue.P != P || val.sfModValue.Type != type)
        {
            val.sfModValue.D = D;
            val.sfModValue.P = P;
            val.sfModValue.Type = type;
            _sf2->prepareNewActions();
            // Reprise des identificateurs si modification
            id = this->tableMod->getID();
            id2 = id;
            if (id2.typeElement == this->contenantMod)
                id.typeElement = this->contenant;
            else
                id.typeElement = this->lien;
            _sf2->set(id2, champ_sfModSrcOper, val);
            if (this->tableMod->selectedItems().count())
            {
                int currentRow = this->tableMod->selectedItems().takeFirst()->row();
                this->afficheMod(id);
                this->tableMod->selectRow(currentRow);
            }
            else
                this->afficheMod(id);
            this->_mainWindow->updateDo();
        }
    }
    this->_preparation = 0;
}

void PageTable::setSourceAmountType(int row, int column)
{
    if (this->_preparation) return;
    this->_preparation = 1;
    EltID id = this->tableMod->getID();
    EltID id2 = id;
    if (id2.typeElement == this->contenantMod)
        id.typeElement = this->contenant;
    else
        id.typeElement = this->lien;
    Valeur val;
    int D = column % 2;
    int P = column / 2;
    int type = row;
    if (id2.typeElement != elementUnknown)
    {
        // Comparaison avec valeur précédente
        val.sfModValue = _sf2->get(id2, champ_sfModAmtSrcOper).sfModValue;
        if (val.sfModValue.D != D || val.sfModValue.P != P || val.sfModValue.Type != type)
        {
            val.sfModValue.D = D;
            val.sfModValue.P = P;
            val.sfModValue.Type = type;
            _sf2->prepareNewActions();
            // Reprise des identificateurs si modification
            id = this->tableMod->getID();
            id2 = id;
            if (id2.typeElement == this->contenantMod)
                id.typeElement = this->contenant;
            else
                id.typeElement = this->lien;
            _sf2->set(id2, champ_sfModAmtSrcOper, val);
            if (this->tableMod->selectedItems().count())
            {
                int currentRow = this->tableMod->selectedItems().takeFirst()->row();
                this->afficheMod(id);
                this->tableMod->selectRow(currentRow);
            }
            else
                this->afficheMod(id);
            this->_mainWindow->updateDo();
        }
    }
    this->_preparation = 0;
}

void PageTable::setDest(int index)
{
    if (this->_preparation) return;
    this->_preparation = 1;
    EltID id = this->tableMod->getID();
    EltID id2 = id;
    if (id2.typeElement == this->contenantMod)
        id.typeElement = this->contenant;
    else
        id.typeElement = this->lien;
    if (index < 32768) index = getDestNumber(index);
    Valeur val;
    if (id2.typeElement != elementUnknown)
    {
        // Comparaison avec valeur précédente
        val.sfGenValue = _sf2->get(id2, champ_sfModDestOper).sfGenValue;
        if (val.sfGenValue != (Champ)index)
        {
            _sf2->prepareNewActions();
            // Reprise des identificateurs si modification
            id = this->tableMod->getID();
            id2 = id;
            if (id2.typeElement == this->contenantMod)
                id.typeElement = this->contenant;
            else
                id.typeElement = this->lien;
            // La valeur précédente était-elle un lien ?
            if (val.wValue >= 32768)
            {
                EltID id3 = id2;
                id3.indexMod = val.wValue - 32768;
                if (id3.indexMod < _sf2->count(id2))
                {
                    Valeur val2;
                    val2.sfModValue = _sf2->get(id3, champ_sfModSrcOper).sfModValue;
                    val2.sfModValue.CC = 0;
                    val2.sfModValue.Index = 0;
                    _sf2->set(id3, champ_sfModSrcOper, val2);
                }
            }
            // Lien ?
            if (index >= 32768 && _sf2->count(id2) > index - 32768)
            {
                EltID id3 = id2;
                id3.indexMod = index - 32768;
                // Destruction des précédents liens vers le nouvel élément
                Valeur val2;
                EltID id4 = id2;
                do
                {
                    id4.indexMod = this->getAssociatedMod(id3);
                    if (id4.indexMod != -1)
                    {
                        val2.sfGenValue = (Champ)0;
                        _sf2->set(id4, champ_sfModDestOper, val2);
                    }
                } while (id4.indexMod != -1);
                val2.sfModValue = _sf2->get(id3, champ_sfModSrcOper).sfModValue;
                val2.sfModValue.Index = 127;
                val2.sfModValue.CC = 0;
                _sf2->set(id3, champ_sfModSrcOper, val2);
            }
            val.sfGenValue = (Champ)index;
            _sf2->set(id2, champ_sfModDestOper, val);
            if (this->tableMod->selectedItems().count())
            {
                int currentRow = this->tableMod->selectedItems().takeFirst()->row();
                this->afficheMod(id);
                this->tableMod->selectRow(currentRow);
            }
            else
                this->afficheMod(id);
            this->_mainWindow->updateDo();
        }
    }
    this->_preparation = 0;
}

void PageTable::setSource(int index)
{
    if (this->_preparation) return;
    this->_preparation = 1;
    EltID id = this->tableMod->getID();
    EltID id2 = id;
    if (id2.typeElement == this->contenantMod)
        id.typeElement = this->contenant;
    else
        id.typeElement = this->lien;
    bool CC;
    if (index < 32768) index = getSrcNumber(index, CC);
    else CC = 0;
    Valeur val;
    if (id2.typeElement != elementUnknown)
    {
        // Comparaison avec valeur précédente
        val.sfModValue = _sf2->get(id2, champ_sfModSrcOper).sfModValue;
        if (index < 32768)
        {
            if (val.sfModValue.Index != index || val.sfModValue.CC != CC)
            {
                // modification, pas de lien
                _sf2->prepareNewActions();
                // Reprise des identificateurs si modification
                id = this->tableMod->getID();
                id2 = id;
                if (id2.typeElement == this->contenantMod)
                    id.typeElement = this->contenant;
                else
                    id.typeElement = this->lien;
                // Valeur précédente était un lien ?
                if (val.sfModValue.Index == 127 && val.sfModValue.CC == 0)
                {
                    // On casse le lien
                    EltID id3 = id2;
                    Valeur val2;
                    do
                    {
                        id3.indexMod = this->getAssociatedMod(id2);
                        if (id3.indexMod != -1)
                        {
                            val2.wValue = 0;
                            _sf2->set(id3, champ_sfModDestOper, val2);
                        }
                    } while (id3.indexMod != -1);
                }
                val.sfModValue.Index = index;
                val.sfModValue.CC = CC;
                _sf2->set(id2, champ_sfModSrcOper, val);
                if (this->tableMod->selectedItems().count())
                {
                    int currentRow = this->tableMod->selectedItems().takeFirst()->row();
                    this->afficheMod(id);
                    this->tableMod->selectRow(currentRow);
                }
                else
                    this->afficheMod(id);
                this->_mainWindow->updateDo();
            }
        }
        else
        {
            _sf2->prepareNewActions();
            // Reprise des identificateurs si modification
            id = this->tableMod->getID();
            id2 = id;
            if (id2.typeElement == this->contenantMod)
                id.typeElement = this->contenant;
            else
                id.typeElement = this->lien;
            if (val.sfModValue.Index != 127 || val.sfModValue.CC != CC)
            {
                // La valeur précédente n'est pas un lien
                // Modification sender
                EltID id3 = id2;
                id3.indexMod = index - 32768;
                // sender était-il lié à un autre mod ?
                Valeur val2;
                val2.wValue = _sf2->get(id3, champ_sfModDestOper).wValue;
                if (val2.wValue >= 32768)
                {
                    // On enlève le lien précédent
                    EltID id4 = id2;
                    id4.indexMod = val2.wValue-32768;
                    if (_sf2->count(id) > id4.indexMod)
                    {
                        val2.sfModValue = _sf2->get(id4, champ_sfModSrcOper).sfModValue;
                        val2.sfModValue.CC = 0;
                        val2.sfModValue.Index = 0;
                        _sf2->set(id4, champ_sfModSrcOper, val2);
                    }
                }
                val2.wValue = 32768 + id2.indexMod;
                _sf2->set(id3, champ_sfModDestOper, val2);
                val.sfModValue.Index = 127;
                val.sfModValue.CC = CC;
                _sf2->set(id2, champ_sfModSrcOper, val);
                if (this->tableMod->selectedItems().count())
                {
                    int currentRow = this->tableMod->selectedItems().takeFirst()->row();
                    this->afficheMod(id);
                    this->tableMod->selectRow(currentRow);
                }
                else
                    this->afficheMod(id);
                this->_mainWindow->updateDo();
            }
            else
            {
                // La valeur précédente était un lien
                EltID id3 = id2;
                id3.indexMod = this->getAssociatedMod(id2);
                if (id3.indexMod != index - 32768)
                {
                    // On enlève les liens précédents
                    Valeur val2;
                    do
                    {
                        id3.indexMod = this->getAssociatedMod(id2);
                        if (id3.indexMod != -1)
                        {
                            val2.wValue = 0;
                            _sf2->set(id3, champ_sfModDestOper, val2);
                        }
                    } while (id3.indexMod != -1);
                    // Modification sender
                    id3 = id2;
                    id3.indexMod = index - 32768;
                    // sender était-il lié à un autre mod ?
                    val2.wValue = _sf2->get(id3, champ_sfModDestOper).wValue;
                    if (val2.wValue >= 32768)
                    {
                        // On enlève le lien précédent
                        EltID id4 = id2;
                        id4.indexMod = val2.wValue-32768;
                        if (_sf2->count(id2) > id4.indexMod)
                        {
                            val2.sfModValue = _sf2->get(id4, champ_sfModSrcOper).sfModValue;
                            val2.sfModValue.CC = 0;
                            val2.sfModValue.Index = 0;
                            _sf2->set(id4, champ_sfModSrcOper, val2);
                        }
                    }
                    val2.wValue = 32768 + id2.indexMod;
                    _sf2->set(id3, champ_sfModDestOper, val2);
                    val.sfModValue.Index = 127;
                    val.sfModValue.CC = CC;
                    _sf2->set(id2, champ_sfModSrcOper, val);
                    if (this->tableMod->selectedItems().count())
                    {
                        int currentRow = this->tableMod->selectedItems().takeFirst()->row();
                        this->afficheMod(id);
                        this->tableMod->selectRow(currentRow);
                    }
                    else
                        this->afficheMod(id);
                    this->_mainWindow->updateDo();
                }
            }
        }
    }
    this->_preparation = 0;
}

void PageTable::setSource2(int index)
{
    if (this->_preparation) return;
    this->_preparation = 1;
    EltID id = this->tableMod->getID();
    EltID id2 = id;
    if (id2.typeElement == this->contenantMod)
        id.typeElement = this->contenant;
    else
        id.typeElement = this->lien;
    bool CC;
    index = getSrcNumber(index, CC);
    Valeur val;
    if (id2.typeElement != elementUnknown)
    {
        // Comparaison avec valeur précédente
        val.sfModValue = _sf2->get(id2, champ_sfModAmtSrcOper).sfModValue;
        if (val.sfModValue.Index != index || val.sfModValue.CC != CC)
        {
            // modification
            _sf2->prepareNewActions();
            // Reprise des identificateurs si modification
            id = this->tableMod->getID();
            id2 = id;
            if (id2.typeElement == this->contenantMod)
                id.typeElement = this->contenant;
            else
                id.typeElement = this->lien;
            val.sfModValue.Index = index;
            val.sfModValue.CC = CC;
            _sf2->set(id2, champ_sfModAmtSrcOper, val);
            if (this->tableMod->selectedItems().count())
            {
                int currentRow = this->tableMod->selectedItems().takeFirst()->row();
                this->afficheMod(id);
                this->tableMod->selectRow(currentRow);
            }
            else
                this->afficheMod(id);
            this->_mainWindow->updateDo();
        }
    }
    this->_preparation = 0;
}

void PageTable::reselect()
{
    this->table->clearSelection();
    this->table->setSelectionMode(QAbstractItemView::MultiSelection);
    QList<EltID> listID = this->_tree->getAllIDs();
    this->table->setRowHidden(0, false); // Selection will be on a visible row
    foreach (EltID id, listID)
        this->select(id);
    this->table->setSelectionMode(QAbstractItemView::ExtendedSelection);
    customizeKeyboard();
    this->table->setRowHidden(0, true); // It's now hidden again
}

void PageTable::select(EltID id)
{
    this->_preparation = true;
    EltID id2;
    int max;
    for (int i = 0; i < this->table->columnCount(); i++)
    {
        id2 = this->table->getID(i);
        if (id.typeElement == id2.typeElement && id.indexSf2 == id2.indexSf2 && id.indexElt == id2.indexElt &&
                (id.indexElt2 == id2.indexElt2 || id.typeElement == elementInst || id.typeElement == elementPrst))
        {
            table->blockSignals(true);
            this->table->item(0, i)->setSelected(true);
            table->blockSignals(false);
            max = this->table->horizontalScrollBar()->maximum();
            if (max / this->table->columnCount() > 62)
                this->table->horizontalScrollBar()->setValue((max*(i-1)) / this->table->columnCount());
            else
                this->table->scrollToItem(this->table->item(10, i), QAbstractItemView::PositionAtCenter);
        }
    }
    this->_preparation = false;
}

void PageTable::selected()
{
    if (this->_preparation) return;

    // Mise à jour de la sélection dans l'arborescence
    this->_tree->blockSignals(true);
    QList<QTableWidgetItem*> listItems = table->selectedItems();
    int compte = listItems.count();
    if (compte)
    {
        QList<EltID> ids;
        for (int i = 0; i < compte; i++)
            ids << this->table->getID(listItems.at(i)->column());
        selectInTree(ids);

        // Mise à jour des informations sur les mods
        this->_preparation = true;
        int colonne = listItems.last()->column();
        this->afficheMod(this->table->getID(colonne));
        this->_preparation = false;
    }
    customizeKeyboard();
    this->_tree->blockSignals(false);
}

void PageTable::updateKeyboard()
{
    customizeKeyboard(true);
}

void PageTable::customizeKeyboard(bool withAllDivisions)
{
    _mainWindow->clearKeyboardCustomisation();

    QList<int> selectedColumns;
    QList<QTableWidgetItem*> listSelectedItems = table->selectedItems();
    foreach (QTableWidgetItem * item, listSelectedItems)
        if (!selectedColumns.contains(item->column()))
            selectedColumns << item->column();

    // Si aucune colonne n'est sélectionnée, on affiche toutes les étendues
    if (withAllDivisions || selectedColumns.isEmpty())
    {
        selectedColumns.clear();
        selectedColumns << 0;
    }

    // Affichage des étendues et rootkeys des divisions sélectionnées
    foreach (int colonne, selectedColumns)
    {
        EltID id = this->table->getID(colonne);
        if (id.typeElement == elementInstSmpl || id.typeElement == elementPrstInst)
        {
            int rootKey = -1;
            if (id.typeElement == elementInstSmpl)
            {
                if (_sf2->isSet(id, champ_overridingRootKey))
                    rootKey = _sf2->get(id, champ_overridingRootKey).wValue;
                else
                {
                    EltID idSmpl = id;
                    idSmpl.typeElement = elementSmpl;
                    idSmpl.indexElt = _sf2->get(id, champ_sampleID).wValue;
                    rootKey = _sf2->get(idSmpl, champ_byOriginalPitch).wValue;
                }
            }

            rangesType keyRange;
            keyRange.byLo = 0;
            keyRange.byHi = 127;
            if (_sf2->isSet(id, champ_keyRange))
                keyRange = _sf2->get(id, champ_keyRange).rValue;
            else
            {
                if (id.typeElement == elementInstSmpl)
                    id.typeElement = elementInst;
                else
                    id.typeElement = elementPrst;
                if (_sf2->isSet(id, champ_keyRange))
                    keyRange = _sf2->get(id, champ_keyRange).rValue;
            }
            _mainWindow->setRangeAndRootKey(rootKey, keyRange.byLo, keyRange.byHi);
        }
        else if (id.typeElement == elementInst || id.typeElement == elementPrst)
        {
            rangesType defaultKeyRange;
            if (_sf2->isSet(id, champ_keyRange))
                defaultKeyRange = _sf2->get(id, champ_keyRange).rValue;
            else
            {
                defaultKeyRange.byLo = 0;
                defaultKeyRange.byHi = 127;
            }
            if (id.typeElement == elementInst)
                id.typeElement = elementInstSmpl;
            else
                id.typeElement = elementPrstInst;
            int nbInstSmpl = _sf2->count(id);
            for (int i = 0; i < nbInstSmpl; i++)
            {
                id.indexElt2 = i;
                if (!_sf2->get(id, champ_hidden).bValue)
                {
                    rangesType keyRange;
                    if (_sf2->isSet(id, champ_keyRange))
                        keyRange = _sf2->get(id, champ_keyRange).rValue;
                    else
                        keyRange = defaultKeyRange;
                    _mainWindow->setRangeAndRootKey(-1, keyRange.byLo, keyRange.byHi);
                }
            }
        }
    }
}

void PageTable::addAvailableReceiverMod(ComboBox *combo, EltID id)
{
    if (id.typeElement != elementInstMod && id.typeElement != elementInstSmplMod && \
            id.typeElement != elementPrstMod && id.typeElement != elementPrstInstMod)
        return;
    // Parcours des mods autour de id
    EltID id2 = id;
    EltID id3 = id;
    quint16 wTmp;
    int compte;
    bool stop, found;
    int nbMod = _sf2->count(id);
    for (int i = 0; i < nbMod; i++)
    {
        id2.indexMod = i;
        if (id2.indexMod != id.indexMod && !_sf2->get(id2, champ_hidden).bValue)
        {
            // On regarde si id2 peut recevoir des signaux de id
            // Condition : id2 n'émet pas, directement ou indirectement, de signaux à id
            compte = 0;
            found = false;
            stop = false;
            id3.indexMod = id2.indexMod;
            while (!stop)
            {
                wTmp = _sf2->get(id3, champ_sfModDestOper).wValue;
                if (wTmp >= 32768)
                {
                    if (wTmp - 32768 < nbMod)
                    {
                        // Lien valide
                        id3.indexMod = wTmp - 32768;
                        if (id3.indexMod == id.indexMod)
                        {
                            // Pas autorisé
                            stop = true;
                            found = true;
                        }
                    }
                    else stop = true; // Lien non valide
                }
                else stop = true; // Pas de lien
                compte++;
                if (compte > nbMod) stop = true; // Boucle infinie
            }
            if (!found)
            {
                // id2 peut recevoir les signaux de id
                combo->addItem(trUtf8("Modulateur") + ": #" + QString::number(id2.indexMod + 1));
            }
        }
    }
}

void PageTable::addAvailableSenderMod(ComboBox *combo, EltID id)
{
    if (id.typeElement != elementInstMod && id.typeElement != elementInstSmplMod && \
            id.typeElement != elementPrstMod && id.typeElement != elementPrstInstMod)
        return;
    // Parcours des mods autour de id
    EltID id2 = id;
    EltID id3 = id;
    EltID id4 = id;
    quint16 wTmp;
    SFModulator sfMod;
    int compte, indModSender;
    bool stop, found;
    int nbMod = _sf2->count(id);
    for (int i = 0; i < nbMod; i++)
    {
        id2.indexMod = i;
        if (id2.indexMod != id.indexMod && !_sf2->get(id2, champ_hidden).bValue)
        {
            // On regarde si id2 peut envoyer des signaux à id
            // Condition : id2 ne reçoit pas, directement ou indirectement, de signaux de id
            compte = 0;
            found = false;
            stop = false;
            id3.indexMod = id2.indexMod;
            while (!stop)
            {
                sfMod = _sf2->get(id3, champ_sfModSrcOper).sfModValue;
                if (sfMod.CC == 0 && sfMod.Index == 127)
                {
                    // On cherche le mod qui envoie des signaux à id3
                    indModSender = -1;
                    for (int j = 0; j < nbMod; j++)
                    {
                        id4.indexMod = j;
                        if (!_sf2->get(id4, champ_hidden).bValue)
                        {
                            wTmp = _sf2->get(id4, champ_sfModDestOper).wValue;
                            if (wTmp == 32768 + id3.indexMod)
                                indModSender = j;
                        }
                    }
                    if (indModSender != -1)
                    {
                        // Le sender a été trouvé
                        if (indModSender == id.indexMod)
                        {
                            // Pas autorisé
                            stop = true;
                            found = true;
                        }
                        else id3.indexMod = indModSender;
                    }
                    else stop = true; // Lien non valide
                }
                else stop = true; // Pas de lien
                compte++;
                if (compte > nbMod) stop = true; // Boucle infinie
            }
            if (!found)
            {
                // id2 peut envoyer des signaux à id
                combo->addItem(trUtf8("Modulateur") + ": #" + QString::number(id2.indexMod + 1));
            }
        }
    }
}

int PageTable::getAssociatedMod(EltID id)
{
    if (id.typeElement != elementInstMod && id.typeElement != elementInstSmplMod && \
            id.typeElement != elementPrstMod && id.typeElement != elementPrstInstMod)
        return -1;
    int iVal = -1;
    EltID id2 = id;
    for (int j = 0; j < _sf2->count(id); j++)
    {
        id2.indexMod = j;
        if (id2.indexMod != id.indexMod  && !_sf2->get(id2, champ_hidden).bValue)
        {
            if (_sf2->get(id2, champ_sfModDestOper).wValue == 32768 + id.indexMod)
                iVal = j;
        }
    }
    if (iVal > -1)
    {
        id2.indexMod = iVal;
        return id2.indexMod;
    }
    else return -1;
}

void PageTable::supprimerMod()
{
    int rowToSelect  = tableMod->currentRow();
    int nbRow = tableMod->rowCount();

    _sf2->prepareNewActions();

    // Suppression mod
    QList<EltID> listIDs = this->tableMod->getSelectedIDs();
    if (listIDs.isEmpty())
        return;

    foreach (EltID id, listIDs)
    {
        // Liens vers le mod ?
        int iVal = -1;
        Valeur val;
        EltID id2 = id;
        do
        {
            iVal = getAssociatedMod(id);
            if (iVal != -1)
            {
                // Suppression du lien
                val.sfGenValue = (Champ)0;
                id2.indexMod = iVal;
                _sf2->set(id2, champ_sfModDestOper, val);
            }
        } while (iVal != -1);

        // Le mod est-il lié ?
        if (_sf2->get(id, champ_sfModDestOper).wValue >= 32768)
        {
            id2.indexMod = _sf2->get(id, champ_sfModDestOper).wValue - 32768;
            if (id2.indexMod < _sf2->count(id))
            {
                val.sfModValue = _sf2->get(id2, champ_sfModSrcOper).sfModValue;
                val.sfModValue.CC = 0;
                val.sfModValue.Index = 0;
                _sf2->set(id2, champ_sfModSrcOper, val);
            }
        }
        _sf2->remove(id);
    }

    this->_mainWindow->updateDo();

    if (rowToSelect >= nbRow - listIDs.size())
        rowToSelect = nbRow - listIDs.size() - 1;
    this->afficheMod(this->_tree->getFirstID());
    this->tableMod->selectRow(rowToSelect);
}

void PageTable::nouveauMod()
{
    _sf2->prepareNewActions();

    // Création nouveau mod
    EltID id = this->_tree->getFirstID();
    if (id.typeElement == elementInst) id.typeElement = elementInstMod;
    else if (id.typeElement == elementPrst) id.typeElement = elementPrstMod;
    else if (id.typeElement == elementInstSmpl) id.typeElement = elementInstSmplMod;
    else if (id.typeElement == elementPrstInst) id.typeElement = elementPrstInstMod;
    else return;
    id.indexMod = _sf2->add(id);

    // initialisation
    Valeur val;
    val.sfGenValue = (Champ)0;
    val.wValue = 0;
    _sf2->set(id, champ_modAmount, val);
    _sf2->set(id, champ_sfModTransOper, val);
    val.sfModValue.CC = 0;
    val.sfModValue.D = 0;
    val.sfModValue.Index = 0;
    val.sfModValue.P = 0;
    val.sfModValue.Type = 0;
    _sf2->set(id, champ_sfModSrcOper, val);
    _sf2->set(id, champ_sfModAmtSrcOper, val);
    if (id.typeElement == elementPrstMod || id.typeElement == elementPrstInstMod)
        val.sfGenValue = (Champ)52;
    _sf2->set(id, champ_sfModDestOper, val);
    this->afficheMod(this->_tree->getFirstID(), id.indexMod);
    this->_mainWindow->updateDo();
}

void PageTable::copyMod()
{
    // Sauvegarde des mods
    if (this->_tree->getSelectedItemsNumber() != 1)
        return;
    EltID id = this->_tree->getFirstID();
    if (id.typeElement == elementInst) id.typeElement = elementInstMod;
    else if (id.typeElement == elementPrst) id.typeElement = elementPrstMod;
    else if (id.typeElement == elementInstSmpl) id.typeElement = elementInstSmplMod;
    else if (id.typeElement == elementPrstInst) id.typeElement = elementPrstInstMod;
    else return;

    _modulatorCopy = getModList(id);
}

QList<PageTable::Modulator> PageTable::getModList(EltID id)
{
    QList<Modulator> listRet;

    QList<EltID> listSelectedMods = tableMod->getSelectedIDs();
    if (listSelectedMods.isEmpty())
    {
        // All modulators are copied
        for (int i = 0; i < _sf2->count(id); i++)
        {
            id.indexMod = i;
            if (!_sf2->get(id, champ_hidden).bValue)
            {
                Modulator modTmp;
                modTmp.modSrcOper = _sf2->get(id, champ_sfModSrcOper).sfModValue;
                modTmp.modDestOper = _sf2->get(id, champ_sfModDestOper).sfGenValue;
                modTmp.modAmount = _sf2->get(id, champ_modAmount).shValue;
                modTmp.modAmtSrcOper = _sf2->get(id, champ_sfModAmtSrcOper).sfModValue;
                modTmp.modTransOper = _sf2->get(id, champ_sfModTransOper).sfTransValue;
                modTmp.index = id.indexMod;
                listRet << modTmp;
            }
        }
    }
    else
    {
        foreach (EltID id, listSelectedMods)
        {
            if (!_sf2->get(id, champ_hidden).bValue)
            {
                Modulator modTmp;
                modTmp.modSrcOper = _sf2->get(id, champ_sfModSrcOper).sfModValue;
                modTmp.modDestOper = _sf2->get(id, champ_sfModDestOper).sfGenValue;
                modTmp.modAmount = _sf2->get(id, champ_modAmount).shValue;
                modTmp.modAmtSrcOper = _sf2->get(id, champ_sfModAmtSrcOper).sfModValue;
                modTmp.modTransOper = _sf2->get(id, champ_sfModTransOper).sfTransValue;
                modTmp.index = id.indexMod;
                listRet << modTmp;
            }
        }
    }

    // Liste de tous les index
    QList<int> listIndex;
    foreach (Modulator mod, listRet)
        if (!listIndex.contains(mod.index))
            listIndex << mod.index;

    for (int i = 0; i < listRet.size(); i++)
    {
        Modulator mod = listRet.at(i);

        if ((int)mod.modDestOper >= 32768)
        {
            // Les liens cassés disparaissent
            int link = mod.modDestOper - 32768;
            if (listIndex.contains(link))
                mod.modDestOper = (Champ)(32768 + listIndex.indexOf(link));
            else
                mod.modDestOper = champ_fineTune;
        }

        // Les index commencent à 0
        mod.index = listIndex.indexOf(mod.index);

        listRet[i] = mod;
    }

    for (int i = 0; i < listRet.size(); i++)
    {
        Modulator mod = listRet.at(i);
        if (mod.modSrcOper.Index == 127 && mod.modSrcOper.CC == 0)
        {
            // On cherche le lien
            bool found = false;
            for (int j = 0; j < listRet.size(); j++)
            {
                if (i != j && listRet.at(j).modDestOper == 32768 + i)
                        found = true;
            }

            if (!found)
            {
                mod.modSrcOper.Index = 0;
                listRet[i] = mod;
            }
        }
    }

    return listRet;
}

void PageTable::pasteMod()
{
    _sf2->prepareNewActions();

    QList<EltID> ids = this->_tree->getAllIDs();
    foreach (EltID id, ids)
        pasteMod(id, _modulatorCopy);

    if (ids.count() == 1)
        this->afficheMod(this->_tree->getFirstID());
    else
        this->afficheEditMod();

    this->_mainWindow->updateDo();
}

void PageTable::pasteMod(EltID id, QList<Modulator> modulators)
{
    if (id.typeElement == elementInst) id.typeElement = elementInstMod;
    else if (id.typeElement == elementPrst) id.typeElement = elementPrstMod;
    else if (id.typeElement == elementInstSmpl) id.typeElement = elementInstSmplMod;
    else if (id.typeElement == elementPrstInst) id.typeElement = elementPrstInstMod;
    else return;

    if (id.typeElement == elementPrstMod || id.typeElement == elementPrstInstMod)
    {
        // Vérification que toutes les destinations sont possibles
        Champ champTmp;
        for (int i = 0; i < modulators.size(); i++)
        {
            champTmp = modulators[i].modDestOper;
            QString warnQStr = trUtf8("Action impossible : ");
            if (champTmp == champ_startAddrsOffset ||
                    champTmp == champ_startAddrsCoarseOffset ||
                    champTmp == champ_startloopAddrsOffset ||
                    champTmp == champ_startloopAddrsCoarseOffset ||
                    champTmp == champ_endAddrsOffset ||
                    champTmp == champ_endAddrsCoarseOffset ||
                    champTmp == champ_endloopAddrsOffset ||
                    champTmp == champ_endloopAddrsCoarseOffset)
            {
                QMessageBox::warning(this, trUtf8("Attention"), warnQStr +
                                     trUtf8("les offsets ne peuvent être modulés dans un preset."));
                return;
            }
            else if (champTmp == champ_keynum || champTmp == champ_velocity ||
                     champTmp == champ_sampleModes ||
                     champTmp == champ_exclusiveClass ||
                     champTmp == champ_overridingRootKey)
            {
                QMessageBox::warning(this, trUtf8("Attention"), warnQStr +
                                     "\"" + getGenName(champTmp) + trUtf8("\" ne peut être modulé dans un preset."));
                return;
            }
        }
    }

    // Index minimal des mods à copier
    int offsetIndex = _sf2->count(id);
    for (int i = 0; i < modulators.size(); i++)
    {
        Modulator mod = modulators.at(i);
        mod.index += offsetIndex;
        if ((int)mod.modDestOper >= 32768)
            mod.modDestOper = (Champ)(mod.modDestOper + offsetIndex);
        modulators[i] = mod;
    }

    // Création de nouveaux mods
    QList<int> listIndex;
    for (int i = 0; i < modulators.size(); i++)
        listIndex.append(_sf2->add(id));

    // Copie des configurations des mods sauvegardés
    Valeur valTmp;
    Modulator modTmp;
    for (int i = 0; i < modulators.size(); i++)
    {
        id.indexMod = listIndex.at(i);
        modTmp = modulators.at(i);
        valTmp.sfModValue = modTmp.modSrcOper;
        _sf2->set(id, champ_sfModSrcOper, valTmp);
        valTmp.sfGenValue = modTmp.modDestOper;
        _sf2->set(id, champ_sfModDestOper, valTmp);
        valTmp.shValue = modTmp.modAmount;
        _sf2->set(id, champ_modAmount, valTmp);
        valTmp.sfModValue = modTmp.modAmtSrcOper;
        _sf2->set(id, champ_sfModAmtSrcOper, valTmp);
        valTmp.sfTransValue = modTmp.modTransOper;
        _sf2->set(id, champ_sfModTransOper, valTmp);
    }
}

void PageTable::duplicateMod()
{
    // Duplication des mods sélectionnés dans tous les autres instruments ou presets
    if (this->_tree->getSelectedItemsNumber() != 1)
        return;
    EltID id = this->_tree->getFirstID();
    if (id.typeElement != elementInst && id.typeElement != elementPrst)
        return;

    DialogSelection * dial = new DialogSelection(_sf2, id, this);
    connect(dial, SIGNAL(listChosen(QList<int>)), this, SLOT(duplicateMod(QList<int>)));
    dial->show();
}

void PageTable::duplicateMod(QList<int> listIndex)
{
    if (this->_tree->getSelectedItemsNumber() != 1)
        return;
    EltID id = this->_tree->getFirstID();
    if (id.typeElement != elementInst && id.typeElement != elementPrst)
        return;

    EltID idMod = id;
    if (idMod.typeElement == elementInst)
        idMod.typeElement = elementInstMod;
    else
        idMod.typeElement = elementPrstMod;

    // Copie des mods
    QList<Modulator> modulators = getModList(idMod);

    _sf2->prepareNewActions();
    foreach (int numElement, listIndex)
    {
        id.indexElt = numElement;
        if (!_sf2->get(id, champ_hidden).bValue)
            pasteMod(id, modulators);
    }

    this->_mainWindow->updateDo();
}

void PageTable::remplirComboSource(ComboBox *comboBox)
{
    comboBox->addItem(getIndexName(0, 0));
    comboBox->addItem(getIndexName(2, 0));
    comboBox->addItem(getIndexName(3, 0));
    comboBox->addItem(getIndexName(10, 0));
    comboBox->addItem(getIndexName(13, 0));
    comboBox->addItem(getIndexName(14, 0));
    comboBox->addItem(getIndexName(16, 0));
    for (int i = 1; i < 6; i++) comboBox->addItem(getIndexName(i, 1));
    for (int i = 7; i < 32; i++) comboBox->addItem(getIndexName(i, 1));
    for (int i = 64; i < 98; i++) comboBox->addItem(getIndexName(i, 1));
    for (int i = 102; i < 120; i++) comboBox->addItem(getIndexName(i, 1));
    comboBox->setLimite(89);
}

quint16 PageTable::getSrcNumber(quint16 wVal, bool &CC)
{
    if (wVal < 7) CC = false;
    else CC = true;
    quint16 wRet = 0;
    switch (wVal)
    {
    case 0: wRet = 0; break;
    case 1: wRet = 2; break;
    case 2: wRet = 3; break;
    case 3: wRet = 10; break;
    case 4: wRet = 13; break;
    case 5: wRet = 14; break;
    case 6: wRet = 16; break;
    case 7: wRet = 1; break;
    case 8: wRet = 2; break;
    case 9: wRet = 3; break;
    case 10: wRet = 4; break;
    case 11: wRet = 5; break;
    case 12: wRet = 7; break;
    case 13: wRet = 8; break;
    case 14: wRet = 9; break;
    case 15: wRet = 10; break;
    case 16: wRet = 11; break;
    case 17: wRet = 12; break;
    case 18: wRet = 13; break;
    case 19: wRet = 14; break;
    case 20: wRet = 15; break;
    case 21: wRet = 16; break;
    case 22: wRet = 17; break;
    case 23: wRet = 18; break;
    case 24: wRet = 19; break;
    case 25: wRet = 20; break;
    case 26: wRet = 21; break;
    case 27: wRet = 22; break;
    case 28: wRet = 23; break;
    case 29: wRet = 24; break;
    case 30: wRet = 25; break;
    case 31: wRet = 26; break;
    case 32: wRet = 27; break;
    case 33: wRet = 28; break;
    case 34: wRet = 29; break;
    case 35: wRet = 30; break;
    case 36: wRet = 31; break;
    case 37: wRet = 64; break;
    case 38: wRet = 65; break;
    case 39: wRet = 66; break;
    case 40: wRet = 67; break;
    case 41: wRet = 68; break;
    case 42: wRet = 69; break;
    case 43: wRet = 70; break;
    case 44: wRet = 71; break;
    case 45: wRet = 72; break;
    case 46: wRet = 73; break;
    case 47: wRet = 74; break;
    case 48: wRet = 75; break;
    case 49: wRet = 76; break;
    case 50: wRet = 77; break;
    case 51: wRet = 78; break;
    case 52: wRet = 79; break;
    case 53: wRet = 80; break;
    case 54: wRet = 81; break;
    case 55: wRet = 82; break;
    case 56: wRet = 83; break;
    case 57: wRet = 84; break;
    case 58: wRet = 85; break;
    case 59: wRet = 86; break;
    case 60: wRet = 87; break;
    case 61: wRet = 88; break;
    case 62: wRet = 89; break;
    case 63: wRet = 90; break;
    case 64: wRet = 91; break;
    case 65: wRet = 92; break;
    case 66: wRet = 93; break;
    case 67: wRet = 94; break;
    case 68: wRet = 95; break;
    case 69: wRet = 96; break;
    case 70: wRet = 97; break;
    case 71: wRet = 102; break;
    case 72: wRet = 103; break;
    case 73: wRet = 104; break;
    case 74: wRet = 105; break;
    case 75: wRet = 106; break;
    case 76: wRet = 107; break;
    case 77: wRet = 108; break;
    case 78: wRet = 109; break;
    case 79: wRet = 110; break;
    case 80: wRet = 111; break;
    case 81: wRet = 112; break;
    case 82: wRet = 113; break;
    case 83: wRet = 114; break;
    case 84: wRet = 115; break;
    case 85: wRet = 116; break;
    case 86: wRet = 117; break;
    case 87: wRet = 118; break;
    case 88: wRet = 119; break;
    default: wRet = 0; CC = false; break;
    }
    return wRet;
}

quint16 PageTable::getSrcIndex(quint16 wVal, bool bVal)
{
    quint16 wRet = 0;
    if (bVal)
    {
        switch (wVal)
        {
        case 1: wRet = 7; break;
        case 2: wRet = 8; break;
        case 3: wRet = 9; break;
        case 4: wRet = 10; break;
        case 5: wRet = 11; break;
        case 7: wRet = 12; break;
        case 8: wRet = 13; break;
        case 9: wRet = 14; break;
        case 10: wRet = 15; break;
        case 11: wRet = 16; break;
        case 12: wRet = 17; break;
        case 13: wRet = 18; break;
        case 14: wRet = 19; break;
        case 15: wRet = 20; break;
        case 16: wRet = 21; break;
        case 17: wRet = 22; break;
        case 18: wRet = 23; break;
        case 19: wRet = 24; break;
        case 20: wRet = 25; break;
        case 21: wRet = 26; break;
        case 22: wRet = 27; break;
        case 23: wRet = 28; break;
        case 24: wRet = 29; break;
        case 25: wRet = 30; break;
        case 26: wRet = 31; break;
        case 27: wRet = 32; break;
        case 28: wRet = 33; break;
        case 29: wRet = 34; break;
        case 30: wRet = 35; break;
        case 31: wRet = 36; break;
        case 64: wRet = 37; break;
        case 65: wRet = 38; break;
        case 66: wRet = 39; break;
        case 67: wRet = 40; break;
        case 68: wRet = 41; break;
        case 69: wRet = 42; break;
        case 70: wRet = 43; break;
        case 71: wRet = 44; break;
        case 72: wRet = 45; break;
        case 73: wRet = 46; break;
        case 74: wRet = 47; break;
        case 75: wRet = 48; break;
        case 76: wRet = 49; break;
        case 77: wRet = 50; break;
        case 78: wRet = 51; break;
        case 79: wRet = 52; break;
        case 80: wRet = 53; break;
        case 81: wRet = 54; break;
        case 82: wRet = 55; break;
        case 83: wRet = 56; break;
        case 84: wRet = 57; break;
        case 85: wRet = 58; break;
        case 86: wRet = 59; break;
        case 87: wRet = 60; break;
        case 88: wRet = 61; break;
        case 89: wRet = 62; break;
        case 90: wRet = 63; break;
        case 91: wRet = 64; break;
        case 92: wRet = 65; break;
        case 93: wRet = 66; break;
        case 94: wRet = 67; break;
        case 95: wRet = 68; break;
        case 96: wRet = 69; break;
        case 97: wRet = 70; break;
        case 102: wRet = 71; break;
        case 103: wRet = 72; break;
        case 104: wRet = 73; break;
        case 105: wRet = 74; break;
        case 106: wRet = 75; break;
        case 107: wRet = 76; break;
        case 108: wRet = 77; break;
        case 109: wRet = 78; break;
        case 110: wRet = 79; break;
        case 111: wRet = 80; break;
        case 112: wRet = 81; break;
        case 113: wRet = 82; break;
        case 114: wRet = 83; break;
        case 115: wRet = 84; break;
        case 116: wRet = 85; break;
        case 117: wRet = 86; break;
        case 118: wRet = 87; break;
        case 119: wRet = 88; break;
        }
    }
    else
    {
        switch (wVal)
        {
        case 0: wRet = 0; break;
        case 2: wRet = 1; break;
        case 3: wRet = 2; break;
        case 10: wRet = 3; break;
        case 13: wRet = 4; break;
        case 14: wRet = 5; break;
        case 16: wRet = 6; break;
        case 127: wRet = 100; break; // lien
        }
    }
    return wRet;
}

int PageTable::limit(int iVal, Champ champ, EltID id)
{
    int ret = iVal;
    EltID id2 = id;
    EltID id3 = id;
    if (id.typeElement != elementInst && id.typeElement != elementInstSmpl) return 0;
    int limInf, limSup;
    switch ((int)champ)
    {
    case champ_startAddrsOffset:
        // Limite inférieure
        if (ret < 0) ret = 0;
        // Limite supérieure
        limSup = 0;
        if (id.typeElement == elementInstSmpl)
        {
            id2.typeElement = elementSmpl;
            id2.indexElt = _sf2->get(id, champ_sampleID).wValue;
            limSup = _sf2->get(id2, champ_dwLength).dwValue;
        }
        else
        {
            id3.typeElement = elementInstSmpl;
            id2.typeElement = elementSmpl;
            for (int i = 0; i < _sf2->count(id3); i++)
            {
                id3.indexElt2 = i;
                id2.indexElt = _sf2->get(id3, champ_sampleID).wValue;
                if (i == 0 || _sf2->get(id2, champ_dwLength).dwValue < (unsigned)limSup)
                    limSup = _sf2->get(id2, champ_dwLength).dwValue;
            }
        }
        if (ret > limSup) ret = limSup;
        break;
    case champ_endAddrsOffset:
        // Limite inférieure
        limInf = 0;
        if (id.typeElement == elementInstSmpl)
        {
            id2.typeElement = elementSmpl;
            id2.indexElt = _sf2->get(id, champ_sampleID).wValue;
            limInf = - (int)_sf2->get(id2, champ_dwLength).dwValue;
        }
        else
        {
            id3.typeElement = elementInstSmpl;
            id2.typeElement = elementSmpl;
            for (int i = 0; i < _sf2->count(id3); i++)
            {
                id3.indexElt2 = i;
                id2.indexElt = _sf2->get(id3, champ_sampleID).wValue;
                if (i == 0 || -(signed)_sf2->get(id2, champ_dwLength).dwValue > limInf)
                    limInf = - (int)_sf2->get(id2, champ_dwLength).dwValue;
            }
        }
        if (ret < limInf) ret = limInf;
        // Limite supérieure
        if (ret > 0) ret = 0;
        break;
    case champ_startloopAddrsOffset:
        // Limite inférieure
        limInf = 0;
        if (id.typeElement == elementInstSmpl)
        {
            id2.typeElement = elementSmpl;
            id2.indexElt = _sf2->get(id, champ_sampleID).wValue;
            limInf = - (int)_sf2->get(id2, champ_dwStartLoop).dwValue;
        }
        else
        {
            id3.typeElement = elementInstSmpl;
            id2.typeElement = elementSmpl;
            for (int i = 0; i < _sf2->count(id3); i++)
            {
                id3.indexElt2 = i;
                id2.indexElt = _sf2->get(id3, champ_sampleID).wValue;
                if (i == 0 || -(signed)_sf2->get(id2, champ_dwStartLoop).dwValue > limInf)
                    limInf = - (int)_sf2->get(id2, champ_dwStartLoop).dwValue;
            }
        }
        if (ret < limInf) ret = limInf;
        // Limite supérieure
        limSup = 0;
        if (id.typeElement == elementInstSmpl)
        {
            id2.typeElement = elementSmpl;
            id2.indexElt = _sf2->get(id, champ_sampleID).wValue;
            limSup = _sf2->get(id2, champ_dwLength).dwValue - _sf2->get(id2, champ_dwStartLoop).dwValue;
        }
        else
        {
            id3.typeElement = elementInstSmpl;
            id2.typeElement = elementSmpl;
            for (int i = 0; i < _sf2->count(id3); i++)
            {
                id3.indexElt2 = i;
                id2.indexElt = _sf2->get(id3, champ_sampleID).wValue;
                if (i == 0 || \
                        _sf2->get(id2, champ_dwLength).dwValue - \
                        _sf2->get(id2, champ_dwStartLoop).dwValue < (unsigned)limSup)
                    limSup = _sf2->get(id2, champ_dwLength).dwValue - \
                            _sf2->get(id2, champ_dwStartLoop).dwValue;
            }
        }
        if (ret > limSup) ret = limSup;
        break;
    case champ_endloopAddrsOffset:
        // Limite inférieure
        limInf = 0;
        if (id.typeElement == elementInstSmpl)
        {
            id2.typeElement = elementSmpl;
            id2.indexElt = _sf2->get(id, champ_sampleID).wValue;
            limInf = - (int)_sf2->get(id2, champ_dwEndLoop).dwValue;
        }
        else
        {
            id3.typeElement = elementInstSmpl;
            id2.typeElement = elementSmpl;
            for (int i = 0; i < _sf2->count(id3); i++)
            {
                id3.indexElt2 = i;
                id2.indexElt = _sf2->get(id3, champ_sampleID).wValue;
                if (i == 0 || -(signed)_sf2->get(id2, champ_dwEndLoop).dwValue > limInf)
                    limInf = - (int)_sf2->get(id2, champ_dwEndLoop).dwValue;
            }
        }
        if (ret < limInf) ret = limInf;
        // Limite supérieure
        limSup = 0;
        if (id.typeElement == elementInstSmpl)
        {
            id2.typeElement = elementSmpl;
            id2.indexElt = _sf2->get(id, champ_sampleID).wValue;
            limSup = _sf2->get(id2, champ_dwLength).dwValue - _sf2->get(id2, champ_dwEndLoop).dwValue;
        }
        else
        {
            id3.typeElement = elementInstSmpl;
            id2.typeElement = elementSmpl;
            for (int i = 0; i < _sf2->count(id3); i++)
            {
                id3.indexElt2 = i;
                id2.indexElt = _sf2->get(id3, champ_sampleID).wValue;
                if (i == 0 || \
                        _sf2->get(id2, champ_dwLength).dwValue - \
                        _sf2->get(id2, champ_dwEndLoop).dwValue < (unsigned)limSup)
                    limSup = _sf2->get(id2, champ_dwLength).dwValue - \
                            _sf2->get(id2, champ_dwEndLoop).dwValue;
            }
        }
        if (ret > limSup) ret = limSup;
        break;
    }
    return ret;
}

void PageTable::paramGlobal()
{
    bool error;
    QList<EltID> ids = this->getUniqueInstOrPrst(error, true, true);
    if (ids.isEmpty() || error)
        return;

    DialogParamGlobal * dialogParam = new DialogParamGlobal(_sf2, _typePage == PAGE_PRST, this);
    dialogParam->setAttribute(Qt::WA_DeleteOnClose, true);
    this->connect(dialogParam, SIGNAL(accepted(QVector<double>,int,int,int,int)),
                  SLOT(paramGlobal(QVector<double>,int,int,int,int)));
    dialogParam->show();
}

void PageTable::paramGlobal(QVector<double> dValues, int typeModif, int champ, int velMin, int velMax)
{
    _sf2->prepareNewActions();

    bool error;
    QList<EltID> ids = this->getUniqueInstOrPrst(error, true, true);
    if (error)
        return;

    if (velMin > velMax)
    {
        int tmp = velMin;
        velMin = velMax;
        velMax = tmp;
    }

    // Modification de tous les éléments
    foreach (EltID id, ids)
    {
        // Pos min et max sur le clavier
        if (id.typeElement == elementInst || id.typeElement == elementInstSmpl)
            id.typeElement = elementInstSmpl;
        else
            id.typeElement = elementPrstInst;

        // Modification des valeurs pour chaque élément associé à id
        double amount;
        int pos;
        char T[30];
        bool ok;
        Valeur value;
        for (int i = 0; i < _sf2->count(id); i++)
        {
            id.indexElt2 = i;
            if (!_sf2->get(id, champ_hidden).bValue)
            {
                int velMin2 = 0;
                int velMax2 = 127;
                if (_sf2->isSet(id, champ_velRange))
                {
                    velMin2 = _sf2->get(id, champ_velRange).rValue.byLo;
                    velMax2 = _sf2->get(id, champ_velRange).rValue.byHi;
                }
                if (velMin2 > velMax2)
                {
                    int tmp = velMin2;
                    velMin2 = velMax2;
                    velMax2 = tmp;
                }
                if (velMin2 >= velMin && velMax2 <= velMax)
                {
                    amount = QString(getTextValue(T, (Champ)champ, _sf2->get(id, (Champ)champ).genValue))
                            .replace(",", ".").toDouble();

                    // Calcul de la modification
                    pos = (double)(_sf2->get(id, champ_keyRange).rValue.byLo +
                                   _sf2->get(id, champ_keyRange).rValue.byHi) / 2 * dValues.size() / 127. + 0.5;
                    if (pos < 0)
                        pos = 0;
                    else if (pos >= dValues.size())
                        pos = dValues.size() - 1;

                    // Application de la modification
                    switch (typeModif)
                    {
                    case 0: // Ajout
                        amount += dValues.at(pos);
                        break;
                    case 1: // Multiplication
                        amount *= dValues.at(pos);
                        break;
                    case 2: // Remplacement
                        amount = dValues.at(pos);
                        break;
                    }
                    value.genValue = getValue(QString::number(amount), (Champ)champ, ok);
                    _sf2->set(id, (Champ)champ, value);
                }
            }
        }
    }

    // Actualisation
    this->_mainWindow->updateDo();
    this->_mainWindow->updateActions();
}

void PageTable::duplication()
{
    bool error;
    QList<EltID> ids = this->getUniqueInstOrPrst(error, true, false);
    if (ids.isEmpty() || error)
        return;

    DialogDuplication * dialogDuplication = new DialogDuplication(_typePage == PAGE_PRST, this);
    dialogDuplication->setAttribute(Qt::WA_DeleteOnClose, true);
    this->connect(dialogDuplication, SIGNAL(accepted(QVector<int>,bool,bool)),
                  SLOT(duplication(QVector<int>,bool,bool)));
    dialogDuplication->show();
}

void PageTable::duplication(QVector<int> listeVelocites, bool duplicKey, bool duplicVel)
{
    _sf2->prepareNewActions();

    bool error;
    QList<EltID> ids = this->getUniqueInstOrPrst(error, true, false);
    if (error)
        return;

    foreach (EltID id, ids)
    {
        if (_typePage == PAGE_INST)
            id.typeElement = elementInstSmpl;
        else
            id.typeElement = elementPrstInst;

        // Duplication pour chaque note
        if (duplicKey)
        {
            int nbElementsLies = _sf2->count(id);
            for (int i = 0; i < nbElementsLies; i++)
            {
                id.indexElt2 = i;
                if (!_sf2->get(id, champ_hidden).bValue)
                    this->duplication(id);
            }
        }

        // Duplication pour chaque velocityRange
        if (duplicVel)
        {
            int nbElementsLies = _sf2->count(id);
            for (int i = 0; i < nbElementsLies; i++)
            {
                id.indexElt2 = i;
                if (!_sf2->get(id, champ_hidden).bValue)
                    this->duplication(id, listeVelocites);
            }
        }
    }

    // Actualisation
    this->_mainWindow->updateDo();
    this->_mainWindow->updateActions();
}

void PageTable::duplication(EltID id)
{
    // Etendue du sample sur le clavier
    int numBas = 0;
    int numHaut = 127;
    if (_sf2->isSet(id, champ_keyRange))
    {
        numBas = _sf2->get(id, champ_keyRange).rValue.byLo;
        numHaut = _sf2->get(id, champ_keyRange).rValue.byHi;
        if (numBas > numHaut)
        {
            int temp = numBas;
            numBas = numHaut;
            numHaut = temp;
        }
    }
    if (numBas != numHaut)
    {
        // Modification keyrange
        Valeur val;
        val.rValue.byLo = numBas;
        val.rValue.byHi = numBas;
        _sf2->set(id, champ_keyRange, val);
        // Création de divisions identiques pour les autres notes
        EltID id2 = id;
        for (int i = numBas+1; i <= numHaut; i++)
        {
            if (_typePage == PAGE_INST)
            {
                id.typeElement = elementInstSmpl;
                id2.typeElement = elementInstSmpl;
                id2.indexElt2 = _sf2->add(id);
                // Recopiage des gens
                id.typeElement = elementInstSmplGen;
            }
            else
            {
                id.typeElement = elementPrstInst;
                id2.typeElement = elementPrstInst;
                id2.indexElt2 = _sf2->add(id);
                // Recopiage des gens
                id.typeElement = elementPrstInstGen;
            }
            for (int j = 0; j < _sf2->count(id); j++)
            {
                id.indexMod = j;
                _sf2->set(id2, (Champ)_sf2->get(id, champ_sfGenOper).wValue,
                               _sf2->get(id, champ_sfGenAmount));
            }
            // Modification keyrange
            val.rValue.byLo = i;
            val.rValue.byHi = i;
            _sf2->set(id2, champ_keyRange, val);
            // Recopiage des mods
            if (_typePage == PAGE_INST)
            {
                id.typeElement = elementInstSmplMod;
                id2.typeElement = elementInstSmplMod;
            }
            else
            {
                id.typeElement = elementPrstInstMod;
                id2.typeElement = elementPrstInstMod;
            }
            for (int j = 0; j < _sf2->count(id); j++)
            {
                id.indexMod = j;
                id2.indexMod = _sf2->add(id2);
                _sf2->set(id2, champ_modAmount, _sf2->get(id, champ_modAmount));
                _sf2->set(id2, champ_sfModSrcOper, _sf2->get(id, champ_sfModSrcOper));
                _sf2->set(id2, champ_sfModAmtSrcOper, _sf2->get(id, champ_sfModAmtSrcOper));
                _sf2->set(id2, champ_sfModDestOper, _sf2->get(id, champ_sfModDestOper));
                _sf2->set(id2, champ_sfModTransOper, _sf2->get(id, champ_sfModTransOper));
            }
        }
    }
}

void PageTable::duplication(EltID id, QVector<int> listeVelocite)
{
    QVector<bool> rangeOk;
    rangeOk.fill(false, listeVelocite.size() / 2);

    // Element lié
    int idLinked;
    if (_typePage == PAGE_INST)
        idLinked = _sf2->get(id, champ_sampleID).wValue;
    else
        idLinked = _sf2->get(id, champ_instrument).wValue;

    // Keyrange
    int noteMin = 0;
    int noteMax = 127;
    if (_sf2->isSet(id, champ_keyRange))
    {
        noteMin = _sf2->get(id, champ_keyRange).rValue.byLo;
        noteMax = _sf2->get(id, champ_keyRange).rValue.byHi;
        if (noteMin > noteMax)
        {
            int tmp = noteMin;
            noteMin = noteMax;
            noteMax = tmp;
        }
    }

    // On scanne tous les éléments liés pour savoir si des étendues sont déjà présentes
    bool selfOk = false;
    int indexSelf = id.indexElt2;
    int nbElementsLies = _sf2->count(id);
    for (int i = 0; i < nbElementsLies; i++)
    {
        id.indexElt2 = i;
        if (!_sf2->get(id, champ_hidden).bValue)
        {
            // Keyrange
            int noteMin2 = 0;
            int noteMax2 = 127;
            if (_sf2->isSet(id, champ_keyRange))
            {
                noteMin2 = _sf2->get(id, champ_keyRange).rValue.byLo;
                noteMax2 = _sf2->get(id, champ_keyRange).rValue.byHi;
                if (noteMin2 > noteMax2)
                {
                    int tmp = noteMin2;
                    noteMin2 = noteMax2;
                    noteMax2 = tmp;
                }
            }
            int idLinked2;
            if (_typePage == PAGE_INST)
                idLinked2 = _sf2->get(id, champ_sampleID).wValue;
            else
                idLinked2 = _sf2->get(id, champ_instrument).wValue;

            if (noteMin == noteMin2 && noteMax == noteMax2 && idLinked == idLinked2)
            {
                int velMin2 = 0;
                int velMax2 = 127;
                if (_sf2->isSet(id, champ_keyRange))
                {
                    velMin2 = _sf2->get(id, champ_velRange).rValue.byLo;
                    velMax2 = _sf2->get(id, champ_velRange).rValue.byHi;
                    if (velMin2 > velMax2)
                    {
                        int tmp = velMin2;
                        velMin2 = velMax2;
                        velMax2 = tmp;
                    }
                }

                for (int j = 0; j < listeVelocite.size() / 2; j++)
                {
                    int velMin = qMin(listeVelocite.at(2 * j), listeVelocite.at(2 * j + 1));
                    int velMax = qMax(listeVelocite.at(2 * j), listeVelocite.at(2 * j + 1));

                    if (velMin == velMin2 && velMax == velMax2)
                    {
                        rangeOk[j] = true;
                        if (i == indexSelf)
                            selfOk = true;
                    }
                }
            }
        }
    }

    if (rangeOk.indexOf(false) != -1)
    {
        id.indexElt2 = indexSelf;

        if (!selfOk)
        {
            // Modification de l'étendue de vélocité de l'élément de départ
            int pos = rangeOk.indexOf(false);
            rangeOk[pos] = true;
            Valeur val;
            val.rValue.byLo = qMin(listeVelocite.at(2 * pos), listeVelocite.at(2 * pos + 1));
            val.rValue.byHi = qMax(listeVelocite.at(2 * pos), listeVelocite.at(2 * pos + 1));
            _sf2->set(id, champ_velRange, val);
        }

        while (rangeOk.indexOf(false) != -1)
        {
            // Duplication
            if (_typePage == PAGE_INST)
                id.typeElement = elementInstSmpl;
            else
                id.typeElement = elementPrstInst;
            EltID id2 = id;
            id2.indexElt2 = _sf2->add(id);

            // Recopiage des gens
            if (_typePage == PAGE_INST)
                id.typeElement = elementInstSmplGen;
            else
                id.typeElement = elementPrstInstGen;
            for (int j = 0; j < _sf2->count(id); j++)
            {
                id.indexMod = j;
                _sf2->set(id2, (Champ)_sf2->get(id, champ_sfGenOper).wValue,
                               _sf2->get(id, champ_sfGenAmount));
            }

            // Modification étendue de vélocité
            int pos = rangeOk.indexOf(false);
            rangeOk[pos] = true;
            Valeur val;
            val.rValue.byLo = qMin(listeVelocite.at(2 * pos), listeVelocite.at(2 * pos + 1));
            val.rValue.byHi = qMax(listeVelocite.at(2 * pos), listeVelocite.at(2 * pos + 1));
            _sf2->set(id2, champ_velRange, val);

            // Recopiage des mods
            if (_typePage == PAGE_INST)
            {
                id.typeElement = elementInstSmplMod;
                id2.typeElement = elementInstSmplMod;
            }
            else
            {
                id.typeElement = elementPrstInstMod;
                id2.typeElement = elementPrstInstMod;
            }
            for (int j = 0; j < _sf2->count(id); j++)
            {
                id.indexMod = j;
                id2.indexMod = _sf2->add(id2);
                _sf2->set(id2, champ_modAmount, _sf2->get(id, champ_modAmount));
                _sf2->set(id2, champ_sfModSrcOper, _sf2->get(id, champ_sfModSrcOper));
                _sf2->set(id2, champ_sfModAmtSrcOper, _sf2->get(id, champ_sfModAmtSrcOper));
                _sf2->set(id2, champ_sfModDestOper, _sf2->get(id, champ_sfModDestOper));
                _sf2->set(id2, champ_sfModTransOper, _sf2->get(id, champ_sfModTransOper));
            }
        }
    }
}

void PageTable::spatialisation()
{
    bool error;
    QList<EltID> ids = this->getUniqueInstOrPrst(error, true, true);
    if (ids.isEmpty() || error)
        return;

    // Détermination note min, note max
    int noteMin = 127;
    int noteMax = 0;
    foreach (EltID id, ids)
    {
        if (_typePage == PAGE_INST)
            id.typeElement = elementInstSmpl;
        else
            id.typeElement = elementPrstInst;

        for (int i = 0; i < _sf2->count(id); i++)
        {
            id.indexElt2 = i;
            if (!_sf2->get(id, champ_hidden).bValue)
            {
                if (_sf2->isSet(id, champ_keyRange))
                {
                    genAmountType amount = _sf2->get(id, champ_keyRange).genValue;
                    if (amount.ranges.byLo < noteMin) noteMin = amount.ranges.byLo;
                    if (amount.ranges.byHi > noteMax) noteMax = amount.ranges.byHi;
                }
            }
        }
    }

    DialogSpace * dialogSpace = new DialogSpace(_typePage == PAGE_PRST, noteMin, noteMax, this);
    dialogSpace->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialogSpace, SIGNAL(accepted(QMap<int,double>)), this, SLOT(spatialisation(QMap<int,double>)));
    dialogSpace->show();
}

void PageTable::spatialisation(QMap<int, double> mapPan)
{
    _sf2->prepareNewActions();

    bool error;
    QList<EltID> ids = this->getUniqueInstOrPrst(error, true, true);
    if (error)
        return;

    foreach (EltID id, ids)
    {
        if (_typePage == PAGE_PRST)
            id.typeElement = elementPrstInst;
        else
            id.typeElement = elementInstSmpl;
        if (_sf2->count(id, false) > 0)
            spatialisation(mapPan, id);
    }

    // Actualisation
    this->_mainWindow->updateDo();
    this->_mainWindow->updateActions();
}

void PageTable::spatialisation(QMap<int, double> mapPan, EltID id)
{
    // Sauvegarde des valeurs
    bool isPrst = (_typePage == PAGE_PRST);

    // Liste des éléments liés avec leur lien (stéréo) le cas échéant
    QList<EltID> list1;
    QList<genAmountType> listRange;
    QList<EltID> list2;
    genAmountType amount;
    bool found;
    int pos;
    int noteMin = 128;
    int noteMax = 0;
    if (isPrst)
        id.typeElement = elementPrstInst;
    else
        id.typeElement = elementInstSmpl;
    for (int i = 0; i < _sf2->count(id); i++)
    {
        id.indexElt2 = i;
        if (!_sf2->get(id, champ_hidden).bValue)
        {
            if (_sf2->isSet(id, champ_keyRange))
                amount = _sf2->get(id, champ_keyRange).genValue;
            else
            {
                amount.ranges.byLo = 0;
                amount.ranges.byHi = 127;
            }
            if (amount.ranges.byLo < noteMin) noteMin = amount.ranges.byLo;
            if (amount.ranges.byHi > noteMax) noteMax = amount.ranges.byHi;

            // Recherche d'une note liée ayant la même étendue sur le clavier
            found = false;
            if (!isPrst)
            {
                pos = 0;
                while (pos < list1.size() && !found)
                {
                    if (amount.ranges.byHi == listRange.at(pos).ranges.byHi &&
                            amount.ranges.byLo == listRange.at(pos).ranges.byLo &&
                            list2.at(pos).indexElt2 == -1)
                    {
                        // Les samples sont-ils liés ?
                        EltID idSmpl1 = id;
                        idSmpl1.indexElt = _sf2->get(id, champ_sampleID).wValue;
                        idSmpl1.typeElement = elementSmpl;
                        EltID idSmpl2 = list1.at(pos);
                        idSmpl2.indexElt = _sf2->get(idSmpl2, champ_sampleID).wValue;
                        idSmpl2.typeElement = elementSmpl;
                        if (idSmpl1.indexElt == _sf2->get(idSmpl2, champ_wSampleLink).wValue)
                        {
                            SFSampleLink type1 = _sf2->get(idSmpl1, champ_sfSampleType).sfLinkValue;
                            SFSampleLink type2 = _sf2->get(idSmpl2, champ_sfSampleType).sfLinkValue;
                            if (((type1 == rightSample || type1 == RomRightSample) && (type2 == leftSample || type2 == RomLeftSample)) ||
                                    ((type1 == leftSample || type1 == RomLeftSample) && (type2 == rightSample || type2 == RomRightSample)))
                                found = true;
                        }
                    }
                    if (!found)
                        pos++;
                }
            }
            if (found)
            {
                // Lien
                list2[pos] = id;
            }
            else
            {
                // Ajout à liste 1
                list1.append(id);
                // Element nul dans liste 2
                id.indexElt2 = -1;
                list2.append(id);
                // Etendue
                listRange.append(amount);
            }
        }
    }

    // Spatialisation
    double pan = 0;
    int note = 64;
    EltID id2, id3;
    int sampleG;
    Valeur val;
    for (int i = 0; i < list1.size(); i++)
    {
        note = (listRange.at(i).ranges.byLo + listRange.at(i).ranges.byHi) / 2;
        pan = mapPan.value(note);

        // Lien ?
        if (list2.at(i).indexElt2 == -1)
        {
            // pas de lien
            val.genValue.shAmount = 1000 * pan - 500;
            _sf2->set(list1.at(i), champ_pan, val);
        }
        else
        {
            // Quel sample est à gauche ?
            sampleG = 0;
            // Sample correspondant 1
            id2 = list1.at(i);
            id2.indexElt = _sf2->get(id2, champ_sampleID).wValue;
            id2.typeElement = elementSmpl;
            SFSampleLink type1 = _sf2->get(id2, champ_sfSampleType).sfLinkValue;
            // Sample correspondant 2
            id3 = list2.at(i);
            id3.indexElt = _sf2->get(id3, champ_sampleID).wValue;
            id3.typeElement = elementSmpl;
            SFSampleLink type2 = _sf2->get(id3, champ_sfSampleType).sfLinkValue;
            if ((type1 == leftSample || type1 == RomLeftSample) &&
                    type2 != leftSample && type2 != RomLeftSample)
            {
                sampleG = 0;
            }
            else if ((type1 == rightSample || type1 == RomRightSample) &&
                     type2 != rightSample && type2 != RomRightSample)
            {
                sampleG = 1;
            }
            else
            {
                if (_sf2->get(list1.at(i), champ_pan).shValue <
                        _sf2->get(list2.at(i), champ_pan).shValue)
                    sampleG = 0;
                else if (_sf2->get(list1.at(i), champ_pan).shValue >
                         _sf2->get(list2.at(i), champ_pan).shValue)
                    sampleG = 1;
            }
            if (sampleG == 0)
            {
                // Inversion
                id2 = list1.at(i);
                list1[i] = list2.at(i);
                list2[i] = id2;
            }
            // lien
            if (pan < 0.5)
            {
                // Gauche
                val.genValue.shAmount = -500;
                _sf2->set(list1.at(i), champ_pan, val);
                // Droite
                val.genValue.shAmount = 2000 * pan - 500;
                _sf2->set(list2.at(i), champ_pan, val);
            }
            else
            {
                // Gauche
                val.genValue.shAmount = 2000 * pan - 1500;
                _sf2->set(list1.at(i), champ_pan, val);
                // Droite
                val.genValue.shAmount = 500;
                _sf2->set(list2.at(i), champ_pan, val);
            }
        }
    }
}

QList<EltID> PageTable::getUniqueInstOrPrst(bool &error, bool errorIfNoDivision, bool errorIfKeyRangeMissing)
{
    QList<EltID> ids = this->_tree->getAllIDs();
    QList<EltID> idsRet;

    bool withDivision = true;
    bool allDivWithKeyrange = true;

    foreach (EltID id, ids)
    {
        if (id.typeElement == elementPrstInst)
            id.typeElement = elementPrst;
        else if (id.typeElement == elementInstSmpl)
            id.typeElement = elementInst;

        bool found = false;
        foreach (EltID idTmp, idsRet)
        {
            if (idTmp == id)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            idsRet << id;

            // Test if divisions inside
            EltID idTmp = id;
            if (idTmp.typeElement == elementPrst)
                idTmp.typeElement = elementPrstInst;
            else if (idTmp.typeElement == elementInst)
                idTmp.typeElement = elementInstSmpl;
            withDivision &= (_sf2->count(idTmp, false) > 0);

            // Test if all keyranges are present
            for (int i = 0; i < _sf2->count(idTmp); i++)
            {
                idTmp.indexElt2 = i;
                if (!_sf2->get(idTmp, champ_hidden).bValue)
                    allDivWithKeyrange &= _sf2->isSet(idTmp, champ_keyRange);
            }
        }
    }

    if (errorIfNoDivision && !withDivision)
    {
        if (_typePage == PAGE_INST)
            QMessageBox::warning(this, trUtf8("Attention"),
                                 trUtf8("Un instrument ne contenant aucun sample n'est pas compatible avec cet outil."));
        else
            QMessageBox::warning(this, trUtf8("Attention"),
                                 trUtf8("Un preset ne contenant aucun instrument n'est pas compatible avec cet outil."));
        error = true;
    }
    else if (errorIfKeyRangeMissing && !allDivWithKeyrange)
    {
        QMessageBox::warning(this, trUtf8("Attention"), trUtf8("L'étendue de notes de toutes les divisions doit être spécifiée."));
        error = true;
    }
    else
        error = false;

    return idsRet;
}

void PageTable::visualize()
{
    EltID id = this->_tree->getFirstID();
    if (_typePage == PAGE_INST)
        id.typeElement = elementInstSmpl;
    else
        id.typeElement = elementPrstInst;

    // Vérification que l'élément possède au moins un élément lié, avec un keyrange valide
    int nbElt = 0;
    int posMin = 128;
    int posMax = 0;
    for (int i = 0; i < _sf2->count(id); i++)
    {
        id.indexElt2 = i;
        if (!_sf2->get(id, champ_hidden).bValue)
        {
            nbElt++;
            if (_sf2->isSet(id, champ_keyRange))
            {
                if (_sf2->get(id, champ_keyRange).rValue.byLo < posMin)
                    posMin = _sf2->get(id, champ_keyRange).rValue.byLo;
                if (_sf2->get(id, champ_keyRange).rValue.byHi > posMax)
                    posMax = _sf2->get(id, champ_keyRange).rValue.byHi;
            }
        }
    }
    if (nbElt == 0)
    {
        if (_typePage == PAGE_INST)
            QMessageBox::warning(this, trUtf8("Attention"), trUtf8("L'instrument doit contenir des sons."));
        else
            QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Le preset doit contenir des instruments."));
        return;
    }
    if (posMin > posMax)
    {
        if (_typePage == PAGE_INST)
            QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Aucune étendue de notes spécifiée pour l'instrument."));
        else
            QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Aucune étendue de notes spécifiée pour le preset."));
        return;
    }
    DialogVisualizer * dialogVisu = new DialogVisualizer(_sf2, id, this);
    dialogVisu->setAttribute(Qt::WA_DeleteOnClose, true);
    dialogVisu->show();
}

int PageTable::getDestNumber(int i)
{
    if (_typePage == PAGE_PRST)
    {
        switch (i)
        {
        case 0:  return 52; // Pitch/Effects
        case 1:  return 51;
        case 2:  return 56;
        case 3:  return 8;
        case 4:  return 9;
        case 5:  return 17;
        case 6:  return 15;
        case 7:  return 16;
        case 8:  return 48; // Volume envelope
        case 9:  return 33;
        case 10: return 34;
        case 11: return 35;
        case 12: return 36;
        case 13: return 37;
        case 14: return 38;
        case 15: return 39;
        case 16: return 40;
        case 17: return 25; // Modulation envelope
        case 18: return 26;
        case 19: return 27;
        case 20: return 28;
        case 21: return 29;
        case 22: return 30;
        case 23: return 7;
        case 24: return 11;
        case 25: return 31;
        case 26: return 32;
        case 27: return 21; // Modulation LFO
        case 28: return 22;
        case 29: return 5;
        case 30: return 13;
        case 31: return 10;
        case 32: return 23; // Vibrato LFO
        case 33: return 24;
        case 34: return 6;
        default: return 100;
        }
    }
    else
    {
        switch (i)
        {
        case 0:  return 0; // Sample
        case 1:  return 4;
        case 2:  return 1;
        case 3:  return 12;
        case 4:  return 2;
        case 5:  return 45;
        case 6:  return 3;
        case 7:  return 50;
        case 8:  return 58; // Pitch/Effects
        case 9:  return 52;
        case 10: return 51;
        case 11: return 56;
        case 12: return 8;
        case 13: return 9;
        case 14: return 17;
        case 15: return 15;
        case 16: return 16;
        case 17: return 46;
        case 18: return 57;
        case 19: return 48; // Volume envelope
        case 20: return 33;
        case 21: return 34;
        case 22: return 35;
        case 23: return 36;
        case 24: return 37;
        case 25: return 38;
        case 26: return 39;
        case 27: return 40;
        case 28: return 47;
        case 29: return 54;
        case 30: return 25; // Modulation envelope
        case 31: return 26;
        case 32: return 27;
        case 33: return 28;
        case 34: return 29;
        case 35: return 30;
        case 36: return 7;
        case 37: return 11;
        case 38: return 31;
        case 39: return 32;
        case 40: return 21; // Modulation LFO
        case 41: return 22;
        case 42: return 5;
        case 43: return 13;
        case 44: return 10;
        case 45: return 23; // Vibrato LFO
        case 46: return 24;
        case 47: return 6;
        default: return 100;
        }
    }
}

int PageTable::getDestIndex(int i)
{
    if (_typePage == PAGE_PRST)
    {
        switch (i)
        {
        case 52: return 0; // Pitch/Effects
        case 51: return 1;
        case 56: return 2;
        case 8:  return 3;
        case 9:  return 4;
        case 17: return 5;
        case 15: return 6;
        case 16: return 7;
        case 48: return 8; // Volume envelope
        case 33: return 9;
        case 34: return 10;
        case 35: return 11;
        case 36: return 12;
        case 37: return 13;
        case 38: return 14;
        case 39: return 15;
        case 40: return 16;
        case 25: return 17; // Modulation envelope
        case 26: return 18;
        case 27: return 19;
        case 28: return 20;
        case 29: return 21;
        case 30: return 22;
        case 7:  return 23;
        case 11: return 24;
        case 31: return 25;
        case 32: return 26;
        case 21: return 27; // Modulation LFO
        case 22: return 28;
        case 5:  return 29;
        case 13: return 30;
        case 10: return 31;
        case 23: return 32; // Vibrato LFO
        case 24: return 33;
        case 6:  return 34;
        default: return 100;
        }
    }
    else
    {
        switch (i)
        {
        case 0:  return 0; // Sample
        case 4:  return 1;
        case 1:  return 2;
        case 12: return 3;
        case 2:  return 4;
        case 45: return 5;
        case 3:  return 6;
        case 50: return 7;
        case 58: return 8;
        case 52: return 9; // Pitch/Effects
        case 51: return 10;
        case 56: return 11;
        case 8:  return 12;
        case 9:  return 13;
        case 17: return 14;
        case 15: return 15;
        case 16: return 16;
        case 46: return 17;
        case 57: return 18;
        case 48: return 19; // Volume envelope
        case 33: return 20;
        case 34: return 21;
        case 35: return 22;
        case 36: return 23;
        case 37: return 24;
        case 38: return 25;
        case 39: return 26;
        case 40: return 27;
        case 47: return 28;
        case 54: return 29;
        case 25: return 30; // Modulation envelope
        case 26: return 31;
        case 27: return 32;
        case 28: return 33;
        case 29: return 34;
        case 30: return 35;
        case 7:  return 36;
        case 11: return 37;
        case 31: return 38;
        case 32: return 39;
        case 21: return 40; // Modulation LFO
        case 22: return 41;
        case 5:  return 42;
        case 13: return 43;
        case 10: return 44;
        case 23: return 45; // Vibrato LFO
        case 24: return 46;
        case 6:  return 47;
        default: return 100;
        }
    }
}

void PageTable::keyPlayed(int key, int velocity)
{
    bool isEnlighted = (velocity > 0);

    // Mise à jour éléments enclenchés
    if (isEnlighted && _listKeyEnlighted.indexOf(key) == -1)
        _listKeyEnlighted.append(key);
    else
        _listKeyEnlighted.removeAll(key);

    // Mise à jour de la table
    for (int i = 1; i < this->table->columnCount(); i++)
    {
        EltID id = this->table->getID(i);
        if (_sf2->isValide(id))
        {
            bool enlighted = false;
            if (!_sf2->get(id, champ_hidden).bValue)
            {
                int key1 = _sf2->get(id, champ_keyRange).rValue.byLo;
                int key2 = _sf2->get(id, champ_keyRange).rValue.byHi;
                if (!_sf2->isSet(id, champ_keyRange))
                {
                    key1 = 0;
                    key2 = 128;
                }
                for (int j = 0; j < _listKeyEnlighted.size(); j++)
                    enlighted = enlighted || (qMin(key1, key2) <= _listKeyEnlighted.at(j)
                                              && qMax(key1, key2) >= _listKeyEnlighted.at(j));
            }
            this->table->setEnlighted(i, enlighted);
        }
    }

    // Visualization on the range editor
    _rangeEditor->playKey(key, velocity);
}
