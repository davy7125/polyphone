/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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

#include "pageprst.h"
#include "ui_pageprst.h"
#include "contextmanager.h"
#include <QMenu>
#include "modulatorsplitter.h"

// Constructeur, destructeur
PagePrst::PagePrst(QWidget *parent) :
    PageTable(true, parent),
    ui(new Ui::PagePrst)
{
    ui->setupUi(this);

    _table = ui->tablePrst;
    _modulatorEditor = ui->modulatorEditor;

    connect(this->_table, SIGNAL(actionBegin()), this, SLOT(actionBegin()));
    connect(this->_table, SIGNAL(actionFinished()), this, SLOT(actionFinished()));
    connect(this->_table, SIGNAL(openElement(EltID)), this, SLOT(onOpenElement(EltID)));
    connect(ui->modulatorEditor, SIGNAL(attributesSelected(QList<AttributeType>)), this, SLOT(onModSelectionChanged(QList<AttributeType>)));

    // QSplitter for being able to resize the modulator area
    ModulatorSplitter * splitter = new ModulatorSplitter(this, ui->tablePrst, ui->modulatorEditor, true);
    QVBoxLayout * layout = dynamic_cast<QVBoxLayout *>(ui->verticalLayout);
    layout->addWidget(splitter);
}

PagePrst::~PagePrst()
{
    delete ui;
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

bool TableWidgetPrst::isInstrumentLevel()
{
    return false;
}
