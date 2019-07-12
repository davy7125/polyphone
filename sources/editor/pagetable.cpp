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

PageTable::PageTable(TypePage typePage, QWidget *parent) : Page(parent, typePage, typePage == PAGE_INST ? "page:inst" : "page:prst"),
    _table(nullptr)
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
    _modulatorEditor->setIds(_currentIds);
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

        if (this->contenant == elementInst)
        {
            // Data about linked samples
            int row = _table->getRow(champ_byOriginalPitch);
            if (row > -1)
            {
                unsigned char pitch = _sf2->get(id3, champ_byOriginalPitch).bValue;
                _table->item(row, numCol)->setText(QString::number(pitch));
            }

            row = _table->getRow(champ_dwLength);
            if (row > -1)
            {
                unsigned int length = _sf2->get(id3, champ_dwLength).dwValue;
                _table->item(row, numCol)->setText(QString::number(length));
            }

            row = _table->getRow(champ_dwStartLoop);
            if (row > -1)
            {
                unsigned int start = _sf2->get(id3, champ_dwStartLoop).dwValue;
                unsigned int end = _sf2->get(id3, champ_dwEndLoop).dwValue;
                _table->item(row, numCol)->setText(QString::number(start) + "-" + QString::number(end));
            }
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
        // Rows with the alternate color
        for (int i = multiGlobal ? 0 : 1; i < _table->columnCount(); i++)
        {
            for (int j = 0; j < _table->rowCount(); j++)
            {
                if (j < 6) {}
                else if (j < 10)
                    _table->item(j, i)->setBackgroundColor(alternateColor);
                else if (j < 13) {}
                else if (j < 21)
                    _table->item(j, i)->setBackgroundColor(alternateColor);
                else if (j < 31) {}
                else if (j < 39)
                    _table->item(j, i)->setBackgroundColor(alternateColor);
                else if (j < 44) {}
                else
                    _table->item(j, i)->setBackgroundColor(alternateColor);
                _table->item(j, i)->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
            }
        }

        if (multiGlobal)
        {
            // Hide rows champ_byOriginalPitch, champ_dwLength and champ_dwStartLoop
            _table->hideRow(6);
            _table->hideRow(44);
            _table->hideRow(47);
        }
        else
        {
            // First column with a hatching pattern
            for (int j = 0; j < _table->rowCount(); j++)
            {
                if (j < 6)
                    _table->item(j, 0)->setBackground(brush1);
                else if (j < 10)
                    _table->item(j, 0)->setBackground(brush2);
                else if (j < 13)
                    _table->item(j, 0)->setBackground(brush1);
                else if (j < 21)
                    _table->item(j, 0)->setBackground(brush2);
                else if (j < 31)
                    _table->item(j, 0)->setBackground(brush1);
                else if (j < 39)
                    _table->item(j, 0)->setBackground(brush2);
                else if (j < 44)
                    _table->item(j, 0)->setBackground(brush1);
                else
                    _table->item(j, 0)->setBackground(brush2);
                _table->item(j, 0)->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
            }

            // Style of rows champ_byOriginalPitch, champ_dwLength and champ_dwStartLoop
            styleFixedRow(6);
            styleFixedRow(44);
            styleFixedRow(47);
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

void PageTable::styleFixedRow(int numRow)
{
    // Color, font
    QFont font(this->font().family(), 4 * this->font().pointSize() / 5, QFont::Normal, true);
    QColor fixedColor = ThemeManager::mix(this->palette().color(QPalette::Text), this->palette().color(QPalette::Base), 0.35);

    // Style the cells
    for (int i = 0; i < _table->columnCount(); i++)
    {
        _table->item(numRow, i)->setFont(font);
        _table->item(numRow, i)->setTextColor(fixedColor);
        _table->item(numRow, i)->setFlags(Qt::NoItemFlags);
    }

    // Visibility of the row
    _table->showRow(numRow);
    _table->setRowHeight(numRow, 2 * font.pointSize());
}

void PageTable::afficheRanges(bool justSelection)
{
    if (!_currentIds.isEmpty())
        _rangeEditor->display(_currentIds, justSelection);
}

void PageTable::afficheEnvelops(bool justSelection)
{
    if (_envelopEditor != nullptr)
        _envelopEditor->display(_currentIds, justSelection);
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
            RangesType keyRange = _sf2->get(id, champ_keyRange).rValue;
            RangesType velRange = _sf2->get(id, champ_velRange).rValue;

            // Recherche d'une correspondance dans les samples liés
            bool ok = true;
            int numCol2 = -1;
            EltID idTmp = id;
            for (int i = 1; i < _table->columnCount(); i++)
            {
                idTmp = _table->getID(i);
                if (i != colonne)
                {
                    RangesType keyRange2 = _sf2->get(idTmp, champ_keyRange).rValue;
                    RangesType velRange2 = _sf2->get(idTmp, champ_velRange).rValue;
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
    _modulatorEditor->setIds(_currentIds);
    if (champ == champ_overridingRootKey || champ == champ_keyRange)
        customizeKeyboard();
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
        _modulatorEditor->setIds(_currentIds);

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

            RangesType keyRange;
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
            RangesType defaultKeyRange;
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
                RangesType keyRange;
                if (_sf2->isSet(id, champ_keyRange))
                    keyRange = _sf2->get(id, champ_keyRange).rValue;
                else
                    keyRange = defaultKeyRange;
                ContextManager::midi()->keyboard()->addRangeAndRootKey(-1, keyRange.byLo, keyRange.byHi);
            }
        }
    }
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
                    AttributeType champ = static_cast<AttributeType>(_sf2->get(id, champ_sfModDestOper).wValue);
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
