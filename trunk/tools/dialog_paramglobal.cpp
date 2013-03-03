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

#include "dialog_paramglobal.h"
#include "ui_dialog_paramglobal.h"
#include "dialog_selectitems.h"

// Constructeur, destructeur
DialogParamGlobal::DialogParamGlobal(Pile_sf2 *sf2, EltID id, QWidget *parent) :
    QDialog(parent),
    _sf2(sf2),
    ui(new Ui::DialogParamGlobal),
    _initialID(id)
{
    ui->setupUi(this);
    if (id.typeElement == elementPrst || id.typeElement == elementPrstInst)
        this->ui->pushApplyToOthers->setText(trUtf8("Appliquer à d'autres presets..."));
    this->ui->graphParamGlobal->raideurChanged(this->ui->doubleSpinRaideur->value());
    _listElt.append(id);
}
DialogParamGlobal::~DialogParamGlobal()
{
    delete ui;
}

void DialogParamGlobal::indexMotifChanged(int index)
{
    if (index == 3 || index == 4)
        this->ui->doubleSpinRaideur->setEnabled(true);
    else
        this->ui->doubleSpinRaideur->setEnabled(false);
    this->ui->graphParamGlobal->indexMotifChanged(index);
}
void DialogParamGlobal::raideurChanged(double value)
{
    this->ui->graphParamGlobal->raideurChanged(value);
    this->ui->graphParamGlobal->indexMotifChanged(this->ui->comboMotif->currentIndex());
}
void DialogParamGlobal::accept()
{
    // Récupération et mise en forme des modificateurs
    QVector<double> dValues = this->ui->graphParamGlobal->getValues();
    double dMin = this->ui->doubleSpinMin->value();
    double dMax = this->ui->doubleSpinMax->value();
    for (int i = 0; i < dValues.size(); i++)
        dValues[i] = dValues.at(i) * (dMax - dMin) + dMin;
    emit(accepted(dValues, _listElt, this->ui->comboModif->currentIndex(),
                  this->ui->comboValeur->currentIndex()));
    QDialog::accept();
}
void DialogParamGlobal::applyToOthers()
{
    // Affichage liste d'éléments
    DialogSelectItems * dialogSelect = new DialogSelectItems(_initialID, _listElt, _sf2, this);
    dialogSelect->setAttribute(Qt::WA_DeleteOnClose, true);
    this->connect(dialogSelect, SIGNAL(accepted(QList<EltID>)),
                  SLOT(eltChanged(QList<EltID>)));
    dialogSelect->show();
}
void DialogParamGlobal::eltChanged(QList<EltID> listElt)
{
    _listElt = listElt;
}

/////////////////////////////////////////////////////
///////////////////// GRAPHIQUE /////////////////////
/////////////////////////////////////////////////////

GraphParamGlobal::GraphParamGlobal(QWidget * parent) : QCustomPlot(parent),
    flagEdit(false),
    editionEnabled(true),
    raideurExp(50.0)
{
    // Configuration du graphe
    this->addGraph();
    QPen graphPen;
    graphPen.setColor(QColor(100, 130, 250));
    graphPen.setWidthF(2);
    this->graph(0)->setPen(graphPen);
    this->graph(0)->setLineStyle(QCPGraph::lsNone);
    this->graph(0)->setScatterStyle(QCP::ssDot);
    this->graph(0)->setAntialiased(true);
    this->graph(0)->setAntialiasedScatters(false);
    // Axes
    this->nbPoints = 140;
    this->xAxis->setRange(0, this->nbPoints);
    this->yAxis->setRange(0, 1);
    this->xAxis->setVisible(false);
    this->xAxis->setTicks(false);
    this->yAxis->setVisible(false);
    this->yAxis->setTicks(false);
    // Marges
    this->setAutoMargin(false);
    this->setMargin(0, 0, 0, 0);
    // Préparation des données
    this->dValues.resize(this->nbPoints);
    this->dValues.fill(0.5);
    // Filtre sur les événements
    this->installEventFilter(this);
    // Affichage
    this->replot();
}

// Méthodes publiques
void GraphParamGlobal::indexMotifChanged(int motif)
{
    switch (motif)
    {
    case 0: // manuel
        this->editionEnabled = true;
        break;
    case 1: // linéaire ascendant
        this->editionEnabled = false;
        this->flagEdit = false;
        this->writeLineaireCroissant();
        break;
    case 2: // linéaire descendant
        this->editionEnabled = false;
        this->flagEdit = false;
        this->writeLineaireDecroissant();
        break;
    case 3: // exponentiel ascendant
        this->editionEnabled = false;
        this->flagEdit = false;
        this->writeExpCroissant();
        break;
    case 4: // exponentiel descendant
        this->editionEnabled = false;
        this->flagEdit = false;
        this->writeExpDecroissant();
        break;
    case 5: // aléatoire
        this->editionEnabled = false;
        this->flagEdit = false;
        this->writeAleatoire();
        break;
    }
}
void GraphParamGlobal::raideurChanged(double value)
{
    this->raideurExp = value;
}
QVector<double> GraphParamGlobal::getValues()
{
    return this->dValues;
}

// Méthodes privées
void GraphParamGlobal::mousePressed(QPoint pos)
{
    if (this->editionEnabled)
    {
        this->flagEdit = true;
        // Inscription du premier point
        this->write(pos);
    }
}
void GraphParamGlobal::mouseReleased(QPoint pos)
{
    Q_UNUSED(pos);
    this->flagEdit = false;
}
void GraphParamGlobal::mouseMoved(QPoint pos)
{
    if (this->flagEdit)
        this->write(pos);
}
void GraphParamGlobal::write(QPoint pos)
{
    // Conversion coordonnées
    int x = (int)this->xAxis->pixelToCoord(pos.x());
    double y = this->yAxis->pixelToCoord(pos.y());
    if (x < 0) x = 0;
    else if (x > this->nbPoints-1) x = this->nbPoints-1;
    // Modification valeur
    this->dValues[x] = y;
    // Affichage
    this->replot();
}
void GraphParamGlobal::writeLineaireCroissant()
{
    for (int i = 0; i < this->nbPoints; i++)
        this->dValues[i] = (double)i / (nbPoints-1);
    // Affichage
    this->replot();
}
void GraphParamGlobal::writeLineaireDecroissant()
{
    for (int i = 0; i < this->nbPoints; i++)
        this->dValues[i] = 1.0 - (double)i / (nbPoints-1);
    // Affichage
    this->replot();
}
void GraphParamGlobal::writeExpCroissant()
{
    double baseExp = this->calculBaseExp();
    double alpha = 1.0 / (pow(baseExp, 140) - 1.0);
    for (int i = 0; i < this->nbPoints; i++)
        this->dValues[i] = alpha * (pow(baseExp, i) - 1.0);
    // Affichage
    this->replot();
}
void GraphParamGlobal::writeExpDecroissant()
{
    double baseExp = this->calculBaseExp();
    double alpha = 1.0 / (pow(baseExp, -140) - 1.0);
    for (int i = 0; i < this->nbPoints; i++)
        this->dValues[i] = 1.0 - alpha * (pow(baseExp, -i) - 1.0);
    // Affichage
    this->replot();
}
void GraphParamGlobal::writeAleatoire()
{
    for (int i = 0; i < this->nbPoints; i++)
        this->dValues[i] = (double)(qrand()%101) / 100;
    // Affichage
    this->replot();
}
void GraphParamGlobal::replot()
{
    QVector<double> x(this->nbPoints);
    for (int i = 0; i < this->nbPoints; i++)
        x[i] = i;
    this->graph(0)->setData(x, this->dValues);
    // Affichage
    QCustomPlot::replot();
}
double GraphParamGlobal::calculBaseExp()
{
    return 1 + 1.0 / (101.0-this->raideurExp);
}
