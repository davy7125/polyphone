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

#include "dialog_mixture.h"
#include "ui_dialog_mixture.h"
#include "sf2_types.h"

// Constructeur, destructeur
DialogMixture::DialogMixture(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogMixture)
{
    ui->setupUi(this);
    // Sélection octaves
    this->on_comboType1_currentIndexChanged(0);
}
DialogMixture::~DialogMixture()
{
    delete ui;
}


// EDITION DIVISIONS

void DialogMixture::on_listDivisions_currentRowChanged(int currentRow)
{
    // Modification des spinboxes
    QList<int> sousListe = this->listeParam.at(currentRow);
    this->ui->spinDivStart->blockSignals(true);
    this->ui->spinDivEnd->blockSignals(true);
    this->ui->spinDivStart->setValue(sousListe.at(0));
    this->ui->spinDivEnd->setValue(sousListe.at(1));
    this->ui->spinDivStart->blockSignals(false);
    this->ui->spinDivEnd->blockSignals(false);
    // Affichage rangs
    this->dispRang(currentRow);
}
void DialogMixture::on_pushAddDiv_pressed()
{
    // Ajout d'une division
    int divStart = this->ui->spinDivStart->value();
    int divEnd = this->ui->spinDivEnd->value();
    QList<int> newSousListe;
    newSousListe.append(divStart);
    newSousListe.append(divEnd);
    this->listeParam.append(newSousListe);
    // Affichage
    this->dispDiv();
    // Sélection de l'élément nouvellement créé
    this->ui->listDivisions->setCurrentRow(this->listeParam.length()-1);
}
void DialogMixture::on_pushRemoveDiv_pressed()
{
    int numDiv = this->getNumDiv();
    if (numDiv < 0) return;
    this->listeParam.removeAt(numDiv);
    // Affichage
    this->dispDiv();
    this->dispRang();
}
void DialogMixture::on_spinDivStart_valueChanged(int arg1)
{
    int numDiv = this->getNumDiv();
    if (numDiv < 0) return;
    this->listeParam[numDiv][0] = arg1;
    // Affichage
    this->dispDiv();
    // Resélection de l'élément
    this->ui->listDivisions->blockSignals(true);
    this->ui->listDivisions->setCurrentRow(numDiv);
    this->ui->listDivisions->blockSignals(false);
}
void DialogMixture::on_spinDivEnd_valueChanged(int arg1)
{
    int numDiv = this->getNumDiv();
    if (numDiv < 0) return;
    this->listeParam[numDiv][1] = arg1;
    // Affichage
    this->dispDiv();
    // Resélection de l'élément
    this->ui->listDivisions->blockSignals(true);
    this->ui->listDivisions->setCurrentRow(numDiv);
    this->ui->listDivisions->blockSignals(false);
}

// EDITION RANGS

void DialogMixture::on_listRangs_currentRowChanged(int currentRow)
{
    // Récupération numéro de division et rang
    int numDiv = this->getNumDiv();
    if (numDiv < 0) return;
    if (currentRow*2+2 >= this->listeParam.at(numDiv).length()) return;
    // Mise à jour des comboboxes
    this->ui->comboType1->blockSignals(true);
    this->ui->comboType2->blockSignals(true);
    this->ui->comboType1->setCurrentIndex(this->listeParam.at(numDiv).at(2+2*currentRow));
    this->dispType2(this->listeParam.at(numDiv).at(2+2*currentRow));
    this->ui->comboType2->blockSignals(true);
    this->ui->comboType2->setCurrentIndex(this->listeParam.at(numDiv).at(3+2*currentRow));
    this->ui->comboType1->blockSignals(false);
    this->ui->comboType2->blockSignals(false);
}
void DialogMixture::on_pushAddRank_clicked()
{
    // Récupération numéro de division
    int numDiv = this->getNumDiv();
    if (numDiv < 0) return;
    // Ajout d'un rang
    int type1 = this->ui->comboType1->currentIndex();
    int type2 = this->ui->comboType2->currentIndex();
    this->listeParam[numDiv].append(type1);
    this->listeParam[numDiv].append(type2);
    // Affichage
    this->dispRang();
    // Sélection de l'élément nouvellement créé
    this->ui->listRangs->setCurrentRow(this->listeParam.at(numDiv).length()/2-2);
}
void DialogMixture::on_pushRemoveRank_clicked()
{
    // Récupération numéro de division et rang
    int numDiv = this->getNumDiv();
    if (numDiv < 0) return;
    int numRang = this->getNumRang();
    if (numRang < 0) return;
    // Suppression des données
    this->listeParam[numDiv].removeAt(2+numRang*2);
    this->listeParam[numDiv].removeAt(2+numRang*2);
    // Affichage
    this->dispRang();
}
void DialogMixture::on_comboType1_currentIndexChanged(int index)
{
    // Modification combobox type2
    this->dispType2(index);
    // Sélection du 3ème élément (fondamentale 8')
    this->ui->comboType2->setCurrentIndex(2);
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
    this->listeParam[numDiv][2+numRang*2] = this->ui->comboType1->currentIndex();
    this->listeParam[numDiv][3+numRang*2] = index;
    // Affichage
    this->dispRang();
    // Resélection de l'élément
    this->ui->listRangs->blockSignals(true);
    this->ui->listRangs->setCurrentRow(numRang);
    this->ui->listRangs->blockSignals(false);
}

// ACCEPTATION

void DialogMixture::accept()
{
    // Envoi des valeurs
    QString text = this->ui->lineNom->text();
    if (text.isEmpty())
        text = tr("sans nom");
    // Suppression des divisions vides
    int nbDiv = this->listeParam.length();
    int currentPos = 0;
    for (int i = 0; i < nbDiv; i++)
    {
        if (this->listeParam[currentPos].length() == 2)
            this->listeParam.removeAt(currentPos);
        else
            currentPos++;
    }
    int freq = 1;
    switch (this->ui->comboFreq->currentIndex())
    {
    case 0: freq = 1; break;
    case 1: freq = 3; break;
    case 2: freq = 6; break;
    }
    if (this->listeParam.length() > 0)
        emit(accepted(this->listeParam, text, this->ui->checkBouclage->isChecked(), freq));
    QDialog::accept();
}

// Affichage

void DialogMixture::dispDiv()
{
    // Effacement liste divisions
    this->ui->listDivisions->blockSignals(true);
    this->ui->listDivisions->clear();
    // Remplissage
    QList<int> sousListe;
    int valMin, valMax;
    for (int i = 0; i < this->listeParam.length(); i++)
    {
        sousListe = this->listeParam.at(i);
        // Ajout élément
        valMin = qMin(sousListe.at(0), sousListe.at(1));
        valMax = qMax(sousListe.at(0), sousListe.at(1));
        char T[30];
        sprintf(T, "%d - %d", valMin, valMax);
        this->ui->listDivisions->addItem(T);
    }
    this->ui->listDivisions->blockSignals(false);
}
void DialogMixture::dispRang(int numDiv)
{
    // Effacement liste rangs
    this->ui->listRangs->blockSignals(true);
    this->ui->listRangs->clear();
    if (this->ui->listDivisions->selectedItems().length() == 1)
    {
        // Numéro de la division
        if (numDiv == -1)
            numDiv = this->getNumDiv();
        if (numDiv < 0) return;
        QList<int> sousListe = this->listeParam.at(numDiv);
        int type1, type2;
        for (int i = 0; i < (sousListe.length() - 2) / 2; i++)
        {
            type1 = sousListe.at(2+2*i);
            type2 = sousListe.at(3+2*i);
            this->ui->listRangs->addItem(getText(type1, type2));
        }
    }
    this->ui->listRangs->blockSignals(false);
}
QString DialogMixture::getText(int type1, int type2)
{
    QString text;
    switch (type1)
    {
    case 0: // octaves
        switch (type2)
        {
        case 0: text = trUtf8("32'"); break;
        case 1: text = trUtf8("16'"); break;
        case 2: text = trUtf8("8'"); break;
        case 3: text = trUtf8("4'"); break;
        case 4: text = trUtf8("2'"); break;
        case 5: text = trUtf8("1'"); break;
        case 6: text = trUtf8("1/2'"); break;
        case 7: text = trUtf8("1/4'"); break;
        case 8: text = trUtf8("1/8'"); break;
        case 9: text = trUtf8("1/16'"); break;
        }
        break;
    case 1: // quintes
        switch (type2)
        {
        case 0: text = trUtf8("10' 2/3"); break;
        case 1: text = trUtf8("5' 1/3"); break;
        case 2: text = trUtf8("2' 2/3"); break;
        case 3: text = trUtf8("1' 1/3"); break;
        case 4: text = trUtf8("2/3'"); break;
        case 5: text = trUtf8("1/3'"); break;
        case 6: text = trUtf8("1/6'"); break;
        case 7: text = trUtf8("1/12'"); break;
        }
        break;
    case 2: // tierces
        switch (type2)
        {
        case 0: text = trUtf8("6' 2/5"); break;
        case 1: text = trUtf8("3' 1/5"); break;
        case 2: text = trUtf8("1' 3/5"); break;
        case 3: text = trUtf8("4/5'"); break;
        case 4: text = trUtf8("2/5'"); break;
        case 5: text = trUtf8("1/5'"); break;
        case 6: text = trUtf8("1/10'"); break;
        }
        break;
    case 3: // septièmes
        switch (type2)
        {
        case 0: text = trUtf8("4' 4/7"); break;
        case 1: text = trUtf8("2' 2/7"); break;
        case 2: text = trUtf8("1' 1/7"); break;
        case 3: text = trUtf8("4/7'"); break;
        case 4: text = trUtf8("2/7'"); break;
        case 5: text = trUtf8("1/7'"); break;
        case 6: text = trUtf8("1/14'"); break;
        }
        break;
    case 4: // neuvièmes
        switch (type2)
        {
        case 0: text = trUtf8("3' 5/9"); break;
        case 1: text = trUtf8("1' 7/9"); break;
        case 2: text = trUtf8("8/9'"); break;
        case 3: text = trUtf8("4/9'"); break;
        case 4: text = trUtf8("2/9'"); break;
        case 5: text = trUtf8("1/9'"); break;
        }
        break;
    case 5: // onzièmes
        switch (type2)
        {
        case 0: text = trUtf8("2' 10/11"); break;
        case 1: text = trUtf8("1' 5/11"); break;
        case 2: text = trUtf8("8/11'"); break;
        case 3: text = trUtf8("4/11'"); break;
        case 4: text = trUtf8("2/11'"); break;
        case 5: text = trUtf8("1/11'"); break;
        }
        break;
    case 6: // treizièmes
        switch (type2)
        {
        case 0: text = trUtf8("2' 6/13"); break;
        case 1: text = trUtf8("1' 3/13"); break;
        case 2: text = trUtf8("8/13'"); break;
        case 3: text = trUtf8("4/13'"); break;
        case 4: text = trUtf8("2/13'"); break;
        case 5: text = trUtf8("1/13'"); break;
        }
        break;
    case 7: // quinzièmes
        switch (type2)
        {
        case 0: text = trUtf8("2' 2/15"); break;
        case 1: text = trUtf8("1' 1/15"); break;
        case 2: text = trUtf8("8/15'"); break;
        case 3: text = trUtf8("4/15'"); break;
        case 4: text = trUtf8("2/15'"); break;
        case 5: text = trUtf8("1/15'"); break;
        }
        break;
    case 8: // dix-septièmes
        switch (type2)
        {
        case 0: text = trUtf8("1' 15/17"); break;
        case 1: text = trUtf8("16/17'"); break;
        case 2: text = trUtf8("8/17'"); break;
        case 3: text = trUtf8("4/17'"); break;
        case 4: text = trUtf8("2/17'"); break;
        }
        break;
    case 9: // dix-neuvièmes
        switch (type2)
        {
        case 0: text = trUtf8("1' 13/19"); break;
        case 1: text = trUtf8("16/19'"); break;
        case 2: text = trUtf8("8/19'"); break;
        case 3: text = trUtf8("4/19'"); break;
        case 4: text = trUtf8("2/19'"); break;
        }
        break;
    }
    return text;
}
void DialogMixture::dispType2(int index)
{
    // Reset du combobox 2
    this->ui->comboType2->blockSignals(true);
    this->ui->comboType2->clear();
    switch (index)
    {
    case 0: // octaves
        for (int i = 0; i < 10; i++)
            this->ui->comboType2->addItem(getText(index, i));
        break;
    case 1: // quintes
        for (int i = 0; i < 8; i++)
            this->ui->comboType2->addItem(getText(index, i));
        break;
    case 2: // tierces
        for (int i = 0; i < 7; i++)
            this->ui->comboType2->addItem(getText(index, i));
        break;
    case 3: // septièmes
        for (int i = 0; i < 7; i++)
            this->ui->comboType2->addItem(getText(index, i));
        break;
    case 4: // neuvièmes
        for (int i = 0; i < 6; i++)
            this->ui->comboType2->addItem(getText(index, i));
        break;
    case 5: // onzièmes
        for (int i = 0; i < 6; i++)
            this->ui->comboType2->addItem(getText(index, i));
        break;
    case 6: // treizièmes
        for (int i = 0; i < 6; i++)
            this->ui->comboType2->addItem(getText(index, i));
        break;
    case 7: // quinzièmes
        for (int i = 0; i < 6; i++)
            this->ui->comboType2->addItem(getText(index, i));
        break;
    case 8: // dix-septièmes
        for (int i = 0; i < 5; i++)
            this->ui->comboType2->addItem(getText(index, i));
        break;
    case 9: // dix-neuvièmes
        for (int i = 0; i < 5; i++)
            this->ui->comboType2->addItem(getText(index, i));
        break;
    }
    this->ui->comboType2->blockSignals(false);
}

// Utilitaires

int DialogMixture::getNumDiv()
{
    if (this->ui->listDivisions->selectedItems().length() != 1)
        return -1;
    int numDiv =  this->ui->listDivisions->row(this->ui->listDivisions->selectedItems().at(0));
    if (this->listeParam.length() <= numDiv)
        return -1;
    return numDiv;
}
int DialogMixture::getNumRang()
{
    if (this->ui->listRangs->selectedItems().length() != 1)
        return -1;
    int numRang = this->ui->listRangs->row(this->ui->listRangs->selectedItems().at(0));
    int numDiv = this->getNumDiv();
    if (numDiv < 0)
        return -1;
    if (this->listeParam.at(numDiv).length() <= numRang*2+2)
        return -1;
    return numRang;
}
