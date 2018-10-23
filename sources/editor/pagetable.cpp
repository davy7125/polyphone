/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2017 Davy Triponney                                **
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
**  Website/Contact: http://polyphone-soundfonts.com                      **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "pagetable.h"
#include "editor_old.h"
#include "contextmanager.h"
#include "dialog_visualizer.h"
#include "dialog_space.h"
#include "dialog_duplication.h"
#include "dialogselection.h"
#include "graphicsviewrange.h"
#include "envelopeditor.h"
#include "utils.h"
#include <QScrollBar>
#include <QMenu>

QList<PageTable::Modulator> PageTable::_modulatorCopy;

PageTable::PageTable(TypePage typePage, QWidget *parent) : Page(parent, typePage, typePage == PAGE_INST ? "page:inst" : "page:prst"),
    _table(NULL),
    tableMod(NULL)
{
}

void PageTable::afficheTable(bool justSelection)
{
    int posV = _table->verticalScrollBar()->value();

    if (!justSelection)
    {
        // Clear the table and repopulate it
        _table->blockSignals(true);
        _table->clear();
        _table->blockSignals(false);

        if (!_currentParentIds.isEmpty())
        {
            // Global division(s)
            addGlobal(_currentParentIds);

            // Child divisions
            if (_currentParentIds.count() == 1)
                addDivisions(_currentParentIds.first());

            // Table style
            formatTable(_currentParentIds.count() > 1);
        }
    }

    // Mods
    if (_currentIds.count() == 1)
        afficheMod(_currentIds[0]);
    else
    {
        this->afficheEditMod();
        this->displayModInTable();
    }

    // Fin de la préparation
    this->reselect();
    _table->verticalScrollBar()->setValue(posV);
}

void PageTable::addGlobal(IdList listIds)
{
    bool multiGlobal = listIds.count() > 1;
    int nbGlobal = 0;

    foreach (EltID id, listIds)
    {
        AttributeValue genValTmp;
        int offsetStart = 0;
        int offsetEnd = 0;
        int offsetStartLoop = 0;
        int offsetEndLoop = 0;
        int row;

        QString qStr;
        if (multiGlobal)
        {
            qStr = _sf2->getQstr(id, champ_name).left(10);
            qStr.append("\n");
            qStr.append(_sf2->getQstr(id, champ_name).mid(10).left(10));
        }
        else
            qStr = trUtf8("Global");

        // Column number
        int numCol = 0;
        for (int i = 0; i < nbGlobal; i++)
        {
            if (Utils::naturalOrder(qStr, _table->horizontalHeaderItem(i)->text()) > 0)
                numCol++;
            else
                break;
        }

        _table->addColumn(numCol, qStr);
        nbGlobal++;
        EltID idGen = id;
        idGen.typeElement = this->contenantGen;
        idGen.indexElt2 = 0;
        foreach (int i, _sf2->getSiblings(idGen))
        {
            genValTmp = _sf2->get(id, (AttributeType)i);
            switch (i)
            {
            case champ_startAddrsOffset:
                offsetStart += genValTmp.shValue;
                break;
            case champ_startAddrsCoarseOffset:
                offsetStart += 32768 * genValTmp.shValue;
                break;
            case champ_endAddrsOffset:
                offsetEnd += genValTmp.shValue;
                break;
            case champ_endAddrsCoarseOffset:
                offsetEnd += 32768 * genValTmp.shValue;
                break;
            case champ_startloopAddrsOffset:
                offsetStartLoop += genValTmp.shValue;
                break;
            case champ_startloopAddrsCoarseOffset:
                offsetStartLoop += 32768 * genValTmp.shValue;
                break;
            case champ_endloopAddrsOffset:
                offsetEndLoop += genValTmp.shValue;
                break;
            case champ_endloopAddrsCoarseOffset:
                offsetEndLoop += 32768 * genValTmp.shValue;
                break;
            default:
                row = _table->getRow(i);
                if (row > -1)
                {
                    if (i == champ_sampleModes)
                        _table->setLoopModeImage(row, numCol, genValTmp.wValue);
                    else
                        _table->item(row, numCol)->setText(Attribute::toString((AttributeType)i, _typePage == PAGE_PRST, genValTmp));
                }
            }
        }
        if (offsetStart && _table->getRow(champ_startAddrsOffset) > -1)
        {
            row = _table->getRow(champ_startAddrsOffset);
            genValTmp.shValue = offsetStart;
            _table->item(row, numCol)->setText(Attribute::toString(champ_startAddrsOffset, _typePage == PAGE_PRST, genValTmp));
        }
        if (offsetEnd && _table->getRow(champ_endAddrsOffset) > -1)
        {
            row = _table->getRow(champ_endAddrsOffset);
            genValTmp.shValue = offsetEnd;
            _table->item(row, numCol)->setText(Attribute::toString(champ_endAddrsOffset, _typePage == PAGE_PRST, genValTmp));
        }
        if (offsetStartLoop && _table->getRow(champ_startloopAddrsOffset) > -1)
        {
            row = _table->getRow(champ_startloopAddrsOffset);
            genValTmp.shValue = offsetStartLoop;
            _table->item(row, numCol)->setText(Attribute::toString(champ_startloopAddrsOffset, _typePage == PAGE_PRST, genValTmp));
        }
        if (offsetEndLoop && _table->getRow(champ_endloopAddrsOffset) > -1)
        {
            row = _table->getRow(champ_endloopAddrsOffset);
            genValTmp.shValue = offsetEndLoop;
            _table->item(row, numCol)->setText(Attribute::toString(champ_endloopAddrsOffset, _typePage == PAGE_PRST, genValTmp));
        }
        _table->setID(id, numCol);
    }
}

void PageTable::addDivisions(EltID id)
{
    AttributeValue genValTmp;
    int row;
    EltID id2 = id;
    EltID id3 = id;
    int nbSmplInst = 0;
    id.typeElement = this->lien;
    id2.typeElement = this->lienGen;
    id3.typeElement = this->contenu;

    foreach (int i, _sf2->getSiblings(id))
    {
        id.indexElt2 = i;
        id2.indexElt2 = i;

        // Ajout d'un élément lié
        int numCol = 1;

        // Détermination de la colonne
        AttributeType cElementLie;
        if (this->contenant == elementInst)
            cElementLie = champ_sampleID;
        else
            cElementLie = champ_instrument;

        id3.indexElt = _sf2->get(id, cElementLie).wValue;

        QString strOrder = QString("%1-%2-%3")
                .arg(_sf2->get(id, champ_keyRange).rValue.byLo, 3, 10, QChar('0'))
                .arg(_sf2->get(id, champ_velRange).rValue.byLo, 3, 10, QChar('0'))
                .arg(_sf2->getQstr(id3, champ_name));
        QString qStr = _sf2->getQstr(id3, champ_name).left(10) + "\n" + _sf2->getQstr(id3, champ_name).mid(10).left(10);
        for (int j = 1; j < nbSmplInst + 1; j++)
        {
            // note et vélocité basses de la colonne et prise en compte du nom de l'élément lié
            id3.indexElt = _sf2->get(_table->getID(j), cElementLie).wValue;
            QString strOrder2 = QString("%1-%2-%3")
                    .arg(_sf2->get(_table->getID(j), champ_keyRange).rValue.byLo, 3, 10, QChar('0'))
                    .arg(_sf2->get(_table->getID(j), champ_velRange).rValue.byLo, 3, 10, QChar('0'))
                    .arg(_sf2->getQstr(id3, champ_name));
            if (Utils::naturalOrder(strOrder, strOrder2) > 0)
                numCol++;
        }

        nbSmplInst++;
        int offsetStart = 0;
        int offsetEnd = 0;
        int offsetStartLoop = 0;
        int offsetEndLoop = 0;
        _table->addColumn(numCol, qStr);
        foreach (int champTmp, _sf2->getSiblings(id2))
        {
            genValTmp = _sf2->get(id, (AttributeType)champTmp);
            switch (champTmp)
            {
            case champ_startAddrsOffset:
                offsetStart += genValTmp.shValue;
                break;
            case champ_startAddrsCoarseOffset:
                offsetStart += 32768 * genValTmp.shValue;
                break;
            case champ_endAddrsOffset:
                offsetEnd += genValTmp.shValue;
                break;
            case champ_endAddrsCoarseOffset:
                offsetEnd += 32768 * genValTmp.shValue;
                break;
            case champ_startloopAddrsOffset:
                offsetStartLoop += genValTmp.shValue;
                break;
            case champ_startloopAddrsCoarseOffset:
                offsetStartLoop += 32768 * genValTmp.shValue;
                break;
            case champ_endloopAddrsOffset:
                offsetEndLoop += genValTmp.shValue;
                break;
            case champ_endloopAddrsCoarseOffset:
                offsetEndLoop += 32768 * genValTmp.shValue;
                break;
            default:
                row = _table->getRow(champTmp);
                if (row > -1)
                {
                    if (champTmp == champ_sampleModes)
                        _table->setLoopModeImage(row, numCol, genValTmp.wValue);
                    else
                        _table->item(row, numCol)->setText(Attribute::toString((AttributeType)champTmp, _typePage == PAGE_PRST, genValTmp));
                }
            }
        }

        if (offsetStart && _table->getRow(champ_startAddrsOffset) > -1)
        {
            row = _table->getRow(champ_startAddrsOffset);
            genValTmp.shValue = offsetStart;
            _table->item(row, numCol)->setText(Attribute::toString(champ_startAddrsOffset, _typePage == PAGE_PRST, genValTmp));
        }
        if (offsetEnd && _table->getRow(champ_endAddrsOffset) > -1)
        {
            row = _table->getRow(champ_endAddrsOffset);
            genValTmp.shValue = offsetEnd;
            _table->item(row, numCol)->setText(Attribute::toString(champ_endAddrsOffset, _typePage == PAGE_PRST, genValTmp));
        }
        if (offsetStartLoop && _table->getRow(champ_startloopAddrsOffset) > -1)
        {
            row = _table->getRow(champ_startloopAddrsOffset);
            genValTmp.shValue = offsetStartLoop;
            _table->item(row, numCol)->setText(Attribute::toString(champ_startloopAddrsOffset, _typePage == PAGE_PRST, genValTmp));
        }
        if (offsetEndLoop && _table->getRow(champ_endloopAddrsOffset) > -1)
        {
            row = _table->getRow(champ_endloopAddrsOffset);
            genValTmp.shValue = offsetEndLoop;
            _table->item(row, numCol)->setText(Attribute::toString(champ_endloopAddrsOffset, _typePage == PAGE_PRST, genValTmp));
        }
        _table->setID(id2, numCol);
    }
}

void PageTable::formatTable(bool multiGlobal)
{
    QColor color = this->palette().color(QPalette::Base);
    QColor alternateColor = this->palette().color(QPalette::AlternateBase);
    QBrush brush1(TableWidget::getPixMap(color, alternateColor));
    QBrush brush2(TableWidget::getPixMap(alternateColor, color));
    if (this->contenant == elementInst)
    {
        // First column with a hatching pattern
        if (!multiGlobal)
        {
            for (int j = 0; j < _table->rowCount(); j++)
            {
                if (j < 6)
                    _table->item(j, 0)->setBackground(brush1);
                else if (j < 10)
                    _table->item(j, 0)->setBackground(brush2);
                else if (j < 12)
                    _table->item(j, 0)->setBackground(brush1);
                else if (j < 20)
                    _table->item(j, 0)->setBackground(brush2);
                else if (j < 30)
                    _table->item(j, 0)->setBackground(brush1);
                else if (j < 38)
                    _table->item(j, 0)->setBackground(brush2);
                else if (j < 43)
                    _table->item(j, 0)->setBackground(brush1);
                else
                    _table->item(j, 0)->setBackground(brush2);
                _table->item(j, 0)->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
            }
        }

        // Yellow rows
        for (int i = multiGlobal ? 0 : 1; i < _table->columnCount(); i++)
        {
            for (int j = 0; j < _table->rowCount(); j++)
            {
                if (j < 6) {}
                else if (j < 10)
                    _table->item(j, i)->setBackgroundColor(alternateColor);
                else if (j < 12) {}
                else if (j < 20)
                    _table->item(j, i)->setBackgroundColor(alternateColor);
                else if (j < 30) {}
                else if (j < 38)
                    _table->item(j, i)->setBackgroundColor(alternateColor);
                else if (j < 43) {}
                else
                    _table->item(j, i)->setBackgroundColor(alternateColor);
                _table->item(j, i)->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
            }
        }
    }
    else
    {
        // First column with a hatching pattern
        if (!multiGlobal)
        {
            for (int j = 0; j < _table->rowCount(); j++)
            {
                if (j < 5)
                    _table->item(j, 0)->setBackground(brush1);
                else if (j < 8)
                    _table->item(j, 0)->setBackground(brush2);
                else if (j < 10)
                    _table->item(j, 0)->setBackground(brush1);
                else if (j < 18)
                    _table->item(j, 0)->setBackground(brush2);
                else if (j < 28)
                    _table->item(j, 0)->setBackground(brush1);
                else if (j < 36)
                    _table->item(j, 0)->setBackground(brush2);
                else
                    _table->item(j, 0)->setBackground(brush1);
                _table->item(j, 0)->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
            }
        }

        // Yellow rows
        for (int i = multiGlobal ? 0 : 1; i < _table->columnCount(); i++)
        {
            for (int j = 0; j < _table->rowCount(); j++)
            {
                if (j < 5) {}
                else if (j < 8)
                    _table->item(j, i)->setBackgroundColor(alternateColor);
                else if (j < 10) {}
                else if (j < 18)
                    _table->item(j, i)->setBackgroundColor(alternateColor);
                else if (j < 28) {}
                else if (j < 36)
                    _table->item(j, i)->setBackgroundColor(alternateColor);
                _table->item(j, i)->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
            }
        }
    }
    _table->hideRow(0);
}

void PageTable::afficheRanges(bool justSelection)
{
    _rangeEditor->display(_currentIds[0], justSelection);
}

void PageTable::afficheEnvelops(bool justSelection)
{
    if (_envelopEditor != NULL)
        _envelopEditor->display(_currentIds, justSelection);
}

void PageTable::afficheMod(EltID id, AttributeType selectedField)
{
    // Try to find a corresponding index
    EltID idMod = id;
    if (id.typeElement == this->contenant)
        idMod.typeElement = this->contenantMod;
    else
        idMod.typeElement = this->lienMod;
    int index = -1;
    if (_table->selectedItems().count() == 1)
    {
        foreach (int i, _sf2->getSiblings(idMod))
        {
            idMod.indexMod = i;
            AttributeType champ = _sf2->get(idMod, champ_sfModDestOper).sfGenValue;
            if (champ == champ_startAddrsCoarseOffset)
                champ = champ_startAddrsOffset;
            else if (champ == champ_endAddrsCoarseOffset)
                champ = champ_endAddrsOffset;
            else if (champ == champ_startloopAddrsCoarseOffset)
                champ = champ_startloopAddrsOffset;
            else if (champ == champ_endloopAddrsCoarseOffset)
                champ = champ_endloopAddrsOffset;
            if (champ == selectedField)
                index = i;
        }
    }
    afficheMod(id, index);
}

void PageTable::afficheMod(EltID id, int selectedIndex)
{
    // Affichage du menu ou non
    if (id.typeElement == elementPrst || id.typeElement == elementInst)
    {
        _pushCopyMod->setMenu(_menu);
        _pushCopyMod->setStyleSheet("QPushButton { text-align: left; padding-left: 4px }");
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
    AttributeValue genValTmp;
    SFModulator sfModTmp;
    if (id.typeElement == this->contenant)
        id.typeElement = this->contenantMod;
    else
        id.typeElement = this->lienMod;
    this->tableMod->clear();
    int iVal;
    int numLigne = 0;

    // Colors of the icons
    QPixmap icon;
    int iconWidth = 48;
    QMap<QString, QString> replacement;
    replacement["currentColor"] = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT).name();
    replacement["secondColor"] = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name();

    foreach (int i, _sf2->getSiblings(id))
    {
        id.indexMod = i;

        // Ajout d'un modulateur
        this->tableMod->addRow(numLigne, id);
        genValTmp.wValue = id.indexMod;
        this->tableMod->item(numLigne, 5)->setText(Attribute::toString(champ_indexMod, _typePage == PAGE_PRST, genValTmp));

        sfModTmp = _sf2->get(id, champ_sfModSrcOper).sfModValue;
        iVal = 4 * sfModTmp.D + 8 * sfModTmp.P + sfModTmp.Type + 1;
        QString iconName = QString(":/icons/courbe%1.svg").arg(iVal, 2, 10, QChar('0'));
        icon = ContextManager::theme()->getColoredSvg(iconName, QSize(iconWidth, iconWidth), replacement);
        this->tableMod->item(numLigne, 6)->setIcon(icon);
        if (sfModTmp.Index == 127 && sfModTmp.CC == 0)
        {
            // On cherche le lien
            iVal = getAssociatedMod(id);
            if (iVal > -1)
                qStr = trUtf8("Modulateur") + ": #" + QString::number(iVal + 1);
            else
                qStr = trUtf8("Lien (invalide)");
        }
        else
            qStr = getIndexName(sfModTmp.Index, sfModTmp.CC);

        this->tableMod->item(numLigne, 6)->setText(qStr);
        sfModTmp = _sf2->get(id, champ_sfModAmtSrcOper).sfModValue;
        iVal = 4*sfModTmp.D + 8*sfModTmp.P + sfModTmp.Type + 1;
        iconName = QString(":/icons/courbe%1.svg").arg(iVal, 2, 10, QChar('0'));
        icon = ContextManager::theme()->getColoredSvg(iconName, QSize(iconWidth, iconWidth), replacement);
        this->tableMod->item(numLigne, 7)->setIcon(icon);
        qStr = getIndexName(sfModTmp.Index, sfModTmp.CC);
        genValTmp = _sf2->get(id, champ_modAmount);
        qStr.append(QString::fromUtf8(" × ")).append(Attribute::toString(champ_modAmount, _typePage == PAGE_PRST, genValTmp));
        this->tableMod->item(numLigne, 7)->setText(qStr);
        this->spinAmount->setValue(genValTmp.shValue);
        genValTmp = _sf2->get(id, champ_sfModDestOper);
        if (genValTmp.wValue > 99)
        {
            // lien vers modulateur
            qStr = trUtf8("Modulateur") + ": #" + QString::number(genValTmp.wValue - 32767);
        }
        else
            qStr = getGenName(genValTmp.wValue);

        genValTmp = _sf2->get(id, champ_sfModTransOper);
        qStr.append(Attribute::toString(champ_sfModTransOper, _typePage == PAGE_PRST, genValTmp));
        this->tableMod->item(numLigne, 8)->setText(qStr);
        numLigne++;
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
    this->displayModInTable();
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
    if (_currentIds.count() != 1)
    {
        singleDivision = false;
        this->tableMod->clear();
    }
    this->pushNouveauMod->setEnabled(singleDivision);
    _pushCopyMod->setEnabled(singleDivision);

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

        // Selection in the table
        EltID idParent = id;
        if (idParent.typeElement == this->contenantMod)
            idParent.typeElement = this->contenant;
        else if (idParent.typeElement == this->lienMod)
            idParent.typeElement = this->lien;
        _table->selectCell(idParent, (AttributeType)_sf2->get(id, champ_sfModDestOper).wValue);
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

void PageTable::resetChamp(int colonne, AttributeType champ1, AttributeType champ2)
{
    EltID id = _table->getID(colonne);
    bool ok = _sf2->isSet(id, champ1);
    if (champ2 != champ_unknown)
        ok = ok || _sf2->isSet(id, champ2);

    if (ok)
    {
        // On efface la donnée
        id = _table->getID(colonne);
        _sf2->reset(id, champ1);
        if (champ2 != champ_unknown)
            _sf2->reset(id, champ2);
    }
}

void PageTable::setOffset(int ligne, int colonne, AttributeType champ1, AttributeType champ2)
{
    EltID id = _table->getID(colonne);
    bool ok;
    QString texte = _table->item(ligne, colonne)->text().left(9);
    AttributeValue genAmount = Attribute::fromString(champ1, _typePage == PAGE_PRST, texte, ok);
    if (ok)
    {
        // Enregistrement de la nouvelle valeur
        AttributeValue genAmount2 = Attribute::fromString(champ2, _typePage == PAGE_PRST, texte, ok);
        int iVal = limit(32768 * genAmount2.shValue + genAmount.shValue, champ1, id);
        genAmount2.shValue = iVal / 32768;
        genAmount.shValue = iVal % 32768;
        if (genAmount.shValue != _sf2->get(id, champ1).shValue ||
                genAmount2.shValue != _sf2->get(id, champ2).shValue)
        {
            // Modification du sf2
            id = _table->getID(colonne);
            _sf2->set(id, champ1, genAmount);
            _sf2->set(id, champ2, genAmount2);
        }
        // Mise à jour de la valeur dans la cellule
        genAmount.shValue = _sf2->get(id, champ1).shValue + 32768 * _sf2->get(id, champ2).shValue;
        _table->item(ligne, colonne)->setText(Attribute::toString(champ1, _typePage == PAGE_PRST, genAmount));
    }
    else
    {
        // Restauration valeur précédente
        if (_sf2->isSet(id, champ1) || _sf2->isSet(id, champ2))
        {
            genAmount.shValue = _sf2->get(id, champ1).shValue + 32768 * _sf2->get(id, champ2).shValue;
            _table->item(ligne, colonne)->setText(Attribute::toString(champ1, _typePage == PAGE_PRST, genAmount));
        }
        else
            _table->item(ligne, colonne)->setText("");
    }
}

void PageTable::actionBegin()
{
    if (_preparingPage)
        return;
}

void PageTable::actionFinished()
{
    if (_preparingPage)
        return;
    _sf2->endEditing(getEditingSource());
}

void PageTable::set(int ligne, int colonne, bool allowPropagation)
{
    if (_preparingPage)
        return;

    // Modification d'un élément du tableau
    AttributeType champ = _table->getChamp(ligne);
    if (champ == champ_unknown)
        return;

    EltID id = _table->getID(colonne);
    if (allowPropagation && id.typeElement == elementInstSmpl && champ != champ_pan &&
            ContextManager::configuration()->getValue(ConfManager::SECTION_NONE, "stereo_modification", false).toBool())
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
            for (int i = 1; i < _table->columnCount(); i++)
            {
                idTmp = _table->getID(i);
                if (i != colonne)
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
                _table->blockSignals(true);

                if (champ == champ_sampleModes)
                {
                    _table->item(ligne, numCol2)->setData(Qt::DecorationRole, _table->item(ligne, colonne)->data(Qt::DecorationRole));
                    _table->item(ligne, numCol2)->setData(Qt::UserRole, _table->item(ligne, colonne)->data(Qt::UserRole));
                }
                else
                    _table->item(ligne, numCol2)->setText(_table->item(ligne, colonne)->text());
                _table->blockSignals(false);
                set(ligne, numCol2, false);
            }
        }
    }

    if (champ == champ_sampleModes)
    {
        // Effacement du champ ?
        if (_table->item(ligne, colonne)->data(Qt::UserRole).isNull())
            resetChamp(colonne, champ, champ_unknown);
        else
        {
            EltID id = _table->getID(colonne);
            AttributeValue genAmount;
            genAmount.wValue = _table->item(ligne, colonne)->data(Qt::UserRole).toInt();

            // Modification champ
            if (genAmount.wValue != _sf2->get(id, champ).wValue || !_sf2->isSet(id, champ))
            {
                // Modification du sf2
                _sf2->set(id, champ, genAmount);
            }
        }
    }
    else
    {
        if (_table->item(ligne, colonne)->text().isEmpty())
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
            _preparingPage = true;
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
                QString texte = _table->item(ligne, colonne)->text().left(9);
                bool ok;
                EltID id = _table->getID(colonne);
                AttributeValue genAmount = Attribute::fromString(champ, _typePage == PAGE_PRST, texte, ok);
                if (ok)
                {
                    // Modification champ
                    if (genAmount.wValue != _sf2->get(id, champ).wValue || !_sf2->isSet(id, champ))
                    {
                        // Modification du sf2
                        _sf2->set(id, champ, genAmount);
                    }
                    // Mise à jour de la valeur dans la cellule
                    _table->item(ligne, colonne)->setText(Attribute::toString(champ, _typePage == PAGE_PRST, genAmount));
                }
                else
                {
                    // Restauration valeur précédente
                    if (_sf2->isSet(id, champ))
                        _table->item(ligne, colonne)->setText(Attribute::toString(champ, _typePage == PAGE_PRST, _sf2->get(id, champ)));
                    else _table->item(ligne, colonne)->setText("");
                }
            }
            }
            _preparingPage = false;
        }
    }

    // Mise à jour partie mod (car entre 2 des mods peuvent être définitivement détruits, et les index peuvent être mis à jour)
    id = _table->getID(colonne);
    this->afficheMod(id);

    if (champ == champ_overridingRootKey || champ == champ_keyRange)
        customizeKeyboard();
}

void PageTable::setAmount()
{
    if (_preparingPage)
        return;
    _preparingPage = true;

    EltID id = this->tableMod->getID();
    EltID id2 = id;
    if (id2.typeElement == this->contenantMod)
        id.typeElement = this->contenant;
    else
        id.typeElement = this->lien;
    AttributeValue val;
    val.shValue = this->spinAmount->value();
    if (id2.typeElement != elementUnknown)
    {
        // Comparaison avec valeur précédente
        if (_sf2->get(id2, champ_modAmount).shValue != val.shValue)
        {
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
            _sf2->endEditing(getEditingSource());
        }
    }
    _preparingPage = false;
}

void PageTable::setAbs()
{
    if (_preparingPage) return;
    _preparingPage = 1;
    EltID id = this->tableMod->getID();
    EltID id2 = id;
    if (id2.typeElement == this->contenantMod)
        id.typeElement = this->contenant;
    else
        id.typeElement = this->lien;
    AttributeValue val;
    if (this->checkAbs->isChecked())
        val.wValue = 2;
    else
        val.wValue = 0;
    if (id2.typeElement != elementUnknown)
    {
        // Comparaison avec valeur précédente
        if (_sf2->get(id2, champ_sfModTransOper).wValue != val.wValue)
        {
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
            _sf2->endEditing(getEditingSource());
        }
    }
    _preparingPage = 0;
}

void PageTable::setSourceType(int row, int column)
{
    if (_preparingPage) return;
    _preparingPage = 1;
    EltID id = this->tableMod->getID();
    EltID id2 = id;
    if (id2.typeElement == this->contenantMod)
        id.typeElement = this->contenant;
    else
        id.typeElement = this->lien;
    AttributeValue val;
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
            _sf2->endEditing(getEditingSource());
        }
    }
    _preparingPage = 0;
}

void PageTable::setSourceAmountType(int row, int column)
{
    if (_preparingPage) return;
    _preparingPage = 1;
    EltID id = this->tableMod->getID();
    EltID id2 = id;
    if (id2.typeElement == this->contenantMod)
        id.typeElement = this->contenant;
    else
        id.typeElement = this->lien;
    AttributeValue val;
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
            _sf2->endEditing(getEditingSource());
        }
    }
    _preparingPage = 0;
}

void PageTable::setDest(int index)
{
    if (_preparingPage)
        return;

    _preparingPage = true;
    EltID id = this->tableMod->getID();
    EltID id2 = id;
    if (id2.typeElement == this->contenantMod)
        id.typeElement = this->contenant;
    else
        id.typeElement = this->lien;
    if (index < 32768)
        index = getDestNumber(index);
    AttributeValue val;
    if (id2.typeElement != elementUnknown)
    {
        // Comparaison avec valeur précédente
        val.sfGenValue = _sf2->get(id2, champ_sfModDestOper).sfGenValue;
        if (val.sfGenValue != (AttributeType)index)
        {
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
                if (_sf2->getSiblings(id2).contains(id3.indexMod))
                {
                    AttributeValue val2;
                    val2.sfModValue = _sf2->get(id3, champ_sfModSrcOper).sfModValue;
                    val2.sfModValue.CC = 0;
                    val2.sfModValue.Index = 0;
                    _sf2->set(id3, champ_sfModSrcOper, val2);
                }
            }

            // Lien ?
            if (index >= 32768 && _sf2->getSiblings(id2).contains(index - 32768))
            {
                EltID id3 = id2;
                id3.indexMod = index - 32768;
                // Destruction des précédents liens vers le nouvel élément
                AttributeValue val2;
                EltID id4 = id2;
                do
                {
                    id4.indexMod = this->getAssociatedMod(id3);
                    if (id4.indexMod != -1)
                    {
                        val2.sfGenValue = (AttributeType)0;
                        _sf2->set(id4, champ_sfModDestOper, val2);
                    }
                } while (id4.indexMod != -1);
                val2.sfModValue = _sf2->get(id3, champ_sfModSrcOper).sfModValue;
                val2.sfModValue.Index = 127;
                val2.sfModValue.CC = 0;
                _sf2->set(id3, champ_sfModSrcOper, val2);
            }
            val.sfGenValue = (AttributeType)index;
            _sf2->set(id2, champ_sfModDestOper, val);
            if (this->tableMod->selectedItems().count())
            {
                int currentRow = this->tableMod->selectedItems().takeFirst()->row();
                this->afficheMod(id);
                this->tableMod->selectRow(currentRow);
            }
            else
                this->afficheMod(id);
            _sf2->endEditing(getEditingSource());
        }
    }
    _preparingPage = 0;
}

void PageTable::setSource(int index)
{
    if (_preparingPage) return;
    _preparingPage = 1;
    EltID id = this->tableMod->getID();
    EltID id2 = id;
    if (id2.typeElement == this->contenantMod)
        id.typeElement = this->contenant;
    else
        id.typeElement = this->lien;
    bool CC;
    if (index < 32768) index = getSrcNumber(index, CC);
    else CC = 0;
    AttributeValue val;
    if (id2.typeElement != elementUnknown)
    {
        // Comparaison avec valeur précédente
        val.sfModValue = _sf2->get(id2, champ_sfModSrcOper).sfModValue;
        if (index < 32768)
        {
            if (val.sfModValue.Index != index || val.sfModValue.CC != CC)
            {
                // modification, pas de lien
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
                    AttributeValue val2;
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
                _sf2->endEditing(getEditingSource());
            }
        }
        else
        {
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
                AttributeValue val2;
                val2.wValue = _sf2->get(id3, champ_sfModDestOper).wValue;
                if (val2.wValue >= 32768)
                {
                    // On enlève le lien précédent
                    EltID id4 = id2;
                    id4.indexMod = val2.wValue-32768;
                    if (_sf2->getSiblings(id).contains(id4.indexMod))
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
                _sf2->endEditing(getEditingSource());
            }
            else
            {
                // La valeur précédente était un lien
                EltID id3 = id2;
                id3.indexMod = this->getAssociatedMod(id2);
                if (id3.indexMod != index - 32768)
                {
                    // On enlève les liens précédents
                    AttributeValue val2;
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
                        if (_sf2->getSiblings(id2).contains(id4.indexMod))
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
                    _sf2->endEditing(getEditingSource());
                }
            }
        }
    }
    _preparingPage = 0;
}

void PageTable::setSource2(int index)
{
    if (_preparingPage) return;
    _preparingPage = 1;
    EltID id = this->tableMod->getID();
    EltID id2 = id;
    if (id2.typeElement == this->contenantMod)
        id.typeElement = this->contenant;
    else
        id.typeElement = this->lien;
    bool CC;
    index = getSrcNumber(index, CC);
    AttributeValue val;
    if (id2.typeElement != elementUnknown)
    {
        // Comparaison avec valeur précédente
        val.sfModValue = _sf2->get(id2, champ_sfModAmtSrcOper).sfModValue;
        if (val.sfModValue.Index != index || val.sfModValue.CC != CC)
        {
            // modification
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
            _sf2->endEditing(getEditingSource());
        }
    }
    _preparingPage = 0;
}

void PageTable::reselect()
{
    _table->clearSelection();
    _table->setSelectionMode(QAbstractItemView::MultiSelection);
    QList<EltID> listID = _currentIds;
    _table->setRowHidden(0, false); // Selection will be on a visible row
    foreach (EltID id, listID)
        this->select(id);
    _table->setSelectionMode(QAbstractItemView::ExtendedSelection);
    customizeKeyboard();
    _table->setRowHidden(0, true); // It's now hidden again
}

void PageTable::select(EltID id)
{
    _preparingPage = true;

    EltID id2;
    int max;
    for (int i = 0; i < _table->columnCount(); i++)
    {
        id2 = _table->getID(i);
        if (id.typeElement == id2.typeElement && id.indexSf2 == id2.indexSf2 && id.indexElt == id2.indexElt &&
                (id.indexElt2 == id2.indexElt2 || id.typeElement == elementInst || id.typeElement == elementPrst))
        {
            _table->blockSignals(true);
            _table->item(0, i)->setSelected(true);
            _table->blockSignals(false);
            max = _table->horizontalScrollBar()->maximum();
            if (max / _table->columnCount() > 62)
                _table->horizontalScrollBar()->setValue((max*(i-1)) / _table->columnCount());
            else
                _table->scrollToItem(_table->item(7, i), QAbstractItemView::PositionAtCenter);
        }
    }
    _preparingPage = false;
}

void PageTable::selected()
{
    if (_preparingPage)
        return;
    _preparingPage = true;

    // Selected items
    IdList ids;
    QList<QTableWidgetItem*> listItems = _table->selectedItems();
    for (int i = 0; i < listItems.count(); i++)
        ids << _table->getID(listItems.at(i)->column());

    // Mise à jour des informations sur les mods
    int colonne = listItems.last()->column();
    if (listItems.count() == 1)
        this->afficheMod(_table->getID(colonne), _table->getChamp(listItems.last()->row()));
    else
        this->afficheMod(_table->getID(colonne));

    // Update the selection outside the table
    emit(selectedIdsChanged(ids));
    _preparingPage = false;

    customizeKeyboard();
}

void PageTable::customizeKeyboard()
{
    ContextManager::midi()->keyboard()->clearCustomization();

    QList<EltID> ids = _currentIds;
    if (ids.isEmpty())
        return;

    // If the global division is in the list, exclude the rest
    foreach (EltID id, ids)
    {
        if (id.typeElement == elementInst || id.typeElement == elementPrst)
        {
            ids.clear();
            ids << id;
            break;
        }
    }

    // Affichage des étendues et rootkeys des divisions sélectionnées
    foreach (EltID id, ids)
    {
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
                    rootKey = _sf2->get(idSmpl, champ_byOriginalPitch).bValue;
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
            ContextManager::midi()->keyboard()->addRangeAndRootKey(rootKey, keyRange.byLo, keyRange.byHi);
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
            foreach (int i, _sf2->getSiblings(id))
            {
                id.indexElt2 = i;
                rangesType keyRange;
                if (_sf2->isSet(id, champ_keyRange))
                    keyRange = _sf2->get(id, champ_keyRange).rValue;
                else
                    keyRange = defaultKeyRange;
                ContextManager::midi()->keyboard()->addRangeAndRootKey(-1, keyRange.byLo, keyRange.byHi);
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
    QList<int> nbMod = _sf2->getSiblings(id);
    foreach (int i, nbMod)
    {
        id2.indexMod = i;
        if (id2.indexMod != id.indexMod)
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
                    if (nbMod.contains(wTmp - 32768))
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
                    else
                        stop = true; // Lien non valide
                }
                else
                    stop = true; // Pas de lien
                compte++;

                if (compte > nbMod.count())
                    stop = true; // Boucle infinie
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
    QList<int> nbMod = _sf2->getSiblings(id);
    foreach (int i, nbMod)
    {
        id2.indexMod = i;
        if (id2.indexMod != id.indexMod)
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
                    foreach (int j, nbMod)
                    {
                        id4.indexMod = j;
                        wTmp = _sf2->get(id4, champ_sfModDestOper).wValue;
                        if (wTmp == 32768 + id3.indexMod)
                            indModSender = j;
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
                if (compte > nbMod.count())
                    stop = true; // Boucle infinie
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
    foreach (int j, _sf2->getSiblings(id))
    {
        id2.indexMod = j;
        if (id2.indexMod != id.indexMod)
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

    // Suppression mod
    QList<EltID> listIDs = this->tableMod->getSelectedIDs();
    if (listIDs.isEmpty())
        return;

    foreach (EltID id, listIDs)
    {
        // Liens vers le mod ?
        int iVal = -1;
        AttributeValue val;
        EltID id2 = id;
        do
        {
            iVal = getAssociatedMod(id);
            if (iVal != -1)
            {
                // Suppression du lien
                val.sfGenValue = (AttributeType)0;
                id2.indexMod = iVal;
                _sf2->set(id2, champ_sfModDestOper, val);
            }
        } while (iVal != -1);

        // Le mod est-il lié ?
        if (_sf2->get(id, champ_sfModDestOper).wValue >= 32768)
        {
            id2.indexMod = _sf2->get(id, champ_sfModDestOper).wValue - 32768;
            if (_sf2->getSiblings(id).contains(id2.indexMod))
            {
                val.sfModValue = _sf2->get(id2, champ_sfModSrcOper).sfModValue;
                val.sfModValue.CC = 0;
                val.sfModValue.Index = 0;
                _sf2->set(id2, champ_sfModSrcOper, val);
            }
        }
        _sf2->remove(id);
    }

    _sf2->endEditing(getEditingSource());

    if (rowToSelect >= nbRow - listIDs.size())
        rowToSelect = nbRow - listIDs.size() - 1;
    this->afficheMod(_currentIds[0]);
    this->tableMod->selectRow(rowToSelect);
}

void PageTable::nouveauMod()
{
    // Création nouveau mod
    EltID id = _currentIds[0];
    if (id.typeElement == elementInst) id.typeElement = elementInstMod;
    else if (id.typeElement == elementPrst) id.typeElement = elementPrstMod;
    else if (id.typeElement == elementInstSmpl) id.typeElement = elementInstSmplMod;
    else if (id.typeElement == elementPrstInst) id.typeElement = elementPrstInstMod;
    else return;
    id.indexMod = _sf2->add(id);

    // initialisation
    AttributeValue val;
    val.sfGenValue = (AttributeType)0;
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
        val.sfGenValue = (AttributeType)52;
    _sf2->set(id, champ_sfModDestOper, val);
    this->afficheMod(_currentIds[0], id.indexMod);
    _sf2->endEditing(getEditingSource());
}

void PageTable::copyMod()
{
    // Sauvegarde des mods
    if (_currentIds.count() != 1)
        return;
    EltID id = _currentIds[0];
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
        foreach (int i, _sf2->getSiblings(id))
        {
            id.indexMod = i;
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
    else
    {
        foreach (EltID id, listSelectedMods)
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
                mod.modDestOper = (AttributeType)(32768 + listIndex.indexOf(link));
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
    QList<EltID> ids = _currentIds;
    foreach (EltID id, ids)
        pasteMod(id, _modulatorCopy);

    if (ids.count() == 1)
        this->afficheMod(ids[0]);
    else
        this->afficheEditMod();

    _sf2->endEditing(getEditingSource());
}

void PageTable::pasteMod(EltID id, QList<Modulator> modulators)
{
    if (modulators.empty())
        return;
    if (id.typeElement == elementInst)
        id.typeElement = elementInstMod;
    else if (id.typeElement == elementPrst)
        id.typeElement = elementPrstMod;
    else if (id.typeElement == elementInstSmpl)
        id.typeElement = elementInstSmplMod;
    else if (id.typeElement == elementPrstInst)
        id.typeElement = elementPrstInstMod;
    else
        return;

    if (id.typeElement == elementPrstMod || id.typeElement == elementPrstInstMod)
    {
        // Vérification que toutes les destinations sont possibles
        AttributeType champTmp;
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

    // Création de nouveaux mods
    QList<int> listIndex;
    for (int i = 0; i < modulators.size(); i++)
        listIndex.append(_sf2->add(id));

    // Index minimal des mods à copier
    int offsetIndex = listIndex[0];
    for (int i = 0; i < modulators.size(); i++)
    {
        Modulator mod = modulators.at(i);
        mod.index += offsetIndex;
        if ((int)mod.modDestOper >= 32768)
            mod.modDestOper = (AttributeType)(mod.modDestOper + offsetIndex);
        modulators[i] = mod;
    }

    // Copie des configurations des mods sauvegardés
    AttributeValue valTmp;
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
    if (_currentIds.count() != 1)
        return;
    EltID id = _currentIds[0];
    if (id.typeElement != elementInst && id.typeElement != elementPrst)
        return;

    DialogSelection * dial = new DialogSelection(_sf2, id, this);
    connect(dial, SIGNAL(listChosen(QList<int>)), this, SLOT(duplicateMod(QList<int>)));
    dial->show();
}

void PageTable::duplicateMod(QList<int> listIndex)
{
    if (_currentIds.count() != 1)
        return;
    EltID id = _currentIds[0];
    if (id.typeElement != elementInst && id.typeElement != elementPrst)
        return;

    EltID idMod = id;
    if (idMod.typeElement == elementInst)
        idMod.typeElement = elementInstMod;
    else
        idMod.typeElement = elementPrstMod;

    // Copie des mods
    QList<Modulator> modulators = getModList(idMod);

    foreach (int numElement, listIndex)
    {
        id.indexElt = numElement;
        pasteMod(id, modulators);
    }

    _sf2->endEditing(getEditingSource());
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

int PageTable::limit(int iVal, AttributeType champ, EltID id)
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
            foreach (int i, _sf2->getSiblings(id3))
            {
                id3.indexElt2 = i;
                id2.indexElt = _sf2->get(id3, champ_sampleID).wValue;
                if (i == 0 || _sf2->get(id2, champ_dwLength).dwValue < (unsigned)limSup)
                    limSup = _sf2->get(id2, champ_dwLength).dwValue;
            }
        }
        if (ret > limSup)
            ret = limSup;
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
            foreach (int i, _sf2->getSiblings(id3))
            {
                id3.indexElt2 = i;
                id2.indexElt = _sf2->get(id3, champ_sampleID).wValue;
                if (i == 0 || -(signed)_sf2->get(id2, champ_dwLength).dwValue > limInf)
                    limInf = - (int)_sf2->get(id2, champ_dwLength).dwValue;
            }
        }
        if (ret < limInf)
            ret = limInf;

        // Limite supérieure
        if (ret > 0)
            ret = 0;
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
            foreach (int i, _sf2->getSiblings(id3))
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
            foreach (int i, _sf2->getSiblings(id3))
            {
                id3.indexElt2 = i;
                id2.indexElt = _sf2->get(id3, champ_sampleID).wValue;
                if (i == 0 || \
                        _sf2->get(id2, champ_dwLength).dwValue -
                        _sf2->get(id2, champ_dwStartLoop).dwValue < (unsigned)limSup)
                    limSup = _sf2->get(id2, champ_dwLength).dwValue -
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
            foreach (int i, _sf2->getSiblings(id3))
            {
                id3.indexElt2 = i;
                id2.indexElt = _sf2->get(id3, champ_sampleID).wValue;
                if (i == 0 || -(signed)_sf2->get(id2, champ_dwEndLoop).dwValue > limInf)
                    limInf = - (int)_sf2->get(id2, champ_dwEndLoop).dwValue;
            }
        }
        if (ret < limInf)
            ret = limInf;

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
            foreach (int i, _sf2->getSiblings(id3))
            {
                id3.indexElt2 = i;
                id2.indexElt = _sf2->get(id3, champ_sampleID).wValue;
                if (i == 0 ||
                        _sf2->get(id2, champ_dwLength).dwValue -
                        _sf2->get(id2, champ_dwEndLoop).dwValue < (unsigned)limSup)
                    limSup = _sf2->get(id2, champ_dwLength).dwValue -
                            _sf2->get(id2, champ_dwEndLoop).dwValue;
            }
        }
        if (ret > limSup) ret = limSup;
        break;
    }
    return ret;
}

QList<EltID> PageTable::getEltIds(bool &error, bool allWithDivisions, bool allDivWithRange)
{
    error = false;
    QList<EltID> result = _currentIds.getSelectedIds(_typePage == PAGE_INST ? elementInst : elementPrst);

    if (allWithDivisions && !_currentIds.areAllWithDivisions(_typePage == PAGE_INST ? elementInst : elementPrst))
    {
        error = true;
        QMessageBox::warning(this, trUtf8("Attention"), _typePage == PAGE_INST ?
                                 trUtf8("Un instrument ne contenant aucun sample n'est pas compatible avec cet outil") :
                                 trUtf8("Un preset ne contenant aucun instrument n'est pas compatible avec cet outil"));
        return result;
    }

    if (allDivWithRange && !_currentIds.areAllWithRange(_typePage == PAGE_INST ? elementInst : elementPrst))
    {
        error = true;
        QMessageBox::warning(this, trUtf8("Attention"), trUtf8("L'étendue de notes de toutes les divisions doit être spécifiée"));
        return result;
    }

    return result;
}

void PageTable::duplication()
{
//    bool error;
//    QList<EltID> ids = this->getEltIds(error, true, false);
//    if (ids.isEmpty() || error)
//        return;

//    DialogDuplication * dialogDuplication = new DialogDuplication(_typePage == PAGE_PRST, this);
//    dialogDuplication->setAttribute(Qt::WA_DeleteOnClose, true);
//    this->connect(dialogDuplication, SIGNAL(accepted(QVector<QPair<int, int> >,bool,bool)),
//                  SLOT(duplication(QVector<QPair<int, int> >,bool,bool)));
//    dialogDuplication->show();
}

void PageTable::duplication(QVector<QPair<int, int> > listeVelocites, bool duplicKey, bool duplicVel)
{
//    if (!duplicKey && !duplicVel)
//        return;

//    bool error;
//    QList<EltID> ids = this->getEltIds(error, true, false);
//    if (error)
//        return;

//    foreach (EltID id, ids)
//    {
//        if (_typePage == PAGE_INST)
//            id.typeElement = elementInstSmpl;
//        else
//            id.typeElement = elementPrstInst;

//        DuplicationTool tool(_sf2, id);

//        // Duplication pour chaque note
//        if (duplicKey)
//            tool.duplicateByKey();

//        // Duplication pour chaque velocityRange
//        if (duplicVel)
//            tool.duplicateByVelocity(listeVelocites);
//    }

//    // Actualisation
//    _sf2->endEditing(getEditingSource());
}

void PageTable::spatialisation()
{
    bool error;
    QList<EltID> ids = this->getEltIds(error, true, true);
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

        foreach (int i, _sf2->getSiblings(id))
        {
            id.indexElt2 = i;
            if (_sf2->isSet(id, champ_keyRange))
            {
                AttributeValue amount = _sf2->get(id, champ_keyRange);
                if (amount.rValue.byLo < noteMin) noteMin = amount.rValue.byLo;
                if (amount.rValue.byHi > noteMax) noteMax = amount.rValue.byHi;
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
    bool error;
    QList<EltID> ids = this->getEltIds(error, true, true);
    if (error)
        return;

    foreach (EltID id, ids)
    {
        if (_typePage == PAGE_PRST)
            id.typeElement = elementPrstInst;
        else
            id.typeElement = elementInstSmpl;
        if (!_sf2->getSiblings(id).empty())
            spatialisation(mapPan, id);
    }

    // Actualisation
    _sf2->endEditing(getEditingSource());
}

void PageTable::spatialisation(QMap<int, double> mapPan, EltID id)
{
    // Sauvegarde des valeurs
    bool isPrst = (_typePage == PAGE_PRST);

    // Liste des éléments liés avec leur lien (stéréo) le cas échéant
    QList<EltID> list1;
    QList<AttributeValue> listRange;
    QList<EltID> list2;
    AttributeValue amount;
    bool found;
    int pos;
    int noteMin = 128;
    int noteMax = 0;
    if (isPrst)
        id.typeElement = elementPrstInst;
    else
        id.typeElement = elementInstSmpl;
    foreach (int i, _sf2->getSiblings(id))
    {
        id.indexElt2 = i;
        if (_sf2->isSet(id, champ_keyRange))
            amount = _sf2->get(id, champ_keyRange);
        else
        {
            amount.rValue.byLo = 0;
            amount.rValue.byHi = 127;
        }
        if (amount.rValue.byLo < noteMin) noteMin = amount.rValue.byLo;
        if (amount.rValue.byHi > noteMax) noteMax = amount.rValue.byHi;

        // Recherche d'une note liée ayant la même étendue sur le clavier
        found = false;
        if (!isPrst)
        {
            pos = 0;
            while (pos < list1.size() && !found)
            {
                if (amount.rValue.byHi == listRange.at(pos).rValue.byHi &&
                        amount.rValue.byLo == listRange.at(pos).rValue.byLo &&
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

    // Spatialisation
    double pan = 0;
    int note = 64;
    EltID id2, id3;
    int sampleG;
    AttributeValue val;
    for (int i = 0; i < list1.size(); i++)
    {
        note = (listRange.at(i).rValue.byLo + listRange.at(i).rValue.byHi) / 2;
        pan = mapPan.value(note);

        // Lien ?
        if (list2.at(i).indexElt2 == -1)
        {
            // pas de lien
            val.shValue = 1000 * pan - 500;
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
                val.shValue = -500;
                _sf2->set(list1.at(i), champ_pan, val);
                // Droite
                val.shValue = 2000 * pan - 500;
                _sf2->set(list2.at(i), champ_pan, val);
            }
            else
            {
                // Gauche
                val.shValue = 2000 * pan - 1500;
                _sf2->set(list1.at(i), champ_pan, val);
                // Droite
                val.shValue = 500;
                _sf2->set(list2.at(i), champ_pan, val);
            }
        }
    }
}

void PageTable::visualize()
{
    EltID id = _currentIds[0];
    if (_typePage == PAGE_INST)
        id.typeElement = elementInstSmpl;
    else
        id.typeElement = elementPrstInst;

    // Vérification que l'élément possède au moins un élément lié, avec un keyrange valide
    int nbElt = 0;
    int posMin = 128;
    int posMax = 0;
    foreach (int i, _sf2->getSiblings(id))
    {
        id.indexElt2 = i;
        nbElt++;
        if (_sf2->isSet(id, champ_keyRange))
        {
            if (_sf2->get(id, champ_keyRange).rValue.byLo < posMin)
                posMin = _sf2->get(id, champ_keyRange).rValue.byLo;
            if (_sf2->get(id, champ_keyRange).rValue.byHi > posMax)
                posMax = _sf2->get(id, champ_keyRange).rValue.byHi;
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

void PageTable::keyPlayedInternal(int key, int velocity)
{
    // Visualization on the table
    if (_table->isVisible())
    {
        // Update triggered elements
        if (velocity > 0 && _listKeyEnlighted.indexOf(key) == -1)
            _listKeyEnlighted.append(key);
        else
            _listKeyEnlighted.removeAll(key);

        // Color table
        for (int i = 1; i < _table->columnCount(); i++)
        {
            EltID id = _table->getID(i);
            if (_sf2->isValid(id))
            {
                bool enlighted = false;
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
                _table->setEnlighted(i, enlighted);
            }
        }
    }

    // Visualization on the range editor
    if (_rangeEditor->isVisible())
        _rangeEditor->playKey(key, velocity);

    // Specific commands for instruments or presets
    keyPlayedInternal2(key, velocity);
}

void PageTable::onOpenElement(EltID id)
{
    // Find the element represented by the division
    if (id.typeElement == elementInstSmpl)
    {
        id.indexElt = _sf2->get(id, champ_sampleID).wValue;
        id.typeElement = elementSmpl;
    }
    else if (id.typeElement == elementPrstInst)
    {
        id.indexElt =_sf2->get(id, champ_instrument).wValue;
        id.typeElement = elementInst;
    }

    emit(selectedIdsChanged(IdList(id)));
}

void PageTable::displayModInTable()
{
    _table->resetModDisplay();

    // Mod for the global division
    for (int i = 0; i < _table->columnCount(); i++)
    {
        EltID id = _table->getID(i);
        if (_sf2->isValid(id))
        {
            if (id.typeElement == this->contenant)
                id.typeElement = this->contenantMod;
            else
                id.typeElement = this->lienMod;

            QList<int> modCount = _sf2->getSiblings(id);
            if (modCount.count() > 0)
            {
                QList<int> rows;
                foreach (int j, modCount)
                {
                    id.indexMod = j;
                    AttributeType champ = _sf2->get(id, champ_sfModDestOper).sfGenValue;
                    if (champ == champ_startAddrsCoarseOffset)
                        champ = champ_startAddrsOffset;
                    else if (champ == champ_endAddrsCoarseOffset)
                        champ = champ_endAddrsOffset;
                    else if (champ == champ_startloopAddrsCoarseOffset)
                        champ = champ_startloopAddrsOffset;
                    else if (champ == champ_endloopAddrsCoarseOffset)
                        champ = champ_endloopAddrsOffset;
                    rows << _table->getRow(champ);
                }
                _table->updateModDisplay(i, rows);
            }
        }
    }
}

void PageTable::onShow()
{
    // Refresh the keyboard
    customizeKeyboard();
}
