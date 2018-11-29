/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2018 Davy Triponney                                **
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

    _destIndex << champ_fineTune
               << champ_coarseTune
               << champ_scaleTuning
               << champ_initialFilterFc
               << champ_initialFilterQ
               << champ_pan
               << champ_chorusEffectsSend
               << champ_reverbEffectsSend
               << champ_initialAttenuation
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
               << champ_modLfoToVolume
               << champ_modLfoToFilterFc
               << champ_delayVibLFO
               << champ_freqVibLFO
               << champ_vibLfoToPitch;

    // Style
    ui->frameBottom->setStyleSheet("QFrame{background-color:" +
                                   ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name() + ";color:" +
                                   ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() + "}");
    ui->tablePrst->setStyleSheet("QTableWidget{border:1px solid " +
                                 this->palette().dark().color().name() +
                                 ";border-top:0;border-left:0;border-right:0}");
    ui->pushCopyMod->setIcon(ContextManager::theme()->getColoredSvg(":/icons/copy.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushPasteMod->setIcon(ContextManager::theme()->getColoredSvg(":/icons/paste.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushNouveauMod->setIcon(ContextManager::theme()->getColoredSvg(":/icons/document-new.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushSupprimerMod->setIcon(ContextManager::theme()->getColoredSvg(":/icons/minus.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));

    this->contenant = elementPrst;
    this->contenantGen = elementPrstGen;
    this->contenantMod = elementPrstMod;
    this->contenu = elementInst;
    this->lien = elementPrstInst;
    this->lienGen = elementPrstInstGen;
    this->lienMod = elementPrstInstMod;
    this->_table = ui->tablePrst;
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
    _envelopEditor = NULL;

    // Remplissage de comboDestination
    for (int i = 0; i < 35; i++)
        this->comboDestination->addItem(Attribute::getDescription(this->getDestNumber(i), true));
    this->comboDestination->setLimite(35);

    // Remplissage des combosources
    this->remplirComboSource(this->comboSource1);
    this->remplirComboSource(this->comboSource2);

    // Initialisation spinBoxes
    ui->spinBank->init(this);
    ui->spinPreset->init(this);

    // Initialisation menu de copie de modulateurs
    _menu = new QMenu();
    _menu->addAction("", this, SLOT(duplicateMod()));
    _menu->addAction("", this, SLOT(copyMod()));

    // Initialisation édition étendues
    ui->rangeEditor->init(_sf2);

#ifdef Q_OS_MAC
    this->table->setStyleSheet("QHeaderView::section:horizontal{padding: 4px 10px 4px 10px;}");
    QFont font = this->table->font();
    font.setPixelSize(10);
    this->table->setFont(font);
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

void PagePrst::setModVisible(bool visible)
{
    ui->frameModulator->setVisible(visible);
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
        ui->frameBottom->setEnabled(true);
        ui->frameModulator->setEnabled(true);
        EltID id = _currentParentIds.first();
        id.typeElement = elementPrst;
        ui->spinBank->setValue(_sf2->get(id, champ_wBank).wValue);
        ui->spinPreset->setValue(_sf2->get(id, champ_wPreset).wValue);
        ui->labelPercussion->setVisible(_sf2->get(id, champ_wBank).wValue == 128);
    }
    else
    {
        ui->frameBottom->setEnabled(false);
        ui->frameModulator->setEnabled(false);
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
    EltID id = _currentIds[0];
    id.typeElement = elementPrst;
    int increment;
    if (steps > 0)
        increment = 1;
    else if (steps < 0)
        increment = -1;
    else return;
    int nbIncrement = 1;
    bool valPossible, bTmp;
    int wPreset = ui->spinPreset->value();
    int wBank = ui->spinBank->value();
    int valInit = spin->value();
    do
    {
        if (spin == ui->spinBank)
        {
            // le numéro de banque est modifié, numéro de preset reste fixe
            wBank = valInit + nbIncrement * increment;
            bTmp = (_sf2->get(id, champ_wBank).wValue == wBank);
        }
        else
        {
            // le numéro de preset est modifié, numéro de banque reste fixe
            wPreset = valInit + nbIncrement * increment;
            bTmp = (_sf2->get(id, champ_wPreset).wValue == wPreset);
        }
        valPossible = _sf2->isAvailable(id, wBank, wPreset) || bTmp;
        nbIncrement++;
    } while (!valPossible && valInit + nbIncrement * increment < 128 \
             && valInit + nbIncrement * increment > -1);
    valInit += (nbIncrement-1) * increment;

    // modification de la valeur
    if (valPossible) spin->setValue(valInit);
}

void PagePrst::setBank()
{
    if (_preparingPage)
        return;
    _preparingPage = true;

    // Compare with the previous value
    EltID id = _currentIds[0];
    id.typeElement = elementPrst;
    int initVal = ui->spinBank->value();
    if (_sf2->get(id, champ_wBank).wValue != initVal)
    {
        // Possible value?
        int nBank = _sf2->get(id, champ_wBank).wValue;
        int nPreset = _sf2->get(id, champ_wPreset).wValue;
        int delta = 0;
        int sens = 0;
        do
        {
            if (initVal + delta < 129)
            {
                if (initVal + delta == nBank)
                    sens = 2;
                else if (_sf2->isAvailable(id, initVal + delta, nPreset))
                    sens = 1;
            }
            if (initVal - delta > -1 && sens == 0)
            {
                if (initVal - delta == nBank)
                    sens = -2;
                else if (_sf2->isAvailable(id, initVal - delta, nPreset))
                    sens = -1;
            }
            delta++;
        } while (sens == 0 && delta < 129);
        if (sens == 1 || sens == -1)
        {
            initVal += sens * (delta-1);
            ui->spinBank->setValue(initVal);

            // Save the new value
            AttributeValue val;
            val.wValue = initVal;
            _sf2->set(id, champ_wBank, val);
            _sf2->endEditing(getEditingSource());
        }
        else
        {
            // Restore the previous value
            ui->spinBank->setValue(nBank);
        }
    }
    _preparingPage = 0;
    ui->labelPercussion->setVisible(ui->spinBank->value() == 128);
}

void PagePrst::setPreset()
{
    if (_preparingPage)
        return;
    _preparingPage = true;

    // Compare with the previous value
    EltID id = _currentIds[0];
    id.typeElement = elementPrst;
    int initVal = ui->spinPreset->value();
    if (_sf2->get(id, champ_wPreset).wValue != initVal)
    {
        // Possible value?
        initVal = _sf2->closestAvailablePreset(id, _sf2->get(id, champ_wBank).wValue, initVal);
        int nPreset = _sf2->get(id, champ_wPreset).wValue;
        if (initVal >= 0 && initVal != nPreset)
        {
            ui->spinPreset->setValue(initVal);

            // Save the new value
            AttributeValue val;
            val.wValue = initVal;
            _sf2->set(id, champ_wPreset, val);
            _sf2->endEditing(getEditingSource());
        }
        else
        {
            // Restore the previous value
            ui->spinPreset->setValue(nPreset);
        }
    }
    _preparingPage = 0;
}

void PagePrst::keyPlayedInternal2(int key, int velocity)
{
    IdList ids = _currentIds.getSelectedIds(elementPrst);
    if (ids.count() == 1)
        ContextManager::audio()->getSynth()->play(2, ids[0].indexSf2, ids[0].indexElt, key, velocity);
}


int PagePrst::getDestIndex(AttributeType type)
{
    return _destIndex.indexOf(type);
}

AttributeType PagePrst::getDestNumber(int row)
{
    if (_destIndex.count() > row)
        return _destIndex[row];
    return champ_unknown;
}
