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

#include "pageprst.h"
#include "ui_pageprst.h"
#include "contextmanager.h"
#include <QMenu>

// Constructeur, destructeur
PagePrst::PagePrst(QWidget *parent) :
    PageTable(PAGE_PRST, parent),
    ui(new Ui::PagePrst)
{
    ui->setupUi(this);

    // Style
    ui->frameBottom->setStyleSheet("QFrame{background-color:" +
                                   ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name() + ";color:" +
                                   ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() + "}");
    ui->tablePrst->setStyleSheet("QTableWidget{border:1px solid " +
                                 this->palette().dark().color().name() +
                                 ";border-top:0;border-left:0;border-right:0}");

    this->contenant = elementPrst;
    this->contenantGen = elementPrstGen;
    this->contenantMod = elementPrstMod;
    this->contenu = elementInst;
    this->lien = elementPrstInst;
    this->lienGen = elementPrstInstGen;
    this->lienMod = elementPrstInstMod;
    this->_table = ui->tablePrst;
    _rangeEditor = ui->rangeEditor;
    _envelopEditor = nullptr;
    _modulatorEditor = ui->modulatorEditor;

    // Initialization of spinBoxes
    ui->spinBank->init(this);
    ui->spinPreset->init(this);

#ifdef Q_OS_MAC
    _table->setStyleSheet("QHeaderView::section:horizontal{padding: 4px 10px 4px 10px;}");
    QFont fontTmp = _table->font();
    fontTmp.setPixelSize(10);
    _table->setFont(fontTmp);
    ui->horizontalLayout_2->setSpacing(15);
#endif
    ui->tablePrst->verticalHeader()->setDefaultSectionSize(QFontMetrics(ui->tablePrst->font()).height() + 8);

    QFont font = ui->tablePrst->font();
    font.setPixelSize(11);
    ui->tablePrst->horizontalHeader()->setFont(font);
    ui->tablePrst->verticalHeader()->setFont(font);

    connect(this->_table, SIGNAL(actionBegin()), this, SLOT(actionBegin()));
    connect(this->_table, SIGNAL(actionFinished()), this, SLOT(actionFinished()));
    connect(this->_table, SIGNAL(openElement(EltID)), this, SLOT(onOpenElement(EltID)));
    connect(ui->rangeEditor, SIGNAL(updateKeyboard()), this, SLOT(customizeKeyboard()));
    connect(ui->rangeEditor, SIGNAL(divisionsSelected(IdList)), this, SIGNAL(selectedIdsChanged(IdList)));
    connect(ui->modulatorEditor, SIGNAL(attributesSelected(QList<AttributeType>)), this, SLOT(onModSelectionChanged(QList<AttributeType>)));
}

PagePrst::~PagePrst()
{
    delete ui;
}

QList<Page::DisplayOption> PagePrst::getDisplayOptions(IdList selectedIds)
{
    return QList<DisplayOption>()
            << DisplayOption(1, ":/icons/table.svg", trUtf8("Table"))
            << DisplayOption(2, ":/icons/range.svg", trUtf8("Ranges"), selectedIds.isElementUnique(elementPrst));
}

bool PagePrst::updateInterface(QString editingSource, IdList selectedIds, int displayOption)
{
    if (selectedIds.empty())
        return false;

    // Check if the new parents are the same
    IdList parentIds = selectedIds.getSelectedIds(elementPrst);
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

    _currentParentIds = parentIds;
    _currentIds = selectedIds;

    if (_currentParentIds.count() == 1)
    {
        ui->spinBank->setEnabled(true);
        ui->spinPreset->setEnabled(true);
        ui->modulatorEditor->show();
        EltID id = _currentParentIds.first();
        id.typeElement = elementPrst;
        ui->spinBank->setValue(_sf2->get(id, champ_wBank).wValue);
        ui->spinPreset->setValue(_sf2->get(id, champ_wPreset).wValue);
        ui->labelPercussion->setVisible(_sf2->get(id, champ_wBank).wValue == 128);
    }
    else
    {
        // Check if the bank is the same in the selection
        bool sameNumber = true;
        int tmp = -1;
        foreach (EltID id, _currentParentIds)
        {
            int number = _sf2->get(id, champ_wBank).wValue;
            if (tmp == -1)
                tmp = number;
            else if (tmp != number)
            {
                sameNumber = false;
                break;
            }
        }
        ui->spinBank->setEnabled(sameNumber);
        if (sameNumber)
            ui->spinBank->setValue(tmp);

        // Check if the preset is the same in the selection
        sameNumber = true;
        tmp = -1;
        foreach (EltID id, _currentParentIds)
        {
            int number = _sf2->get(id, champ_wPreset).wValue;
            if (tmp == -1)
                tmp = number;
            else if (tmp != number)
            {
                sameNumber = false;
                break;
            }
        }
        ui->spinPreset->setEnabled(sameNumber);
        if (sameNumber)
            ui->spinPreset->setValue(tmp);

        // Hide modulators
        ui->modulatorEditor->hide();

        // Hide the percussion label
        ui->labelPercussion->hide();
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
    default:
        return false;
    }
    customizeKeyboard();

    return true;
}

// TableWidgetPrst
TableWidgetPrst::TableWidgetPrst(QWidget *parent) : TableWidget(parent)
{
    _fieldList << champ_keyRange
               << champ_velRange
               << champ_initialAttenuation
               << champ_pan
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
               << champ_chorusEffectsSend
               << champ_reverbEffectsSend;

    this->setRowCount(_fieldList.count() + 1);
    for (int i = 1; i < this->rowCount(); i++)
        this->setVerticalHeaderItem(i, new QTableWidgetItem(Attribute::getDescription(_fieldList[i - 1], true)));
}

TableWidgetPrst::~TableWidgetPrst() {}

int TableWidgetPrst::getRow(quint16 champ)
{
    return _fieldList.indexOf((AttributeType)champ) + 1;
}

AttributeType TableWidgetPrst::getChamp(int row)
{
    row--;
    if (row >= 0 && row < _fieldList.count())
        return _fieldList[row];
    return champ_unknown;
}

void PagePrst::spinUpDown(int steps, SpinBox *spin)
{
    if (_preparingPage || _currentParentIds.empty() || steps == 0)
        return;

    _preparingPage = true;
    if (spin == ui->spinBank)
        this->setBank(ui->spinBank->value(), steps > 0 ? 1 : -1);
    else
        this->setPreset(ui->spinPreset->value(), steps > 0 ? 1 : -1);
    _preparingPage = false;
}

void PagePrst::setBank()
{
    if (_preparingPage)
        return;
    _preparingPage = true;
    this->setBank(ui->spinBank->value(), 0);
    _preparingPage = false;
}

void PagePrst::setPreset()
{
    if (_preparingPage)
        return;
    _preparingPage = true;
    this->setPreset(ui->spinPreset->value(), 0);
    _preparingPage = false;
}

void PagePrst::keyPlayedInternal2(int key, int velocity)
{
    IdList ids = _currentIds.getSelectedIds(elementPrst);
    if (ids.count() == 1)
        ContextManager::audio()->getSynth()->play(ids[0], key, velocity);
}

void PagePrst::setBank(quint16 desiredBank, int collisionResolution)
{
    // Previous bank
    if (_currentParentIds.empty())
        return;
    quint16 previousBank = _sf2->get(_currentParentIds[0], champ_wBank).wValue;

    // Find the bank to set, according to the collision resolution method
    while (!isBankAvailable(desiredBank))
    {
        bool goBack = false;
        if (collisionResolution == 0)
            goBack = true;
        else if (collisionResolution > 0)
        {
            if (desiredBank == 128) // Max is 128
                goBack = true;
            else
                desiredBank++;
        }
        else if (collisionResolution < 0)
        {
            if (desiredBank == 0)
                goBack = true;
            else
                desiredBank--;
        }

        if (goBack)
        {
            // Back to the previous bank
            ui->spinBank->setValue(previousBank);
            return;
        }
    }

    // A bank has been found, the current ids are edited
    AttributeValue v;
    v.wValue = desiredBank;
    foreach (EltID id, _currentParentIds)
        _sf2->set(id, champ_wBank, v);
    _sf2->endEditing(getEditingSource());

    // GUI update
    ui->spinBank->setValue(desiredBank);
    ui->labelPercussion->setVisible(ui->spinBank->value() == 128);
}

bool PagePrst::isBankAvailable(quint16 wBank)
{
    // Current sf2
    if (_currentParentIds.empty())
        return false;
    int indexSf2 = _currentParentIds[0].indexSf2;

    // Is it possible to change the bank of all current ids?
    QList<int> usePresets = getUsedPresetsForBank(indexSf2, wBank);
    foreach (EltID id, _currentParentIds)
        if (usePresets.contains(_sf2->get(id, champ_wPreset).wValue))
            return false;

    return true;
}

QList<int> PagePrst::getUsedPresetsForBank(int sf2Index, quint16 wBank)
{
    QList<int> ret;
    EltID id(elementPrst, sf2Index);
    foreach (int i, _sf2->getSiblings(id))
    {
        id.indexElt = i;
        if (_sf2->get(id, champ_wBank).wValue == wBank)
            ret << _sf2->get(id, champ_wPreset).wValue;
    }
    return ret;
}

void PagePrst::setPreset(quint16 desiredPreset, int collisionResolution)
{
    // Previous preset
    if (_currentParentIds.empty())
        return;
    quint16 previousPreset = _sf2->get(_currentParentIds[0], champ_wPreset).wValue;

    // Find the preset to set, according to the collision resolution method
    while (!isPresetAvailable(desiredPreset))
    {
        bool goBack = false;
        if (collisionResolution == 0)
            goBack = true;
        else if (collisionResolution > 0)
        {
            if (desiredPreset == 127) // Max is 127
                goBack = true;
            else
                desiredPreset++;
        }
        else if (collisionResolution < 0)
        {
            if (desiredPreset == 0)
                goBack = true;
            else
                desiredPreset--;
        }

        if (goBack)
        {
            // Back to the previous preset
            ui->spinPreset->setValue(previousPreset);
            return;
        }
    }

    // A preset has been found, the current ids are edited
    AttributeValue v;
    v.wValue = desiredPreset;
    foreach (EltID id, _currentParentIds)
        _sf2->set(id, champ_wPreset, v);
    _sf2->endEditing(getEditingSource());

    // GUI update
    ui->spinPreset->setValue(desiredPreset);
}

bool PagePrst::isPresetAvailable(quint16 wPreset)
{
    // Current sf2
    if (_currentParentIds.empty())
        return false;
    int indexSf2 = _currentParentIds[0].indexSf2;

    // Is it possible to change the preset of all current ids?
    QList<int> useBanks = getUsedBanksForPreset(indexSf2, wPreset);
    foreach (EltID id, _currentParentIds)
        if (useBanks.contains(_sf2->get(id, champ_wBank).wValue))
            return false;

    return true;
}

QList<int> PagePrst::getUsedBanksForPreset(int sf2Index, quint16 wPreset)
{
    QList<int> ret;
    EltID id(elementPrst, sf2Index);
    foreach (int i, _sf2->getSiblings(id))
    {
        id.indexElt = i;
        if (_sf2->get(id, champ_wPreset).wValue == wPreset)
            ret << _sf2->get(id, champ_wBank).wValue;
    }
    return ret;
}
