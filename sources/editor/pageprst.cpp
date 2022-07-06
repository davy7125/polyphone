/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2020 Davy Triponney                                **
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
#include "modulatorsplitter.h"

// Constructeur, destructeur
PagePrst::PagePrst(QWidget *parent) :
    PageTable(PAGE_PRST, parent),
    ui(new Ui::PagePrst)
{
    ui->setupUi(this);

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

    connect(this->_table, SIGNAL(actionBegin()), this, SLOT(actionBegin()));
    connect(this->_table, SIGNAL(actionFinished()), this, SLOT(actionFinished()));
    connect(this->_table, SIGNAL(openElement(EltID)), this, SLOT(onOpenElement(EltID)));
    connect(ui->rangeEditor, SIGNAL(updateKeyboard()), this, SLOT(customizeKeyboard()));
    connect(ui->rangeEditor, SIGNAL(divisionsSelected(IdList)), this, SIGNAL(selectedIdsChanged(IdList)));
    connect(ui->modulatorEditor, SIGNAL(attributesSelected(QList<AttributeType>)), this, SLOT(onModSelectionChanged(QList<AttributeType>)));

    // QSplitter for being able to resize the modulator area
    ModulatorSplitter * splitter = new ModulatorSplitter(ui->page, ui->tablePrst, ui->modulatorEditor, true);
    QVBoxLayout * layout = dynamic_cast<QVBoxLayout *>(ui->page->layout());
    layout->addWidget(splitter);
}

PagePrst::~PagePrst()
{
    delete ui;
}

QList<Page::DisplayOption> PagePrst::getDisplayOptions(IdList selectedIds)
{
    return QList<DisplayOption>()
            << DisplayOption(1, ":/icons/table.svg", tr("Table"))
            << DisplayOption(2, ":/icons/range.svg", tr("Ranges"), selectedIds.isElementUnique(elementPrst));
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

    // Show or hide the modulator section
    ui->modulatorEditor->setVisible(_currentParentIds.count() == 1);

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

    // Unit warning
    this->verticalHeaderItem(3)->setToolTip(tr("Values on this row are expressed in real dB.\nOther soundfont editors might display other units."));
    this->verticalHeaderItem(3)->setData(Qt::DecorationRole,
                                         ContextManager::theme()->getColoredSvg(":/icons/info.svg", QSize(12, 12),
                                                                                ThemeManager::HIGHLIGHTED_BACKGROUND));
}

TableWidgetPrst::~TableWidgetPrst() {}

int TableWidgetPrst::getRow(AttributeType champ)
{
    return _fieldList.indexOf(champ) + 1;
}

AttributeType TableWidgetPrst::getChamp(int row)
{
    row--;
    if (row >= 0 && row < _fieldList.count())
        return _fieldList[row];
    return champ_unknown;
}

void PagePrst::keyPlayedInternal2(int key, int velocity)
{
    IdList ids = _currentIds.getSelectedIds(elementPrst);
    if (ids.count() == 1)
        ContextManager::audio()->getSynth()->play(ids[0], -1, key, velocity);
}
