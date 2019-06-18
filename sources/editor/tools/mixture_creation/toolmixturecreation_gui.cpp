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

#include "toolmixturecreation_gui.h"
#include "ui_toolmixturecreation_gui.h"
#include "contextmanager.h"
#include <QMessageBox>

ToolMixtureCreation_gui::ToolMixtureCreation_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolMixtureCreation_gui)
{
    ui->setupUi(this);

    // Icons
    ui->pushAddDiv->setIcon(ContextManager::theme()->getColoredSvg(":/icons/document-new.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushRemoveDiv->setIcon(ContextManager::theme()->getColoredSvg(":/icons/minus.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushAddRank->setIcon(ContextManager::theme()->getColoredSvg(":/icons/document-new.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushRemoveRank->setIcon(ContextManager::theme()->getColoredSvg(":/icons/minus.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
}

ToolMixtureCreation_gui::~ToolMixtureCreation_gui()
{
    delete ui;
}

void ToolMixtureCreation_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    Q_UNUSED(ids)
    ToolMixtureCreation_parameters * params = (ToolMixtureCreation_parameters *) parameters;

    // Recall values
    _divisions = params->getDivisions();
    ui->lineNom->setText(params->getInstrumentName());
    ui->comboFreq->setCurrentIndex(params->getDensityType());
    ui->checkBouclage->setChecked(params->getLoopSample());
    ui->checkStereo->setChecked(params->getStereoSample());

    // Select octaves
    this->on_comboType1_currentIndexChanged(0);

    // Display divisions
    this->dispDiv();
}

void ToolMixtureCreation_gui::saveParameters(AbstractToolParameters * parameters)
{
    // Save values
    ToolMixtureCreation_parameters * params = (ToolMixtureCreation_parameters *) parameters;
    params->setDivisions(_divisions);
    params->setInstrumentName(ui->lineNom->text());
    params->setDensityType(ui->comboFreq->currentIndex());
    params->setLoopSample(ui->checkBouclage->isChecked());
    params->setStereoSample(ui->checkStereo->isChecked());
}

void ToolMixtureCreation_gui::on_buttonBox_accepted()
{
    // Check that the name is specified
    if (ui->lineNom->text().isEmpty())
    {
        QMessageBox::warning(this, trUtf8("Warning"), trUtf8("The instrument name must be provided."));
        return;
    }

    // Check that at least one division is not empty
    bool ok = false;
    for (int i = 0; i < _divisions.length(); i++)
    {
        if (!_divisions[i].getRanks().empty())
        {
            ok = true;
            break;
        }
    }
    if (!ok)
    {
        QMessageBox::warning(this, trUtf8("Warning"), trUtf8("At least one rank must be specified."));
        return;
    }

    emit(this->validated());
}

void ToolMixtureCreation_gui::on_buttonBox_rejected()
{
    emit(this->canceled());
}

// Display

void ToolMixtureCreation_gui::dispDiv()
{
    // Clear the list
    ui->listDivisions->blockSignals(true);
    ui->listDivisions->clear();
    ui->listRangs->clear();

    // Fill the list
    int valMin, valMax;
    foreach (DivisionInfo di, _divisions)
    {
        // Add an element
        valMin = qMin(di.getMinKey(), di.getMaxKey());
        valMax = qMax(di.getMinKey(), di.getMaxKey());
        ui->listDivisions->addItem(ContextManager::keyName()->getKeyName(valMin) + " - " +
                                   ContextManager::keyName()->getKeyName(valMax));
    }
    ui->listDivisions->blockSignals(false);
}

void ToolMixtureCreation_gui::dispRang(int numDiv)
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
        DivisionInfo di = _divisions.at(numDiv);
        foreach (RankInfo ri, di.getRanks())
            ui->listRangs->addItem(getText(ri.getOvertoneType(), ri.getOctave()));
    }
    ui->listRangs->blockSignals(false);
}

QString ToolMixtureCreation_gui::getText(int overtoneType, int octave)
{
    QString text;
    switch (overtoneType)
    {
    case 0: // octaves
        switch (octave)
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
        switch (octave)
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
        switch (octave)
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
        switch (octave)
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
        switch (octave)
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
        switch (octave)
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
        switch (octave)
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
        switch (octave)
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
        switch (octave)
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
        switch (octave)
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
        switch (octave)
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
        switch (octave)
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
        switch (octave)
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
        switch (octave)
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

void ToolMixtureCreation_gui::dispType2(int index)
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

int ToolMixtureCreation_gui::getNumDiv()
{
    if (ui->listDivisions->selectedItems().length() != 1)
        return -1;
    int numDiv =  ui->listDivisions->row(ui->listDivisions->selectedItems().first());
    if (_divisions.length() <= numDiv)
        return -1;
    return numDiv;
}

int ToolMixtureCreation_gui::getNumRang()
{
    if (ui->listRangs->selectedItems().length() != 1)
        return -1;
    int numRang = ui->listRangs->row(ui->listRangs->selectedItems().first());
    int numDiv = this->getNumDiv();
    if (numDiv < 0)
        return -1;
    if (_divisions[numDiv].getRanks().count() <= numRang)
        return -1;
    return numRang;
}

// Edit divisions

void ToolMixtureCreation_gui::on_listDivisions_itemSelectionChanged()
{
    int currentRow = ui->listDivisions->currentRow();

    // Modification des spinboxes
    DivisionInfo di = _divisions[currentRow];
    ui->spinDivStart->blockSignals(true);
    ui->spinDivEnd->blockSignals(true);
    ui->spinDivStart->setValue(di.getMinKey());
    ui->spinDivEnd->setValue(di.getMaxKey());
    ui->spinDivStart->blockSignals(false);
    ui->spinDivEnd->blockSignals(false);

    // Affichage rangs
    this->dispRang(currentRow);
}

void ToolMixtureCreation_gui::on_pushAddDiv_pressed()
{
    // Ajdd a division
    _divisions << DivisionInfo(ui->spinDivStart->value(), ui->spinDivEnd->value());

    // Recopie des rangs précédents
    int numDiv = this->getNumDiv();
    if (numDiv >= 0)
        foreach (RankInfo ri, _divisions[numDiv].getRanks())
            _divisions[_divisions.size() - 1].addRank(ri.getOvertoneType(), ri.getOctave());

    // Affichage
    this->dispDiv();

    // Sélection de l'élément nouvellement créé
    ui->listDivisions->setCurrentRow(_divisions.length()-1);
}

void ToolMixtureCreation_gui::on_pushRemoveDiv_pressed()
{
    int numDiv = this->getNumDiv();
    if (numDiv < 0) return;
    _divisions.removeAt(numDiv);

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

void ToolMixtureCreation_gui::on_spinDivStart_valueChanged(int arg1)
{
    int numDiv = this->getNumDiv();
    if (numDiv < 0)
        return;
    _divisions[numDiv].setMinKey(arg1);

    // Affichage
    this->dispDiv();

    // Resélection de l'élément
    ui->listDivisions->blockSignals(true);
    ui->listDivisions->setCurrentRow(numDiv);
    ui->listDivisions->blockSignals(false);
}

void ToolMixtureCreation_gui::on_spinDivEnd_valueChanged(int arg1)
{
    int numDiv = this->getNumDiv();
    if (numDiv < 0) return;
    _divisions[numDiv].setMaxKey(arg1);

    // Affichage
    this->dispDiv();

    // Resélection de l'élément
    ui->listDivisions->blockSignals(true);
    ui->listDivisions->setCurrentRow(numDiv);
    ui->listDivisions->blockSignals(false);
}

// Edit ranks

void ToolMixtureCreation_gui::on_listRangs_itemSelectionChanged()
{
    int currentRow = ui->listRangs->currentRow();

    // Récupération numéro de division et rang
    int numDiv = this->getNumDiv();
    if (numDiv < 0) return;
    if (currentRow >= _divisions[numDiv].getRanks().count())
        return;

    // Update comboboxes
    ui->comboType1->blockSignals(true);
    ui->comboType2->blockSignals(true);
    ui->comboType1->setCurrentIndex(_divisions[numDiv].getRanks()[currentRow].getOvertoneType());
    this->dispType2(_divisions[numDiv].getRanks()[currentRow].getOvertoneType());
    ui->comboType2->blockSignals(true);
    ui->comboType2->setCurrentIndex(_divisions[numDiv].getRanks()[currentRow].getOctave());
    ui->comboType1->blockSignals(false);
    ui->comboType2->blockSignals(false);
}

void ToolMixtureCreation_gui::on_pushAddRank_clicked()
{
    // Récupération numéro de division
    int numDiv = this->getNumDiv();
    if (numDiv < 0) return;

    // Ajout d'un rang
    int type1 = ui->comboType1->currentIndex();
    int type2 = ui->comboType2->currentIndex();
    _divisions[numDiv].addRank(type1, type2);

    // Affichage
    this->dispRang();

    // Sélection de l'élément nouvellement créé
    ui->listRangs->setCurrentRow(_divisions[numDiv].getRanks().count() - 1);
}

void ToolMixtureCreation_gui::on_pushRemoveRank_clicked()
{
    // Récupération numéro de division et rang
    int numDiv = this->getNumDiv();
    if (numDiv < 0) return;
    int numRang = this->getNumRang();
    if (numRang < 0) return;

    // Remove a rank
    _divisions[numDiv].removeRank(numRang);

    // Affichage
    this->dispRang();

    // Selection
    if (numRang >= ui->listRangs->count())
        numRang--;
    if (numRang >= 0)
        ui->listRangs->setCurrentRow(numRang);
}

void ToolMixtureCreation_gui::on_comboType1_currentIndexChanged(int index)
{
    // Modification combobox type2
    this->dispType2(index);

    // Sélection du 4ème élément (fondamentale 8')
    ui->comboType2->setCurrentIndex(3);
}

void ToolMixtureCreation_gui::on_comboType2_currentIndexChanged(int index)
{
    // Récupération numéro de division
    int numDiv = this->getNumDiv();
    if (numDiv < 0) return;

    // Récupération numéro de rang
    int numRang = this->getNumRang();
    if (numRang < 0) return;

    // Modification liste paramètres
    _divisions[numDiv].getRanks()[numRang].setOvertoneType(ui->comboType1->currentIndex());
    _divisions[numDiv].getRanks()[numRang].setOctave(index);

    // Affichage
    this->dispRang();

    // Resélection de l'élément
    ui->listRangs->blockSignals(true);
    ui->listRangs->setCurrentRow(numRang);
    ui->listRangs->blockSignals(false);
}
