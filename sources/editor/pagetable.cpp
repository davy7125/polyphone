/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "pagetable.h"
#include "contextmanager.h"
#include "dialogselection.h"
#include "graphicsviewrange.h"
#include "utils.h"
#include "modulatoreditor.h"
#include <QMenu>
#include <QScrollBar>
#include <QFontMetrics>

PageTable::PageTable(bool isPrst, QWidget *parent) : Page(parent, isPrst ? "page:prst" : "page:inst"),
    _table(nullptr),
    _isPrst(isPrst)
{
    connect(ContextManager::configuration(), SIGNAL(divisionSortChanged()), this, SLOT(divisionSortChanged()));

    if (_isPrst)
    {
        this->contenant = elementPrst;
        this->contenantGen = elementPrstGen;
        this->contenantMod = elementPrstMod;
        this->contenu = elementInst;
        this->lien = elementPrstInst;
        this->lienGen = elementPrstInstGen;
        this->lienMod = elementPrstInstMod;
    }
    else
    {
        this->contenant = elementInst;
        this->contenantGen = elementInstGen;
        this->contenantMod = elementInstMod;
        this->contenu = elementSmpl;
        this->lien = elementInstSmpl;
        this->lienGen = elementInstSmplGen;
        this->lienMod = elementInstSmplMod;
    }
}

void PageTable::updateInterface(QString editingSource)
{
    // Check if the new parents are the same
    IdList parentIds = _currentIds.getSelectedIds(_isPrst ? elementPrst : elementInst);
    bool sameElement = true;
    if (parentIds.count() == _currentParentIds.count())
    {
        for (int i = 0; i < parentIds.count(); i++)
        {
            if (parentIds[i] != _currentParentIds[i])
            {
                sameElement = false;
                break;
            }
        }
    }
    else
        sameElement = false;
    bool justSelection = (sameElement && editingSource == "command:selection");

    // Store the new parent ids
    _currentParentIds = parentIds;

    // Show or hide the modulator section
    _modulatorEditor->setVisible(_currentParentIds.count() == 1);

    this->afficheTable(justSelection);
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
    else
        // Update the horizontal header if the solo on selection is enabled
        _table->horizontalHeader()->viewport()->update();

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

        // Column number
        int numCol = 0;
        for (int i = 0; i < nbGlobal; i++)
        {
            if (Utils::naturalOrder(_sf2->getQstr(id, champ_nameSort), _sf2->getQstr(_table->getID(i), champ_nameSort)) > 0)
                numCol++;
            else
                break;
        }

        _table->addColumn(numCol, multiGlobal ? _sf2->getQstr(id, champ_name) : tr("Global"), id);
        nbGlobal++;
        EltID idGen = id;
        idGen.typeElement = this->contenantGen;
        idGen.indexElt2 = 0;
        foreach (int i, _sf2->getSiblings(idGen))
        {
            genValTmp = _sf2->get(id, static_cast<AttributeType>(i));
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
                row = _table->getRow(static_cast<AttributeType>(i));
                if (row > -1)
                {
                    if (i == champ_sampleModes)
                        _table->setLoopModeImage(row, numCol, genValTmp.wValue);
                    else
                        _table->item(row, numCol)->setText(
                                    attributeToString(static_cast<AttributeType>(i),
                                                      _isPrst, genValTmp));
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
        for (int j = 1; j < nbSmplInst + 1; j++)
        {
            if (_sf2->sortDivisions(id, _table->getID(j), _sortType) > 0)
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
        _table->addColumn(numCol, _sf2->getQstr(id3, champ_name), id);
        foreach (int champTmp, _sf2->getSiblings(id2))
        {
            genValTmp = _sf2->get(id, static_cast<AttributeType>(champTmp));
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
                row = _table->getRow(static_cast<AttributeType>(champTmp));
                if (row > -1)
                {
                    if (champTmp == champ_sampleModes)
                        _table->setLoopModeImage(row, numCol, genValTmp.wValue);
                    else
                        _table->item(row, numCol)->setText(
                                    attributeToString(static_cast<AttributeType>(champTmp),
                                                      _isPrst, genValTmp));
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
                quint8 pitch = _sf2->get(id3, champ_byOriginalPitch).bValue;
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
    QColor color = ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND);
    QColor alternateColor = ContextManager::theme()->getColor(ThemeManager::LIST_ALTERNATIVE_BACKGROUND);
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
                    _table->item(j, i)->setBackground(alternateColor);
                else if (j < 13) {}
                else if (j < 21)
                    _table->item(j, i)->setBackground(alternateColor);
                else if (j < 31) {}
                else if (j < 39)
                    _table->item(j, i)->setBackground(alternateColor);
                else if (j < 44) {}
                else
                    _table->item(j, i)->setBackground(alternateColor);
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
                    _table->item(j, i)->setBackground(alternateColor);
                else if (j < 10) {}
                else if (j < 18)
                    _table->item(j, i)->setBackground(alternateColor);
                else if (j < 28) {}
                else if (j < 36)
                    _table->item(j, i)->setBackground(alternateColor);
                _table->item(j, i)->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
            }
        }
    }
    _table->hideRow(0);
}

void PageTable::styleFixedRow(int numRow)
{
    // Color, font
    QFont font(this->font().family(), this->font().pointSize() - 1, QFont::Normal, true);
    QColor fixedColor = ThemeManager::mix(ContextManager::theme()->getColor(ThemeManager::LIST_TEXT),
                                          ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND), 0.35);

    // Style the cells
    for (int i = 0; i < _table->columnCount(); i++)
    {
        _table->item(numRow, i)->setFont(font);
        _table->item(numRow, i)->setForeground(fixedColor);
        _table->item(numRow, i)->setFlags(Qt::NoItemFlags);
    }

    // Visibility of the row
    _table->showRow(numRow);
    _table->setRowHeight(numRow, QFontMetrics(font).height() + 6);
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
    AttributeValue genAmount = attributeFromString(champ1, _isPrst, texte, ok);
    if (ok)
    {
        // Enregistrement de la nouvelle valeur
        AttributeValue genAmount2 = attributeFromString(champ2, _isPrst, texte, ok);
        int iVal = limit(32768 * genAmount2.shValue + genAmount.shValue, champ1, id);
        genAmount2.shValue = static_cast<qint16>(iVal / 32768);
        genAmount.shValue = static_cast<qint16>(iVal % 32768);
        if (!_sf2->isSet(id, champ1) || !_sf2->isSet(id, champ2) ||
                genAmount.shValue != _sf2->get(id, champ1).shValue ||
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
            _table->item(ligne, colonne)->setText(attributeToString(champ1, _isPrst, genAmount));
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
    _sf2->endEditing(_editingSource);
}

void PageTable::set(int ligne, int colonne, bool allowPropagation)
{
    if (_preparingPage)
        return;

    // Modification d'un élément du tableau
    AttributeType champ = _table->getChamp(ligne);
    if (champ == champ_unknown || champ >= champ_endOper)
        return;

    EltID id = _table->getID(colonne);
    if (allowPropagation && id.typeElement == elementInstSmpl && champ != champ_pan &&
            ContextManager::configuration()->getValue(ConfManager::SECTION_NONE, "stereo_modification", true).toBool())
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
            genAmount.wValue = static_cast<quint16>(_table->item(ligne, colonne)->data(Qt::UserRole).toInt());

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
            switch (champ)
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
            switch (champ)
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
                AttributeValue genAmount = attributeFromString(champ, _isPrst, texte, ok);
                if (ok)
                {
                    // Modification champ
                    if (genAmount.wValue != _sf2->get(id, champ).wValue || !_sf2->isSet(id, champ))
                    {
                        // Modification du sf2
                        _sf2->set(id, champ, genAmount);
                    }
                    // Mise à jour de la valeur dans la cellule
                    _table->item(ligne, colonne)->setText(attributeToString(champ, _isPrst, genAmount));
                }
                else
                {
                    // Restauration valeur précédente
                    if (_sf2->isSet(id, champ))
                        _table->item(ligne, colonne)->setText(attributeToString(champ, _isPrst, _sf2->get(id, champ)));
                    else _table->item(ligne, colonne)->setText("");
                }
            }
            }
            _preparingPage = false;
        }
    }

    // Mise à jour partie mod (car entre 2 des mods peuvent être définitivement détruits, et les index peuvent être mis à jour)
    _modulatorEditor->setIds(_currentIds);
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
        _modulatorEditor->blockSignals(true);
        _modulatorEditor->setIds(_currentIds, attributes);
        _modulatorEditor->blockSignals(false);
    }
    else
        _modulatorEditor->setIds(_currentIds);

    // Update the selection outside the table
    emit(selectedIdsChanged(ids));
    _preparingPage = false;
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
        // Lower limit
        if (ret < 0) ret = 0;

        // Upper limit
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
        // Lower limit
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

        // Upper limit
        if (ret > 0)
            ret = 0;
        break;
    case champ_startloopAddrsOffset:
        // Lower limit
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

        // Upper limit
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
        // Lower limit
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

        // Upper limit
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

    // Update triggered elements
    if (velocity > 0 && !_listKeyEnlighted.contains(key))
        _listKeyEnlighted[key] = velocity;
    else
        _listKeyEnlighted.remove(key);

    // Color table

    if (_table->columnCount() >= 1)
    {
        // Default ranges
        EltID globalId = _table->getID(0);
        RangesType defaultKeyRange, defaultVelRange;
        if (_sf2->isSet(globalId, champ_keyRange))
            defaultKeyRange = _sf2->get(globalId, champ_keyRange).rValue;
        else
        {
            defaultKeyRange.byLo = 0;
            defaultKeyRange.byHi = 128;
        }
        if (_sf2->isSet(globalId, champ_velRange))
            defaultVelRange = _sf2->get(globalId, champ_velRange).rValue;
        else
        {
            defaultVelRange.byLo = 0;
            defaultVelRange.byHi = 128;
        }

        for (int i = 1; i < _table->columnCount(); i++)
        {
            EltID id = _table->getID(i);
            if (_sf2->isValid(id))
            {
                // Division range
                RangesType keyRange = defaultKeyRange;
                if (_sf2->isSet(id, champ_keyRange))
                    keyRange = _sf2->get(id, champ_keyRange).rValue;
                RangesType velRange = defaultVelRange;
                if (_sf2->isSet(id, champ_velRange))
                    velRange = _sf2->get(id, champ_velRange).rValue;

                bool enlighted = false;
                QMapIterator<int, int> it(_listKeyEnlighted);
                while (it.hasNext()) {
                    it.next();
                    if (it.key() >= keyRange.byLo && it.key() <= keyRange.byHi && it.value() >= velRange.byLo && it.value() <= velRange.byHi)
                    {
                        enlighted = true;
                        break;
                    }
                }
                _table->setEnlighted(i, enlighted);
            }
        }
    }
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

void PageTable::divisionSortChanged()
{
    this->afficheTable(false);
}

void PageTable::onModSelectionChanged(QList<AttributeType> attributes)
{
    if (!attributes.empty() && _currentIds.count() == 1)
        _table->selectCells(_currentIds[0], attributes);
}

QString PageTable::attributeToString(AttributeType champ, bool isPrst, AttributeValue storedValue)
{
    QString result = "";

    switch (champ)
    {
    case champ_velRange:
        if (storedValue.rValue.byLo == storedValue.rValue.byHi)
            result = QString::number(storedValue.rValue.byLo);
        else
            result = QString::number(storedValue.rValue.byLo) + "-" + QString::number(storedValue.rValue.byHi);
        break;
    case champ_keyRange:
        if (storedValue.rValue.byLo == storedValue.rValue.byHi)
            result = ContextManager::keyName()->getKeyName(storedValue.rValue.byLo);
        else
            result = ContextManager::keyName()->getKeyName(storedValue.rValue.byLo) + "-" + ContextManager::keyName()->getKeyName(storedValue.rValue.byHi);
        break;
    case champ_initialAttenuation:
        // Now with two digits since we have real dB (one step is 0.04 dB)
        result = QLocale::system().toString(Attribute::toRealValue(champ, isPrst, storedValue), 'f', 2);
        break;
    case champ_pan: case champ_initialFilterQ:
    case champ_modLfoToVolume:
    case champ_sustainVolEnv: case champ_sustainModEnv:
    case champ_chorusEffectsSend: case champ_reverbEffectsSend:
        result = QLocale::system().toString(Attribute::toRealValue(champ, isPrst, storedValue), 'f', 1);
        break;
    case champ_keynum: case champ_overridingRootKey:
        result = ContextManager::keyName()->getKeyName(storedValue.wValue);
        break;
    case champ_sampleModes: case champ_exclusiveClass: case champ_velocity: case champ_sfModDestOper:
        result = QString::number(storedValue.wValue);
        break;
    case champ_byOriginalPitch: case champ_chPitchCorrection:
    case champ_dwEndLoop: case champ_dwStartLoop:
    case champ_scaleTuning: case champ_coarseTune: case champ_fineTune:
    case champ_modLfoToFilterFc: case champ_modEnvToFilterFc:
    case champ_modEnvToPitch: case champ_modLfoToPitch: case champ_vibLfoToPitch:
    case champ_keynumToModEnvHold: case champ_keynumToVolEnvHold:
    case champ_keynumToModEnvDecay: case champ_keynumToVolEnvDecay:
    case champ_startAddrsOffset: case champ_startAddrsCoarseOffset:
    case champ_startloopAddrsOffset: case champ_startloopAddrsCoarseOffset:
    case champ_endAddrsOffset: case champ_endAddrsCoarseOffset:
    case champ_endloopAddrsOffset: case champ_endloopAddrsCoarseOffset:
    case champ_modAmount:
        result = QString::number(storedValue.shValue);
        break;
    case champ_initialFilterFc:
        result = QLocale::system().toString(Attribute::toRealValue(champ, isPrst, storedValue), 'f', isPrst ? 3 : 0);
        break;
    case champ_freqModLFO: case champ_freqVibLFO:
    case champ_delayModEnv: case champ_attackModEnv: case champ_holdModEnv: case champ_decayModEnv: case champ_releaseModEnv:
    case champ_delayVolEnv: case champ_attackVolEnv: case champ_holdVolEnv: case champ_decayVolEnv: case champ_releaseVolEnv:
    case champ_delayModLFO: case champ_delayVibLFO:
        result = QLocale::system().toString(Attribute::toRealValue(champ, isPrst, storedValue), 'f', 3);
        break;
    case champ_sfModTransOper:
        if (storedValue.wValue == 2)
            result = ", " + tr("absolute value");
        break;
    case champ_indexMod:
        result = QString::number(storedValue.wValue + 1);
        break;
    case champ_sfModAmtSrcOper: case champ_sfModSrcOper:
        result = QString::number(storedValue.sfModValue.Index);
        break;
    default: break;
    }

    return result;
}

AttributeValue PageTable::attributeFromString(AttributeType champ, bool isPrst, QString strValue, bool &ok)
{
    AttributeValue value;
    value.wValue = 0;
    ok = true;

    switch (champ)
    {
    case champ_keyRange: case champ_velRange: {
        QRegularExpression regEx("[0-9]-");
        int posSeparator = static_cast<int>(strValue.indexOf(regEx)) + 1;
        QString txtLeft, txtRight;
        if (posSeparator == 0)
            txtLeft = txtRight = strValue;
        else
        {
            txtLeft = strValue.left(posSeparator);
            txtRight = strValue.right(strValue.size() - posSeparator - 1);
        }

        int val1, val2;
        if (champ == champ_velRange)
        {
            bool tmp;
            val1 = txtLeft.toInt(&tmp);
            val2 = txtRight.toInt(&ok);
            ok &= tmp;
        }
        else
        {
            val1 = ContextManager::keyName()->getKeyNum(txtLeft);
            val2 = ContextManager::keyName()->getKeyNum(txtRight);
            ok = (val1 != -1) && (val2 != -1);
        }

        if (txtLeft.isEmpty())
            val1 = 0;
        if (txtRight.isEmpty())
            val2 = 127;

        if (val1 > val2)
        {
            int val3 = val1;
            val1 = val2;
            val2 = val3;
        }

        value.rValue.byLo = static_cast<quint8>(val1);
        value.rValue.byHi = static_cast<quint8>(val2);
        value = Attribute::limit(champ, value, isPrst);
    } break;
    case champ_chPitchCorrection: case champ_byOriginalPitch:
    case champ_dwEndLoop: case champ_dwStartLoop:
    case champ_initialAttenuation: case champ_sustainVolEnv: case champ_pan:
    case champ_coarseTune: case champ_fineTune: case champ_scaleTuning:
    case champ_initialFilterFc: case champ_initialFilterQ:
    case champ_modEnvToPitch: case champ_modLfoToPitch: case champ_vibLfoToPitch:
    case champ_modLfoToFilterFc: case champ_modEnvToFilterFc:
    case champ_modLfoToVolume: case champ_sustainModEnv:
    case champ_delayModEnv: case champ_holdModEnv:
    case champ_delayVolEnv: case champ_holdVolEnv:
    case champ_delayModLFO: case champ_delayVibLFO:
    case champ_attackModEnv: case champ_decayModEnv: case champ_releaseModEnv:
    case champ_attackVolEnv: case champ_decayVolEnv: case champ_releaseVolEnv:
    case champ_keynumToModEnvHold: case champ_keynumToVolEnvHold:
    case champ_keynumToModEnvDecay: case champ_keynumToVolEnvDecay:
    case champ_freqModLFO: case champ_freqVibLFO:
    case champ_exclusiveClass: case champ_velocity:
    case champ_chorusEffectsSend: case champ_reverbEffectsSend:
    case champ_startAddrsOffset: case champ_startloopAddrsOffset:
    case champ_endAddrsOffset: case champ_endloopAddrsOffset:
    case champ_startAddrsCoarseOffset: case champ_startloopAddrsCoarseOffset:
    case champ_endAddrsCoarseOffset: case champ_endloopAddrsCoarseOffset:
        value = Attribute::fromRealValue(champ, isPrst, QLocale::system().toDouble(strValue, &ok));
        break;
    case champ_sampleModes: {
        int iTmp = Utils::round16(QLocale::system().toDouble(strValue, &ok));
        if (iTmp != 0 && iTmp != 1 && iTmp != 3)
            iTmp = 0;
        value.wValue = static_cast<quint16>(iTmp);
    } break;
    case champ_overridingRootKey: case champ_keynum: {
        int keyNum = ContextManager::keyName()->getKeyNum(strValue);
        ok = (keyNum > -1 && keyNum <= 127);
        value.shValue = ok ? static_cast<qint16>(keyNum) : 0;
    } break;
    default:
        ok = false;
    }

    if (!ok)
        value.dwValue = 0;

    return value;
}
