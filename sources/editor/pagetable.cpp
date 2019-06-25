/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
**                2014      Andrea Celani                                 **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program. If not, see http://www.gnu.org/licenses/.    **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "pagetable.h"
#include "contextmanager.h"
#include "dialogselection.h"
#include "graphicsviewrange.h"
#include "envelopeditor.h"
#include "utils.h"
#include <QScrollBar>
#include <QMenu>
#include "pianokeybdcustom.h"
#include "modulatoreditor.h"

QList<PageTable::Modulator> PageTable::_modulatorCopy;

PageTable::PageTable(TypePage typePage, QWidget *parent) : Page(parent, typePage, typePage == PAGE_INST ? "page:inst" : "page:prst"),
    _table(nullptr),
    tableMod(nullptr)
{
    connect(ContextManager::configuration(), SIGNAL(divisionSortChanged()), this, SLOT(divisionSortChanged()));
}

void PageTable::afficheTable(bool justSelection)
{
    int posV = _table->verticalScrollBar()->value();
    _sortType = ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "division_sort", 0).toInt();

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
    _modulatorEditor->setIds(_currentIds, QList<AttributeType>());
    this->displayModInTable();

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
        bool offsetStartDefined = false;
        int offsetEnd = 0;
        bool offsetEndDefined = false;
        int offsetStartLoop = 0;
        bool offsetStartLoopDefined = false;
        int offsetEndLoop = 0;
        bool offsetEndLoopDefined = false;
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
            if (Utils::naturalOrder(_sf2->getQstr(id, champ_nameSort), _sf2->getQstr(_table->getID(i), champ_nameSort)) > 0)
                numCol++;
            else
                break;
        }

        _table->addColumn(numCol, qStr, id);
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
                offsetStartDefined = true;
                offsetStart += genValTmp.shValue;
                break;
            case champ_startAddrsCoarseOffset:
                offsetStartDefined = true;
                offsetStart += 32768 * genValTmp.shValue;
                break;
            case champ_endAddrsOffset:
                offsetEndDefined = true;
                offsetEnd += genValTmp.shValue;
                break;
            case champ_endAddrsCoarseOffset:
                offsetEndDefined = true;
                offsetEnd += 32768 * genValTmp.shValue;
                break;
            case champ_startloopAddrsOffset:
                offsetStartLoopDefined = true;
                offsetStartLoop += genValTmp.shValue;
                break;
            case champ_startloopAddrsCoarseOffset:
                offsetStartLoopDefined = true;
                offsetStartLoop += 32768 * genValTmp.shValue;
                break;
            case champ_endloopAddrsOffset:
                offsetEndLoopDefined = true;
                offsetEndLoop += genValTmp.shValue;
                break;
            case champ_endloopAddrsCoarseOffset:
                offsetEndLoopDefined = true;
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
        if (offsetStartDefined && _table->getRow(champ_startAddrsOffset) > -1)
        {
            row = _table->getRow(champ_startAddrsOffset);
            _table->item(row, numCol)->setText(QString::number(offsetStart));
        }
        if (offsetEndDefined && _table->getRow(champ_endAddrsOffset) > -1)
        {
            row = _table->getRow(champ_endAddrsOffset);
            _table->item(row, numCol)->setText(QString::number(offsetEnd));
        }
        if (offsetStartLoopDefined && _table->getRow(champ_startloopAddrsOffset) > -1)
        {
            row = _table->getRow(champ_startloopAddrsOffset);
            _table->item(row, numCol)->setText(QString::number(offsetStartLoop));
        }
        if (offsetEndLoopDefined && _table->getRow(champ_endloopAddrsOffset) > -1)
        {
            row = _table->getRow(champ_endloopAddrsOffset);
            _table->item(row, numCol)->setText(QString::number(offsetEndLoop));
        }
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
            if (Utils::sortDivisions(id, _table->getID(j), _sortType) > 0)
                numCol++;
            else
                break;
        }

        nbSmplInst++;
        int offsetStart = 0;
        bool offsetStartDefined = false;
        int offsetEnd = 0;
        bool offsetEndDefined = false;
        int offsetStartLoop = 0;
        bool offsetStartLoopDefined = false;
        int offsetEndLoop = 0;
        bool offsetEndLoopDefined = false;
        _table->addColumn(numCol, qStr, id);
        foreach (int champTmp, _sf2->getSiblings(id2))
        {
            genValTmp = _sf2->get(id, (AttributeType)champTmp);
            switch (champTmp)
            {
            case champ_startAddrsOffset:
                offsetStartDefined = true;
                offsetStart += genValTmp.shValue;
                break;
            case champ_startAddrsCoarseOffset:
                offsetStartDefined = true;
                offsetStart += 32768 * genValTmp.shValue;
                break;
            case champ_endAddrsOffset:
                offsetEndDefined = true;
                offsetEnd += genValTmp.shValue;
                break;
            case champ_endAddrsCoarseOffset:
                offsetEndDefined = true;
                offsetEnd += 32768 * genValTmp.shValue;
                break;
            case champ_startloopAddrsOffset:
                offsetStartLoopDefined = true;
                offsetStartLoop += genValTmp.shValue;
                break;
            case champ_startloopAddrsCoarseOffset:
                offsetStartLoopDefined = true;
                offsetStartLoop += 32768 * genValTmp.shValue;
                break;
            case champ_endloopAddrsOffset:
                offsetEndLoopDefined = true;
                offsetEndLoop += genValTmp.shValue;
                break;
            case champ_endloopAddrsCoarseOffset:
                offsetEndLoopDefined = true;
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

        if (offsetStartDefined && _table->getRow(champ_startAddrsOffset) > -1)
        {
            row = _table->getRow(champ_startAddrsOffset);
            _table->item(row, numCol)->setText(QString::number(offsetStart));
        }
        if (offsetEndDefined && _table->getRow(champ_endAddrsOffset) > -1)
        {
            row = _table->getRow(champ_endAddrsOffset);
            _table->item(row, numCol)->setText(QString::number(offsetEnd));
        }
        if (offsetStartLoopDefined && _table->getRow(champ_startloopAddrsOffset) > -1)
        {
            row = _table->getRow(champ_startloopAddrsOffset);
            _table->item(row, numCol)->setText(QString::number(offsetStartLoop));
        }
        if (offsetEndLoopDefined && _table->getRow(champ_endloopAddrsOffset) > -1)
        {
            row = _table->getRow(champ_endloopAddrsOffset);
            _table->item(row, numCol)->setText(QString::number(offsetEndLoop));
        }
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
    if (_envelopEditor != nullptr)
        _envelopEditor->display(_currentIds, justSelection);
}

void PageTable::afficheMod(EltID id, int selectedIndex)
{return;
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
                qStr = trUtf8("Modulator") + ": #" + QString::number(iVal + 1);
            else
                qStr = trUtf8("Link (invalid)");
        }
        else
        {
            //qStr = getIndexName(sfModTmp.Index, sfModTmp.CC);
        }

        this->tableMod->item(numLigne, 6)->setText(qStr);
        sfModTmp = _sf2->get(id, champ_sfModAmtSrcOper).sfModValue;
        iVal = 4 * sfModTmp.D + 8 * sfModTmp.P + sfModTmp.Type + 1;
        iconName = QString(":/icons/courbe%1.svg").arg(iVal, 2, 10, QChar('0'));
        icon = ContextManager::theme()->getColoredSvg(iconName, QSize(iconWidth, iconWidth), replacement);
        this->tableMod->item(numLigne, 7)->setIcon(icon);
        //qStr = getIndexName(sfModTmp.Index, sfModTmp.CC);
        genValTmp = _sf2->get(id, champ_modAmount);
        qStr.append(QString::fromUtf8(" × ")).append(Attribute::toString(champ_modAmount, _typePage == PAGE_PRST, genValTmp));
        this->tableMod->item(numLigne, 7)->setText(qStr);
        this->spinAmount->setValue(genValTmp.shValue);
        genValTmp = _sf2->get(id, champ_sfModDestOper);
        if (genValTmp.wValue > 99)
        {
            // lien vers modulateur
            qStr = trUtf8("Modulator") + ": #" + QString::number(genValTmp.wValue - 32767);
        }
        else
            qStr = Attribute::getDescription(genValTmp.sfGenValue, _typePage == PAGE_PRST);

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

    this->afficheEditMod();
    this->displayModInTable();
}

void PageTable::afficheEditMod()
{
    if (_pushCopyMod->menu())
    {
        _pushCopyMod->setToolTip(trUtf8("Copy / duplicate modulators"));
        if (this->tableMod->getSelectedIDs().isEmpty())
        {
            _menu->actions()[0]->setText(trUtf8("Duplicate modulators toward..."));
            _menu->actions()[1]->setText(trUtf8("Copy all modulators"));
        }
        else
        {
            _menu->actions()[0]->setText(trUtf8("Duplicate the selection toward..."));
            _menu->actions()[1]->setText(trUtf8("Copy the selected modulators"));
        }
    }
    else
    {
        if (this->tableMod->getSelectedIDs().isEmpty())
            _pushCopyMod->setToolTip(trUtf8("Copy all modulators"));
        else
            _pushCopyMod->setToolTip(trUtf8("Copy the selected modulators"));
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
        //this->comboSource1->removeElements2();
        //addAvailableSenderMod(this->comboSource1, id);

        // Sélection et activation
        wTmp = sfMod.Index;
        bTmp = sfMod.CC;
        if (wTmp == 127 && bTmp == 0)
        {
            // On cherche le lien
            int iVal = getAssociatedMod(id);
            //if (iVal > -1) this->comboSource1->selectIndex(100, 32768 + iVal);
            //else this->comboSource1->selectIndex(this->getSrcIndex(0, 0));
        }
        //else this->comboSource1->selectIndex(this->getSrcIndex(wTmp, bTmp));
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
        //this->comboSource2->selectIndex(this->getSrcIndex(wTmp, bTmp));
        this->comboSource2->setEnabled(true);

        // COMBOBOX DESTINATION
        //this->comboDestination->removeElements2();
        //addAvailableReceiverMod(this->comboDestination, id);

        // Sélection et activation
        wTmp = _sf2->get(id, champ_sfModDestOper).wValue;
        //this->comboDestination->selectIndex(this->getDestIndex((AttributeType)wTmp), wTmp);
        this->comboDestination->setEnabled(true);

        // Selection in the table
        EltID idParent = id;
        if (idParent.typeElement == this->contenantMod)
            idParent.typeElement = this->contenant;
        else if (idParent.typeElement == this->lienMod)
            idParent.typeElement = this->lien;
        //_table->selectCell(idParent, (AttributeType)_sf2->get(id, champ_sfModDestOper).wValue);
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

        // Update the cell value
        _table->item(ligne, colonne)->setText(QString::number(genAmount.shValue + 32768 * genAmount2.shValue));
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
    //    if (index < 32768)
    //        index = getDestNumber(index);
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

    // List of distinct selected ids
    IdList ids;
    QList<QTableWidgetItem*> listItems = _table->selectedItems();
    if (listItems.empty()) {
        _preparingPage = false;
        return;
    }
    for (int i = 0; i < listItems.count(); i++)
    {
        EltID idTmp = _table->getID(listItems.at(i)->column());
        if (!ids.contains(idTmp))
            ids << idTmp;
    }
    _currentIds = ids;

    // Update mods
    if (_currentIds.count() == 1)
    {
        // List of attributes
        QList<AttributeType> attributes;
        for (int i = 0; i < listItems.count(); i++)
            attributes << _table->getChamp(listItems.at(i)->row());
        _modulatorEditor->setIds(_currentIds, attributes);
    }
    else
        _modulatorEditor->setIds(_currentIds, QList<AttributeType>());

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

/*void PageTable::addAvailableReceiverMod(ModulatorComboSrcDest *combo, EltID id)
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
                combo->addItem(trUtf8("Modulator") + ": #" + QString::number(id2.indexMod + 1));
            }
        }
    }
}

void PageTable::addAvailableSenderMod(ModulatorComboSrcDest *combo, EltID id)
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
                combo->addItem(trUtf8("Modulator") + ": #" + QString::number(id2.indexMod + 1));
            }
        }
    }
}*/

int PageTable::getAssociatedMod(EltID id)
{
    if (id.typeElement != elementInstMod && id.typeElement != elementInstSmplMod &&
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
            QString warnQStr = trUtf8("Forbidden action:");
            if (champTmp == champ_startAddrsOffset ||
                    champTmp == champ_startAddrsCoarseOffset ||
                    champTmp == champ_startloopAddrsOffset ||
                    champTmp == champ_startloopAddrsCoarseOffset ||
                    champTmp == champ_endAddrsOffset ||
                    champTmp == champ_endAddrsCoarseOffset ||
                    champTmp == champ_endloopAddrsOffset ||
                    champTmp == champ_endloopAddrsCoarseOffset)
            {
                QMessageBox::warning(this, trUtf8("Warning"), warnQStr +
                                     trUtf8("offsets cannot be modulated in a preset."));
                return;
            }
            else if (champTmp == champ_keynum || champTmp == champ_velocity ||
                     champTmp == champ_sampleModes ||
                     champTmp == champ_exclusiveClass ||
                     champTmp == champ_overridingRootKey)
            {
                QMessageBox::warning(this, trUtf8("Warning"), warnQStr +
                                     trUtf8("%1 cannot be modulated in a preset.").arg("<b>" + Attribute::getDescription(champTmp, true) + "</b>"));
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
        QMessageBox::warning(this, trUtf8("Warning"), _typePage == PAGE_INST ?
                                 trUtf8("An instrument comprising no samples is not compatible with this tool.") :
                                 trUtf8("A preset comprising no instruments is not compatible with this tool."));
        return result;
    }

    if (allDivWithRange && !_currentIds.areAllWithRange(_typePage == PAGE_INST ? elementInst : elementPrst))
    {
        error = true;
        QMessageBox::warning(this, trUtf8("Warning"), trUtf8("The keyrange of all divisions must be specified."));
        return result;
    }

    return result;
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

void PageTable::divisionSortChanged()
{
    this->afficheTable(false);
}

void PageTable::onModSelectionChanged(QList<AttributeType> attributes)
{
    if (!attributes.empty() && _currentIds.count() == 1)
        _table->selectCells(_currentIds[0], attributes);
}
