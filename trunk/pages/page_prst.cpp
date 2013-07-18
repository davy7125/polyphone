/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013 Davy Triponney                                     **
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
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "page_prst.h"
#include "ui_page_prst.h"
#include "mainwindow.h"

// Constructeur, destructeur
Page_Prst::Page_Prst(QWidget *parent) :
    PageTable(PAGE_PRST, parent),  ui(new Ui::Page_Prst)
{
    ui->setupUi(this);
    this->preparation = 0;
    this->contenant = elementPrst;
    this->contenantGen = elementPrstGen;
    this->contenantMod = elementPrstMod;
    this->contenu = elementInst;
    this->lien = elementPrstInst;
    this->lienGen = elementPrstInstGen;
    this->lienMod = elementPrstInstMod;
    this->table = this->ui->tablePrst;
    connect(this->table, SIGNAL(set(int,int,bool)), this, SLOT(set(int,int,bool)));
    this->tableMod = this->ui->tableMod;
    this->spinAmount = this->ui->spinSource2;
    this->checkAbs = this->ui->checkAbs;
    this->pushNouveauMod = this->ui->pushNouveauMod;
    this->pushSupprimerMod = this->ui->pushSupprimerMod;
    this->comboSource1Courbe = this->ui->comboCourbeSource1;
    this->comboSource2Courbe = this->ui->comboCourbeSource2;
    this->comboSource1 = this->ui->comboSource1;
    this->comboSource2 = this->ui->comboSource2;
    this->comboDestination = this->ui->comboDestination;
    // Remplissage de comboDestination
    for (int i = 0; i < 35; i++)
        this->comboDestination->addItem(getGenName(this->getDestNumber(i)));
    this->comboDestination->setLimite(35);
    // Remplissage des combosources
    this->remplirComboSource(this->comboSource1);
    this->remplirComboSource(this->comboSource2);
    // Initialisation spinBoxes
    this->ui->spinBank->init(this);
    this->ui->spinPreset->init(this);
}
Page_Prst::~Page_Prst()
{
    delete ui;
}
void Page_Prst::setModVisible(bool visible)
{
    this->ui->dockWidget->setHidden(!visible);
}
void Page_Prst::afficher()
{
    PageTable::afficher();
    EltID id = this->tree->getID(0);
    id.typeElement = elementPrst;
    this->preparation = 1;
    this->ui->spinBank->setValue(this->sf2->get(id, champ_wBank).wValue);
    this->ui->spinPreset->setValue(this->sf2->get(id, champ_wPreset).wValue);
    this->preparation = 0;
}

// TableWidgetPrst
TableWidgetPrst::TableWidgetPrst(QWidget *parent) : TableWidget(parent)
{
}
TableWidgetPrst::~TableWidgetPrst()
{
}

int TableWidgetPrst::getRow(WORD champ)
{
    int row = -1;
    switch (champ)
    {
    case champ_keyRange: row = 4; break;
    case champ_velRange: row = 5; break;
    case champ_initialAttenuation: row = 6; break;
    case champ_pan: row = 7; break;
    case champ_coarseTune: row = 8; break;
    case champ_fineTune: row = 9; break;
    case champ_scaleTuning: row = 10; break;
    case champ_initialFilterFc: row = 11; break;
    case champ_initialFilterQ: row = 12; break;
    case champ_delayVolEnv: row = 13; break;
    case champ_attackVolEnv: row = 14; break;
    case champ_holdVolEnv: row = 15; break;
    case champ_decayVolEnv: row = 16; break;
    case champ_sustainVolEnv: row = 17; break;
    case champ_releaseVolEnv: row = 18; break;
    case champ_keynumToVolEnvHold: row = 19; break;
    case champ_keynumToVolEnvDecay: row = 20; break;
    case champ_delayModEnv: row = 21; break;
    case champ_attackModEnv: row = 22; break;
    case champ_holdModEnv: row = 23; break;
    case champ_decayModEnv: row = 24; break;
    case champ_sustainModEnv: row = 25; break;
    case champ_releaseModEnv: row = 26; break;
    case champ_modEnvToPitch: row = 27; break;
    case champ_modEnvToFilterFc: row = 28; break;
    case champ_keynumToModEnvHold: row = 29; break;
    case champ_keynumToModEnvDecay: row = 30; break;
    case champ_delayModLFO: row = 31; break;
    case champ_freqModLFO: row = 32; break;
    case champ_modLfoToPitch: row = 33; break;
    case champ_modLfoToFilterFc: row = 34; break;
    case champ_modLfoToVolume: row = 35; break;
    case champ_delayVibLFO: row = 36; break;
    case champ_freqVibLFO: row = 37; break;
    case champ_vibLfoToPitch: row = 38; break;
    case champ_chorusEffectsSend: row = 39; break;
    case champ_reverbEffectsSend: row = 40; break;
    }
    return row;
}
Champ TableWidgetPrst::getChamp(int row)
{
    Champ champ = champ_unknown;
    switch (row)
    {
    case 4: champ = champ_keyRange; break;
    case 5: champ = champ_velRange; break;
    case 6: champ = champ_initialAttenuation; break;
    case 7: champ = champ_pan; break;
    case 8: champ = champ_coarseTune; break;
    case 9: champ = champ_fineTune; break;
    case 10: champ = champ_scaleTuning; break;
    case 11: champ = champ_initialFilterFc; break;
    case 12: champ = champ_initialFilterQ; break;
    case 13: champ = champ_delayVolEnv; break;
    case 14: champ = champ_attackVolEnv; break;
    case 15: champ = champ_holdVolEnv; break;
    case 16: champ = champ_decayVolEnv; break;
    case 17: champ = champ_sustainVolEnv; break;
    case 18: champ = champ_releaseVolEnv; break;
    case 19: champ = champ_keynumToVolEnvHold; break;
    case 20: champ = champ_keynumToVolEnvDecay; break;
    case 21: champ = champ_delayModEnv; break;
    case 22: champ = champ_attackModEnv; break;
    case 23: champ = champ_holdModEnv; break;
    case 24: champ = champ_decayModEnv; break;
    case 25: champ = champ_sustainModEnv; break;
    case 26: champ = champ_releaseModEnv; break;
    case 27: champ = champ_modEnvToPitch; break;
    case 28: champ = champ_modEnvToFilterFc; break;
    case 29: champ = champ_keynumToModEnvHold; break;
    case 30: champ = champ_keynumToModEnvDecay; break;
    case 31: champ = champ_delayModLFO; break;
    case 32: champ = champ_freqModLFO; break;
    case 33: champ = champ_modLfoToPitch; break;
    case 34: champ = champ_modLfoToFilterFc; break;
    case 35: champ = champ_modLfoToVolume; break;
    case 36: champ = champ_delayVibLFO; break;
    case 37: champ = champ_freqVibLFO; break;
    case 38: champ = champ_vibLfoToPitch; break;
    case 39: champ = champ_chorusEffectsSend; break;
    case 40: champ = champ_reverbEffectsSend; break;
    default: champ = champ_unknown;
    }
    return champ;
}

void Page_Prst::spinUpDown(int steps, SpinBox *spin)
{
    EltID id = this->tree->getID(0);
    id.typeElement = elementPrst;
    int increment;
    if (steps > 0)
        increment = 1;
    else if (steps < 0)
        increment = -1;
    else return;
    int nbIncrement = 1;
    bool valPossible, bTmp;
    int wPreset = this->ui->spinPreset->value();
    int wBank = this->ui->spinBank->value();
    int valInit = spin->value();
    do
    {
        if (spin == this->ui->spinBank)
        {
            // le numéro de banque est modifié, numéro de preset reste fixe
            wBank = valInit + nbIncrement * increment;
            bTmp = (this->sf2->get(id, champ_wBank).wValue == wBank);
        }
        else
        {
            // le numéro de preset est modifié, numéro de banque reste fixe
            wPreset = valInit + nbIncrement * increment;
            bTmp = (this->sf2->get(id, champ_wPreset).wValue == wPreset);
        }
        valPossible = isAvailable(id, wBank, wPreset) || bTmp;
        nbIncrement++;
    } while (!valPossible && valInit + nbIncrement * increment < 128 \
             && valInit + nbIncrement * increment > -1);
    valInit += (nbIncrement-1) * increment;
    // modification de la valeur
    if (valPossible) spin->setValue(valInit);
}
void Page_Prst::setBank()
{
    if (this->preparation) return;
    this->preparation = 1;
    // Comparaison avec valeur précédente
    EltID id = this->tree->getID(0);
    id.typeElement = elementPrst;
    int initVal = this->ui->spinBank->value();
    if (this->sf2->get(id, champ_wBank).wValue != initVal)
    {
        // Valeur possible ?
        int nBank = this->sf2->get(id, champ_wBank).wValue;
        int nPreset = this->sf2->get(id, champ_wPreset).wValue;
        int delta = 0;
        int sens = 0;
        do
        {
            if (initVal + delta < 128)
            {
                if (initVal + delta == nBank)
                    sens = 2;
                else if (isAvailable(id, initVal + delta, nPreset))
                    sens = 1;
            }
            if (initVal - delta > -1 && sens == 0)
            {
                if (initVal - delta == nBank)
                    sens = -2;
                else if (isAvailable(id, initVal - delta, nPreset))
                    sens = -1;
            }
            delta++;
        } while (sens == 0 && delta < 128);
        if (sens == 1 || sens == -1)
        {
            initVal += sens * (delta-1);
            this->ui->spinBank->setValue(initVal);
            // Sauvegarde
            Valeur val;
            val.wValue = initVal;
            this->sf2->prepareNewActions();
            // Reprise de l'identificateur si modification
            id = this->tree->getID(0);
            id.typeElement = elementPrst;
            this->sf2->set(id, champ_wBank, val);
            this->mainWindow->updateDo();
        }
        else
        {
            // restauration de la valeur précédente
            this->ui->spinBank->setValue(nBank);
        }
    }
    this->preparation = 0;
}
void Page_Prst::setPreset()
{
    if (this->preparation) return;
    this->preparation = 1;
    // Comparaison avec valeur précédente
    EltID id = this->tree->getID(0);
    id.typeElement = elementPrst;
    int initVal = this->ui->spinPreset->value();
    if (this->sf2->get(id, champ_wPreset).wValue != initVal)
    {
        // Valeur possible ?
        initVal = closestAvailablePreset(id, this->sf2->get(id, champ_wBank).wValue, initVal);
        int nPreset = this->sf2->get(id, champ_wPreset).wValue;
        if (initVal >= 0 && initVal != nPreset)
        {
            this->ui->spinPreset->setValue(initVal);
            // Sauvegarde
            Valeur val;
            val.wValue = initVal;
            this->sf2->prepareNewActions();
            // Reprise de l'identificateur si modification
            id = this->tree->getID(0);
            id.typeElement = elementPrst;
            this->sf2->set(id, champ_wPreset, val);
            this->mainWindow->updateDo();
        }
        else
        {
            // restauration de la valeur précédente
            this->ui->spinPreset->setValue(nPreset);
        }
    }
    this->preparation = 0;
}

void Page_Prst::firstAvailablePresetBank(EltID id, int &nBank, int &nPreset)
{
    nBank = -2;
    nPreset = -1;
    int nBankParcours = 0;
    int nPresetParcours = 0;
    do
    {
        if (isAvailable(id, nBankParcours, nPresetParcours))
        {
            nBank = nBankParcours;
            nPreset = nPresetParcours;
        }
        else
        {
            nPresetParcours++;
            if (nPresetParcours > 127)
            {
                nPresetParcours = 0;
                nBankParcours++;
                if (nBankParcours > 127)
                    nBank = -1;
            }
        }
    }
    while (nBank == -2);
}
int Page_Prst::closestAvailablePreset(EltID id, WORD wBank, WORD wPreset)
{
    int initVal = wPreset;
    int delta = 0;
    int sens = 0;
    do
    {
        if (initVal + delta < 128)
        {
            if (isAvailable(id, wBank, initVal + delta))
                sens = 1;
        }
        if (initVal - delta > -1 && sens == 0)
        {
            if (isAvailable(id, wBank, initVal - delta))
                sens = -1;
        }
        delta++;
    } while (sens == 0 && delta < 128);
    if (sens == 1 || sens == -1)
        return initVal + sens * (delta-1);
    else
        return -1;
}
bool Page_Prst::isAvailable(EltID id, WORD wBank, WORD wPreset)
{
    id.typeElement = elementPrst;
    for (int i = 0; i < sf2->count(id); i++)
    {
        id.indexElt = i;
        if (!sf2->get(id, champ_hidden).bValue)
        {
            if (sf2->get(id, champ_wBank).wValue == wBank && \
                    sf2->get(id, champ_wPreset).wValue == wPreset)
                return false;
        }
    }
    return true;
}
