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

#include "dialog_space.h"
#include "ui_dialog_space.h"
#include "config.h"

// Constructeur, destructeur
DialogSpace::DialogSpace(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSpace)
{
    ui->setupUi(this);
    Config * conf = Config::getInstance();
    this->ui->comboMotif->blockSignals(true);
    this->ui->comboMotif->setCurrentIndex(conf->getTools_i_space_motif());
    this->ui->comboMotif->blockSignals(false);
    this->ui->spinNbDivision->blockSignals(true);
    this->ui->spinNbDivision->setValue(conf->getTools_i_space_divisions());
    this->ui->spinNbDivision->blockSignals(false);
    this->ui->spinEtalement->blockSignals(true);
    this->ui->spinEtalement->setValue(conf->getTools_i_space_etalement());
    this->ui->spinEtalement->blockSignals(false);
    this->ui->spinOccupation->blockSignals(true);
    this->ui->spinOccupation->setValue(conf->getTools_i_space_occupation());
    this->ui->spinOccupation->blockSignals(false);
    this->ui->spinOffset->blockSignals(true);
    this->ui->spinOffset->setValue(conf->getTools_i_space_offset());
    this->ui->spinOffset->blockSignals(false);
    this->ui->checkSens->blockSignals(true);
    this->ui->checkSens->setChecked(conf->getTools_i_space_renversement1());
    this->ui->checkSens->blockSignals(false);
    this->ui->checkSens2->blockSignals(true);
    this->ui->checkSens2->setChecked(conf->getTools_i_space_renversement2());
    this->ui->checkSens2->blockSignals(false);

    // Activation des renversements
    if (ui->spinNbDivision->value() == 1)
        this->ui->checkSens->setEnabled(false);
    else
        this->ui->checkSens->setEnabled(true);
    if (ui->comboMotif->currentIndex() == 0 || ui->comboMotif->currentIndex() == 2)
        this->ui->checkSens2->setEnabled(false);
    else
        this->ui->checkSens2->setEnabled(true);


    // Remplissage du graphique
    this->ui->grapheSpace->setData(
                this->ui->comboMotif->currentIndex(),
                this->ui->spinNbDivision->value(),
                this->ui->spinEtalement->value(),
                this->ui->spinOccupation->value(),
                this->ui->spinOffset->value(),
                this->ui->checkSens->isChecked(),
                this->ui->checkSens2->isChecked());
}
DialogSpace::~DialogSpace()
{
    delete ui;
}


void DialogSpace::accept()
{
    emit(accepted(this->ui->comboMotif->currentIndex(),
                  this->ui->spinNbDivision->value(),
                  this->ui->spinEtalement->value(),
                  this->ui->spinOccupation->value(),
                  this->ui->spinOffset->value(),
                  this->ui->checkSens->isChecked(),
                  this->ui->checkSens2->isChecked()));
    QDialog::accept();
}

void DialogSpace::motifChanged(int value)
{
    if (value == 0 || value == 2)
        this->ui->checkSens2->setEnabled(false);
    else
        this->ui->checkSens2->setEnabled(true);
    this->ui->grapheSpace->setData(
                value,
                this->ui->spinNbDivision->value(),
                this->ui->spinEtalement->value(),
                this->ui->spinOccupation->value(),
                this->ui->spinOffset->value(),
                this->ui->checkSens->isChecked(),
                this->ui->checkSens2->isChecked());
}
void DialogSpace::nbDivChanged(int value)
{
    if (value == 1)
        this->ui->checkSens->setEnabled(false);
    else
        this->ui->checkSens->setEnabled(true);
    this->ui->grapheSpace->setData(
                this->ui->comboMotif->currentIndex(),
                value,
                this->ui->spinEtalement->value(),
                this->ui->spinOccupation->value(),
                this->ui->spinOffset->value(),
                this->ui->checkSens->isChecked(),
                this->ui->checkSens2->isChecked());
}
void DialogSpace::etalementChanged(int value)
{
    this->ui->grapheSpace->setData(
                this->ui->comboMotif->currentIndex(),
                this->ui->spinNbDivision->value(),
                value,
                this->ui->spinOccupation->value(),
                this->ui->spinOffset->value(),
                this->ui->checkSens->isChecked(),
                this->ui->checkSens2->isChecked());
}
void DialogSpace::occupationChanged(int value)
{
    this->ui->grapheSpace->setData(
                this->ui->comboMotif->currentIndex(),
                this->ui->spinNbDivision->value(),
                this->ui->spinEtalement->value(),
                value,
                this->ui->spinOffset->value(),
                this->ui->checkSens->isChecked(),
                this->ui->checkSens2->isChecked());
}
void DialogSpace::offsetChanged(int value)
{
    this->ui->grapheSpace->setData(
                this->ui->comboMotif->currentIndex(),
                this->ui->spinNbDivision->value(),
                this->ui->spinEtalement->value(),
                this->ui->spinOccupation->value(),
                value,
                this->ui->checkSens->isChecked(),
                this->ui->checkSens2->isChecked());
}
void DialogSpace::sensChanged(int value)
{
    this->ui->grapheSpace->setData(
                this->ui->comboMotif->currentIndex(),
                this->ui->spinNbDivision->value(),
                this->ui->spinEtalement->value(),
                this->ui->spinOccupation->value(),
                this->ui->spinOffset->value(),
                value,
                this->ui->checkSens2->isChecked());
}
void DialogSpace::sens2Changed(int value)
{
    this->ui->grapheSpace->setData(
                this->ui->comboMotif->currentIndex(),
                this->ui->spinNbDivision->value(),
                this->ui->spinEtalement->value(),
                this->ui->spinOccupation->value(),
                this->ui->spinOffset->value(),
                this->ui->checkSens->isChecked(),
                value);
}

double DialogSpace::space(int noteMin, int noteMax, int note,
                    int motif, int nbDiv, int etalement, int occupation,
                    int offset, int sens, int sens2)
{
    double pan = 0.5;
    switch (motif)
    {
    case 0: case 2:{ // Ascendant, descendant
        // Détermination de l'axe après lequel se trouvera la note
        int noteRelative = 0;
        if (motif == 0)
            noteRelative = noteMax - note;
        else
            noteRelative = note - noteMin;
        int axe = noteRelative % nbDiv;
        if (axe % 2 == 0) axe = axe / 2;
        else axe = nbDiv - (axe+1) / 2;
        if (sens != 0) axe = nbDiv - 1 - axe;
        double posAxe = (double)axe / nbDiv;
        // Calcul de l'écart à l'axe
        double e = 1.0 / (noteMax - noteMin + nbDiv); // écart unitaire
        int ne_axe = noteRelative / nbDiv;
        double ecart_axe = e * ne_axe;
        // Prise en compte de l'occupation
        ecart_axe *= (double)occupation/100;
        // Synthèse
        pan = posAxe + ecart_axe + (1 - (double)occupation/100) / (2*nbDiv);
        pan = (double)etalement / 100       * pan
            + (1 - (double)etalement / 100) * (double)offset / 100 + 0.007;
        }break;
    case 1: case 3:{ // Creux, pointe
        // Ajustement note
        int noteRelative = 0;
        if (motif == 1)
        {
            // Calcul décalage
            int resteNotes = (noteMax - noteMin + 1) % (2 * nbDiv);
            int decalage = 0;
            if (resteNotes < nbDiv)
            {
                decalage = nbDiv - resteNotes;
            }
            else if (resteNotes > nbDiv)
            {
                decalage = 3 * nbDiv - resteNotes;
            }
            noteRelative = noteMax - note + decalage;
        }
        else
            noteRelative = note - noteMin;
        // Détermination de l'axe autour duquel se trouve la note
        int axe = noteRelative % nbDiv;
        if (axe % 2 == 0) axe = axe / 2;
        else axe = nbDiv - (axe+1) / 2;
        if (sens != 0) axe = nbDiv - 1 - axe;
        double posAxe = (double)(2*axe+1) / (2 * nbDiv);
        // Calcul de l'écart à l'axe
        double e = 1.0 / (ceil((double)(noteMax - noteMin + 1 - nbDiv)/(2*nbDiv))*(2*nbDiv) + nbDiv); // écart unitaire
        int ne_axe = (noteRelative + nbDiv) / (2 * nbDiv);
        double ecart_axe = e * ne_axe;
        // Prise en compte de l'occupation
        double mult = 1; // dilation future
        if (motif == 1)
        {
            ecart_axe = (double)(ecart_axe * occupation + (double)(100 - occupation) / (2*nbDiv)) / 100;
        }
        else
        {
            ecart_axe *= (double)occupation / 100;
            mult = (double)(nbDiv) / (nbDiv - 1 + (double)occupation / 100);
        }
        // Côté de l'axe
        int cote = noteRelative % (2*nbDiv);
        if (cote >= nbDiv)
            ecart_axe = -ecart_axe;
        if (sens2 != 0) ecart_axe = -ecart_axe;
        // Synthèse
        pan = (double)etalement / 100       * ((double)mult*(posAxe + ecart_axe - 0.5) + 0.5)
            + (1 - (double)etalement / 100) * (double)offset / 100;
        }break;
    default:
        break;
    }
    return pan;
}


// Graphe space

GraphSpace::GraphSpace(QWidget * parent) : QCustomPlot(parent)
{
    // Configuration du graphe
    this->addGraph();
    QPen graphPen;
    graphPen.setColor(QColor(100, 130, 250));
    graphPen.setWidthF(2);
    this->graph(0)->setPen(graphPen);
    this->graph(0)->setLineStyle(QCPGraph::lsImpulse);
    this->graph(0)->setAntialiased(false);
    // Axes
    this->xAxis->setRange(0, 1);
    this->yAxis->setRange(0, 1.05);
    this->xAxis->setVisible(false);
    this->xAxis->setTicks(false);
    this->yAxis->setVisible(false);
    this->yAxis->setTicks(false);
    // Marges
    this->setAutoMargin(false);
    this->setMargin(0, 0, 0, 0);
}

void GraphSpace::setData(int motif, int nbDiv, int etalement, int occupation, int offset, int sens, int sens2)
{
    // Affichage courbe
    int noteMin = 36;
    int noteMax = 96;
    unsigned int nbNotes = noteMax - noteMin + 1;
    QVector<double> x(nbNotes), y(nbNotes);
    // Longueur max tuyau
    double freq = exp2((noteMin + 36.3763) / 12);
    double l = 172 / freq; // en mètres pour un tuyau ouvert
    for (unsigned long i = 0; i < nbNotes; i++)
    {
        // Longueur tuyau
        freq = exp2((i + noteMin + 36.3763) / 12);
        y[i] = 172 / freq / l;
        x[i] = DialogSpace::space(noteMin, noteMax, i + noteMin,
                                  motif, nbDiv, etalement, occupation,
                                  offset, sens, sens2);
    }
    this->graph(0)->setData(x, y);
    this->replot();
}
