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

#include "dialog_mixture.h"
#include "ui_dialog_mixture.h"
#include "sf2_types.h"
#include "contextmanager.h"

// Constructeur, destructeur
DialogMixture::DialogMixture(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogMixture)
{
    // Prepare the interface
    ui->setupUi(this);
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint));
    this->setAttribute(Qt::WA_DeleteOnClose, true);

    // Recall values
    _listeParam = getStoredRanks();
    ui->lineNom->setText(ContextManager::configuration()->getToolValue(
                             ConfManager::TOOL_TYPE_INSTRUMENT, "mixture", "nom", "").toString());
    ui->comboFreq->setCurrentIndex(ContextManager::configuration()->getToolValue(
                                       ConfManager::TOOL_TYPE_INSTRUMENT, "mixture", "density", 0).toInt());
    ui->checkBouclage->setChecked(ContextManager::configuration()->getToolValue(
                                      ConfManager::TOOL_TYPE_INSTRUMENT, "mixture", "boucle", true).toBool());
    ui->checkStereo->setChecked(ContextManager::configuration()->getToolValue(
                                    ConfManager::TOOL_TYPE_INSTRUMENT, "mixture", "stereo", true).toBool());

    // Select octaves
    this->on_comboType1_currentIndexChanged(0);

    // Display divisions
    this->dispDiv();
}
DialogMixture::~DialogMixture()
{
    delete ui;
}

QList<QList<int> > DialogMixture::getStoredRanks()
{
    QList<QList<int> > listRet;
    QList<QVariant> listTmp = ContextManager::configuration()->getToolValue(
                ConfManager::TOOL_TYPE_INSTRUMENT, "mixture", "ranks", QList<QVariant>()).toList();
    if (!listTmp.isEmpty())
    {
        QByteArray sousListTmp;
        for (int i = 0; i < listTmp.size(); i++)
        {
            listRet << QList<int>();
            sousListTmp = listTmp.at(i).toByteArray();
            for (int j = 0; j < sousListTmp.size(); j++)
                listRet[i] << sousListTmp.at(j);
        }
    }
    return listRet;
}

void DialogMixture::storeRanks(QList<QList<int> > val)
{
    QList<QVariant> listTmp;
    QByteArray sousListTmp;
    for (int i = 0; i < val.size(); i++)
    {
        sousListTmp.clear();
        for (int j = 0; j < val.at(i).size(); j++)
            sousListTmp.append(val.at(i).at(j));
        listTmp << sousListTmp;
    }
    ContextManager::configuration()->setToolValue(
                    ConfManager::TOOL_TYPE_INSTRUMENT, "mixture", "ranks", listTmp);
}


// EDITION DIVISIONS

void DialogMixture::on_listDivisions_itemSelectionChanged()
{
    int currentRow = ui->listDivisions->currentRow();

    // Modification des spinboxes
    QList<int> sousListe = _listeParam.at(currentRow);
    ui->spinDivStart->blockSignals(true);
    ui->spinDivEnd->blockSignals(true);
    ui->spinDivStart->setValue(sousListe.at(0));
    ui->spinDivEnd->setValue(sousListe.at(1));
    ui->spinDivStart->blockSignals(false);
    ui->spinDivEnd->blockSignals(false);

    // Affichage rangs
    this->dispRang(currentRow);
}

void DialogMixture::on_pushAddDiv_pressed()
{
    // Ajout d'une division
    int divStart = ui->spinDivStart->value();
    int divEnd = ui->spinDivEnd->value();
    QList<int> newSousListe;
    newSousListe.append(divStart);
    newSousListe.append(divEnd);
    _listeParam.append(newSousListe);

    // Recopie des rangs précédents
    int numDiv = this->getNumDiv();
    if (numDiv >= 0)
        for (int i = 2; i < _listeParam[numDiv].count(); i++)
            _listeParam[_listeParam.size() - 1] << _listeParam[numDiv][i];

    // Affichage
    this->dispDiv();

    // Sélection de l'élément nouvellement créé
    ui->listDivisions->setCurrentRow(_listeParam.length()-1);
}
void DialogMixture::on_pushRemoveDiv_pressed()
{
    int numDiv = this->getNumDiv();
    if (numDiv < 0) return;
    _listeParam.removeAt(numDiv);

    // Affichage
    this->dispDiv();

    // Selection
    if (numDiv >= ui->listDivisions->count())
        numDiv--;
    if (numDiv >= 0)
        ui->listDivisions->setCurrentRow(numDiv);
    else
        this->dispRang();
}
void DialogMixture::on_spinDivStart_valueChanged(int arg1)
{
    int numDiv = this->getNumDiv();
    if (numDiv < 0) return;
    _listeParam[numDiv][0] = arg1;

    // Affichage
    this->dispDiv();

    // Resélection de l'élément
    ui->listDivisions->blockSignals(true);
    ui->listDivisions->setCurrentRow(numDiv);
    ui->listDivisions->blockSignals(false);
}
void DialogMixture::on_spinDivEnd_valueChanged(int arg1)
{
    int numDiv = this->getNumDiv();
    if (numDiv < 0) return;
    _listeParam[numDiv][1] = arg1;

    // Affichage
    this->dispDiv();

    // Resélection de l'élément
    ui->listDivisions->blockSignals(true);
    ui->listDivisions->setCurrentRow(numDiv);
    ui->listDivisions->blockSignals(false);
}

// EDITION RANGS

void DialogMixture::on_listRangs_itemSelectionChanged()
{
    int currentRow = ui->listRangs->currentRow();

    // Récupération numéro de division et rang
    int numDiv = this->getNumDiv();
    if (numDiv < 0) return;
    if (currentRow * 2 + 2 >= _listeParam.at(numDiv).length()) return;

    // Mise à jour des combobox
    ui->comboType1->blockSignals(true);
    ui->comboType2->blockSignals(true);
    ui->comboType1->setCurrentIndex(_listeParam.at(numDiv).at( 2 + 2 * currentRow));
    this->dispType2(_listeParam.at(numDiv).at(2 + 2 * currentRow));
    ui->comboType2->blockSignals(true);
    ui->comboType2->setCurrentIndex(_listeParam.at(numDiv).at(3 + 2 * currentRow));
    ui->comboType1->blockSignals(false);
    ui->comboType2->blockSignals(false);
}

void DialogMixture::on_pushAddRank_clicked()
{
    // Récupération numéro de division
    int numDiv = this->getNumDiv();
    if (numDiv < 0) return;

    // Ajout d'un rang
    int type1 = ui->comboType1->currentIndex();
    int type2 = ui->comboType2->currentIndex();
    _listeParam[numDiv].append(type1);
    _listeParam[numDiv].append(type2);

    // Affichage
    this->dispRang();

    // Sélection de l'élément nouvellement créé
    ui->listRangs->setCurrentRow(_listeParam.at(numDiv).length() / 2 - 2);
}
void DialogMixture::on_pushRemoveRank_clicked()
{
    // Récupération numéro de division et rang
    int numDiv = this->getNumDiv();
    if (numDiv < 0) return;
    int numRang = this->getNumRang();
    if (numRang < 0) return;

    // Suppression des données
    _listeParam[numDiv].removeAt(2 + numRang * 2);
    _listeParam[numDiv].removeAt(2 + numRang * 2);

    // Affichage
    this->dispRang();

    // Selection
    if (numRang >= ui->listRangs->count())
        numRang--;
    if (numRang >= 0)
        ui->listRangs->setCurrentRow(numRang);
}
void DialogMixture::on_comboType1_currentIndexChanged(int index)
{
    // Modification combobox type2
    this->dispType2(index);

    // Sélection du 4ème élément (fondamentale 8')
    ui->comboType2->setCurrentIndex(3);
}
void DialogMixture::on_comboType2_currentIndexChanged(int index)
{
    // Récupération numéro de division
    int numDiv = this->getNumDiv();
    if (numDiv < 0) return;

    // Récupération numéro de rang
    int numRang = this->getNumRang();
    if (numRang < 0) return;

    // Modification liste paramètres
    _listeParam[numDiv][2 + numRang * 2] = ui->comboType1->currentIndex();
    _listeParam[numDiv][3 + numRang * 2] = index;

    // Affichage
    this->dispRang();

    // Resélection de l'élément
    ui->listRangs->blockSignals(true);
    ui->listRangs->setCurrentRow(numRang);
    ui->listRangs->blockSignals(false);
}

// ACCEPTATION

void DialogMixture::accept()
{    
    // Save values
    storeRanks(_listeParam);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "mixture", "nom", ui->lineNom->text());
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "mixture", "density", ui->comboFreq->currentIndex());
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "mixture", "boucle", ui->checkBouclage->isChecked());
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "mixture", "stereo", ui->checkStereo->isChecked());

    // Adapt empty titles
    QString text = ui->lineNom->text();
    if (text.isEmpty())
        text = trUtf8("sans nom");

    // Remove empty divisions
    int nbDiv = _listeParam.length();
    int currentPos = 0;
    for (int i = 0; i < nbDiv; i++)
    {
        if (_listeParam[currentPos].length() == 2)
            _listeParam.removeAt(currentPos);
        else
            currentPos++;
    }
    int freq = 1;
    switch (ui->comboFreq->currentIndex())
    {
    case 0: freq = 1; break;
    case 1: freq = 3; break;
    case 2: freq = 6; break;
    }

    // Send values
    if (_listeParam.length() > 0)
        emit(accepted(_listeParam, text, ui->checkBouclage->isChecked(),
                      freq, ui->checkStereo->isChecked()));
    QDialog::accept();
}

// Affichage

void DialogMixture::dispDiv()
{
    // Clear the list
    ui->listDivisions->blockSignals(true);
    ui->listDivisions->clear();

    // Fill the list
    QList<int> sousListe;
    int valMin, valMax;
    for (int i = 0; i < _listeParam.length(); i++)
    {
        sousListe = _listeParam.at(i);

        // Add an element
        valMin = qMin(sousListe.at(0), sousListe.at(1));
        valMax = qMax(sousListe.at(0), sousListe.at(1));
        ui->listDivisions->addItem(ContextManager::keyName()->getKeyName(valMin) + " - " +
                                         ContextManager::keyName()->getKeyName(valMax));
    }
    ui->listDivisions->blockSignals(false);
}
void DialogMixture::dispRang(int numDiv)
{
    // Effacement liste rangs
    ui->listRangs->blockSignals(true);
    ui->listRangs->clear();
    if (ui->listDivisions->selectedItems().length() == 1)
    {
        // Numéro de la division
        if (numDiv == -1)
            numDiv = this->getNumDiv();
        if (numDiv < 0) return;
        QList<int> sousListe = _listeParam.at(numDiv);
        int type1, type2;
        for (int i = 0; i < (sousListe.length() - 2) / 2; i++)
        {
            type1 = sousListe.at(2 + 2 * i);
            type2 = sousListe.at(3 + 2 * i);
            ui->listRangs->addItem(getText(type1, type2));
        }
    }
    ui->listRangs->blockSignals(false);
}
QString DialogMixture::getText(int type1, int type2)
{
    QString text;
    switch (type1)
    {
    case 0: // octaves
        switch (type2)
        {
        case  0: text = "64'"  ; break;
        case  1: text = "32'"  ; break;
        case  2: text = "16'"  ; break;
        case  3: text = "8'"   ; break;
        case  4: text = "4'"   ; break;
        case  5: text = "2'"   ; break;
        case  6: text = "1'"   ; break;
        case  7: text = "1/2'" ; break;
        case  8: text = "1/4'" ; break;
        case  9: text = "1/8'" ; break;
        case 10: text = "1/16'"; break;
        }
        break;
    case 1: // quintes
        switch (type2)
        {
        case 0: text = "21' 1/3"; break;
        case 1: text = "10' 2/3"; break;
        case 2: text = "5' 1/3" ; break;
        case 3: text = "2' 2/3" ; break;
        case 4: text = "1' 1/3" ; break;
        case 5: text = "2/3'"   ; break;
        case 6: text = "1/3'"   ; break;
        case 7: text = "1/6'"   ; break;
        case 8: text = "1/12'"  ; break;
        }
        break;
    case 2: // tierces
        switch (type2)
        {
        case 0: text = "12' 4/5"; break;
        case 1: text = "6' 2/5" ; break;
        case 2: text = "3' 1/5" ; break;
        case 3: text = "1' 3/5" ; break;
        case 4: text = "4/5'"   ; break;
        case 5: text = "2/5'"   ; break;
        case 6: text = "1/5'"   ; break;
        case 7: text = "1/10'"  ; break;
        }
        break;
    case 3: // septièmes
        switch (type2)
        {
        case 0: text = "9' 1/7"; break;
        case 1: text = "4' 4/7"; break;
        case 2: text = "2' 2/7"; break;
        case 3: text = "1' 1/7"; break;
        case 4: text = "4/7'"  ; break;
        case 5: text = "2/7'"  ; break;
        case 6: text = "1/7'"  ; break;
        case 7: text = "1/14'" ; break;
        }
        break;
    case 4: // neuvièmes
        switch (type2)
        {
        case 0: text = "7' 1/9"; break;
        case 1: text = "3' 5/9"; break;
        case 2: text = "1' 7/9"; break;
        case 3: text = "8/9'"  ; break;
        case 4: text = "4/9'"  ; break;
        case 5: text = "2/9'"  ; break;
        case 6: text = "1/9'"  ; break;
        }
        break;
    case 5: // onzièmes
        switch (type2)
        {
        case 0: text = "5' 9/11" ; break;
        case 1: text = "2' 10/11"; break;
        case 2: text = "1' 5/11" ; break;
        case 3: text = "8/11'"   ; break;
        case 4: text = "4/11'"   ; break;
        case 5: text = "2/11'"   ; break;
        case 6: text = "1/11'"   ; break;
        }
        break;
    case 6: // treizièmes
        switch (type2)
        {
        case 0: text = "4' 12/13"; break;
        case 1: text = "2' 6/13"; break;
        case 2: text = "1' 3/13"; break;
        case 3: text = "8/13'"  ; break;
        case 4: text = "4/13'"  ; break;
        case 5: text = "2/13'"  ; break;
        case 6: text = "1/13'"  ; break;
        }
        break;
    case 7: // quinzièmes
        switch (type2)
        {
        case 0: text = "4' 4/15"; break;
        case 1: text = "2' 2/15"; break;
        case 2: text = "1' 1/15"; break;
        case 3: text = "8/15'"  ; break;
        case 4: text = "4/15'"  ; break;
        case 5: text = "2/15'"  ; break;
        case 6: text = "1/15'"  ; break;
        }
        break;
    case 8: // dix-septièmes
        switch (type2)
        {
        case 0: text = "3' 13/17"; break;
        case 1: text = "1' 15/17"; break;
        case 2: text = "16/17'"  ; break;
        case 3: text = "8/17'"   ; break;
        case 4: text = "4/17'"   ; break;
        case 5: text = "2/17'"   ; break;
        }
        break;
    case 9: // dix-neuvièmes
        switch (type2)
        {
        case 0: text = "3' 7/19" ; break;
        case 1: text = "1' 13/19"; break;
        case 2: text = "16/19'"  ; break;
        case 3: text = "8/19'"   ; break;
        case 4: text = "4/19'"   ; break;
        case 5: text = "2/19'"   ; break;
        }
        break;
    case 10: // vingt-et-unièmes
        switch (type2)
        {
        case 0: text = "3' 1/21" ; break;
        case 1: text = "1' 11/21"; break;
        case 2: text = "16/21'"  ; break;
        case 3: text = "8/21'"   ; break;
        case 4: text = "4/21'"   ; break;
        case 5: text = "2/21'"   ; break;
        }
        break;
    case 11: // vingt-troisièmes
        switch (type2)
        {
        case 0: text = "2' 18/23"; break;
        case 1: text = "1' 9/23" ; break;
        case 2: text = "16/23'"  ; break;
        case 3: text = "8/23'"   ; break;
        case 4: text = "4/23'"   ; break;
        case 5: text = "2/23'"   ; break;
        }
        break;
    case 12: // vingt-cinquièmes
        switch (type2)
        {
        case 0: text = "2' 14/25"; break;
        case 1: text = "1' 7/25" ; break;
        case 2: text = "16/25'"  ; break;
        case 3: text = "8/25'"   ; break;
        case 4: text = "4/25'"   ; break;
        case 5: text = "2/25'"   ; break;
        }
        break;
    case 13: // vingt-septièmes
        switch (type2)
        {
        case 0: text = "2' 10/27"; break;
        case 1: text = "1' 5/27" ; break;
        case 2: text = "16/27'"  ; break;
        case 3: text = "8/27'"   ; break;
        case 4: text = "4/27'"   ; break;
        case 5: text = "2/27'"   ; break;
        }
        break;
    }
    return text;
}
void DialogMixture::dispType2(int index)
{
    // Reset du combobox 2
    ui->comboType2->blockSignals(true);
    ui->comboType2->clear();

    // Nombre d'éléments en fonction du type de mutation
    int nbItem = 0;
    if (index == 0)
        nbItem = 11;
    else if (index == 1)
        nbItem = 9;
    else if (index  < 4)
        nbItem = 8;
    else if (index < 8)
        nbItem = 7;
    else
        nbItem = 6;

    for (int i = 0; i < nbItem; i++)
        ui->comboType2->addItem(getText(index, i));

    ui->comboType2->blockSignals(false);
}

// Utilitaires

int DialogMixture::getNumDiv()
{
    if (ui->listDivisions->selectedItems().length() != 1)
        return -1;
    int numDiv =  ui->listDivisions->row(ui->listDivisions->selectedItems().first());
    if (_listeParam.length() <= numDiv)
        return -1;
    return numDiv;
}
int DialogMixture::getNumRang()
{
    if (ui->listRangs->selectedItems().length() != 1)
        return -1;
    int numRang = ui->listRangs->row(ui->listRangs->selectedItems().first());
    int numDiv = this->getNumDiv();
    if (numDiv < 0)
        return -1;
    if (_listeParam.at(numDiv).length() <= numRang * 2 + 2)
        return -1;
    return numRang;
}
