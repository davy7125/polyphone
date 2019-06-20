/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
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

#include "pageinst.h"
#include "ui_pageinst.h"
#include <QProgressDialog>
#include <QInputDialog>
#include <QMenu>
#include <qmath.h>
#include "pianokeybdcustom.h"
#include "contextmanager.h"


PageInst::PageInst(QWidget *parent) :
    PageTable(PAGE_INST, parent),
    ui(new Ui::PageInst)
{
    ui->setupUi(this);

    _destIndex << champ_startAddrsOffset
               << champ_startAddrsCoarseOffset
               << champ_endAddrsOffset
               << champ_endAddrsCoarseOffset
               << champ_startloopAddrsOffset
               << champ_startloopAddrsCoarseOffset
               << champ_endloopAddrsOffset
               << champ_endloopAddrsCoarseOffset
               << champ_overridingRootKey
               << champ_fineTune
               << champ_coarseTune
               << champ_scaleTuning
               << champ_initialFilterFc
               << champ_initialFilterQ
               << champ_pan
               << champ_chorusEffectsSend
               << champ_reverbEffectsSend
               << champ_keynum
               << champ_exclusiveClass
               << champ_initialAttenuation
               << champ_delayVolEnv
               << champ_attackVolEnv
               << champ_holdVolEnv
               << champ_decayVolEnv
               << champ_sustainVolEnv
               << champ_releaseVolEnv
               << champ_keynumToVolEnvHold
               << champ_keynumToVolEnvDecay
               << champ_velocity
               << champ_sampleModes
               << champ_delayModEnv
               << champ_attackModEnv
               << champ_holdModEnv
               << champ_decayModEnv
               << champ_sustainModEnv
               << champ_releaseModEnv
               << champ_modEnvToPitch
               << champ_modEnvToFilterFc
               << champ_keynumToModEnvHold
               << champ_keynumToModEnvDecay
               << champ_delayModLFO
               << champ_freqModLFO
               << champ_modLfoToPitch
               << champ_modLfoToVolume
               << champ_modLfoToFilterFc
               << champ_delayVibLFO
               << champ_freqVibLFO
               << champ_vibLfoToPitch;

    // Style
    QString resetHoverColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT, ThemeManager::HOVERED).name();
    ui->frameBottom->setStyleSheet("QFrame{background-color:" +
                                   ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name() + ";color:" +
                                   ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() + "}" +
                                   "QPushButton{background-color:transparent;color:" +
                                   ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() +
                                   ";border:0;padding:0px 5px}" +
                                   "QPushButton:hover{color:" + resetHoverColor + "}");
    ui->tableInst->setStyleSheet("QTableWidget{border:1px solid " +
                                 this->palette().dark().color().name() +
                                 ";border-top:0;border-left:0;border-right:0}");

    this->contenant = elementInst;
    this->contenantGen = elementInstGen;
    this->contenantMod = elementInstMod;
    this->contenu = elementSmpl;
    this->lien = elementInstSmpl;
    this->lienGen = elementInstSmplGen;
    this->lienMod = elementInstSmplMod;
    this->_table = ui->tableInst;
    _rangeEditor = ui->rangeEditor;
    _envelopEditor = ui->envelopEditor;
    _modulatorEditor = ui->modulatorEditor;

    /*this->tableMod = ui->tableMod;
    this->spinAmount = ui->spinSource2;
    this->checkAbs = ui->checkAbs;
    this->pushNouveauMod = ui->pushNouveauMod;
    this->pushSupprimerMod = ui->pushSupprimerMod;
    this->comboSource1Courbe = ui->comboCourbeSource1;
    this->comboSource2Courbe = ui->comboCourbeSource2;
    this->comboSource1 = ui->comboSource1;
    this->comboSource2 = ui->comboSource2;
    this->comboDestination = ui->comboDestination;
    _pushCopyMod = ui->pushCopyMod;*/

    /* Remplissage de comboDestination
    for (int i = 0; i < 48; i++)
        this->comboDestination->addItem(Attribute::getDescription(this->getDestNumber(i), false));
    this->comboDestination->setLimite(48);

    // Remplissage des combosources
    this->remplirComboSource(this->comboSource1);
    this->remplirComboSource(this->comboSource2);

    // Initialisation menu de copie de modulateurs
    _menu = new QMenu();
    _menu->addAction("", this, SLOT(duplicateMod()));
    _menu->addAction("", this, SLOT(copyMod()));*/

    // Initialisation édition étendues, enveloppes
    ui->rangeEditor->init(_sf2);
    ui->envelopEditor->init(_sf2);

#ifdef Q_OS_MAC
    _table->setStyleSheet("QHeaderView::section:horizontal{padding: 4px 10px 4px 10px;}");
    QFont fontTmp = _table->font();
    fontTmp.setPixelSize(10);
    _table->setFont(fontTmp);
    ui->horizontalLayout_2->setSpacing(15);
#endif
    ui->tableInst->verticalHeader()->setDefaultSectionSize(QFontMetrics(ui->tableInst->font()).height() + 8);

    QFont font = ui->tableInst->font();
    font.setPixelSize(11);
    ui->tableInst->horizontalHeader()->setFont(font);
    ui->tableInst->verticalHeader()->setFont(font);

    connect(this->_table, SIGNAL(actionBegin()), this, SLOT(actionBegin()));
    connect(this->_table, SIGNAL(actionFinished()), this, SLOT(actionFinished()));
    connect(this->_table, SIGNAL(openElement(EltID)), this, SLOT(onOpenElement(EltID)));
    connect(ui->rangeEditor, SIGNAL(updateKeyboard()), this, SLOT(customizeKeyboard()));
    connect(ui->rangeEditor, SIGNAL(divisionsSelected(IdList)), this, SIGNAL(selectedIdsChanged(IdList)));
    connect(ui->widgetLinkedTo, SIGNAL(itemClicked(EltID)), this, SLOT(onLinkClicked(EltID)));
}

PageInst::~PageInst()
{
    delete ui;
}

QList<Page::DisplayOption> PageInst::getDisplayOptions(IdList selectedIds)
{
    return QList<DisplayOption>()
            << DisplayOption(1, ":/icons/table.svg", trUtf8("Table"))
            << DisplayOption(2, ":/icons/range.svg", trUtf8("Ranges"), selectedIds.isElementUnique(elementInst))
            << DisplayOption(3, ":/icons/adsr.svg", trUtf8("Envelopes"), selectedIds.isElementUnique(elementInst));
}

bool PageInst::updateInterface(QString editingSource, IdList selectedIds, int displayOption)
{
    if (selectedIds.empty())
        return false;

    // Check if the new parents are the same
    IdList parentIds = selectedIds.getSelectedIds(elementInst);
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

    // Update the selection
    _currentParentIds = parentIds;
    _currentIds = selectedIds;

    if (_currentParentIds.count() == 1)
    {
        // List of presets that use the instrument
        ui->widgetLinkedTo->initialize(_currentParentIds[0]);
        int nbPrst = ui->widgetLinkedTo->getLinkNumber();

        if (nbPrst == 0)
            ui->labelLinkedTo->setText(trUtf8("Instrument not linked to a preset yet."));
        else if (nbPrst == 1)
            ui->labelLinkedTo->setText(trUtf8("Instrument linked to preset:"));
        else
            ui->labelLinkedTo->setText(trUtf8("Instrument linked to presets:"));
    }
    else
    {
        ui->labelLinkedTo->setText("-");
        ui->widgetLinkedTo->clear();
    }

    switch (displayOption)
    {
    case 1:
        ui->stackedWidget->setCurrentIndex(0);
        this->afficheTable(justSelection);
        break;
    case 2:
        ui->stackedWidget->setCurrentIndex(1);
        this->afficheRanges(justSelection);
        break;
    case 3:
        ui->stackedWidget->setCurrentIndex(2);
        this->afficheEnvelops(justSelection);
        break;
    default:
        return false;
    }
    customizeKeyboard();

    return true;
}

// TableWidgetInst
TableWidgetInst::TableWidgetInst(QWidget *parent) : TableWidget(parent)
{
    _fieldList << champ_keyRange
               << champ_velRange
               << champ_initialAttenuation
               << champ_pan
               << champ_sampleModes
               << champ_overridingRootKey
               << champ_coarseTune
               << champ_fineTune
               << champ_scaleTuning
               << champ_initialFilterFc
               << champ_initialFilterQ
               << champ_delayVolEnv
               << champ_attackVolEnv
               << champ_holdVolEnv
               << champ_decayVolEnv
               << champ_sustainVolEnv
               << champ_releaseVolEnv
               << champ_keynumToVolEnvHold
               << champ_keynumToVolEnvDecay
               << champ_delayModEnv
               << champ_attackModEnv
               << champ_holdModEnv
               << champ_decayModEnv
               << champ_sustainModEnv
               << champ_releaseModEnv
               << champ_modEnvToPitch
               << champ_modEnvToFilterFc
               << champ_keynumToModEnvHold
               << champ_keynumToModEnvDecay
               << champ_delayModLFO
               << champ_freqModLFO
               << champ_modLfoToPitch
               << champ_modLfoToFilterFc
               << champ_modLfoToVolume
               << champ_delayVibLFO
               << champ_freqVibLFO
               << champ_vibLfoToPitch
               << champ_exclusiveClass
               << champ_chorusEffectsSend
               << champ_reverbEffectsSend
               << champ_keynum
               << champ_velocity
               << champ_startAddrsOffset
               << champ_endAddrsOffset
               << champ_startloopAddrsOffset
               << champ_endloopAddrsOffset;

    this->setRowCount(_fieldList.count() + 1);
    for (int i = 1; i < this->rowCount(); i++)
        this->setVerticalHeaderItem(i, new QTableWidgetItem(Attribute::getDescription(_fieldList[i - 1], false)));
}

TableWidgetInst::~TableWidgetInst() {}

int TableWidgetInst::getRow(quint16 champ)
{
    return _fieldList.indexOf((AttributeType)champ) + 1;
}

AttributeType TableWidgetInst::getChamp(int row)
{
    row--;
    if (row >= 0 && row < _fieldList.count())
        return _fieldList[row];
    return champ_unknown;
}

void PageInst::onLinkClicked(EltID id)
{
    emit(selectedIdsChanged(id));
}

void PageInst::keyPlayedInternal2(int key, int velocity)
{
    IdList ids = _currentIds.getSelectedIds(elementInst);
    if (ids.count() == 1)
    {
        ContextManager::audio()->getSynth()->play(1, ids[0].indexSf2, ids[0].indexElt, key, velocity);

        if (velocity > 0)
        {
            // Emphasize the related ranges
            EltID idInst = ids[0];
            idInst.typeElement = elementInst;
            rangesType defaultKeyRange, defaultVelRange;
            if (_sf2->isSet(idInst, champ_keyRange))
                defaultKeyRange = _sf2->get(idInst, champ_keyRange).rValue;
            else
            {
                defaultKeyRange.byLo = 0;
                defaultKeyRange.byHi = 127;
            }
            if (_sf2->isSet(idInst, champ_velRange))
                defaultVelRange = _sf2->get(idInst, champ_velRange).rValue;
            else
            {
                defaultVelRange.byLo = 0;
                defaultVelRange.byHi = 127;
            }

            EltID idInstSmpl = ids[0];
            idInstSmpl.typeElement = elementInstSmpl;
            foreach (int i, _sf2->getSiblings(idInstSmpl))
            {
                idInstSmpl.indexElt2 = i;
                int keyMin, keyMax, velMin, velMax;
                if (_sf2->isSet(idInstSmpl, champ_keyRange))
                {
                    rangesType rangeTmp = _sf2->get(idInstSmpl, champ_keyRange).rValue;
                    keyMin = rangeTmp.byLo;
                    keyMax = rangeTmp.byHi;
                }
                else
                {
                    keyMin = defaultKeyRange.byLo;
                    keyMax = defaultKeyRange.byHi;
                }
                if (_sf2->isSet(idInstSmpl, champ_velRange))
                {
                    rangesType rangeTmp = _sf2->get(idInstSmpl, champ_velRange).rValue;
                    velMin = rangeTmp.byLo;
                    velMax = rangeTmp.byHi;
                }
                else
                {
                    velMin = defaultVelRange.byLo;
                    velMax = defaultVelRange.byHi;
                }
                if (keyMin <= key && keyMax >= key && velMin <= velocity && velMax >= velocity)
                    ContextManager::midi()->keyboard()->addCurrentRange(key, keyMin, keyMax);
            }
        }
    }
}

int PageInst::getDestIndex(AttributeType type)
{
    return _destIndex.indexOf(type);
}

AttributeType PageInst::getDestNumber(int row)
{
    if (_destIndex.count() > row)
        return _destIndex[row];
    return champ_unknown;
}
