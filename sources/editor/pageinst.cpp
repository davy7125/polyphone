/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2017 Davy Triponney                                **
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
    ui->pushCopyMod->setIcon(ContextManager::theme()->getColoredSvg(":/icons/copy.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushPasteMod->setIcon(ContextManager::theme()->getColoredSvg(":/icons/paste.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushNouveauMod->setIcon(ContextManager::theme()->getColoredSvg(":/icons/document-new.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushSupprimerMod->setIcon(ContextManager::theme()->getColoredSvg(":/icons/minus.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));

    this->contenant = elementInst;
    this->contenantGen = elementInstGen;
    this->contenantMod = elementInstMod;
    this->contenu = elementSmpl;
    this->lien = elementInstSmpl;
    this->lienGen = elementInstSmplGen;
    this->lienMod = elementInstSmplMod;
    this->_table = ui->tableInst;
    this->tableMod = ui->tableMod;
    this->spinAmount = ui->spinSource2;
    this->checkAbs = ui->checkAbs;
    this->pushNouveauMod = ui->pushNouveauMod;
    this->pushSupprimerMod = ui->pushSupprimerMod;
    this->comboSource1Courbe = ui->comboCourbeSource1;
    this->comboSource2Courbe = ui->comboCourbeSource2;
    this->comboSource1 = ui->comboSource1;
    this->comboSource2 = ui->comboSource2;
    this->comboDestination = ui->comboDestination;
    _pushCopyMod = ui->pushCopyMod;
    _rangeEditor = ui->rangeEditor;
    _envelopEditor = ui->envelopEditor;

    // Remplissage de comboDestination
    for (int i = 0; i < 48; i++)
        this->comboDestination->addItem(Attribute::getDescription(this->getDestNumber(i), false));
    this->comboDestination->setLimite(48);

    // Remplissage des combosources
    this->remplirComboSource(this->comboSource1);
    this->remplirComboSource(this->comboSource2);

    // Initialisation menu de copie de modulateurs
    _menu = new QMenu();
    _menu->addAction("", this, SLOT(duplicateMod()));
    _menu->addAction("", this, SLOT(copyMod()));

    // Initialisation édition étendues, enveloppes
    ui->rangeEditor->init(_sf2);
    ui->envelopEditor->init(_sf2);

#ifdef Q_OS_MAC
    this->table->setStyleSheet("QHeaderView::section:horizontal{padding: 4px 10px 4px 10px;}");
    QFont font = this->table->font();
    font.setPixelSize(10);
    this->table->setFont(font);
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
            << DisplayOption(2, ":/icons/range.svg", trUtf8("Étendues"), selectedIds.isElementUnique(elementInst))
            << DisplayOption(3, ":/icons/adsr.svg", trUtf8("Enveloppes"), selectedIds.isElementUnique(elementInst));
}

void PageInst::setModVisible(bool visible)
{
    ui->frameModulator->setVisible(visible);
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
            ui->labelLinkedTo->setText(trUtf8("Instrument lié à aucun preset."));
        else if (nbPrst == 1)
            ui->labelLinkedTo->setText(trUtf8("Instrument lié au preset :"));
        else
            ui->labelLinkedTo->setText(trUtf8("Instrument lié aux presets :"));
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
TableWidgetInst::TableWidgetInst(QWidget *parent) : TableWidget(parent) {}

TableWidgetInst::~TableWidgetInst() {}

int TableWidgetInst::getRow(quint16 champ)
{
    int row = -1;
    switch (champ)
    {
    case champ_keyRange: row = 0; break;
    case champ_velRange: row = 1; break;
    case champ_initialAttenuation: row = 2; break;
    case champ_pan: row = 3; break;
    case champ_sampleModes: row = 4; break;
    case champ_overridingRootKey: row = 5; break;
    case champ_coarseTune: row = 6; break;
    case champ_fineTune: row = 7; break;
    case champ_scaleTuning: row = 8; break;
    case champ_initialFilterFc: row = 9; break;
    case champ_initialFilterQ: row = 10; break;
    case champ_delayVolEnv: row = 11; break;
    case champ_attackVolEnv: row = 12; break;
    case champ_holdVolEnv: row = 13; break;
    case champ_decayVolEnv: row = 14; break;
    case champ_sustainVolEnv: row = 15; break;
    case champ_releaseVolEnv: row = 16; break;
    case champ_keynumToVolEnvHold: row = 17; break;
    case champ_keynumToVolEnvDecay: row = 18; break;
    case champ_delayModEnv: row = 19; break;
    case champ_attackModEnv: row = 20; break;
    case champ_holdModEnv: row = 21; break;
    case champ_decayModEnv: row = 22; break;
    case champ_sustainModEnv: row = 23; break;
    case champ_releaseModEnv: row = 24; break;
    case champ_modEnvToPitch: row = 25; break;
    case champ_modEnvToFilterFc: row = 26; break;
    case champ_keynumToModEnvHold: row = 27; break;
    case champ_keynumToModEnvDecay: row = 28; break;
    case champ_delayModLFO: row = 29; break;
    case champ_freqModLFO: row = 30; break;
    case champ_modLfoToPitch: row = 31; break;
    case champ_modLfoToFilterFc: row = 32; break;
    case champ_modLfoToVolume: row = 33; break;
    case champ_delayVibLFO: row = 34; break;
    case champ_freqVibLFO: row = 35; break;
    case champ_vibLfoToPitch: row = 36; break;
    case champ_exclusiveClass: row = 37; break;
    case champ_chorusEffectsSend: row = 38; break;
    case champ_reverbEffectsSend: row = 39; break;
    case champ_keynum: row = 40; break;
    case champ_velocity: row = 41; break;
    case champ_startAddrsOffset: row = 42; break;
    case champ_endAddrsOffset: row = 43; break;
    case champ_startloopAddrsOffset: row = 44; break;
    case champ_endloopAddrsOffset: row = 45; break;
    }
    return row + 1;
}

AttributeType TableWidgetInst::getChamp(int row)
{
    AttributeType champ = champ_unknown;
    switch (row - 1)
    {
    case 0: champ = champ_keyRange; break;
    case 1: champ = champ_velRange; break;
    case 2: champ = champ_initialAttenuation; break;
    case 3: champ = champ_pan; break;
    case 4: champ = champ_sampleModes; break;
    case 5: champ = champ_overridingRootKey; break;
    case 6: champ = champ_coarseTune; break;
    case 7: champ = champ_fineTune; break;
    case 8: champ = champ_scaleTuning; break;
    case 9: champ = champ_initialFilterFc; break;
    case 10: champ = champ_initialFilterQ; break;
    case 11: champ = champ_delayVolEnv; break;
    case 12: champ = champ_attackVolEnv; break;
    case 13: champ = champ_holdVolEnv; break;
    case 14: champ = champ_decayVolEnv; break;
    case 15: champ = champ_sustainVolEnv; break;
    case 16: champ = champ_releaseVolEnv; break;
    case 17: champ = champ_keynumToVolEnvHold; break;
    case 18: champ = champ_keynumToVolEnvDecay; break;
    case 19: champ = champ_delayModEnv; break;
    case 20: champ = champ_attackModEnv; break;
    case 21: champ = champ_holdModEnv; break;
    case 22: champ = champ_decayModEnv; break;
    case 23: champ = champ_sustainModEnv; break;
    case 24: champ = champ_releaseModEnv; break;
    case 25: champ = champ_modEnvToPitch; break;
    case 26: champ = champ_modEnvToFilterFc; break;
    case 27: champ = champ_keynumToModEnvHold; break;
    case 28: champ = champ_keynumToModEnvDecay; break;
    case 29: champ = champ_delayModLFO; break;
    case 30: champ = champ_freqModLFO; break;
    case 31: champ = champ_modLfoToPitch; break;
    case 32: champ = champ_modLfoToFilterFc; break;
    case 33: champ = champ_modLfoToVolume; break;
    case 34: champ = champ_delayVibLFO; break;
    case 35: champ = champ_freqVibLFO; break;
    case 36: champ = champ_vibLfoToPitch; break;
    case 37: champ = champ_exclusiveClass; break;
    case 38: champ = champ_chorusEffectsSend; break;
    case 39: champ = champ_reverbEffectsSend; break;
    case 40: champ = champ_keynum; break;
    case 41: champ = champ_velocity; break;
    case 42: champ = champ_startAddrsOffset; break;
    case 43: champ = champ_endAddrsOffset; break;
    case 44: champ = champ_startloopAddrsOffset; break;
    case 45: champ = champ_endloopAddrsOffset; break;
    default: champ = champ_unknown;
    }
    return champ;
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
