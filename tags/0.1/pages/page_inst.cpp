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
****************************************************************************/

#include "page_inst.h"
#include "mainwindow.h"
#include "ui_page_inst.h"
#include "dialog_space.h"
#include "dialog_paramglobal.h"
#include "dialog_mixture.h"

// Constructeur, destructeur
Page_Inst::Page_Inst(QWidget *parent) :
    PageTable(parent), ui(new Ui::Page_Inst)
{
    ui->setupUi(this);
    this->preparation = 0;
    this->contenant = elementInst;
    this->contenantGen = elementInstGen;
    this->contenantMod = elementInstMod;
    this->contenu = elementSmpl;
    this->lien = elementInstSmpl;
    this->lienGen = elementInstSmplGen;
    this->lienMod = elementInstSmplMod;
    this->table = this->ui->tableInst;
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
    for (int i = 0; i < 48; i++)
        this->comboDestination->addItem(getGenName(this->getDestNumber(i)));
    this->comboDestination->setLimite(48);
    // Remplissage des combosources
    this->remplirComboSource(this->comboSource1);
    this->remplirComboSource(this->comboSource2);
}
Page_Inst::~Page_Inst()
{
    delete ui;
}
void Page_Inst::setModVisible(bool visible)
{
    this->ui->dockWidget->setHidden(!visible);
}
void Page_Inst::afficher()
{
    PageTable::afficher();
    EltID id = this->tree->getID(0);
    id.typeElement = elementInst;
    // Liste des presets qui utilisent l'instrument
    int nbPrst = 0;
    bool isFound;
    int j;
    QString qStr = "";
    EltID id2 = id;
    id2.typeElement = elementPrst;
    EltID id3 = id;
    id3.typeElement = elementPrstInst;
    // Parcours de tous les presets
    for (int i = 0; i < this->sf2->count(id2); i++)
    {
        id2.indexElt = i;
        id3.indexElt = i;
        // Parcours de tous les instruments liés au preset
        isFound = false;
        j = 0;
        while(j < this->sf2->count(id3) && !isFound)
        {
            id3.indexElt2 = j;
            if (!this->sf2->get(id3, champ_hidden).bValue)
            {
                if (this->sf2->get(id3, champ_instrument).wValue == id.indexElt)
                {
                    // Ajout d'un preset
                    if (nbPrst)
                        qStr.append(", ");
                    qStr.append(this->sf2->getQstr(id2, champ_name).toStdString().c_str());
                    nbPrst++;
                    isFound = true;
                }
            }
            j++;
        }
    }
    if (nbPrst == 0)
        qStr = QString::fromUtf8(tr("<b>Instrument lié à aucun preset.</b>").toStdString().c_str());
    else if (nbPrst == 1)
        qStr.prepend(QString::fromUtf8(tr("<b>Instrument lié au preset : </b>").toStdString().c_str()));
    else
        qStr.prepend(QString::fromUtf8(tr("<b>Instrument lié aux presets : </b>").toStdString().c_str()));
    this->ui->labelPrst->setText(qStr);
}

// Outils instrument
void Page_Inst::desaccorder()
{
    EltID id = this->tree->getID(0);
    id.typeElement = elementInstSmpl;
    if (this->sf2->count(id) == 0)
    {
        QMessageBox::warning(NULL, tr("Attention"), tr("L'instrument doit contenir des sons."));
        return;
    }
    // Désaccordage ondulant
    // Nombre de battements par secondes
    bool ok;
    double rep = QInputDialog::getDouble(this, tr("Question"),
        QString::fromUtf8(tr("Nombre de battements par seconde :\n(le signe définit le sens du désaccordage)").toStdString().c_str()),
        4, -10, 10, 1, &ok);
    if (!ok || rep == 0) return;
    this->sf2->prepareNewActions();
    // Reprise de l'identificateur si modification
    id = this->tree->getID(0);
    // Modification pour chaque sample lié
    id.typeElement = elementInstSmpl;
    for (int i = 0; i < this->sf2->count(id); i++)
    {
        id.indexElt2 = i;
        if (!this->sf2->get(id, champ_hidden).bValue)
            this->desaccorder(id, rep);
    }
    // Actualisation
    this->mainWindow->updateDo();
    this->afficher();
}
void Page_Inst::desaccorder(EltID id, double bps)
{
    // Sample lié
    EltID id2 = id;
    id2.typeElement = elementSmpl;
    id2.indexElt = this->sf2->get(id, champ_sampleID).genValue.wAmount;
    // Numero de la note du sample
    int numNoteSmpl = this->sf2->get(id2, champ_byOriginalPitch).wValue;
    // Note de base sur le clavier correspondant à numNoteSmpl;
    int numBase = numNoteSmpl;
    if (this->sf2->isSet(id, champ_overridingRootKey))
        numBase = this->sf2->get(id, champ_overridingRootKey).wValue;
    // Etendue du sample sur le clavier
    int numBas = 0;
    int numHaut = 0;
    // Intervalle joué par le sample
    if (this->sf2->isSet(id, champ_keynum))
    {
        numBas = numNoteSmpl + this->sf2->get(id, champ_keynum).wValue - numBase;
        numHaut = numBas;
    }
    else
    {
        numBas = numNoteSmpl + this->sf2->get(id, champ_keyRange).rValue.byLo - numBase;
        numHaut = numNoteSmpl + this->sf2->get(id, champ_keyRange).rValue.byHi - numBase;
    }
    // Note moyenne
    double noteMoy = (double)(numBas + numHaut) / 2;
    // Calcul du désaccordage, passage en frequence
    double freqMoy = exp2((noteMoy + 36.3763) / 12);
    // Ajout du désaccordage
    // - octave ondulante : division par 2 de bps
    // - diminution désaccordage vers les graves
    // - accentuation désaccordage vers les aigus
    double freqMod = freqMoy + 1.2*exp2((noteMoy - 60)/30) * bps / 2;
    // Retour en pitch
    double noteMod = 12*log2(freqMod)-36.3763;
    // Décalage
    int decalage = ceil(100*(noteMod - noteMoy)-0.5);
    if (bps > 0)
    {
        if (decalage < 1) decalage = 1;
        else if (decalage > 50) decalage = 50;
    }
    else
    {
        if (decalage < -50) decalage = -50;
        else if (decalage > -1) decalage = -1;
    }
    // Modification instSmpl
    if (this->sf2->get(id, champ_fineTune).shValue != decalage)
    {
        Valeur val;
        val.shValue = decalage;
        this->sf2->set(id, champ_fineTune, val);
    }
}
void Page_Inst::duplication()
{
    EltID id = this->tree->getID(0);
    id.typeElement = elementInstSmpl;
    if (this->sf2->count(id) == 0)
    {
        QMessageBox::warning(NULL, tr("Attention"), tr("L'instrument doit contenir des sons."));
        return;
    }
    // Obtention d'une division par note
    this->sf2->prepareNewActions();
    // Reprise de l'identificateur si modification
    id = this->tree->getID(0);
    // Modification pour chaque sample lié
    EltID id2 = id;
    id2.typeElement = elementInstSmpl;
    int nbInstSmpl = this->sf2->count(id2);
    for (int i = 0; i < nbInstSmpl; i++)
    {
        id2.indexElt2 = i;
        if (!this->sf2->get(id2, champ_hidden).bValue)
            this->duplication(id2);
    }
    // Actualisation
    this->mainWindow->updateDo();
    this->afficher();
}
void Page_Inst::duplication(EltID id)
{
    // Etendue du sample sur le clavier
    int numBas = this->sf2->get(id, champ_keyRange).rValue.byLo;
    int numHaut = this->sf2->get(id, champ_keyRange).rValue.byHi;
    if (numBas != numHaut)
    {
        // Modification keyrange
        Valeur val;
        val.rValue.byLo = numBas;
        val.rValue.byHi = numBas;
        this->sf2->set(id, champ_keyRange, val);
        // Création de divisions identiques pour les autres notes
        EltID id2 = id;
        for (int i = numBas+1; i <= numHaut; i++)
        {
            id.typeElement = elementInstSmpl;
            id2.typeElement = elementInstSmpl;
            id2.indexElt2 = this->sf2->add(id);
            // Recopiage des gens
            id.typeElement = elementInstSmplGen;
            for (int j = 0; j < this->sf2->count(id); j++)
            {
                id.indexMod = j;
                this->sf2->set(id2, (Champ)this->sf2->get(id, champ_sfGenOper).wValue,
                               this->sf2->get(id, champ_sfGenAmount));
            }
            // Modification keyrange
            val.rValue.byLo = i;
            val.rValue.byHi = i;
            this->sf2->set(id2, champ_keyRange, val);
            // Recopiage des mods
            id.typeElement = elementInstSmplMod;
            id2.typeElement = elementInstSmplMod;
            for (int j = 0; j < this->sf2->count(id); j++)
            {
                id.indexMod = j;
                id2.indexMod = this->sf2->add(id2);
                this->sf2->set(id2, champ_modAmount, this->sf2->get(id, champ_modAmount));
                this->sf2->set(id2, champ_sfModSrcOper, this->sf2->get(id, champ_sfModSrcOper));
                this->sf2->set(id2, champ_sfModAmtSrcOper, this->sf2->get(id, champ_sfModAmtSrcOper));
                this->sf2->set(id2, champ_sfModDestOper, this->sf2->get(id, champ_sfModDestOper));
                this->sf2->set(id2, champ_sfModTransOper, this->sf2->get(id, champ_sfModTransOper));
            }
        }
    }
}
void Page_Inst::paramGlobal()
{
    EltID idInst = this->tree->getID(0);
    idInst.typeElement = elementInstSmpl;
    if (this->sf2->count(idInst) == 0)
    {
        QMessageBox::warning(NULL, tr("Attention"), tr("L'instrument doit contenir des sons."));
        return;
    }
    DialogParamGlobal * dialogParam = new DialogParamGlobal(this);
    dialogParam->setAttribute(Qt::WA_DeleteOnClose, true);
    this->connect(dialogParam, SIGNAL(accepted(QVector<double>,int,int)),
                  SLOT(paramGlobal(QVector<double>,int,int)));
    dialogParam->show();
}
void Page_Inst::paramGlobal(QVector<double> dValues, int typeModif, int param)
{
    // Paramètre à modifier
    Champ champ = champ_unknown;
    switch (param)
    {
    case 0: champ = champ_initialAttenuation; break;
    case 1: champ = champ_coarseTune; break;
    case 2: champ = champ_fineTune; break;
    case 3: champ = champ_scaleTuning; break;
    case 4: champ = champ_initialFilterFc; break;
    case 5: champ = champ_initialFilterQ; break;
    case 6: champ = champ_delayVolEnv; break;
    case 7: champ = champ_attackVolEnv; break;
    case 8: champ = champ_holdVolEnv; break;
    case 9: champ = champ_decayVolEnv; break;
    case 10: champ = champ_sustainVolEnv; break;
    case 11: champ = champ_releaseVolEnv; break;
    case 12: champ = champ_keynumToVolEnvHold; break;
    case 13: champ = champ_keynumToVolEnvDecay; break;
    case 14: champ = champ_delayModEnv; break;
    case 15: champ = champ_attackModEnv; break;
    case 16: champ = champ_holdModEnv; break;
    case 17: champ = champ_decayModEnv; break;
    case 18: champ = champ_sustainModEnv; break;
    case 19: champ = champ_releaseModEnv; break;
    case 20: champ = champ_modEnvToPitch; break;
    case 21: champ = champ_modEnvToFilterFc; break;
    case 22: champ = champ_keynumToModEnvHold; break;
    case 23: champ = champ_keynumToModEnvDecay; break;
    case 24: champ = champ_delayModLFO; break;
    case 25: champ = champ_freqModLFO; break;
    case 26: champ = champ_modLfoToPitch; break;
    case 27: champ = champ_modLfoToFilterFc; break;
    case 28: champ = champ_modLfoToVolume; break;
    case 29: champ = champ_delayVibLFO; break;
    case 30: champ = champ_freqVibLFO; break;
    case 31: champ = champ_vibLfoToPitch; break;
    case 32: champ = champ_chorusEffectsSend; break;
    case 33: champ = champ_reverbEffectsSend; break;
    }
    // Ligne correspondante dans la table
    int ligne = this->ui->tableInst->getRow(champ);
    // Pos min et max sur le clavier
    EltID id = this->tree->getID(0);
    id.typeElement = elementInstSmpl;
    int posMin = 128;
    int posMax = 0;
    for (int i = 0; i < this->sf2->count(id); i++)
    {
        id.indexElt2 = i;
        if (!this->sf2->get(id, champ_hidden).bValue)
        {
            if (this->sf2->get(id, champ_keyRange).rValue.byLo < posMin)
                posMin = this->sf2->get(id, champ_keyRange).rValue.byLo;
            if (this->sf2->get(id, champ_keyRange).rValue.byHi > posMax)
                posMax = this->sf2->get(id, champ_keyRange).rValue.byHi;
        }
    }
    if (posMin >= posMax) return;
    // Modification pour chaque colonne du tableau
    this->sf2->prepareNewActions();
    double value, value2;
    int pos;
    this->ui->tableInst->blockSignals(true);
    for (int i = 1; i < this->ui->tableInst->columnCount(); i++)
    {
        value = this->ui->tableInst->item(ligne, i)->text().toDouble();
        // Calcul de la modification
        pos = (this->sf2->get(this->ui->tableInst->getID(i), champ_keyRange).rValue.byLo +
               this->sf2->get(this->ui->tableInst->getID(i), champ_keyRange).rValue.byHi) / 2;
        pos = (double)((dValues.size()-1) * (pos - posMin)) / (posMax - posMin);
        value2 = dValues.at(pos);
        // Application de la modification
        switch (typeModif)
        {
        case 0: // Ajout
            value += value2;
            break;
        case 1: // Multiplication
            value *= value2;
            break;
        case 2: // Remplacement
            value = value2;
            break;
        }
        this->ui->tableInst->item(ligne, i)->setText(QString::number(value));
        this->set(ligne, i, false);
    }
    this->ui->tableInst->blockSignals(false);
    // Actualisation
    this->mainWindow->updateDo();
    this->afficher();
}
void Page_Inst::repartitionAuto()
{
    EltID id = this->tree->getID(0);
    id.typeElement = elementInstSmpl;
    if (this->sf2->count(id) == 0)
    {
        QMessageBox::warning(NULL, tr("Attention"), tr("L'instrument doit contenir des sons."));
        return;
    }
    // Répartition automatique des notes sur le clavier
    this->sf2->prepareNewActions();
    // Reprise de l'identificateur si modification
    id = this->tree->getID(0);
    // Liste des samples liés avec note de base
    QList<EltID> listID;
    QList<int> listNote;
    int notePrecedente, note, noteSuivante;
    id.typeElement = elementInstSmpl;
    for (int i = 0; i < this->sf2->count(id); i++)
    {
        id.indexElt2 = i;
        if (!this->sf2->get(id, champ_hidden).bValue)
        {
            if (this->sf2->isSet(id, champ_overridingRootKey))
                note = this->sf2->get(id, champ_overridingRootKey).wValue;
            else
            {
                EltID id2 = id;
                id2.typeElement = elementSmpl;
                id2.indexElt = this->sf2->get(id, champ_sampleID).wValue;
                note = this->sf2->get(id2, champ_byOriginalPitch).shValue;
            }
            // Insertion triée
            int pos = 0;
            for (int j = 0; j < listID.size(); j++)
                if (note > listNote.at(j)) pos++;
            listID.insert(pos, id);
            listNote.insert(pos, note);
        }
    }
    // Répartition
    Valeur val;
    noteSuivante = -1;
    notePrecedente = -1;
    for (int i = 0; i < listID.size(); i++)
    {
        // Note actuelle
        note = listNote.at(i);
        if (note != notePrecedente)
        {
            // Recherche de la note suivante
            noteSuivante = -1;
            for (int j = i+1; j < listID.size(); j++)
            {
                if (noteSuivante == -1 && listNote.at(i) != listNote.at(j))
                    noteSuivante = listNote.at(j);
            }
            // borne inférieure de la division
            if (notePrecedente == -1)
                val.rValue.byLo = 0;
            else
                val.rValue.byLo = val.rValue.byHi + 1;
            if (val.rValue.byLo > 128) val.rValue.byLo = 128;
            // borne supérieure de la division
            if (noteSuivante == -1) val.rValue.byHi = 128;
            else
            {
                val.rValue.byHi = qMin(note+(128 - note)/20,
                                       (int)floor((double)(2*note + noteSuivante)/3));
                if (val.rValue.byLo > val.rValue.byHi)
                    val.rValue.byHi = val.rValue.byLo;
            }
            notePrecedente = note;
        }
        // Modification
        if (this->sf2->get(listID.at(i), champ_keyRange).rValue.byLo != val.rValue.byLo ||
            this->sf2->get(listID.at(i), champ_keyRange).rValue.byHi != val.rValue.byHi)
            this->sf2->set(listID.at(i), champ_keyRange, val);
    }
    // Actualisation
    this->mainWindow->updateDo();
    this->afficher();
}
void Page_Inst::spatialisation()
{
    EltID idInst = this->tree->getID(0);
    idInst.typeElement = elementInstSmpl;
    if (this->sf2->count(idInst) == 0)
    {
        QMessageBox::warning(NULL, tr("Attention"), tr("L'instrument doit contenir des sons."));
        return;
    }
    DialogSpace * dialogSpace = new DialogSpace(this);
    dialogSpace->setAttribute(Qt::WA_DeleteOnClose, true);
    this->connect(dialogSpace, SIGNAL(accepted(int,int,int,int,int,int,int)),
                  SLOT(spatialisation(int,int,int,int,int,int,int)));
    dialogSpace->show();
}
void Page_Inst::spatialisation(int motif, int nbDiv, int etalement, int occupation, int offset, int sens, int sens2)
{
    this->sf2->prepareNewActions();
    // Liste des samples avec le lien le cas échéant
    QList<EltID> list1;
    QList<genAmountType> listRange;
    QList<EltID> list2;
    EltID id = this->tree->getID(0);
    genAmountType amount;
    bool found;
    int pos;
    int noteMin = 128;
    int noteMax = 0;
    id.typeElement = elementInstSmpl;
    for (int i = 0; i < this->sf2->count(id); i++)
    {
        id.indexElt2 = i;
        if (!this->sf2->get(id, champ_hidden).bValue)
        {
            amount = this->sf2->get(id, champ_keyRange).genValue;
            if (amount.ranges.byLo < noteMin) noteMin = amount.ranges.byLo;
            if (amount.ranges.byHi > noteMax) noteMax = amount.ranges.byHi;
            // Recherche d'une note ayant la même étendue sur le clavier
            found = false;
            pos = 0;
            while (pos < list1.size() && !found)
            {
                if (amount.ranges.byHi == listRange.at(pos).ranges.byHi &&
                    amount.ranges.byLo == listRange.at(pos).ranges.byLo)
                    found = true;
                else
                    pos++;
            }
            if (found)
            {
                // Element déjà lié ?
                if (list2.at(pos).indexElt2 != -1)
                {
                    // Ajout à liste 1
                    list1.append(id);
                    // Element nul dans liste 2
                    id.indexElt2 = -1;
                    list2.append(id);
                    // Etendue
                    listRange.append(amount);
                }
                else
                {
                    // Lien
                    list2[pos] = id;
                }
            }
            else
            {
                // Ajout à liste 1
                list1.append(id);
                // Element nul dans liste 2
                id.indexElt2 = -1;
                list2.append(id);
                // Etendue
                listRange.append(amount);
            }
        }
    }
    // Spatialisation
    double pan = 0;
    int note = 64;
    EltID id2, id3;
    int sampleG;
    Valeur val;
    for (int i = 0; i < list1.size(); i++)
    {
        note = (listRange.at(i).ranges.byLo + listRange.at(i).ranges.byHi) / 2;
        pan = DialogSpace::space(noteMin, noteMax, note, motif, nbDiv,
                                 etalement, occupation, offset, sens, sens2);
        // Lien ?
        if (list2.at(i).indexElt2 == -1)
        {
            // pas de lien
            val.genValue.shAmount = 1000*pan - 500;
            this->sf2->set(list1.at(i), champ_pan, val);
        }
        else
        {
            // Quel sample est à gauche ?
            sampleG = 0;
            // Sample correspondant 1
            id2 = list1.at(i);
            id2.indexElt = this->sf2->get(id2, champ_sampleID).wValue;
            id2.typeElement = elementSmpl;
            // Sample correspondant 2
            id3 = list2.at(i);
            id3.indexElt = this->sf2->get(id3, champ_sampleID).wValue;
            id3.typeElement = elementSmpl;
            if ((this->sf2->get(id2, champ_sfSampleType).sfLinkValue == leftSample ||
                 this->sf2->get(id2, champ_sfSampleType).sfLinkValue == RomLeftSample) &&
                 this->sf2->get(id3, champ_sfSampleType).sfLinkValue != leftSample &&
                 this->sf2->get(id3, champ_sfSampleType).sfLinkValue != RomLeftSample)
            {
                sampleG = 0;
            }
            else if((this->sf2->get(id2, champ_sfSampleType).sfLinkValue == rightSample ||
                     this->sf2->get(id2, champ_sfSampleType).sfLinkValue == RomRightSample) &&
                     this->sf2->get(id3, champ_sfSampleType).sfLinkValue != rightSample &&
                     this->sf2->get(id3, champ_sfSampleType).sfLinkValue != RomRightSample)
            {
                sampleG = 1;
            }
            else
            {
                if (this->sf2->get(list1.at(i), champ_pan).shValue <
                        this->sf2->get(list2.at(i), champ_pan).shValue)
                    sampleG = 0;
                else if (this->sf2->get(list1.at(i), champ_pan).shValue >
                        this->sf2->get(list2.at(i), champ_pan).shValue)
                    sampleG = 1;
            }
            if (sampleG == 0)
            {
                // Inversion
                id2 = list1.at(i);
                list1[i] = list2.at(i);
                list2[i] = id2;
            }
            // lien
            if (pan < 0.5)
            {
                // Gauche
                val.genValue.shAmount = -500;
                this->sf2->set(list1.at(i), champ_pan, val);
                // Droite
                val.genValue.shAmount = 2000 * pan - 500;
                this->sf2->set(list2.at(i), champ_pan, val);
            }
            else
            {
                // Gauche
                val.genValue.shAmount = 2000 * pan - 1500;
                this->sf2->set(list1.at(i), champ_pan, val);
                // Droite
                val.genValue.shAmount = 500;
                this->sf2->set(list2.at(i), champ_pan, val);
            }
        }
    }
    // Actualisation
    this->mainWindow->updateDo();
    this->afficher();
}
void Page_Inst::mixture()
{
    EltID idInst = this->tree->getID(0);
    idInst.typeElement = elementInstSmpl;
    if (this->sf2->count(idInst) == 0)
    {
        QMessageBox::warning(NULL, tr("Attention"), tr("L'instrument doit contenir des sons."));
        return;
    }
    DialogMixture * dialogMixture = new DialogMixture(this);
    dialogMixture->setAttribute(Qt::WA_DeleteOnClose, true);
    this->connect(dialogMixture, SIGNAL(accepted(QList<QList<int> >, QString, bool, int)),
                  SLOT(mixture(QList<QList<int> >, QString, bool, int)));
    dialogMixture->show();
}
void Page_Inst::mixture(QList<QList<int> > listeParam, QString nomInst, bool bouclage, int freq)
{
    // Création d'une mixture
    this->sf2->prepareNewActions();
    // Nombre d'étapes
    int nbEtapes = 0;
    for (int i = 0; i < listeParam.length(); i++)
    {
        int nbNotes = (qAbs(listeParam.at(i).at(0) - listeParam.at(i).at(1))) / freq + 1;
        nbEtapes += nbNotes * (listeParam.at(i).length() + 2 * bouclage);
    }
    // Ouverture d'une barre de progression
    QString textProgress = trUtf8("Création ");
    QProgressDialog progress("", tr("Annuler"), 0, nbEtapes, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setFixedWidth(350);
    progress.show();
    double dureeSmpl = 7;
    qint32 fEch = 48000;
    EltID idInst = this->tree->getID(0);
    idInst.typeElement = elementInst;
    EltID idSmpl;
    QByteArray baData;
    // Création d'un nouvel instrument
    EltID idNewInst = idInst;
    idNewInst.indexElt = this->sf2->add(idNewInst);
    // Configuration instrument
    if (nomInst.isEmpty())
        nomInst = tr("sans nom");
    this->sf2->set(idNewInst, champ_name, nomInst.left(20));
    if (bouclage)
    {
        Valeur value;
        value.wValue = 1;
        this->sf2->set(idNewInst, champ_sampleModes, value);
    }
    EltID idInstSmpl = idNewInst;
    idInstSmpl.typeElement = elementInstSmpl;
    // Création de samples et ajout dans l'instrument
    for (int numDiv = 0; numDiv < listeParam.length(); numDiv++)
    {
        QList<int> listRangs = listeParam.at(numDiv);
        // Etendue de note
        int noteStart2 = qMin(listRangs[0], listRangs[1]);
        int noteEnd = qMax(listRangs[0], listRangs[1]);
        int noteStart = noteStart2 + (noteEnd - noteStart2) % freq;
        listRangs.takeAt(0);
        listRangs.takeAt(0);
        // Pour chaque note
        for (int note = noteStart; note <= noteEnd; note += freq)
        {
            // Pour chaque cote
            for (int cote = 0; cote < 2; cote++)
            {
                QString name = nomInst.left(15);
                char str2[20];
                sprintf(str2,"%.3hu", note);
                if (cote == 0)
                    name = name + ' ' + str2 + 'R';
                else
                    name = name + ' ' + str2 + 'L';
                progress.setLabelText(textProgress + name);
                QApplication::processEvents();
                baData.resize(dureeSmpl*fEch*4);
                baData.fill(0);
                // Calcul de l'atténuation mini de tous les rangs
                double attMini = 1000000;
                for (int numRang = 0; numRang < listRangs.length()/2; numRang++)
                {
                    double noteTmp = (double)note + getOffset(listRangs.at(2*numRang), listRangs.at(2*numRang+1));
                    double ecart;
                    EltID idInstSmplTmp;
                    idSmpl = closestSample(idInst, noteTmp, ecart, cote, idInstSmplTmp);
                    double attenuation = 0;
                    if (sf2->isSet(idInstSmplTmp, champ_initialAttenuation))
                        attenuation = (double)sf2->get(idInstSmplTmp, champ_initialAttenuation).wValue / 10.0;
                    if (attenuation < attMini)
                        attMini = attenuation;
                }
                // Pour chaque rang
                for (int numRang = 0; numRang < listRangs.length()/2; numRang++)
                {
                    if (!progress.wasCanceled())
                        progress.setValue(progress.value() + 1);
                    // Calcul de la note à ajouter à la mixture
                    double noteTmp = (double)note + getOffset(listRangs.at(2*numRang), listRangs.at(2*numRang+1));
                    if (noteTmp <= 120)
                    {
                        // Sample le plus proche et écart associé
                        double ecart;
                        EltID idInstSmplTmp;
                        idSmpl = closestSample(idInst, noteTmp, ecart, cote, idInstSmplTmp);
    //                    printf("touche %d, note cherchee %.2f, sample %s, instsmpl %d-%d\n",
    //                           note, noteTmp, sf2->getQstr(idSmpl, champ_name).toStdString().c_str(),
    //                           sf2->get(idInstSmplTmp, champ_keyRange).rValue.byLo,
    //                           sf2->get(idInstSmplTmp, champ_keyRange).rValue.byHi);
                        // Fréquence d'échantillonnage initiale fictive (pour accordage)
                        double fEchInit = (double)this->sf2->get(idSmpl, champ_dwSampleRate).dwValue * pow(2, ecart/12.0);
                        // Récupération du son
                        QByteArray baDataTmp = getSampleData(idSmpl, dureeSmpl * fEchInit);
                        // Prise en compte atténuation en dB
                        double attenuation = 1;
                        if (sf2->isSet(idInstSmplTmp, champ_initialAttenuation))
                        {
                            attenuation = (double)sf2->get(idInstSmplTmp, champ_initialAttenuation).wValue / 10.0 - attMini;
                            attenuation = pow(10, -attenuation / 20.0);
                        }
    //                    // Prise en compte fréquence de coupure en Hz (filtre passe bas 2ème ordre)
    //                    if (sf2->isSet(idInstSmplTmp, champ_initialFilterFc))
    //                    {
    //                        double cut = exp2((double)sf2->get(idInstSmplTmp, champ_initialFilterFc).wValue/1200)*8.176;
    //                        DWORD dwSmplRate = sf2->get(idSmpl, champ_dwSampleRate).dwValue;
    //                        baDataTmp = Sound::bandFilter(baDataTmp, 32, dwSmplRate, cut, 0, 1);
    //                    }
                        // Rééchantillonnage
                        double d = 0;
                        baDataTmp = Sound::resampleMono(baDataTmp, fEchInit, fEch, 32, 0, d, false);
                        // Ajout du son
                        baData = addSampleData(baData, baDataTmp, attenuation);
                    }
                }
                if (!progress.wasCanceled())
                    progress.setValue(progress.value() + 1);
                qint32 loopStart = 0;
                qint32 loopEnd = 0;
                // Bouclage du sample
                if (bouclage)
                {
                    QByteArray baData2 = Sound::bouclage(baData, fEch, loopStart, loopEnd, 32);
                    if (!baData2.isEmpty())
                        baData = baData2;
                    if (!progress.wasCanceled())
                        progress.setValue(progress.value() + 1);
                }
                // Création d'un nouveau sample
                idSmpl.indexElt = this->sf2->add(idSmpl);
                // Ajout des données
                this->sf2->set(idSmpl, champ_sampleData16, Sound::bpsConversion(baData, 32, 16));
                EltID idSf2 = idSmpl;
                idSf2.typeElement = elementSf2;
                if (this->sf2->get(idSf2, champ_wBpsSave).wValue == 24)
                    this->sf2->set(idSmpl, champ_sampleData24, Sound::bpsConversion(baData, 32, 824));
                // Configuration
                Valeur value;
                value.dwValue = baData.length() / 4;
                this->sf2->set(idSmpl, champ_dwLength, value);
                value.dwValue = fEch;
                this->sf2->set(idSmpl, champ_dwSampleRate, value);
                value.wValue = note;
                this->sf2->set(idSmpl, champ_byOriginalPitch, value);
                value.cValue = 0;
                this->sf2->set(idSmpl, champ_chPitchCorrection, value);
                value.dwValue = loopStart;
                this->sf2->set(idSmpl, champ_dwStartLoop, value);
                value.dwValue = loopEnd;
                this->sf2->set(idSmpl, champ_dwEndLoop, value);
                this->sf2->set(idSmpl, champ_name, name);
                // Lien
                if (cote == 0)
                    value.sfLinkValue = rightSample;
                else
                    value.sfLinkValue = leftSample;
                this->sf2->set(idSmpl, champ_sfSampleType, value);
                if (cote == 1)
                {
                    EltID idLink = idSmpl;
                    idLink.indexElt = idSmpl.indexElt - 1;
                    value.wValue = idLink.indexElt;
                    this->sf2->set(idSmpl, champ_wSampleLink, value);
                    value.wValue = idSmpl.indexElt;
                    this->sf2->set(idLink, champ_wSampleLink, value);
                }
                // Ajout du sample dans l'instrument
                idInstSmpl.indexElt2 = this->sf2->add(idInstSmpl);
                // Configuration
                value.wValue = idSmpl.indexElt;
                this->sf2->set(idInstSmpl, champ_sampleID, value);
                value.rValue.byLo = qMax(noteStart2, note-freq+1);
                value.rValue.byHi = note;
                this->sf2->set(idInstSmpl, champ_keyRange, value);
                if (cote == 0)
                    value.shValue = 500;
                else
                    value.shValue = -500;
                this->sf2->set(idInstSmpl, champ_pan, value);
                value.wValue = attMini * 10;
                this->sf2->set(idInstSmpl, champ_initialAttenuation, value);
                if (progress.wasCanceled())
                {
                    // Actualisation et retour
                    this->mainWindow->updateDo();
                    return;
                }
            }
        }
    }
    // Actualisation
    this->mainWindow->updateDo();
}

double Page_Inst::getOffset(int type1, int type2)
{
    // Calcul du multiple de la fréquence fondamentale
    double multiple = (double)(2 * type1 + 1) * pow(2, type2-2);
    // Renvoi du nombre de demi-tons à ajouter à la fondamentale pour obtenir l'harmonique
    return 12.0 * log2(multiple);
}
EltID Page_Inst::closestSample(EltID idInst, double pitch, double &ecart, int cote, EltID &idInstSmpl)
{
    // Recherche du sample le plus proche de pitch dans l'instrument idInst
    double ecart_min_abs = 1000;
    EltID idInstSmplTmp = idInst;
    idInstSmplTmp.typeElement = elementInstSmpl;
    EltID idSmpl = idInst;
    idSmpl.indexElt = -1;
    idSmpl.typeElement = elementSmpl;
    EltID idSmplRet = idSmpl;
    for (int i = 0; i < sf2->count(idInstSmplTmp); i++)
    {
        idInstSmplTmp.indexElt2 = i;
        if (!sf2->get(idInstSmplTmp, champ_hidden).bValue)
        {
            // Hauteur du sample
            idSmpl.indexElt = sf2->get(idInstSmplTmp, champ_sampleID).wValue;
            double pitchSmpl = sf2->get(idSmpl, champ_byOriginalPitch).wValue
                - (double)sf2->get(idSmpl, champ_chPitchCorrection).cValue / 100.0;
            // Mesure de l'écart
            double ecartTmp = pitchSmpl - pitch;
            double absEcart;
            if (ecartTmp < 0) absEcart = -3 * ecartTmp;
            else absEcart = ecartTmp;
            if (absEcart < ecart_min_abs)
            {
                ecart_min_abs = absEcart;
                ecart = -ecartTmp;
                idSmplRet = idSmpl;
                idInstSmpl = idInstSmplTmp;
            }
        }
    }
    // Type de sample
    SFSampleLink type = sf2->get(idSmplRet, champ_sfSampleType).sfLinkValue;
    if (!(type == RomMonoSample || type == monoSample ||
       ((type == RomRightSample || type == rightSample || type == RomLinkedSample || type == linkedSample) && cote == 0) ||
       ((type == RomLeftSample || type == leftSample) && cote == 1)))
        idSmplRet.indexElt = sf2->get(idSmplRet, champ_wSampleLink).wValue;
    double ecartMin = 1000;
    double ecartTmp;
    int rootKeySmpl = sf2->get(idSmplRet, champ_byOriginalPitch).shValue;
    // Recherche de l'instSmpl le plus proche de pitch, ayant comme sample_ID idSmplRet
    for (int i = 0; i < sf2->count(idInstSmplTmp); i++)
    {
        idInstSmplTmp.indexElt2 = i;
        if (!sf2->get(idInstSmplTmp, champ_hidden).bValue)
        {
            if (sf2->get(idInstSmplTmp, champ_sampleID).wValue == idSmplRet.indexElt)
            {
                // Notes min et max pour lesquels le sample est joué
                int noteMin = sf2->get(idInstSmplTmp, champ_keyRange).rValue.byLo;
                int noteMax = sf2->get(idInstSmplTmp, champ_keyRange).rValue.byHi;
                // Ajustement
                int rootKeyInstSmpl = rootKeySmpl;
                if (sf2->isSet(idInstSmplTmp, champ_overridingRootKey))
                    rootKeyInstSmpl = sf2->get(idInstSmplTmp, champ_overridingRootKey).wValue;
                noteMin += rootKeySmpl - rootKeyInstSmpl;
                noteMax += rootKeySmpl - rootKeyInstSmpl;
                // Mesure de l'écart
                if (pitch < noteMin)
                    ecartTmp = noteMin - pitch;
                else if (pitch > noteMax)
                    ecartTmp = pitch - noteMax;
                else
                    ecartTmp = 0;
                if (ecartTmp < ecartMin)
                {
                    ecartMin = ecartTmp;
                    idInstSmpl = idInstSmplTmp;
                }
            }
        }
    }
    return idSmplRet;
}
QByteArray Page_Inst::getSampleData(EltID idSmpl, qint32 nbRead)
{
    // Récupération de données provenant d'un sample, en prenant en compte la boucle
    QByteArray baData = sf2->getData(idSmpl, champ_sampleDataFull24);
    baData = Sound::bpsConversion(baData, 24, 32);
    qint64 loopStart = sf2->get(idSmpl, champ_dwStartLoop).dwValue;
    qint64 loopEnd = sf2->get(idSmpl, champ_dwEndLoop).dwValue;
    QByteArray baDataRet;
    baDataRet.resize(nbRead * 4);
    qint64 posInit = 0;
    const char * data = baData.constData();
    char * dataRet = baDataRet.data();
    if (loopStart != loopEnd)
    {
        // Boucle
        qint64 total = 0;
        while (nbRead - total > 0)
        {
            const qint64 chunk = qMin(loopEnd - posInit, nbRead - total);
            memcpy(dataRet + 4 * total, data + 4 * posInit, 4 * chunk);
            posInit += chunk;
            if (posInit >= loopEnd) posInit = loopStart;
            total += chunk;
        }
    }
    else
    {
        // Pas de boucle
        if (baData.size()/4 - posInit < nbRead)
        {
            baDataRet.fill(0);
            memcpy(dataRet, data + 4 * posInit, baData.size() - 4 * posInit);
        }
        else
            memcpy(dataRet, data + 4 * posInit, 4 * nbRead);
    }
    return baDataRet;
}
QByteArray Page_Inst::addSampleData(QByteArray baData1, QByteArray baData2, double mult)
{
    // Ajout de baData2 multiplié par mult dans baData1
    qint32 * data1 = (qint32 *)baData1.data();
    qint32 * data2 = (qint32 *)baData2.data();
    for (int i = 0; i < qMin(baData1.size(), baData2.size())/4; i++)
        data1[i] += mult * data2[i];
    return baData1;
}

// TableWidgetInst
TableWidgetInst::TableWidgetInst(QWidget *parent) : TableWidget(parent)
{
}
TableWidgetInst::~TableWidgetInst()
{
}

int TableWidgetInst::getRow(WORD champ)
{
    int row = -1;
    switch (champ)
    {
    case champ_keyRange: row = 4; break;
    case champ_velRange: row = 5; break;
    case champ_initialAttenuation: row = 6; break;
    case champ_pan: row = 7; break;
    case champ_sampleModes: row = 8; break;
    case champ_overridingRootKey: row = 9; break;
    case champ_coarseTune: row = 10; break;
    case champ_fineTune: row = 11; break;
    case champ_scaleTuning: row = 12; break;
    case champ_initialFilterFc: row = 13; break;
    case champ_initialFilterQ: row = 14; break;
    case champ_delayVolEnv: row = 15; break;
    case champ_attackVolEnv: row = 16; break;
    case champ_holdVolEnv: row = 17; break;
    case champ_decayVolEnv: row = 18; break;
    case champ_sustainVolEnv: row = 19; break;
    case champ_releaseVolEnv: row = 20; break;
    case champ_keynumToVolEnvHold: row = 21; break;
    case champ_keynumToVolEnvDecay: row = 22; break;
    case champ_delayModEnv: row = 23; break;
    case champ_attackModEnv: row = 24; break;
    case champ_holdModEnv: row = 25; break;
    case champ_decayModEnv: row = 26; break;
    case champ_sustainModEnv: row = 27; break;
    case champ_releaseModEnv: row = 28; break;
    case champ_modEnvToPitch: row = 29; break;
    case champ_modEnvToFilterFc: row = 30; break;
    case champ_keynumToModEnvHold: row = 31; break;
    case champ_keynumToModEnvDecay: row = 32; break;
    case champ_delayModLFO: row = 33; break;
    case champ_freqModLFO: row = 34; break;
    case champ_modLfoToPitch: row = 35; break;
    case champ_modLfoToFilterFc: row = 36; break;
    case champ_modLfoToVolume: row = 37; break;
    case champ_delayVibLFO: row = 38; break;
    case champ_freqVibLFO: row = 39; break;
    case champ_vibLfoToPitch: row = 40; break;
    case champ_exclusiveClass: row = 41; break;
    case champ_chorusEffectsSend: row = 42; break;
    case champ_reverbEffectsSend: row = 43; break;
    case champ_keynum: row = 44; break;
    case champ_velocity: row = 45; break;
    case champ_startAddrsOffset: row = 46; break;
    case champ_endAddrsOffset: row = 47; break;
    case champ_startloopAddrsOffset: row = 48; break;
    case champ_endloopAddrsOffset: row = 49; break;
    }
    return row;
}
Champ TableWidgetInst::getChamp(int row)
{
    Champ champ = champ_unknown;
    switch (row)
    {
    case 4: champ = champ_keyRange; break;
    case 5: champ = champ_velRange; break;
    case 6: champ = champ_initialAttenuation; break;
    case 7: champ = champ_pan; break;
    case 8: champ = champ_sampleModes; break;
    case 9: champ = champ_overridingRootKey; break;
    case 10: champ = champ_coarseTune; break;
    case 11: champ = champ_fineTune; break;
    case 12: champ = champ_scaleTuning; break;
    case 13: champ = champ_initialFilterFc; break;
    case 14: champ = champ_initialFilterQ; break;
    case 15: champ = champ_delayVolEnv; break;
    case 16: champ = champ_attackVolEnv; break;
    case 17: champ = champ_holdVolEnv; break;
    case 18: champ = champ_decayVolEnv; break;
    case 19: champ = champ_sustainVolEnv; break;
    case 20: champ = champ_releaseVolEnv; break;
    case 21: champ = champ_keynumToVolEnvHold; break;
    case 22: champ = champ_keynumToVolEnvDecay; break;
    case 23: champ = champ_delayModEnv; break;
    case 24: champ = champ_attackModEnv; break;
    case 25: champ = champ_holdModEnv; break;
    case 26: champ = champ_decayModEnv; break;
    case 27: champ = champ_sustainModEnv; break;
    case 28: champ = champ_releaseModEnv; break;
    case 29: champ = champ_modEnvToPitch; break;
    case 30: champ = champ_modEnvToFilterFc; break;
    case 31: champ = champ_keynumToModEnvHold; break;
    case 32: champ = champ_keynumToModEnvDecay; break;
    case 33: champ = champ_delayModLFO; break;
    case 34: champ = champ_freqModLFO; break;
    case 35: champ = champ_modLfoToPitch; break;
    case 36: champ = champ_modLfoToFilterFc; break;
    case 37: champ = champ_modLfoToVolume; break;
    case 38: champ = champ_delayVibLFO; break;
    case 39: champ = champ_freqVibLFO; break;
    case 40: champ = champ_vibLfoToPitch; break;
    case 41: champ = champ_exclusiveClass; break;
    case 42: champ = champ_chorusEffectsSend; break;
    case 43: champ = champ_reverbEffectsSend; break;
    case 44: champ = champ_keynum; break;
    case 45: champ = champ_velocity; break;
    case 46: champ = champ_startAddrsOffset; break;
    case 47: champ = champ_endAddrsOffset; break;
    case 48: champ = champ_startloopAddrsOffset; break;
    case 49: champ = champ_endloopAddrsOffset; break;
    default: champ = champ_unknown;
    }
    return champ;
}
int Page_Inst::getDestNumber(int i)
{
    switch (i)
    {
    case 0: return 0; // Sample
    case 1: return 4;
    case 2: return 1;
    case 3: return 12;
    case 4: return 2;
    case 5: return 45;
    case 6: return 3;
    case 7: return 50;
    case 8: return 58; // Pitch/Effects
    case 9: return 52;
    case 10: return 51;
    case 11: return 56;
    case 12: return 8;
    case 13: return 9;
    case 14: return 17;
    case 15: return 15;
    case 16: return 16;
    case 17: return 46;
    case 18: return 57;
    case 19: return 48; // Volume envelope
    case 20: return 33;
    case 21: return 34;
    case 22: return 35;
    case 23: return 36;
    case 24: return 37;
    case 25: return 38;
    case 26: return 39;
    case 27: return 40;
    case 28: return 47;
    case 29: return 54;
    case 30: return 25; // Modulation envelope
    case 31: return 26;
    case 32: return 27;
    case 33: return 28;
    case 34: return 29;
    case 35: return 30;
    case 36: return 7;
    case 37: return 11;
    case 38: return 31;
    case 39: return 32;
    case 40: return 21; // Modulation LFO
    case 41: return 22;
    case 42: return 5;
    case 43: return 13;
    case 44: return 10;
    case 45: return 23; // Vibrato LFO
    case 46: return 24;
    case 47: return 6;
    default: return 100;
    }
}
int Page_Inst::getDestIndex(int i)
{
    switch (i)
    {
    case 0: return 0; // Sample
    case 4: return 1;
    case 1: return 2;
    case 12: return 3;
    case 2: return 4;
    case 45: return 5;
    case 3: return 6;
    case 50: return 7;
    case 58: return 8;
    case 52: return 9; // Pitch/Effects
    case 51: return 10;
    case 56: return 11;
    case 8: return 12;
    case 9: return 13;
    case 17: return 14;
    case 15: return 15;
    case 16: return 16;
    case 46: return 17;
    case 57: return 18;
    case 48: return 19; // Volume envelope
    case 33: return 20;
    case 34: return 21;
    case 35: return 22;
    case 36: return 23;
    case 37: return 24;
    case 38: return 25;
    case 39: return 26;
    case 40: return 27;
    case 47: return 28;
    case 54: return 29;
    case 25: return 30; // Modulation envelope
    case 26: return 31;
    case 27: return 32;
    case 28: return 33;
    case 29: return 34;
    case 30: return 35;
    case 7: return 36;
    case 11: return 37;
    case 31: return 38;
    case 32: return 39;
    case 21: return 40; // Modulation LFO
    case 22: return 41;
    case 5: return 42;
    case 13: return 43;
    case 10: return 44;
    case 23: return 45; // Vibrato LFO
    case 24: return 46;
    case 6: return 47;
    default: return 100;
    }
}
